#include "../storage/global_storage.h"

#include "modified_type.h"
#include "function_type.h"
#include "primitive_type.h"

cminus::type::primitive::primitive(const std::string &name)
	: object(name, nullptr){}

cminus::type::primitive::~primitive() = default;

bool cminus::type::primitive::is_exact(const object &target) const{
	return (dynamic_cast<const primitive *>(target.get_non_proxy()) != nullptr && object::is_exact(target));
}

int cminus::type::primitive::get_score(const object &target) const{
	if (target.is(query_type::explicit_auto))
		return get_score_value(score_result_type::auto_assignable);
	return get_score_value(is_exact(target) ? score_result_type::exact : score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if ((type != cast_type::static_ && type != cast_type::rval_static) || !target_type->convert(conversion_type::remove_ref_const, target_type)->is_exact(*this))
		return nullptr;

	auto is_ref = target_type->is(query_type::ref);
	if (is_ref && !target_type->is(query_type::const_) && !data->is_lvalue())
		return nullptr;

	return ((!is_ref && type == cast_type::static_) ? runtime::object::global_storage->copy(data) : data);
}

bool cminus::type::primitive::is(query_type type, const object *arg) const{
	return (type == query_type::primitive || object::is(type, arg));
}

std::string cminus::type::primitive::convert_id_to_string(id_type value){
	switch (value){
	case id_type::void_:
		return "Void";
	case id_type::bool_:
		return "Boolean";
	case id_type::byte_:
		return "Byte";
	case id_type::char_:
		return "Char";
	case id_type::wchar_:
		return "WideChar";
	default:
		break;
	}

	return "";
}

cminus::type::undefined_primitive::undefined_primitive()
	: primitive("UndefinedType"){}

cminus::type::undefined_primitive::~undefined_primitive() = default;

std::size_t cminus::type::undefined_primitive::get_size() const{
	return 0u;
}

bool cminus::type::undefined_primitive::is_exact(const object &target) const{
	return target.is(query_type::undefined);
}

int cminus::type::undefined_primitive::get_score(const object &target) const{
	return 0u;
}

std::shared_ptr<cminus::memory::reference> cminus::type::undefined_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

bool cminus::type::undefined_primitive::is(query_type type, const object *arg) const{
	return (type == query_type::undefined || primitive::is(type, arg));
}

cminus::type::number_primitive::number_primitive(state_type state)
	: primitive(""), state_(state){
	switch (state_){
	case state_type::integer:
		name_ = "Integer";
		size_ = sizeof(__int32);
		break;
	case state_type::long_integer:
		name_ = "LongInteger";
		size_ = sizeof(__int64);
		break;
	case state_type::unsigned_integer:
		name_ = "UnsignedInteger";
		size_ = sizeof(unsigned __int32);
		break;
	case state_type::unsigned_long_integer:
		name_ = "UnsignedLongInteger";
		size_ = sizeof(unsigned __int64);
		break;
	case state_type::real:
		name_ = "Float";
		size_ = sizeof(float);
		break;
	case state_type::long_real:
		name_ = "LongFloat";
		size_ = sizeof(long double);
		break;
	default:
		name_ = "Number";
		size_ = 0u;
		break;
	}
}

cminus::type::number_primitive::~number_primitive() = default;

std::shared_ptr<cminus::memory::reference> cminus::type::number_primitive::get_default_value(std::shared_ptr<object> self) const{
	if (state_ == state_type::nil){//Return a NaN value
		if (self.get() == this)
			return std::make_shared<memory::scalar_reference<__int32>>(self, integer_nan_value);
		return std::make_shared<memory::scalar_reference<__int32>>(runtime::object::global_storage->get_cached_type(*this), integer_nan_value);
	}

	return primitive::get_default_value(self);
}

std::size_t cminus::type::number_primitive::get_size() const{
	return size_;
}

bool cminus::type::number_primitive::is_exact(const object &target) const{
	auto number_target = dynamic_cast<number_primitive *>(target.get_non_proxy());
	return (number_target != nullptr && number_target->state_ == state_);
}

int cminus::type::number_primitive::get_score(const object &target) const{
	if (target.is(query_type::explicit_auto) || target.is(query_type::string))
		return get_score_value(score_result_type::auto_assignable);

	auto number_target = dynamic_cast<number_primitive *>(target.get_non_proxy());
	if (number_target == nullptr)
		return get_score_value(score_result_type::nil);

	return get_score_value((number_target->state_ == state_ && number_target->size_ == size_) ? score_result_type::exact : score_result_type::assignable);
}

std::shared_ptr<cminus::memory::reference> cminus::type::number_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto number_target_type = dynamic_cast<const number_primitive *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (number_target_type == nullptr){
		if (type == cast_type::reinterpret){
			if (target_type->is(query_type::ref))
				return nullptr;

			if (target_type->is(query_type::pointer) && (state_ == state_type::integer || state_ == state_type::unsigned_integer))
				return std::make_shared<memory::scalar_reference<std::size_t>>(target_type->convert(conversion_type::remove_ref_const, target_type), read_value_<std::size_t>(data));

			if (target_type->is(query_type::function) && (state_ == state_type::integer || state_ == state_type::unsigned_integer))
				return std::make_shared<memory::function_reference>(read_value_<std::size_t>(data), target_type, nullptr);

			return nullptr;
		}

		if ((type != cast_type::static_ && type != cast_type::rval_static) || !target_type->is(query_type::string))
			return nullptr;

		return nullptr;//#TODO: Create string
	}

	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

	auto is_lval = data->is_lvalue();
	auto is_ref = target_type->is(query_type::ref);

	if (!is_lval && is_ref && !target_type->is(query_type::const_))
		return nullptr;//L-value required

	if (number_target_type->state_ == state_type::nil || number_target_type->state_ == state_){//Exact
		if (type == cast_type::rval_static || !is_lval || is_ref)
			return data;//No copy needed
	}

	auto converted_target_type = target_type->convert(conversion_type::remove_ref_const, target_type);
	switch (number_target_type->state_){
	case state_type::integer:
		return std::make_shared<memory::scalar_reference<__int32>>(converted_target_type, read_value_<__int32>(data));
	case state_type::long_integer:
		return std::make_shared<memory::scalar_reference<__int64>>(converted_target_type, read_value_<__int64>(data));
	case state_type::unsigned_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int32>>(converted_target_type, read_value_<unsigned __int32>(data));
	case state_type::unsigned_long_integer:
		return std::make_shared<memory::scalar_reference<unsigned __int64>>(converted_target_type, read_value_<unsigned __int64>(data));
	case state_type::real:
		return std::make_shared<memory::scalar_reference<float>>(converted_target_type, read_value_<float>(data));
	case state_type::long_real:
		return std::make_shared<memory::scalar_reference<long double>>(converted_target_type, read_value_<long double>(data));
	default:
		break;
	}

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::number_primitive::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type != conversion_type::infer || state_ != state_type::nil)
		return primitive::convert(type, self_or_other);

	auto number_other = dynamic_cast<number_primitive *>(self_or_other->get_non_proxy());
	if (number_other == nullptr)
		return nullptr;//Incompatible types

	return std::make_shared<number_primitive>(number_other->state_);
}

