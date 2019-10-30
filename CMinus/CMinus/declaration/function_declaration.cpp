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

	auto is_variadic = value->get_type()->is<type::variadic>();
	auto has_initialization = (!is_variadic && value->get_initialization() != nullptr);

	if (max_arg_count_ == static_cast<std::size_t>(-1) || (min_arg_count_ != max_arg_count_ && !is_variadic && !has_initialization))
		throw exception::bad_parameter_list();

	++max_arg_count_;
	if (is_variadic)
		max_arg_count_ = static_cast<std::size_t>(-1);
	else if (!has_initialization)
		++min_arg_count_;

	parameter_list_.push_back(value);
	dynamic_cast<type::function *>(type_.get())->add_parameter_type(value->get_type());
}

bool cminus::declaration::function::is_defined() const{
	return (get_definition() != nullptr);
}

int cminus::declaration::function::get_score(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	auto param_it = parameter_list_.begin();
	auto lowest_rank_score = get_meta_score_(context, args.size(), param_it);

	if (lowest_rank_score == type::object::get_score_value(type::object::score_result_type::nil))
		return lowest_rank_score;

	return get_args_score_(args, param_it, lowest_rank_score, required_size);
}

int cminus::declaration::function::get_score(std::shared_ptr<memory::reference> context, const std::vector<arg_info> &args, std::size_t required_size) const{
	auto param_it = parameter_list_.begin();
	auto lowest_rank_score = get_meta_score_(context, args.size(), param_it);

	if (lowest_rank_score == type::object::get_score_value(type::object::score_result_type::nil))
		return lowest_rank_score;

	return get_args_score_(args, param_it, lowest_rank_score, required_size);
}

int cminus::declaration::function::get_args_score(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	return get_args_score_(args, parameter_list_.begin(), type::object::get_score_value(type::object::score_result_type::exact), required_size);
}

int cminus::declaration::function::get_args_score(const std::vector<arg_info> &args, std::size_t required_size) const{
	return get_args_score_(args, parameter_list_.begin(), type::object::get_score_value(type::object::score_result_type::exact), required_size);
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::call_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
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
		copy_args_(context, args, required_size);
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
	if (return_type != nullptr && !return_type->is<type::undefined_primitive>()){
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
	if (runtime::object::current_storage->exists((*it)->get_name()))
		throw exception::bad_parameter_list();

	auto entry = (*it)->evaluate(0u, context);
	if (entry == nullptr)
		throw memory::exception::allocation_failure();

	runtime::object::current_storage->add_entry(*it, entry, false);
	++it;
}

void cminus::declaration::function::copy_args_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	auto arg_it = args.begin();
	auto param_it = parameter_list_.begin();

	copy_context_(context, param_it);

	std::shared_ptr<variable> variadic_declaration;
	std::list<std::shared_ptr<memory::reference>> variadic_args;

	std::shared_ptr<memory::reference> param_ref;	
	for (; arg_it != args.end(); ++arg_it){
		if (variadic_declaration == nullptr){
			if (param_it == parameter_list_.end())
				break;

			if (!(*param_it)->get_type()->is<type::variadic>()){
				if (runtime::object::current_storage->exists((*param_it)->get_name()))
					throw exception::bad_parameter_list();

				if ((param_ref = (*param_it)->evaluate(0u, *arg_it)) != nullptr)
					runtime::object::current_storage->add_entry(*param_it, param_ref, false);
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

	if (arg_it != args.end() && std::distance(args.begin(), arg_it) < required_size)
		throw exception::bad_parameter_list();

	for (; param_it != parameter_list_.end(); ++param_it){//Evaluate parameters with default values
		if (variadic_declaration == nullptr){
			if (!(*param_it)->get_type()->is<type::variadic>()){
				if ((*param_it)->get_initialization() != nullptr){
					if (runtime::object::current_storage->exists((*param_it)->get_name()))
						throw exception::bad_parameter_list();

					if ((param_ref = (*param_it)->evaluate(0u)) != nullptr)
						runtime::object::current_storage->add_entry(*param_it, param_ref, false);
					else//Error
						throw memory::exception::allocation_failure();
				}
				else//Error
					throw exception::bad_parameter_list();
			}
			else//Variadic declaration
				variadic_declaration = *param_it;
		}
		else
			throw exception::bad_parameter_list();
	}

	if (variadic_declaration == nullptr)
		return;//No variadic declaration

	if (runtime::object::current_storage->exists(variadic_declaration->get_name()))
		throw exception::bad_parameter_list();

	auto variadic_type = std::make_shared<type::in_memory_variadic>(
		variadic_declaration->get_type()->as<type::variadic>()->get_base_type(),
		variadic_args.size()
	);

	auto computed_variadic_declaration = std::make_shared<variable>(
		variadic_declaration->get_name(),
		variadic_type,
		variadic_declaration->get_attributes().get_list(),
		flags::nil,
		std::shared_ptr<node::object>()
	);

	if ((param_ref = computed_variadic_declaration->evaluate(0u, variadic_args)) != nullptr)
		runtime::object::current_storage->add_entry(variadic_declaration, param_ref, false);
	else//Error
		throw memory::exception::allocation_failure();
}

void cminus::declaration::function::evaluate_body_() const{
	if (auto definition = get_definition(); definition != nullptr)
		definition->evaluate();
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::copy_return_value_(std::shared_ptr<memory::reference> value) const{
	if (return_declaration_->get_type()->is<type::void_primitive>()){
		if (value != nullptr)
			throw exception::void_function_value_return();
		return nullptr;
	}

	if (value == nullptr)
		throw exception::value_function_no_return();

	return return_declaration_->evaluate(0u, value);
}

int cminus::declaration::function::get_meta_score_(std::shared_ptr<memory::reference> context, std::size_t args_count, parameter_list_type::const_iterator &it) const{
	args_count = get_args_count_(context, args_count);
	if (args_count < min_arg_count_ || max_arg_count_ < args_count)
		return type::object::get_score_value(type::object::score_result_type::nil);
	return get_context_score_(context, it);
}

int cminus::declaration::function::get_meta_score_(std::size_t args_count) const{
	if (args_count < min_arg_count_ || max_arg_count_ < args_count)
		return type::object::get_score_value(type::object::score_result_type::nil);
	return type::object::get_score_value(type::object::score_result_type::exact);
}

int cminus::declaration::function::get_arg_score_(std::shared_ptr<type::object> param_type, const arg_info &info) const{
	return info.type->remove_const_ref()->get_score(*param_type, info.is_lval, info.is_const);
}

int cminus::declaration::function::get_arg_score_(std::shared_ptr<type::object> param_type, std::shared_ptr<memory::reference> arg) const{
	auto arg_type = arg->get_type();
	return get_arg_score_(param_type, arg_info{
		arg_type->remove_const_ref(),
		arg->is_lvalue(),
		arg->is_const()
	});
}

std::size_t cminus::declaration::function::get_args_count_(std::shared_ptr<memory::reference> context, std::size_t args_count) const{
	return ((context == nullptr) ? args_count : (args_count + 1u));
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
