#include "../storage/global_storage.h"

#include "proxy_type.h"
#include "modified_type.h"

#include "pointer_type.h"
#include "function_type.h"

#include "string_type.h"
#include "primitive_type.h"

cminus::type::primitive::primitive(const std::string &name)
	: primitive(name, nullptr){}

cminus::type::primitive::primitive(const std::string &name, storage::object *parent)
	: object(name, parent){}

cminus::type::primitive::~primitive() = default;

std::shared_ptr<cminus::evaluator::object> cminus::type::primitive::get_evaluator() const{
	return runtime::object::global_storage->get_evaluator(get_evaluator_id());
}

cminus::evaluator::object::id_type cminus::type::primitive::get_evaluator_id() const{
	return evaluator::object::id_type::nil;
}

std::shared_ptr<cminus::memory::reference> cminus::type::primitive::cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto data_is_lval = data->is_lvalue();
	if (!is_valid_static_cast(type, data_is_lval, data->is_const()))
		return nullptr;

	if (!is_exact(*target_type->remove_const_ref()))
		return nullptr;

	return ((!data_is_lval || type != cast_type::static_) ? data : copy_data(data, target_type));
}

cminus::type::no_storage_primitive::~no_storage_primitive() = default;

std::shared_ptr<cminus::memory::reference> cminus::type::no_storage_primitive::get_default_value() const{
	return nullptr;
}

void cminus::type::no_storage_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	throw runtime::exception::not_supported();
}

std::size_t cminus::type::no_storage_primitive::get_size() const{
	return 0u;
}

std::shared_ptr<cminus::memory::reference> cminus::type::no_storage_primitive::cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if ((type != cast_type::static_ && type != cast_type::static_rval) || !is_exact(*target_type->remove_const_ref()))
		return nullptr;

	return ((type == cast_type::static_rval) ? data : std::make_shared<memory::rval_reference>(0u, target_type->remove_const_ref(target_type)));
}

cminus::type::undefined_primitive::undefined_primitive()
	: no_storage_primitive("UndefinedType"){}

cminus::type::undefined_primitive::~undefined_primitive() = default;

void cminus::type::undefined_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	writer.write_buffer("Undefined");
}

cminus::evaluator::object::id_type cminus::type::undefined_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::undefined;
}

bool cminus::type::undefined_primitive::is_exact_(const object &target) const{
	return target.is<undefined_primitive>(false);
}

cminus::type::void_primitive::void_primitive()
	: no_storage_primitive("Void"){}

cminus::type::void_primitive::~void_primitive() = default;

bool cminus::type::void_primitive::is_exact_(const object &target) const{
	return target.is<void_primitive>(false);
}

cminus::type::boolean_primitive::boolean_primitive()
	: primitive("Boolean"){}

cminus::type::boolean_primitive::~boolean_primitive() = default;

void cminus::type::boolean_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	switch (data->read_scalar<boolean_constant>()){
	case boolean_constant::false_:
		writer.write_buffer("false");
		break;
	case boolean_constant::true_:
		writer.write_buffer("true");
		break;
	default:
		writer.write_buffer("indeterminate");
		break;
	}
}

std::size_t cminus::type::boolean_primitive::get_size() const{
	return sizeof(boolean_constant);
}

cminus::evaluator::object::id_type cminus::type::boolean_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::boolean;
}

bool cminus::type::boolean_primitive::is_exact_(const object &target) const{
	return target.is<boolean_primitive>(false);
}

cminus::type::byte_primitive::byte_primitive()
	: primitive("Byte"){}

cminus::type::byte_primitive::~byte_primitive() = default;

void cminus::type::byte_primitive::print_value(io::writer & writer, std::shared_ptr<memory::reference> data) const{
	auto str = runtime::to_hex_string<int>::get(static_cast<int>(data->read_scalar<std::byte>()), 2);
	writer.write_buffer(str.data(), str.size());
}

std::size_t cminus::type::byte_primitive::get_size() const{
	return sizeof(std::byte);
}

