#include "modified_type.h"

cminus::type::modified::modified(const std::string &name, std::shared_ptr<object> base_type)
	: object(name, nullptr), base_type_(base_type){}

cminus::type::modified::~modified() = default;

std::size_t cminus::type::modified::get_size() const{
	return base_type_->get_size();
}

std::shared_ptr<cminus::memory::reference> cminus::type::modified::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return base_type_->cast(data, target_type, type);
}

bool cminus::type::modified::is(query_type type, const object *arg) const{
	return (type == query_type::modified || base_type_->is(type, arg));
}

cminus::type::constant::constant(std::shared_ptr<object> base_type)
	: modified("", base_type){
	if (base_type->is(query_type::pointer))
		name_ = (base_type_->get_name() + "Const");
	else
		name_ = ("Const " + base_type->get_name());
}

cminus::type::constant::~constant() = default;

bool cminus::type::constant::is_exact(const object &target) const{
	auto constant_target = dynamic_cast<constant *>(target.get_non_proxy());
	return (constant_target != nullptr && base_type_->is_exact(*constant_target->base_type_));
}

int cminus::type::constant::get_score(const object &target) const{
	if (auto base_score = base_type_->get_score(target); base_score != get_score_value(score_result_type::nil))
		return (!target.is(query_type::const_) ? (base_score - 1) : base_score);
	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::type::object> cminus::type::constant::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type == conversion_type::clone || type == conversion_type::infer){
		if (auto computed_base_type = base_type_->convert(type, base_type_); computed_base_type != nullptr && computed_base_type != base_type_)
			return std::make_shared<constant>(computed_base_type);
	}

	if (type == conversion_type::remove_ref_const)
		return base_type_->convert(conversion_type::remove_ref_const, base_type_);

	return ((type == conversion_type::remove_const) ? base_type_ : modified::convert(type, self_or_other));
}

bool cminus::type::constant::is(query_type type, const object *arg) const{
	return (type == query_type::const_ || modified::is(type, arg));
}

cminus::type::ref::ref(std::shared_ptr<object> base_type)
	: modified("", base_type){
	if (base_type->is(query_type::pointer))
		name_ = (base_type_->get_name() + "&");
	else
		name_ = (base_type_->get_name() + " &");
}

cminus::type::ref::~ref() = default;

bool cminus::type::ref::is_exact(const object &target) const{
	auto constant_target = dynamic_cast<ref *>(target.get_non_proxy());
	return (constant_target != nullptr && base_type_->is_exact(*constant_target->base_type_));
}

int cminus::type::ref::get_score(const object &target) const{
	if (auto base_score = base_type_->get_score(target); base_score != get_score_value(score_result_type::nil))
		return (!target.is(query_type::ref) ? (base_score - 1) : base_score);
	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::type::object> cminus::type::ref::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	if (type == conversion_type::clone || type == conversion_type::infer){
		if (auto computed_base_type = base_type_->convert(type, base_type_); computed_base_type != nullptr && computed_base_type != base_type_)
			return std::make_shared<ref>(computed_base_type);
	}

	if (type == conversion_type::correct_ref_const && base_type_->is(query_type::const_))//Ref Constant __TYPE__ ==> Constant Ref __TYPE__
		return std::make_shared<constant>(std::make_shared<ref>(base_type_->convert(conversion_type::remove_ref_const)));

	if (type == conversion_type::remove_ref_const)
		return base_type_->convert(conversion_type::remove_ref_const, base_type_);

	return ((type == conversion_type::remove_ref) ? base_type_ : modified::convert(type, self_or_other));
}

bool cminus::type::ref::is(query_type type, const object *arg) const{
	return (type == query_type::ref || modified::is(type, arg));
}