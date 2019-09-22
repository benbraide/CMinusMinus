#include "primitive_type.h"

#include "function_type.h"

cminus::type::function::function(std::shared_ptr<object> return_type, const std::vector<std::shared_ptr<object>> &parameter_types)
	: object("", nullptr), return_type_(return_type), parameter_types_(parameter_types){
	compute_name_();
}

cminus::type::function::function(std::shared_ptr<object> return_type, std::vector<std::shared_ptr<object>> &&parameter_types)
	: object("", nullptr), return_type_(return_type), parameter_types_(std::move(parameter_types)){
	compute_name_();
}

cminus::type::function::~function() = default;

std::size_t cminus::type::function::get_size() const{
	return 0u;
}

bool cminus::type::function::is_exact(const object &target) const{
	auto function_target = dynamic_cast<function *>(target.get_non_proxy());
	return (function_target != nullptr && is_exact_return_type_(*function_target) && is_exact_parameter_types_(*function_target));
}

int cminus::type::function::get_score(const object &target) const{
	return get_score_value(is_exact(target) ? score_result_type::exact : score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::function::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto function_target_type = dynamic_cast<function *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (function_target_type == nullptr){
		if (type != cast_type::rval_static && type != cast_type::static_)
			return ((type == cast_type::reinterpret && !target_type->is(query_type::ref)) ? primitive::convert_value_to_number(data->read_scalar<std::size_t>(), target_type) : nullptr);

		if (!target_type->is(query_type::generic_function))
			return nullptr;

		return ((type == cast_type::rval_static) ? data : std::make_shared<memory::function_reference>(data->get_address(), target_type, data->get_context()));
	}

	return (((type == cast_type::rval_static || type == cast_type::static_) && is_exact(*function_target_type)) ? data : nullptr);
}

bool cminus::type::function::is(query_type type, const object *arg) const{
	if (type == query_type::exact_parameter_types){
		if (auto function_arg = dynamic_cast<const function *>(arg); function_arg != nullptr)
			return is_exact_parameter_types_(*function_arg);
		return false;
	}

	if (type == query_type::exact_return_type){
		if (auto function_arg = dynamic_cast<const function *>(arg); function_arg != nullptr)
			return is_exact_return_type_(*function_arg);
		return false;
	}

	return (type == query_type::function || object::is(type, arg));
}

void cminus::type::function::compute_name_(){
	std::string parameters_name;

	auto it = parameter_types_.begin();
	if (it != parameter_types_.end())
		parameters_name = (*it)->get_name();

	for (++it; it != parameter_types_.end(); ++it)
		parameters_name += (", " + (*it)->get_name());

	name_ = (((return_type_ == nullptr) ? "UndefinedType" : return_type_->get_name()) + "(" + parameters_name + ")");
}

bool cminus::type::function::is_exact_return_type_(const function &function_target) const{
	if (return_type_ == nullptr)
		return (function_target.return_type_ == nullptr || function_target.return_type_->is(query_type::undefined));
	
	if (function_target.return_type_ == nullptr)
		return (return_type_ == nullptr || return_type_->is(query_type::undefined));

	return return_type_->is_exact(*function_target.return_type_);
}

bool cminus::type::function::is_exact_parameter_types_(const function &function_target) const{
	if (parameter_types_.size() != function_target.parameter_types_.size())
		return false;

	for (auto it = parameter_types_.begin(), target_it = function_target.parameter_types_.begin(); it != parameter_types_.end(); ++it, ++target_it){
		if (!(*it)->is_exact(**target_it))
			return false;
	}

	return false;
}