cminus::evaluator::object::id_type cminus::type::byte_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::byte;
}

bool cminus::type::byte_primitive::is_exact_(const object &target) const{
	return target.is<byte_primitive>(false);
}

cminus::type::char_primitive::char_primitive()
	: primitive("Char"){}

cminus::type::char_primitive::~char_primitive() = default;

void cminus::type::char_primitive::print_value(io::writer & writer, std::shared_ptr<memory::reference> data) const{
	writer.write_scalar(data->read_scalar<char>());
}

std::size_t cminus::type::char_primitive::get_size() const{
	return sizeof(char);
}

cminus::evaluator::object::id_type cminus::type::char_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::byte;
}

bool cminus::type::char_primitive::is_exact_(const object &target) const{
	return target.is<char_primitive>(false);
}

cminus::type::wchar_primitive::wchar_primitive()
	: primitive("WideChar"){}

cminus::type::wchar_primitive::~wchar_primitive() = default;

void cminus::type::wchar_primitive::print_value(io::writer & writer, std::shared_ptr<memory::reference> data) const{
	writer.write_scalar(data->read_scalar<wchar_t>());
}

std::size_t cminus::type::wchar_primitive::get_size() const{
	return sizeof(wchar_t);
}

cminus::evaluator::object::id_type cminus::type::wchar_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::byte;
}

bool cminus::type::wchar_primitive::is_exact_(const object &target) const{
	return target.is<wchar_primitive>(false);
}

cminus::type::number_primitive::number_primitive(state_type state)
	: primitive(""), state_(state){
	switch (state_){
	case state_type::small_integer:
		name_ = "SmallInteger";
		size_ = sizeof(__int16);
		break;
	case state_type::integer:
		name_ = "Integer";
		size_ = sizeof(__int32);
		break;
	case state_type::big_integer:
		name_ = "BigInteger";
		size_ = sizeof(__int64);
		break;
	case state_type::unsigned_small_integer:
		name_ = "UnsignedSmallInteger";
		size_ = sizeof(unsigned __int16);
		break;
	case state_type::unsigned_integer:
		name_ = "UnsignedInteger";
		size_ = sizeof(unsigned __int32);
		break;
	case state_type::unsigned_big_integer:
		name_ = "UnsignedBigInteger";
		size_ = sizeof(unsigned __int64);
		break;
	case state_type::small_float:
		name_ = "SmallFloat";
		size_ = sizeof(float);
		break;
	case state_type::float_:
		name_ = "Float";
		size_ = sizeof(double);
		break;
	case state_type::big_float:
		name_ = "BigFloat";
		size_ = sizeof(long double);
		break;
	case state_type::small_number:
		name_ = "SmallNumber";
		size_ = 0u;
		break;
	case state_type::big_number:
		name_ = "BigNumber";
		size_ = 0u;
		break;
	default:
		name_ = "Number";
		size_ = 0u;
		break;
	}
}

cminus::type::number_primitive::~number_primitive() = default;

std::shared_ptr<cminus::memory::reference> cminus::type::number_primitive::get_default_value() const{
	switch (state_){
	case state_type::small_number:
		return std::make_shared<memory::scalar_reference<__int16>>(storage::get_cached_type<__int16>::type(), type::get_nan<__int16>::value());
	case state_type::number:
		return std::make_shared<memory::scalar_reference<__int32>>(storage::get_cached_type<__int32>::type(), type::get_nan<__int32>::value());
	case state_type::big_number:
		return std::make_shared<memory::scalar_reference<__int64>>(storage::get_cached_type<__int64>::type(), type::get_nan<__int64>::value());
	default:
		break;
	}

	return primitive::get_default_value();
}

