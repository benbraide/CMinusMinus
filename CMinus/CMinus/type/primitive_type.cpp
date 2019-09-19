#include "../storage/global_storage.h"

#include "primitive_type.h"

cminus::type::primitive::primitive(const std::string &name)
	: object(name, nullptr){}

cminus::type::primitive::~primitive() = default;

bool cminus::type::primitive::is_exact(const object &target) const{
	return (dynamic_cast<const primitive *>(&target) != nullptr && object::is_exact(target));
}

int cminus::type::primitive::get_score(const object &target) const{
	return get_score_value(is_exact(target) ? score_result_type::exact : score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if ((type != cast_type::static_ && type != cast_type::rval_static) || !target_type->is_exact(*this))
		return nullptr;

	auto is_ref = target_type->is(query_type::ref);
	if (is_ref && !target_type->is(query_type::const_) && !data->is_lvalue())
		return nullptr;

	return ((!is_ref && type == cast_type::static_) ? runtime::object::global_storage->copy(data) : data);
}

bool cminus::type::primitive::is(query_type type) const{
	return (type == query_type::primitive || object::is(type));
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

cminus::type::number_primitive::number_primitive(state_type state, std::size_t size)
	: primitive("Number"), state_(state), size_(size){}

cminus::type::number_primitive::~number_primitive() = default;

std::size_t cminus::type::number_primitive::get_size() const{
	return size_;
}

bool cminus::type::number_primitive::is_exact(const object &target) const{
	auto number_target = dynamic_cast<const number_primitive *>(&target);
	return (number_target != nullptr && number_target->state_ == state_ && number_target->size_ == size_);
}

int cminus::type::number_primitive::get_score(const object &target) const{
	if (target.is(query_type::explicit_auto))
		return get_score_value(score_result_type::assignable);

	auto number_target = dynamic_cast<const number_primitive *>(&target);
	if (number_target == nullptr)
		return get_score_value(score_result_type::nil);

	return get_score_value((number_target->state_ == state_ && number_target->size_ == size_) ? score_result_type::exact : score_result_type::assignable);
}

std::shared_ptr<cminus::memory::reference> cminus::type::number_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto number_target_type = dynamic_cast<const number_primitive *>(target_type.get());
	if (number_target_type == nullptr){
		if (type == cast_type::reinterpret){
			if (target_type->is(query_type::ref))
				return nullptr;

			if (target_type->is(query_type::pointer) && state_ == state_type::integer)
				return std::make_shared<memory::scalar_reference<unsigned __int64>>(target_type, read_value_<unsigned __int64>(data));

			if (target_type->is(query_type::function) && state_ == state_type::integer)
				return std::make_shared<memory::function_reference>(read_value_<unsigned __int64>(data), target_type, nullptr);

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

	auto cloned_target_type = std::make_shared<number_primitive>(number_target_type->state_, number_target_type->size_);
	if (number_target_type->state_ == state_ && number_target_type->size_ == size_){//Exact
		if (type == cast_type::rval_static || !is_lval || is_ref)
			return data;//No copy needed

		auto value = std::make_shared<memory::rval_reference>(cloned_target_type);
		if (value != nullptr)//Copy bytes
			value->write(data->get_address(), size_);

		return value;
	}

	if (number_target_type->state_ == state_type::integer){
		switch (number_target_type->size_){
		case 1u:
			return std::make_shared<memory::scalar_reference<__int8>>(cloned_target_type, read_value_<__int8>(data));
		case 2u:
			return std::make_shared<memory::scalar_reference<__int16>>(cloned_target_type, read_value_<__int16>(data));
		case 4u:
			return std::make_shared<memory::scalar_reference<__int32>>(cloned_target_type, read_value_<__int32>(data));
		case 8u:
			return std::make_shared<memory::scalar_reference<__int64>>(cloned_target_type, read_value_<__int64>(data));
		default:
			break;
		}
	}
	else if (number_target_type->state_ == state_type::real){
		switch (number_target_type->size_){
		case 4u:
			return std::make_shared<memory::scalar_reference<float>>(cloned_target_type, read_value_<float>(data));
		case 8u:
			return std::make_shared<memory::scalar_reference<long double>>(cloned_target_type, read_value_<long double>(data));
		default:
			break;
		}
	}
	else if (number_target_type->state_ == state_type::nan)
		return runtime::object::global_storage->get_zero_value(cloned_target_type);

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::number_primitive::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type == conversion_type::update){
		if (auto number_type = dynamic_cast<number_primitive *>(self_or_other.get()); number_type != nullptr)
			state_ = number_type->state_;
		return nullptr;
	}

	if (type != conversion_type::infer)
		return primitive::convert(type, self_or_other);

	auto number_other = dynamic_cast<const number_primitive *>(self_or_other.get());
	if (number_other == nullptr || (number_other->state_ == state_ && number_other->size_ == size_))
		return nullptr;//Incompatible or exact types

	return std::make_shared<number_primitive>(number_other->state_, number_other->size_);
}

bool cminus::type::number_primitive::is(query_type type) const{
	if (primitive::is(type) || type == query_type::numeric)
		return true;

	if (type == query_type::floating_point)
		return (state_ == state_type::real);

	if (type == query_type::integral)
		return (state_ == state_type::integer);

	return (type == query_type::nan && state_ == state_type::nan);
}
