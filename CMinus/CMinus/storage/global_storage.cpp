#include "../type/function_type.h"
#include "../type/pointer_type.h"
#include "../type/string_type.h"

#include "../evaluator/initializer.h"
#include "../evaluator/byte_evaluator.h"
#include "../evaluator/boolean_evaluator.h"

#include "../evaluator/number_evaluator.h"
#include "../evaluator/pointer_evaluator.h"

#include "global_storage.h"

cminus::storage::global::global()
	: named_object("", nullptr){
	runtime::object::global_storage = this;
	runtime::object::current_storage = this;

	default_initializer_ = std::make_shared<evaluator::initializer>();
	evaluators_[evaluator::object::id_type::byte] = std::make_shared<evaluator::byte>();
	evaluators_[evaluator::object::id_type::boolean] = std::make_shared<evaluator::boolean>();

	evaluators_[evaluator::object::id_type::number] = std::make_shared<evaluator::number>();
	evaluators_[evaluator::object::id_type::pointer] = std::make_shared<evaluator::pointer>();

	cached_types_[cached_type::undefined] = std::make_shared<type::undefined_primitive>();
	cached_types_[cached_type::void_] = std::make_shared<type::void_primitive>();

	cached_types_[cached_type::bool_] = std::make_shared<type::bool_primitive>();
	cached_types_[cached_type::byte_] = std::make_shared<type::byte_primitive>();

	cached_types_[cached_type::char_] = std::make_shared<type::char_primitive>();
	cached_types_[cached_type::wchar_] = std::make_shared<type::wchar_primitive>();

	cached_types_[cached_type::number] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::nil);
	cached_types_[cached_type::integer] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::integer);
	cached_types_[cached_type::long_integer] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::long_integer);

	cached_types_[cached_type::unsigned_integer] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::unsigned_integer);
	cached_types_[cached_type::unsigned_long_integer] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::unsigned_long_integer);

	cached_types_[cached_type::real] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::real);
	cached_types_[cached_type::long_real] = std::make_shared<type::number_primitive>(type::number_primitive::state_type::long_real);

	cached_types_[cached_type::function] = std::make_shared<type::function_primitive>();
	cached_types_[cached_type::nullptr_] = std::make_shared<type::pointer_primitive>(nullptr);

	boolean_values_[type::boolean_constant::false_] = std::make_shared<memory::scalar_reference<type::boolean_constant>>(
		cached_types_[cached_type::bool_],
		type::boolean_constant::false_
	);

	boolean_values_[type::boolean_constant::true_] = std::make_shared<memory::scalar_reference<type::boolean_constant>>(
		cached_types_[cached_type::bool_],
		type::boolean_constant::true_
	);

	boolean_values_[type::boolean_constant::indeterminate] = std::make_shared<memory::scalar_reference<type::boolean_constant>>(
		cached_types_[cached_type::bool_],
		type::boolean_constant::indeterminate
	);

	nullptr_value_ = std::make_shared<memory::rval_reference>(0u, cached_types_[cached_type::nullptr_]);
	undefined_value_ = std::make_shared<memory::rval_reference>(0u, cached_types_[cached_type::undefined]);

	cached_types_[cached_type::string] = std::make_shared<type::string>();
}

cminus::storage::global::~global(){
	destroy_entries_();
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_cached_type(cached_type type) const{
	if (auto it = cached_types_.find(type); it != cached_types_.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_cached_type(const type::object &type) const{
	for (auto &entry : cached_types_){
		if (entry.second.get() == &type)
			return entry.second;
	}

	return nullptr;
}

std::shared_ptr<cminus::evaluator::object> cminus::storage::global::get_evaluator(evaluator::object::id_type type) const{
	if (auto it = evaluators_.find(type); it != evaluators_.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<cminus::evaluator::initializer> cminus::storage::global::get_default_initializer() const{
	return default_initializer_;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::copy(std::shared_ptr<memory::reference> value, std::shared_ptr<type::object> target_type) const{
	if (target_type == nullptr)
		target_type = value->get_type();

	auto reference = std::make_shared<memory::rval_reference>(target_type);
	if (reference != nullptr)
		target_type->construct(reference, value);

	return reference;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_zero_value(const type::object &type) const{
	return get_zero_value(get_cached_type(type));
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_zero_value(std::shared_ptr<type::object> type) const{
	if (type->get_size() == 0u)
		return nullptr;

	auto reference = std::make_shared<memory::rval_reference>(type);
	if (reference != nullptr)
		reference->set(std::byte(0), type->get_size());

	return reference;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const std::string_view &value) const{
	return nullptr;
}

std::string_view cminus::storage::global::get_string_value(std::shared_ptr<memory::reference> value) const{
	return std::string_view();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_boolean_value(type::boolean_constant value) const{
	if (auto it = boolean_values_.find(value); it != boolean_values_.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_boolean_value(bool value) const{
	return get_boolean_value(value ? type::boolean_constant::true_ : type::boolean_constant::false_);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_nullptr_value() const{
	return nullptr_value_;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_undefined_value() const{
	return undefined_value_;
}