void cminus::type::number_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	switch (state_){
	case state_type::small_integer:
		if (is_nan_<__int16>(*data))
			writer.write_buffer("NaN");
		else
			writer.write_scalar(data->read_scalar<__int16>());
		break;
	case state_type::integer:
		if (is_nan_<__int32>(*data))
			writer.write_buffer("NaN");
		else
			writer.write_scalar(data->read_scalar<__int32>());
		break;
	case state_type::big_integer:
		if (!is_nan_<__int64>(*data)){
			writer.write_scalar(data->read_scalar<__int64>());
			writer.write_scalar('L');
		}
		else
			writer.write_buffer("NaN");
		break;
	case state_type::unsigned_small_integer:
		if (!is_nan_<unsigned __int16>(*data)){
			writer.write_scalar(data->read_scalar<unsigned __int16>());
			writer.write_buffer("U");
		}
		else
			writer.write_buffer("NaN");
		break;
	case state_type::unsigned_integer:
		if (!is_nan_<unsigned __int32>(*data)){
			writer.write_scalar(data->read_scalar<unsigned __int32>());
			writer.write_buffer("U");
		}
		else
			writer.write_buffer("NaN");
		break;
	case state_type::unsigned_big_integer:
		if (!is_nan_<unsigned __int64>(*data)){
			writer.write_scalar(data->read_scalar<unsigned __int64>());
			writer.write_buffer("UL");
		}
		else
			writer.write_buffer("NaN");
		break;
	case state_type::small_float:
		if (!is_nan_<float>(*data)){
			writer.write_scalar(data->read_scalar<float>());
			writer.write_scalar('F');
		}
		else
			writer.write_buffer("NaN");
		break;
	case state_type::float_:
		if (is_nan_<double>(*data))
			writer.write_buffer("NaN");
		else
			writer.write_scalar(data->read_scalar<double>());
		break;
	case state_type::big_float:
		if (!is_nan_<long double>(*data)){
			writer.write_scalar(data->read_scalar<long double>());
			writer.write_scalar('L');
		}
		else
			writer.write_buffer("NaN");
		break;
	default:
		writer.write_buffer("NaN");
		break;
	}
}

std::size_t cminus::type::number_primitive::get_size() const{
	return size_;
}

std::shared_ptr<cminus::type::object> cminus::type::number_primitive::get_inferred(std::shared_ptr<object> target) const{
	auto number_target = dynamic_cast<const number_primitive *>(target->remove_const_ref());
	if (number_target == nullptr || number_target->is_inferred())
		return nullptr;

	switch (state_){
	case state_type::small_number:
		switch (number_target->state_){
		case state_type::small_integer:
		case state_type::unsigned_small_integer:
		case state_type::small_float:
			break;
		default:
			return nullptr;
		}
		break;
	case state_type::big_number:
		switch (number_target->state_){
		case state_type::big_integer:
		case state_type::unsigned_big_integer:
		case state_type::big_float:
			break;
		default:
			return nullptr;
		}
		break;
	case state_type::number:
		break;
	default:
		return nullptr;
	}

	return target->remove_const_ref(target);
}

bool cminus::type::number_primitive::can_be_inferred_from(const object &target) const{
	auto number_target = dynamic_cast<const number_primitive *>(target.remove_const_ref());
	if (number_target == nullptr)
		return false;

	switch (state_){
	case state_type::small_number:
		switch (number_target->state_){
		case state_type::small_integer:
		case state_type::unsigned_small_integer:
		case state_type::small_float:
			return true;
		default:
			break;
		}
		break;
	case state_type::big_number:
		switch (number_target->state_){
		case state_type::big_integer:
		case state_type::unsigned_big_integer:
		case state_type::big_float:
			return true;
		default:
			break;
		}
		break;
	case state_type::number:
		return !number_target->is_inferred();
	default:
		break;
	}

	return false;
}

bool cminus::type::number_primitive::is_inferred() const{
	switch (state_){
	case state_type::small_number:
	case state_type::number:
	case state_type::big_number:
		return true;
	default:
		break;
	}

	return false;
}

cminus::evaluator::object::id_type cminus::type::number_primitive::get_evaluator_id() const{
	return (is_inferred() ? evaluator::object::id_type::nil : evaluator::object::id_type::number);
}

