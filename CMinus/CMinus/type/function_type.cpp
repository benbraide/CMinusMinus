#include "../storage/global_storage.h"

#include "function_type.h"

cminus::type::function::function(bool is_constant, std::shared_ptr<object> return_type)
	: primitive("", nullptr), is_constant_(is_constant), return_type_(return_type){
	compute_name_();
}

cminus::type::function::function(std::shared_ptr<object> return_type, const std::list<std::shared_ptr<object>> &parameter_types)
	: primitive("", nullptr), return_type_(return_type), parameter_types_(parameter_types){
	compute_name_();
}

cminus::type::function::function(std::shared_ptr<object> return_type, std::list<std::shared_ptr<object>> &&parameter_types)
	: primitive("", nullptr), return_type_(return_type), parameter_types_(std::move(parameter_types)){
	compute_name_();
}

cminus::type::function::~function() = default;

std::string cminus::type::function::get_qname() const{
	return qname_;
}

void cminus::type::function::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	throw runtime::exception::not_supported();
}

std::size_t cminus::type::function::get_size() const{
	return 0u;
}

bool cminus::type::function::is_exact(const object &target) const{
	if (primitive::is_exact(target))
		return true;

	auto function_target = target.as<function>(false);
	return (function_target != nullptr && is_exact_return_type(*function_target) && is_exact_parameter_types(*function_target));
}

std::shared_ptr<cminus::memory::reference> cminus::type::function::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if (auto result = primitive::cast(data, target_type, type); result != nullptr)
		return result;

	if (type != cast_type::reinterpret)
		return nullptr;

	auto number_target_type = target_type->as<number_primitive>();
	if (number_target_type == nullptr)
		return nullptr;

	switch (number_target_type->get_state()){
	case number_primitive::state_type::small_integer:
		return std::make_shared<memory::scalar_reference<__int16>>(
			storage::get_cached_type<__int16>::type(),
			static_cast<__int16>(data->read_scalar<std::size_t>())
		);
	case number_primitive::state_type::integer:
		return std::make_shared<memory::scalar_reference<__int32>>(
			storage::get_cached_type<__int32>::type(),
			static_cast<__int32>(data->read_scalar<std::size_t>())
		);
	case number_primitive::state_type::big_integer:
		return std::make_shared<memory::scalar_reference<__int64>>(
			storage::get_cached_type<__int64>::type(),
			static_cast<__int64>(data->read_scalar<std::size_t>())
		);
	case number_primitive::state_type::unsigned_small_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int16>>(
			storage::get_cached_type<unsigned __int16>::type(),
			static_cast<unsigned __int16>(data->read_scalar<std::size_t>())
		);
	case number_primitive::state_type::unsigned_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int32>>(
			storage::get_cached_type<unsigned __int32>::type(),
			static_cast<unsigned __int32>(data->read_scalar<std::size_t>())
		);
	case number_primitive::state_type::unsigned_big_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int64>>(
			storage::get_cached_type<unsigned __int64>::type(),
			static_cast<unsigned __int64>(data->read_scalar<std::size_t>())
		);
	default:
		break;
	}

	return nullptr;
}

void cminus::type::function::add_parameter_type(std::shared_ptr<object> value){
	parameter_types_.push_back(value);
	name_.insert(name_.find_last_of(')'), ("," + value->get_name()));
	qname_.insert(qname_.find_last_of(')'), ("," + value->get_name()));
}

bool cminus::type::function::is_exact_parameter_types(const function &target) const{
	if (is_constant_ != target.is_constant_ || parameter_types_.size() != target.parameter_types_.size())
		return false;

	for (auto it = parameter_types_.begin(), target_it = target.parameter_types_.begin(); it != parameter_types_.end(); ++it, ++target_it){
		if (!(*it)->is_exact(**target_it))
			return false;
	}

	return true;
}

bool cminus::type::function::is_exact_return_type(const function &function_target) const{
	if (return_type_ != nullptr && return_type_->is<function_return_primitive>())
		return true;

	if (function_target.return_type_ != nullptr && function_target.return_type_->is<function_return_primitive>())
		return true;

	if (return_type_ == nullptr)
		return (function_target.return_type_ == nullptr || function_target.return_type_->is<undefined_primitive>());

	if (function_target.return_type_ == nullptr)
		return (return_type_ == nullptr || return_type_->is<undefined_primitive>());

	return return_type_->is_exact(*function_target.return_type_);
}

void cminus::type::function::compute_name_(){
	std::string parameters_name, parameters_qname;

	auto it = parameter_types_.begin();
	if (it != parameter_types_.end()){
		parameters_name = (*it)->get_name();
		parameters_qname = (*it)->get_qname();

		for (++it; it != parameter_types_.end(); ++it){
			parameters_name += ("," + (*it)->get_name());
			parameters_qname += ("," + (*it)->get_qname());
		}
	}

	name_ = (((return_type_ == nullptr) ? "UndefinedType" : return_type_->get_name()) + "(" + parameters_name + (is_constant_ ? ")Const" : ")"));
	qname_ = (((return_type_ == nullptr) ? "UndefinedType" : return_type_->get_qname()) + "(" + parameters_qname + (is_constant_ ? ")Const" : ")"));
}
