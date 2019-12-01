#include "pointer_type.h"
#include "modified_type.h"

cminus::type::modified::modified(const std::string &name, std::shared_ptr<object> base_type)
	: object(name, nullptr), base_type_(base_type){}

cminus::type::modified::~modified() = default;

void cminus::type::modified::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	base_type_->print_value(writer, data);
}

std::size_t cminus::type::modified::get_size() const{
	return base_type_->get_size();
}

int cminus::type::modified::get_score(const object &target, bool is_lval, bool is_const) const{
	return base_type_->get_score(target, is_lval, is_const);
}

std::shared_ptr<cminus::memory::reference> cminus::type::modified::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return base_type_->cast(data, target_type, type);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::modified::get_evaluator() const{
	return base_type_->get_evaluator();
}

const cminus::type::object *cminus::type::modified::remove_const_ref() const{
	return base_type_->remove_const_ref();
}

std::shared_ptr<cminus::type::object> cminus::type::modified::remove_const_ref(std::shared_ptr<object> self) const{
	return base_type_->remove_const_ref(base_type_);
}

bool cminus::type::modified::can_be_inferred_from(const object &target) const{
	return base_type_->can_be_inferred_from(target);
}

bool cminus::type::modified::is_inferred() const{
	return base_type_->is_inferred();
}

bool cminus::type::modified::is_const() const{
	return base_type_->is_const();
}

bool cminus::type::modified::is_ref() const{
	return base_type_->is_ref();
}

cminus::type::constant::constant(std::shared_ptr<object> base_type)
	: modified("", base_type){
	if (base_type_->is<pointer_primitive>())
		name_ = (base_type_->get_name() + "Const");
	else
		name_ = ("Const " + base_type_->get_name());
}

cminus::type::constant::~constant() = default;

std::string cminus::type::constant::get_qname() const{
	if (base_type_->is<pointer_primitive>() && !base_type_->is_ref())
		return (base_type_->get_qname() + "Const");
	return ("Const " + base_type_->get_qname());
}

std::shared_ptr<cminus::type::object> cminus::type::constant::get_inferred(std::shared_ptr<object> target) const{
	auto result = base_type_->get_inferred(target);
	return ((result == nullptr || result->is_const()) ? result : std::make_shared<constant>(result));
}

bool cminus::type::constant::can_be_iterated() const{
	return base_type_->can_be_iterated();
}

bool cminus::type::constant::is_const() const{
	return true;
}

bool cminus::type::constant::is_exact_(const object &target) const{
	auto constant_target = dynamic_cast<const constant *>(target.remove_proxy());
	return (constant_target != nullptr && base_type_->is_exact(*constant_target->base_type_));
}

cminus::type::ref::ref(std::shared_ptr<object> base_type)
	: modified("", base_type){
	name_ = (base_type_->get_name() + "&");
}

cminus::type::ref::~ref() = default;

std::string cminus::type::ref::get_qname() const{
	return (base_type_->get_qname() + "&");
}

std::shared_ptr<cminus::memory::reference> cminus::type::ref::get_default_value() const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::ref::get_inferred(std::shared_ptr<object> target) const{
	auto result = base_type_->get_inferred(target);
	if (result == nullptr)
		return nullptr;

	if (!result->is_ref())
		result = std::make_shared<ref>(result);

	return ((target->is_const() && !result->is_const()) ? std::make_shared<constant>(result) : result);
}

bool cminus::type::ref::is_default_constructible(bool ignore_callable) const{
	return false;
}

bool cminus::type::ref::is_copy_constructible(bool ignore_callable) const{
	return false;
}

bool cminus::type::ref::is_copy_assignable(bool ignore_callable) const{
	return false;
}

bool cminus::type::ref::is_ref() const{
	return true;
}

bool cminus::type::ref::is_exact_(const object &target) const{
	auto constant_target = dynamic_cast<const ref *>(target.remove_proxy());
	return (constant_target != nullptr && base_type_->is_exact(*constant_target->base_type_));
}
