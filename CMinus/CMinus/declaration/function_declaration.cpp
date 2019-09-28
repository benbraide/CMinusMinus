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

	if (auto & name = value->get_name(); !name.empty()){
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

	type_->add_parameter_type(value->get_type());
}

bool cminus::declaration::function::is_defined() const{
	return (get_definition() != nullptr);
}

int cminus::declaration::function::get_score(const std::list<std::shared_ptr<memory::reference>> &args) const{
	if (args.size() < min_arg_count_ || max_arg_count_ < args.size())
		return type::object::get_score_value(type::object::score_result_type::nil);

	if (args.empty())
		return type::object::get_score_value(type::object::score_result_type::exact);

	auto arg_it = args.begin();
	auto param_it = parameter_list_.begin();

	int lowest_rank_score = type::object::get_score_value(type::object::score_result_type::exact), current_rank_score;
	auto param_is_const = false, param_is_ref = false, arg_is_lval = false, arg_is_const = false;

	std::shared_ptr<type::object> variadic_base_type, param_type, non_ref_param_type, non_ref_const_param_type;
	for (; arg_it != args.end(); ++arg_it){
		if (variadic_base_type == nullptr){
			if (param_it == parameter_list_.end())
				break;

			param_type = (*param_it)->get_type();
			if (auto variadic_type = dynamic_cast<type::variadic *>(param_type->get_non_proxy()); variadic_type != nullptr)
				param_type = variadic_base_type = variadic_type->get_base_type();

			++param_it;
			non_ref_param_type = param_type->convert(type::object::conversion_type::remove_ref, param_type);
			non_ref_const_param_type = param_type->convert(type::object::conversion_type::remove_ref_const, param_type);

			param_is_const = param_type->is(type::object::query_type::const_);
			param_is_ref = param_type->is(type::object::query_type::ref);
		}

		arg_is_lval = (*arg_it)->is_lvalue();
		arg_is_const = (*arg_it)->is_const();

		if (param_is_ref){//Reference parameter
			if (!param_is_const && (!arg_is_lval || arg_is_const))
				current_rank_score = type::object::get_score_value(type::object::score_result_type::nil);
			else
				current_rank_score = (*arg_it)->get_type()->get_score(*non_ref_const_param_type);
		}
		else
			current_rank_score = (*arg_it)->get_type()->get_score(*non_ref_const_param_type);

		if (current_rank_score == type::object::get_score_value(type::object::score_result_type::nil)){
			if ((param_is_ref && !param_is_const) || !non_ref_param_type->is_constructible(*arg_it))
				return current_rank_score;
			current_rank_score = type::object::get_score_value(type::object::score_result_type::class_compatible);
		}

		if (current_rank_score < lowest_rank_score)
			lowest_rank_score = (current_rank_score - ((param_is_const == arg_is_const && param_is_ref == arg_is_lval) ? 0 : 1));
	}

	return ((arg_it == args.end()) ? lowest_rank_score : type::object::get_score_value(type::object::score_result_type::nil));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::call_(const std::list<std::shared_ptr<memory::reference>> &args) const{
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
		copy_args_(args);
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
	if (return_type != nullptr && !return_type->is(type::object::query_type::undefined)){
		return_declaration_ = std::make_shared<variable>(
			"",											//Name
			return_type,								//Type
			attribute::collection::list_type{},			//Attributes
			flags::nil,									//Flags
			nullptr										//Initialization
		);
	}

	type_ = std::make_shared<type::function>(return_type);
	if (auto class_parent = dynamic_cast<type::class_ *>(get_parent()); class_parent != nullptr){//Insert implicit parameter
		std::shared_ptr<type::object> implicit_type = std::make_shared<type::proxy>(*class_parent);
		if (!is(flags::static_)){//Non-static member -- Make implicit parameter a reference
			implicit_type = std::make_shared<type::ref>(implicit_type);
			if (is(flags::const_))//Constant function -- Make implicit parameter constant
				implicit_type = std::make_shared<type::constant>(implicit_type);
		}

		add_parameter(std::make_shared<variable>(
			"self",									//Name
			implicit_type,							//Type
			attribute::collection::list_type{},		//Attributes
			flags::nil,								//Flags
			nullptr									//Initialization
		));
	}
}

void cminus::declaration::function::copy_args_(const std::list<std::shared_ptr<memory::reference>> &args) const{
	auto arg_it = args.begin();
	auto param_it = parameter_list_.begin();

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
	auto computed_variadic_declaration = std::make_shared<variable>(
		*param_name,
		variadic_type,
		variadic_declaration->get_attributes().get_list(),
		flags::nil,
		nullptr
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
