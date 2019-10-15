#include "initializer.h"

cminus::evaluator::initializer::~initializer() = default;

void cminus::evaluator::initializer::initialize(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const{
	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto is_ref = target_type->is(type::object::query_type::ref);
	if (is_ref && !target_type->is(type::object::query_type::const_)){//Non-constant reference target
		if (!value->is_lvalue())
			throw exception::rval_ref_assignment();

		if (target_type->is(type::object::query_type::const_))
			throw exception::const_ref_assignment();
	}

	if (auto compatible_value = target_type->cast(value, target_type, type::cast_type::rval_static); compatible_value != nullptr)
		target->initialize(compatible_value);
	else
		throw exception::incompatible_rval();
}
