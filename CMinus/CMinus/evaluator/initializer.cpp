#include "initializer.h"

cminus::evaluator::initializer::~initializer() = default;

void cminus::evaluator::initializer::initialize(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const{
	auto target_type = target->get_type(), value_type = value->get_type();
	if (target_type == nullptr || value_type == nullptr)
		throw exception::invalid_type();

	attribute::read_guard guard(value, nullptr);
	auto compatible_value = value_type->cast(value, target_type, type::cast_type::rval_static);

	if (compatible_value == nullptr)
		throw exception::incompatible_rval();

	if (target_type->is(type::object::query_type::indirect))
		target->write_ownership(compatible_value);
	else//Copy value
		target->write(*compatible_value, target_type->get_size());

	target_type->convert(type::object::conversion_type::update, value_type);
}
