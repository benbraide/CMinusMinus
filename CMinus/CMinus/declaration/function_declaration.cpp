#include "../type/variadic_type.h"

#include "function_declaration.h"

cminus::declaration::function::~function() = default;

cminus::storage::object *cminus::declaration::function::get_parent() const{
	return parent_;
}

std::shared_ptr<cminus::type::object> cminus::declaration::function::get_type() const{
	return type_;
}

void cminus::declaration::function::add_parameter(std::shared_ptr<object> value){
	auto variable_value = dynamic_cast<variable *>(value.get());
	if (variable_value == nullptr)
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
	auto has_initialization = (!is_variadic && variable_value->get_initialization() != nullptr);

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

int cminus::declaration::function::get_score(const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (args.size() < min_arg_count_ || max_arg_count_ < args.size())
		return type::object::get_score_value(type::object::score_result_type::nil);

	if (args.empty())
		return type::object::get_score_value(type::object::score_result_type::exact);

	auto arg_it = args.begin();
	auto param_it = parameter_list_.begin();

	int lowest_rank_score = type::object::get_score_value(type::object::score_result_type::exact), current_rank_score;
	bool param_is_const, arg_is_lval, arg_is_const;

	std::shared_ptr<type::object> variadic_base_type, param_type;
	for (; arg_it != args.end(); ++arg_it){
		if (variadic_base_type == nullptr){
			if (param_it == parameter_list_.end())
				break;

			param_type = (*param_it)->get_type();
			if (auto variadic_type = dynamic_cast<type::variadic *>(param_type->get_non_proxy()); variadic_type != nullptr)
				param_type = variadic_base_type = variadic_type->get_base_type();

			++param_it;
			param_is_const = param_type->is(type::object::query_type::const_);
		}
		else{
			param_type = variadic_base_type;
			param_is_const = param_type->is(type::object::query_type::const_);
		}

		arg_is_lval = (*arg_it)->is_lvalue();
		arg_is_const = (*arg_it)->is_const();

		if (param_type->is(type::object::query_type::ref)){//Reference parameter
			if (!param_is_const && (!arg_is_lval || arg_is_const))
				current_rank_score = type::object::get_score_value(type::object::score_result_type::nil);
			else
				current_rank_score = (*arg_it)->get_type()->get_score(*param_type);
		}
		else
			current_rank_score = (*arg_it)->get_type()->get_score(*param_type);

		if (current_rank_score == type::object::get_score_value(type::object::score_result_type::nil))
			return current_rank_score;

		if (current_rank_score < lowest_rank_score)
			lowest_rank_score = (current_rank_score - ((param_is_const == arg_is_const) ? 0 : 1));
	}

	return ((arg_it == args.end()) ? lowest_rank_score : type::object::get_score_value(type::object::score_result_type::nil));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::call_(const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (!is_defined())
		throw exception::function_not_defined();

	std::shared_ptr<memory::reference> return_value;
	//logic::storage::runtime_storage_guard guard(runtime, std::make_shared<logic::storage::function>(*this, context, dynamic_cast<logic::storage::object *>(parent_)));

	try{
		copy_args_( args);
		evaluate_body_();
		return_value = copy_return_value_(nullptr);
	}
	catch (const runtime::exception::return_interrupt &e){//Value returned
		return_value = copy_return_value_(e.get_value());
	}

	return return_value;
}

void cminus::declaration::function::copy_args_(const std::vector<std::shared_ptr<memory::reference>> &args) const{

}

void cminus::declaration::function::evaluate_body_() const{

}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function::copy_return_value_(std::shared_ptr<memory::reference> value) const{
	return nullptr;
}
