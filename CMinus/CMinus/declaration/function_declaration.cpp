#include "../type/class_type.h"
#include "../type/proxy_type.h"
#include "../type/variadic_type.h"
#include "../type/modified_type.h"

#include "../node/memory_reference_node.h"
#include "../storage/specialized_storage.h"
#include "../runtime/runtime_value_guard.h"

#include "variable_declaration.h"
#include "function_declaration.h"

cminus::declaration::function::~function() = default;

cminus::declaration::callable::id_type cminus::declaration::function::get_id() const{
	return id_type::function;
}

cminus::storage::object *cminus::declaration::function::get_parent() const{
	return parent_;
}

std::shared_ptr<cminus::type::object> cminus::declaration::function::get_type() const{
	return type_;
}

void cminus::declaration::function::add_parameter(std::shared_ptr<variable> value){
	if (value->get_flags() != flags::nil)
		throw exception::bad_parameter_list();

	auto &name = value->get_name();
	if (!name.empty()){
		for (auto parameter : parameter_list_){
			if (parameter->get_name() == name)
				throw exception::bad_parameter_list();
		}
	}

	if (max_arg_count_ == static_cast<std::size_t>(-1))//Variadic declaration must be last in list
		throw exception::bad_parameter_list();

	auto is_variadic = value->get_type()->is(type::object::query_type::variadic);
	auto has_initialization = (!is_variadic && value->get_initialization() != nullptr);

	if (min_arg_count_ != max_arg_count_ && !is_variadic && !has_initialization)
		throw exception::bad_parameter_list();

	++max_arg_count_;
	if (is_variadic)
		max_arg_count_ = static_cast<std::size_t>(-1);
	else if (!has_initialization)
		++min_arg_count_;

	parameter_list_.push_back(value);
	type_->add_parameter_type(value->get_type());
}

bool cminus::declaration::function::is_defined() const{
	return (get_definition() != nullptr);
}

int cminus::declaration::function::get_score(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	if (auto args_count = get_args_count_(context, args); args_count < min_arg_count_ || max_arg_count_ < args_count)
		return type::object::get_score_value(type::object::score_result_type::nil);

	auto param_it = parameter_list_.begin();
	auto lowest_rank_score = get_context_score_(context, param_it);
	if (lowest_rank_score == type::object::get_score_value(type::object::score_result_type::nil) || args.empty())
		return lowest_rank_score;

	auto arg_it = args.begin();
	auto current_rank_score = lowest_rank_score;

	std::shared_ptr<type::object> variadic_base_type, param_type;
	for (; arg_it != args.end(); ++arg_it){
		if (variadic_base_type == nullptr){
			if (param_it == parameter_list_.end())
				break;

			param_type = (*param_it)->get_type();
			if (auto variadic_type = dynamic_cast<type::variadic *>(param_type->get_non_proxy()); variadic_type != nullptr)
				param_type = variadic_base_type = variadic_type->get_base_type();

			++param_it;
		}

		if ((current_rank_score = get_arg_score_(param_type, *arg_it)) < lowest_rank_score)
			lowest_rank_score = current_rank_score;
	}

	return ((arg_it == args.end()) ? lowest_rank_score : type::object::get_score_value(type::object::score_result_type::nil));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::call_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	if (!is_defined())
		throw exception::function_not_defined();

	std::shared_ptr<memory::reference> return_value;
	std::shared_ptr<storage::object> storage;

	if (dynamic_cast<type::class_ *>(get_parent()) == nullptr)
		storage = std::make_shared<storage::named_object>(name_, parent_);
	else//Member function
		storage = std::make_shared<storage::class_member>(*this);

	runtime::value_guard guard(runtime::object::current_storage, storage.get());
	try{
		copy_args_(context, args);
		evaluate_body_();
		if (return_declaration_ != nullptr)
			return_value = copy_return_value_(nullptr);
	}
	catch (const runtime::exception::return_interrupt &e){//Value returned
		if (return_declaration_ != nullptr)
			return_value = copy_return_value_(e.get_value());
		else
			throw exception::invalid_function_return();
	}

	return return_value;
}

void cminus::declaration::function::init_(std::shared_ptr<type::object> return_type){
	type_ = std::make_shared<type::function>(((flags_ & flags::const_) != 0u), return_type);
	if (return_type != nullptr && !return_type->is(type::object::query_type::undefined)){
		return_declaration_ = std::make_shared<variable>(
			"",											//Name
			return_type,								//Type
			attribute::collection::list_type{},			//Attributes
			flags::rval,								//Flags
			std::shared_ptr<memory::reference>()		//Initialization
		);
	}
}

int cminus::declaration::function::get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	return get_arg_score_((*it++)->get_type(), context);
}

void cminus::declaration::function::copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	auto  &param_name = (*it)->get_name();
	if (runtime::object::current_storage->exists(param_name, storage::object::entry_type::mem_ref))
		throw exception::bad_parameter_list();

	auto entry = (*it)->evaluate(0u, context);
	if (entry == nullptr)
		throw memory::exception::allocation_failure();

	runtime::object::current_storage->add_entry(param_name, entry, false);
	++it;
}

