#include "initializer.h"

cminus::evaluator::initializer::~initializer() = default;

void cminus::evaluator::initializer::initialize(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const{
	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto cast_type = type::cast_type::static_rval;
	auto is_ref = target_type->is_ref();

	if (is_ref && !target_type->is_const()){//Non-constant reference target
		if (!value->is_lvalue())
			throw exception::rval_ref_assignment();

		if ((runtime::object::state & runtime::flags::ignore_const_ref) == 0u && value->is_const())
			throw exception::const_ref_assignment();

		cast_type = type::cast_type::static_ref;
	}
	else if (is_ref)
		cast_type = type::cast_type::static_const_ref;

	if (auto compatible_value = target_type->cast(value, target_type, cast_type); compatible_value != nullptr)
		target->initialize(compatible_value);
	else
		throw exception::incompatible_rval();
}
