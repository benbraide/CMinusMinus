#include "../type/function_type.h"
#include "../type/pointer_type.h"
#include "../type/string_type.h"
#include "../type/system_enum_types.h"

#include "../evaluator/initializer.h"
#include "../evaluator/byte_evaluator.h"
#include "../evaluator/boolean_evaluator.h"

#include "../evaluator/number_evaluator.h"
#include "../evaluator/pointer_evaluator.h"

#include "../evaluator/enum_evaluator.h"
#include "../evaluator/string_evaluator.h"

#include "../declaration/string_function_definitions.h"

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

	evaluators_[evaluator::object::id_type::enum_] = std::make_shared<evaluator::enum_>();
	evaluators_[evaluator::object::id_type::string] = std::make_shared<evaluator::string>();

	cached_types_[cached_type::undefined] = std::make_shared<type::undefined_primitive>();
	cached_types_[cached_type::auto_] = std::make_shared<type::auto_primitive>();
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

	cached_types_[cached_type::compare_result] = std::make_shared<type::compare_result>();
	cached_types_[cached_type::attribute_stage] = std::make_shared<type::attribute_stage>();
	cached_types_[cached_type::attribute_result] = std::make_shared<type::attribute_result>();

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

std::shared_ptr<cminus::type::object> cminus::storage::global::get_pointer_type(std::shared_ptr<type::object> base_type, bool is_const) const{
	if (!is_const)
		return std::make_shared<cminus::type::pointer_primitive>(base_type);
	return std::make_shared<cminus::type::pointer_primitive>(std::make_shared<cminus::type::constant>(base_type));
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_ref_type(std::shared_ptr<type::object> base_type, bool is_const) const{
	if (!is_const)
		return std::make_shared<cminus::type::ref>(base_type);
	return std::make_shared<cminus::type::constant>(std::make_shared<cminus::type::ref>(base_type));
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_auto_pointer_type(bool is_const) const{
	return get_pointer_type(get_cached_type(cached_type::auto_), is_const);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_auto_ref_type(bool is_const) const{
	return get_ref_type(get_cached_type(cached_type::auto_), is_const);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_char_pointer_type(bool is_const) const{
	return get_pointer_type(get_cached_type(cached_type::char_), is_const);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_char_ref_type(bool is_const) const{
	return get_ref_type(get_cached_type(cached_type::char_), is_const);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_string_ref_type(bool is_const) const{
	return get_ref_type(get_cached_type(cached_type::string), is_const);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_auto_type() const{
	return get_cached_type(cached_type::auto_);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_void_type() const{
	return get_cached_type(cached_type::void_);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_boolean_type() const{
	return get_cached_type(cached_type::bool_);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_char_type() const{
	return get_cached_type(cached_type::char_);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_int_type() const{
	return get_cached_type(cached_type::integer);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_size_type() const{
	return get_cached_type(cached_type::unsigned_long_integer);
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_string_type() const{
	return get_cached_type(cached_type::string);
}

cminus::type::string *cminus::storage::global::get_raw_string_type() const{
	return dynamic_cast<type::string *>(get_string_type().get());
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
		runtime::object::memory_object->set(reference->get_address(), std::byte(0), type->get_size());

	return reference;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const char *value, bool lval) const{
	return create_string(std::string_view(value), lval);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const std::string &value, bool lval) const{
	return create_string(std::string_view(value.data(), value.size()), lval);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const std::string_view &value, bool lval) const{
	return create_string(value, std::string_view(), lval);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const std::string_view &first, const std::string_view &second, bool lval) const{
	std::shared_ptr<memory::reference> ref;
	if (lval)
		ref = std::make_shared<memory::reference>(get_string_type(), nullptr);
	else//Allocate write protected memory
		ref = std::make_shared<memory::write_protected_rval_reference>(get_string_type());

	if (ref == nullptr)
		throw memory::exception::allocation_failure();

	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
	auto data_address = declaration::string::helper::allocate_block((first.size() + second.size()), declaration::string::helper::allocation_type::nil, 0u, ref, !lval);

	runtime::object::memory_object->write_buffer(data_address, first.data(), first.size());
	if (!second.empty())
		runtime::object::memory_object->write_buffer((data_address + first.size()), second.data(), second.size());

	ref->set_constructed_state();
	return ref;
}

std::string_view cminus::storage::global::get_string_value(std::shared_ptr<memory::reference> value) const{
	return declaration::string::helper::read_data("data_", value);
}

std::size_t cminus::storage::global::read_enum_value(std::shared_ptr<memory::reference> value) const{
	switch (value->get_type()->get_size()){
	case sizeof(unsigned __int8):
		return static_cast<std::size_t>(value->read_scalar<unsigned __int8>());
	case sizeof(unsigned __int16):
		return static_cast<std::size_t>(value->read_scalar<unsigned __int16>());
	case sizeof(unsigned __int32):
		return static_cast<std::size_t>(value->read_scalar<unsigned __int32>());
	case sizeof(unsigned __int64):
		return static_cast<std::size_t>(value->read_scalar<unsigned __int64>());
	default:
		break;
	}

	return static_cast<std::size_t>(-1);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_enum_value(cached_type type, std::size_t value) const{
	switch (auto target_type = get_cached_type(type); ((target_type == nullptr) ? 0u : target_type->get_size())){
	case sizeof(unsigned __int8):
		return std::make_shared<cminus::memory::scalar_reference<unsigned __int8>>(target_type, static_cast<unsigned __int8>(value));
	case sizeof(unsigned __int16):
		return std::make_shared<cminus::memory::scalar_reference<unsigned __int16>>(target_type, static_cast<unsigned __int16>(value));
	case sizeof(unsigned __int32):
		return std::make_shared<cminus::memory::scalar_reference<unsigned __int32>>(target_type, static_cast<unsigned __int32>(value));
	case sizeof(unsigned __int64):
		return std::make_shared<cminus::memory::scalar_reference<unsigned __int64>>(target_type, static_cast<unsigned __int64>(value));
	default:
		break;
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_compare_value(int value) const{
	if (value < 0)
		value = 1;
	else if (value == 0)
		value = 2;
	else
		value = 3;

	return get_enum_value(cached_type::compare_result, value);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_not_equal_compare_value() const{
	return get_enum_value(cached_type::compare_result, 0);
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

bool cminus::storage::global::compare_enum(const type::object &type, std::shared_ptr<memory::reference> left, std::shared_ptr<memory::reference> right) const{
	switch (type.get_size()){
	case sizeof(unsigned __int8):
		return (left->read_scalar<unsigned __int8>() == right->read_scalar<unsigned __int8>());
	case sizeof(unsigned __int16):
		return (left->read_scalar<unsigned __int16>() == right->read_scalar<unsigned __int16>());
	case sizeof(unsigned __int32):
		return (left->read_scalar<unsigned __int32>() == right->read_scalar<unsigned __int32>());
	case sizeof(unsigned __int64):
		return (left->read_scalar<unsigned __int64>() == right->read_scalar<unsigned __int64>());
	default:
		break;
	}

	return false;
}