bool cminus::type::number_primitive::is_integral() const{
	switch (state_){
	case state_type::small_integer:
	case state_type::integer:
	case state_type::big_integer:
	case state_type::unsigned_small_integer:
	case state_type::unsigned_integer:
	case state_type::unsigned_big_integer:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::number_primitive::is_unsigned_integral() const{
	switch (state_){
	case state_type::unsigned_small_integer:
	case state_type::unsigned_integer:
	case state_type::unsigned_big_integer:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::number_primitive::is_nan(const memory::reference &data) const{
	switch (state_){
	case state_type::small_integer:
		return is_nan_<__int16>(data);
	case state_type::integer:
		return is_nan_<__int32>(data);
	case state_type::big_integer:
		return is_nan_<__int64>(data);
	case state_type::unsigned_small_integer:
		return is_nan_<unsigned __int16>(data);
	case state_type::unsigned_integer:
		return is_nan_<unsigned __int32>(data);
	case state_type::unsigned_big_integer:
		return is_nan_<unsigned __int64>(data);
	case state_type::small_float:
		return is_nan_<float>(data);
	case state_type::float_:
		return is_nan_<double>(data);
	case state_type::big_float:
		return is_nan_<long double>(data);
	default:
		break;
	}

	return false;
}

std::string cminus::type::number_primitive::get_string_value(std::shared_ptr<memory::reference> data) const{
	switch (state_){
	case state_type::small_integer:
		return get_string_value_(data->read_scalar<__int16>());
	case state_type::integer:
		return get_string_value_(data->read_scalar<__int32>());
	case state_type::big_integer:
		return get_string_value_(data->read_scalar<__int64>());
	case state_type::unsigned_small_integer:
		return get_string_value_(data->read_scalar<unsigned __int16>());
	case state_type::unsigned_integer:
		return get_string_value_(data->read_scalar<unsigned __int32>());
	case state_type::unsigned_big_integer:
		return get_string_value_(data->read_scalar<unsigned __int64>());
	case state_type::small_float:
		return get_string_value_(data->read_scalar<float>());
	case state_type::float_:
		return get_string_value_(data->read_scalar<double>());
	case state_type::big_float:
		return get_string_value_(data->read_scalar<long double>());
	default:
		break;
	}

	return "NaN";
}

cminus::type::number_primitive::state_type cminus::type::number_primitive::get_state() const{
	return state_;
}

cminus::type::number_primitive::state_type cminus::type::number_primitive::get_precedence(const number_primitive &target) const{
	if (state_ == target.state_)
		return state_;

	if (state_ == state_type::big_float)
		return state_;

	if (state_ == state_type::float_)
		return ((target.state_ != state_type::big_float) ? state_ : target.state_);

	if (state_ == state_type::small_float)
		return ((target.state_ != state_type::float_ && target.state_ != state_type::big_float) ? state_ : target.state_);

	if (target.state_ == state_type::small_float || target.state_ == state_type::float_ || target.state_ == state_type::big_float)
		return target.state_;

	if (size_ != target.size_)
		return ((target.size_ < size_) ? state_ : target.state_);

	if (state_ == state_type::unsigned_big_integer)
		return state_;

	if (state_ == state_type::unsigned_integer)
		return ((target.state_ != state_type::unsigned_big_integer && target.state_ != state_type::big_integer) ? state_ : target.state_);

	if (state_ == state_type::unsigned_small_integer)
		return ((target.state_ == state_type::small_integer) ? state_ : target.state_);

	if (state_ == state_type::big_integer)
		return ((target.state_ != state_type::unsigned_big_integer) ? state_ : target.state_);

	if (state_ == state_type::integer)
		return ((target.state_ == state_type::small_integer) ? state_ : target.state_);

	return target.state_;
}

bool cminus::type::number_primitive::is_exact_(const object &target) const{
	auto number_target = target.as<number_primitive>(false);
	return (number_target != nullptr && number_target->state_ == state_);
}

int cminus::type::number_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	auto number_target = dynamic_cast<const number_primitive *>(target.remove_const_ref());
	if (number_target == nullptr)
		return get_score_value(score_result_type::nil);

	return get_score_value((number_target->state_ == state_ && number_target->size_ == size_) ? score_result_type::exact : score_result_type::assignable);
}

std::shared_ptr<cminus::memory::reference> cminus::type::number_primitive::cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto number_target_type = target_type->as<number_primitive>();
	if (number_target_type == nullptr){
		if (type == cast_type::reinterpret){
			if (!is_integral())
				return nullptr;

			if (target_type->as<pointer_primitive>() != nullptr || target_type->as<function>() != nullptr)
				return std::make_shared<memory::scalar_reference<std::size_t>>(target_type->remove_const_ref(target_type), read_value<std::size_t>(data));

			return nullptr;
		}

		if (is_static_rval_cast(type) && target_type->as<string>() != nullptr)
			return runtime::object::global_storage->create_string(get_string_value(data));

		return nullptr;
	}

	if (!is_static_cast(type) || is_inferred() || number_target_type->is_inferred())
		return nullptr;

	auto data_is_lval = data->is_lvalue();
	if (is_non_const_ref_cast(type) && (state_ != number_target_type->state_ || !data_is_lval || data->is_const()))
		return nullptr;

	if (state_ == number_target_type->state_)//Duplicate
		return ((!data_is_lval || type != cast_type::static_) ? data : copy_data(data, target_type));

	auto converted_target_type = target_type->remove_const_ref(target_type);
	switch (number_target_type->state_){
	case state_type::small_integer:
		return std::make_shared<memory::scalar_reference<__int16>>(converted_target_type, read_value<__int16>(data));
	case state_type::integer:
		return std::make_shared<memory::scalar_reference<__int32>>(converted_target_type, read_value<__int32>(data));
	case state_type::big_integer:
		return std::make_shared<memory::scalar_reference<__int64>>(converted_target_type, read_value<__int64>(data));
	case state_type::unsigned_small_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int16>>(converted_target_type, read_value<unsigned __int16>(data));
	case state_type::unsigned_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int32>>(converted_target_type, read_value<unsigned __int32>(data));
	case state_type::unsigned_big_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int64>>(converted_target_type, read_value<unsigned __int64>(data));
	case state_type::small_float:
		return std::make_shared<memory::scalar_reference<float>>(converted_target_type, read_value<float>(data));
	case state_type::float_:
		return std::make_shared<memory::scalar_reference<double>>(converted_target_type, read_value<double>(data));
	case state_type::big_float:
		return std::make_shared<memory::scalar_reference<long double>>(converted_target_type, read_value<long double>(data));
	default:
		break;
	}

	return nullptr;
}

cminus::type::function_primitive::function_primitive()
	: base_type("FunctionType"){}

cminus::type::function_primitive::~function_primitive() = default;

std::shared_ptr<cminus::type::object> cminus::type::function_primitive::get_inferred(std::shared_ptr<object> target) const{
	return ((dynamic_cast<const function *>(target->remove_const_ref()) == nullptr) ? nullptr : target->remove_const_ref(target));
}

bool cminus::type::function_primitive::can_be_inferred_from(const object &target) const{
	return target.is<function>();
}

bool cminus::type::function_primitive::is_exact_(const object &target) const{
	return target.is<function_primitive>(false);
}

cminus::type::function_return_primitive::function_return_primitive()
	: primitive("FunctionReturnType"){}

cminus::type::function_return_primitive::~function_return_primitive() = default;

bool cminus::type::function_return_primitive::is_exact_(const object &target) const{
	return target.is<function_return_primitive>(false);
}

cminus::type::auto_primitive::auto_primitive()
	: base_type("Auto"){}

cminus::type::auto_primitive::~auto_primitive() = default;

std::shared_ptr<cminus::type::object> cminus::type::auto_primitive::get_inferred(std::shared_ptr<object> target) const{
	return target->remove_const_ref(target);
}

bool cminus::type::auto_primitive::can_be_inferred_from(const object &target) const{
	return true;
}

bool cminus::type::auto_primitive::is_exact_(const object &target) const{
	return target.is<auto_primitive>(false);
}
