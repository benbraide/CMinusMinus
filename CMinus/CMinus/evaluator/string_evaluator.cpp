#include "../storage/global_storage.h"
#include "../declaration/string_function_definitions.h"

#include "string_evaluator.h"

cminus::evaluator::string::~string() = default;

cminus::evaluator::object::id_type cminus::evaluator::string::get_id() const{
	return id_type::string;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::string::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::string::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::string::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	auto is_relational = (op == operators::id::less || op == operators::id::less_or_equal || op == operators::id::equal ||
		op == operators::id::not_equal || op == operators::id::greater_or_equal || op == operators::id::greater || op == operators::id::spaceship);

	if (!is_relational && op != operators::id::index && op != operators::id::assignment && op != operators::id::compound_plus && op != operators::id::plus)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	if (op == operators::id::index){
		auto right_number_type = right_type->as<type::number_primitive>();
		if (right_number_type == nullptr || !right_number_type->is_integral())
			throw exception::unsupported_op();

		auto index = right_number_type->read_value<std::size_t>(right_value);
		declaration::string::helper::data_address_size_value_info info{};
		declaration::string::helper::retrieve_info(info, left_value);

		if (info.size <= index)
			throw runtime::exception::out_of_range();

		return std::make_shared<memory::reference>(
			(info.data + index),
			runtime::object::global_storage->get_char_ref_type(left_value->is_const()),
			nullptr
		);
	}

	std::string right_str;
	std::string_view temp_right_str;

	auto compatible_value = right_type->cast(right_value, left_type->remove_const_ref(left_type), type::cast_type::static_rval);
	if (compatible_value == nullptr){//Try character
		if (op != operators::id::compound_plus && op != operators::id::plus)
			throw exception::unsupported_op();

		if (auto char_value = right_type->cast(right_value, runtime::object::global_storage->get_char_type(), type::cast_type::static_rval); char_value != nullptr){
			right_str.assign(1u, char_value->read_scalar<char>());
			temp_right_str = right_str;
		}
		else
			throw exception::unsupported_op();
	}
	else
		temp_right_str = runtime::object::global_storage->get_string_value(compatible_value);

	if (op == operators::id::assignment || op == operators::id::compound_plus){
		if (left_value->is_const())
			throw exception::const_assignment();

		if (op == operators::id::assignment){
			declaration::string::helper::assign(
				temp_right_str.data(),
				temp_right_str.size(),
				false,
				false,
				left_value
			);
		}
		else{//Compound assignment
			declaration::string::helper::insert(
				temp_right_str.data(),
				temp_right_str.size(),
				declaration::string::helper::read_value<std::size_t>("size_", left_value),
				false,
				left_value
			);
		}

		return left_value;
	}

	if (op == operators::id::plus){
		return runtime::object::global_storage->create_string(
			runtime::object::global_storage->get_string_value(left_value),
			temp_right_str,
			false
		);
	}

	auto result = runtime::object::global_storage->get_string_value(left_value).compare(temp_right_str);
	switch (op){
	case operators::id::less:
		return runtime::object::global_storage->get_boolean_value(result < 0);
	case operators::id::less_or_equal:
		return runtime::object::global_storage->get_boolean_value(result <= 0);
	case operators::id::equal:
		return runtime::object::global_storage->get_boolean_value(result == 0);
	case operators::id::not_equal:
		return runtime::object::global_storage->get_boolean_value(result != 0);
	case operators::id::greater_or_equal:
		return runtime::object::global_storage->get_boolean_value(result >= 0);
	case operators::id::greater:
		return runtime::object::global_storage->get_boolean_value(result > 0);
	case operators::id::spaceship:
		return runtime::object::global_storage->get_compare_value(result);
	default:
		break;
	}

	throw exception::unsupported_op();
	return nullptr;
}