void cminus::declaration::function::copy_args_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	auto arg_it = args.begin();
	auto param_it = parameter_list_.begin();

	copy_context_(context, param_it);

	std::shared_ptr<variable> variadic_declaration;
	std::list<std::shared_ptr<memory::reference>> variadic_args;

	const std::string *param_name = nullptr;
	std::shared_ptr<memory::reference> param_ref;
	
	for (; arg_it != args.end(); ++arg_it){
		if (variadic_declaration == nullptr){
			if (param_it == parameter_list_.end())
				break;

			if (dynamic_cast<type::variadic *>((*param_it)->get_type()->get_non_proxy()) == nullptr){
				param_name = &(*param_it)->get_name();
				if (runtime::object::current_storage->exists(*param_name, storage::object::entry_type::mem_ref))
					throw exception::bad_parameter_list();

				if ((param_ref = (*param_it)->evaluate(0u, *arg_it)) != nullptr)
					runtime::object::current_storage->add_entry(*param_name, param_ref, false);
				else//Error
					throw memory::exception::allocation_failure();
			}
			else{//Variadic declaration
				variadic_declaration = *param_it;
				variadic_args.push_back(*arg_it);
			}

			++param_it;
		}
		else
			variadic_args.push_back(*arg_it);
	}

	if (arg_it != args.end())
		throw exception::bad_parameter_list();

	for (; param_it != parameter_list_.end(); ++param_it){//Evaluate parameters with default values
		if (variadic_declaration == nullptr){
			if (dynamic_cast<type::variadic *>((*param_it)->get_type()->get_non_proxy()) == nullptr){
				if ((*param_it)->get_initialization() != nullptr){
					param_name = &(*param_it)->get_name();
					if (runtime::object::current_storage->exists(*param_name, storage::object::entry_type::mem_ref))
						throw exception::bad_parameter_list();

					if ((param_ref = (*param_it)->evaluate(0u)) != nullptr)
						runtime::object::current_storage->add_entry(*param_name, param_ref, false);
					else//Error
						throw memory::exception::allocation_failure();
				}
				else//Error
					throw exception::bad_parameter_list();
			}
			else//Variadic declaration
				variadic_declaration = *param_it;

			++param_it;
		}
		else
			throw exception::bad_parameter_list();
	}

	if (variadic_declaration == nullptr)
		return;//No variadic declaration

	auto variadic_type = std::make_shared<type::in_memory_variadic>(
		dynamic_cast<type::variadic *>(variadic_declaration->get_type()->get_non_proxy())->get_base_type(),
		variadic_args.size()
	);

	param_name = &variadic_declaration->get_name();
	std::shared_ptr<memory::reference> empty_initialization;

	auto computed_variadic_declaration = std::make_shared<variable>(
		*param_name,
		variadic_type,
		variadic_declaration->get_attributes().get_list(),
		flags::nil,
		empty_initialization
	);

	if (runtime::object::current_storage->exists(*param_name, storage::object::entry_type::mem_ref))
		throw exception::bad_parameter_list();

	if ((param_ref = computed_variadic_declaration->evaluate(0u, variadic_args)) != nullptr)
		runtime::object::current_storage->add_entry(*param_name, param_ref, false);
	else//Error
		throw memory::exception::allocation_failure();
}

void cminus::declaration::function::evaluate_body_() const{
	if (auto definition = get_definition(); definition != nullptr)
		definition->evaluate();
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::copy_return_value_(std::shared_ptr<memory::reference> value) const{
	if (return_declaration_->get_type()->is(type::object::query_type::void_)){
		if (value != nullptr)
			throw exception::void_function_value_return();
		return nullptr;
	}

	if (value == nullptr)
		throw exception::value_function_no_return();

	return return_declaration_->evaluate(0u, value);
}

int cminus::declaration::function::get_arg_score_(std::shared_ptr<type::object> param_type, std::shared_ptr<memory::reference> arg) const{
	auto arg_type = arg->get_type();
	auto arg_class_type = dynamic_cast<type::class_ *>(arg_type.get());

	auto non_ref_const_param_type = param_type->convert(type::object::conversion_type::remove_ref_const, param_type);
	auto param_is_const = param_type->is(type::object::query_type::const_);
	auto param_is_ref = param_type->is(type::object::query_type::ref);

	auto arg_is_lval = arg->is_lvalue();
	auto arg_is_const = arg->is_const();

	if (arg_class_type != nullptr && arg_class_type->is_assignable_to(param_type)){
		auto score = type::object::get_score_value(type::object::score_result_type::class_compatible);
		return (score - ((param_is_const == arg_is_const && param_is_ref == arg_is_lval) ? 0 : 1));
	}

	int score;
	if (param_is_ref && !param_is_const && (!arg_is_lval || arg_is_const))//Reference parameter
		score = type::object::get_score_value(type::object::score_result_type::nil);
	else
		score = arg_type->get_score(*non_ref_const_param_type);

	if (score != type::object::get_score_value(type::object::score_result_type::nil))
		return (score - ((param_is_const == arg_is_const && param_is_ref == arg_is_lval) ? 0 : 1));
	
	if (param_type->convert(type::object::conversion_type::remove_ref, param_type)->is_constructible(arg))
		return type::object::get_score_value(type::object::score_result_type::class_compatible);

	return score;
}

std::size_t cminus::declaration::function::get_args_count_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	return ((context == nullptr) ? args.size() : (args.size() + 1u));
}

cminus::declaration::defined_function::~defined_function() = default;

void cminus::declaration::defined_function::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_function::get_definition() const{
	return definition_;
}

cminus::declaration::external_function::~external_function() = default;

void cminus::declaration::external_function::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_function::get_definition() const{
	return nullptr;
}

bool cminus::declaration::external_function::is_defined() const{
	return true;
}
