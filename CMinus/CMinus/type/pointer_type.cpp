#include "proxy_type.h"
#include "pointer_type.h"

cminus::type::pointer_primitive::pointer_primitive(std::shared_ptr<object> base_type)
	: primitive(""), base_type_(base_type){
	if (base_type == nullptr)
		name_ = "NullptrType";
	else if (base_type->is(query_type::pointer))
		name_ = (base_type_->get_name() + "*");
	else
		name_ = (base_type_->get_name() + " *");
}

cminus::type::pointer_primitive::~pointer_primitive() = default;

std::size_t cminus::type::pointer_primitive::get_size() const{
	return sizeof(void *);
}

bool cminus::type::pointer_primitive::is_exact(const object &target) const{
	auto pointer_target = dynamic_cast<pointer_primitive *>(target.get_non_proxy());
	return (pointer_target != nullptr && (base_type_ == nullptr) == (pointer_target->base_type_ == nullptr) && (base_type_ == nullptr || base_type_->is_exact(*pointer_target->base_type_)));
}

int cminus::type::pointer_primitive::get_score(const object &target) const{
	if (target.is(query_type::explicit_auto))
		return get_score_value(score_result_type::auto_assignable);

	if (base_type_ == nullptr){//Null
		if (target.is(query_type::nullptr_))
			return get_score_value(score_result_type::exact);
		return get_score_value(target.is(query_type::pointer) ? score_result_type::assignable : score_result_type::nil);
	}

	auto pointer_target = dynamic_cast<pointer_primitive *>(target.get_non_proxy());
	if (pointer_target == nullptr || pointer_target->base_type_ == nullptr)
		return get_score_value(score_result_type::nil);

	if (pointer_target->base_type_->is(query_type::void_))//Pointer ==> Void *
		return get_score_value(score_result_type::assignable);

	if (auto base_score = base_type_->get_score(*pointer_target->base_type_); get_score_value(score_result_type::ancestor) <= base_score)
		return ((base_score == get_score_value(score_result_type::exact)) ? base_score : get_score_value(score_result_type::assignable));

	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::pointer_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	auto is_ref = target_type->is(query_type::ref);
	if (is_ref && type == cast_type::reinterpret)
		return nullptr;

	auto pointer_target_type = dynamic_cast<pointer_primitive *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (is_ref && !target_type->is(query_type::const_)){
		if ((type != cast_type::static_ && type != cast_type::rval_static) || pointer_target_type == nullptr || !data->is_lvalue())
			return nullptr;

		if (base_type_ == nullptr)
			return ((pointer_target_type->base_type_ == nullptr) ? data : nullptr);

		return (base_type_->is_exact(*pointer_target_type->base_type_) ? data : nullptr);
	}

	if (pointer_target_type == nullptr){
		if (base_type_ == nullptr)//This is null
			return ((type == cast_type::reinterpret) ? convert_value_to_number(0u, target_type) : nullptr);

		return ((type == cast_type::reinterpret) ? convert_value_to_number(data->read_scalar<std::size_t>(), target_type) : nullptr);
	}

	if (base_type_ == nullptr){//This is null
		if (type != cast_type::static_ && type != cast_type::rval_static && type != cast_type::reinterpret)
			return nullptr;

		if (type == cast_type::rval_static && pointer_target_type->base_type_ == nullptr)
			return data;

		return std::make_shared<memory::scalar_reference<std::size_t>>(target_type->convert(conversion_type::remove_ref_const, target_type), 0u);
	}

	if (pointer_target_type->base_type_ == nullptr)//Target is null
		return nullptr;

	if (type == cast_type::reinterpret)
		return std::make_shared<memory::scalar_reference<std::size_t>>(target_type, data->read_scalar<std::size_t>());

	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

	if (base_type_->is(query_type::offspring_of, pointer_target_type->base_type_.get())){//Return adjusted pointer
		return std::make_shared<memory::scalar_reference<std::size_t>>(
			target_type->convert(conversion_type::remove_ref_const, target_type),
			(data->read_scalar<std::size_t>() + base_type_->compute_base_offset(*pointer_target_type->base_type_))
		);
	}

	if (!base_type_->is_exact(*pointer_target_type->base_type_))
		return nullptr;

	if (type == cast_type::static_)//Create copy
		return std::make_shared<memory::scalar_reference<std::size_t>>(target_type->convert(conversion_type::remove_ref_const, target_type), data->read_scalar<std::size_t>());

	return data;
}

std::shared_ptr<cminus::type::object> cminus::type::pointer_primitive::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type == conversion_type::remove_pointer)
		return base_type_;

	if (base_type_ == nullptr)
		return primitive::convert(type, self_or_other);

	if (type == conversion_type::infer){
		if (auto computed_base_type = base_type_->convert(type, base_type_); computed_base_type != nullptr && computed_base_type != base_type_)
			return std::make_shared<pointer_primitive>(computed_base_type);
	}

	return primitive::convert(type, self_or_other);
}

bool cminus::type::pointer_primitive::is(query_type type, const object *arg) const{
	if (type == query_type::pointer || (type == query_type::nullptr_ && base_type_ == nullptr))
		return true;

	if (type == query_type::updatable && base_type_ != nullptr && base_type_->is(type))
		return true;

	if (type == query_type::auto_ && base_type_ != nullptr && base_type_->is(type))
		return true;

	return primitive::is(type, arg);
}