bool cminus::type::number_primitive::is(query_type type, const object *arg) const{
	if (type == query_type::numeric || type == query_type::updatable || primitive::is(type, arg))
		return true;

	switch (state_){
	case state_type::integer:
	case state_type::long_integer:
		return (type == query_type::integral);
	case state_type::unsigned_integer:
	case state_type::unsigned_long_integer:
		return (type == query_type::integral || type == query_type::unsigned_integral);
	case state_type::real:
	case state_type::long_real:
		return (type == query_type::floating_point);
	default:
		break;
	}

	return false;
}

bool cminus::type::number_primitive::is_nan(const memory::reference &data) const{
	switch (state_){
	case state_type::integer:
		return (data.read_scalar<__int32>() == integer_nan_value);
	case state_type::long_integer:
		return (data.read_scalar<__int64>() == long_integer_nan_value);
	case state_type::unsigned_integer:
		return (data.read_scalar<unsigned __int32>() == unsigned_integer_nan_value);
	case state_type::unsigned_long_integer:
		return (data.read_scalar<unsigned __int64>() == unsigned_long_integer_nan_value);
	case state_type::real:
		return (data.read_scalar<float>() == real_nan_value);
	case state_type::long_real:
		return (data.read_scalar<long double>() == long_real_nan_value);
	default:
		break;
	}

	return false;
}

const float cminus::type::number_primitive::real_nan_value = std::numeric_limits<float>::min();

const long double cminus::type::number_primitive::long_real_nan_value = std::numeric_limits<long double>::min();

cminus::type::function_primitive::function_primitive()
	: primitive("FunctionType"){}

cminus::type::function_primitive::~function_primitive() = default;

std::size_t cminus::type::function_primitive::get_size() const{
	return sizeof(void *);
}

bool cminus::type::function_primitive::is_exact(const object &target) const{
	return (dynamic_cast<function_primitive *>(target.get_non_proxy()) != nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::type::function_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if (target_type->is(query_type::generic_function))
		return ((type == cast_type::rval_static || type == cast_type::static_) ? data : nullptr);

	if (type != cast_type::rval_static && type != cast_type::static_)
		return ((type == cast_type::reinterpret && !target_type->is(query_type::ref)) ? convert_value_to_number(data->read_scalar<std::size_t>(), target_type) : nullptr);

	auto function_target_type = dynamic_cast<function *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (function_target_type == nullptr)
		return nullptr;

	auto function_group = data->read_scalar<declaration::function_group_base *>();
	if (function_group == nullptr || function_group->find(*function_target_type) == nullptr)
		return nullptr;

	return ((type == cast_type::rval_static) ? data : std::make_shared<memory::function_reference>(function_group->get_address(), target_type, data->get_context()));
}

bool cminus::type::function_primitive::is(query_type type, const object *arg) const{
	return (type == query_type::function || type == query_type::generic_function || primitive::is(type, arg));
}

cminus::type::auto_primitive::auto_primitive()
	: primitive("Auto"){}

cminus::type::auto_primitive::~auto_primitive() = default;

std::size_t cminus::type::auto_primitive::get_size() const{
	return 0u;
}

bool cminus::type::auto_primitive::is_exact(const object &target) const{
	return (dynamic_cast<auto_primitive *>(target.get_non_proxy()) != nullptr);
}

int cminus::type::auto_primitive::get_score(const object &target) const{
	return 0;
}

std::shared_ptr<cminus::memory::reference> cminus::type::auto_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::auto_primitive::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type == conversion_type::infer)
		return ((self_or_other == nullptr) ? nullptr : self_or_other->convert(conversion_type::remove_ref_const, self_or_other));
	return primitive::convert(type, self_or_other);
}

bool cminus::type::auto_primitive::is(query_type type, const object *arg) const{
	return (type == query_type::auto_ || type == query_type::explicit_auto || type == query_type::inferred || primitive::is(type, arg));
}
