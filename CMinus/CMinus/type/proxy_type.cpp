#include "proxy_type.h"

cminus::type::proxy::proxy(object &target)
	: object(target.get_name(), target.get_parent()), target_(&target){}

cminus::type::proxy::~proxy() = default;

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const{
	target_->construct(target, initialization);
}

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	target_->construct(target, initialization);
}

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const{
	target_->construct(target, initialization);
}

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target) const{
	target_->construct(target);
}

void cminus::type::proxy::destruct(std::shared_ptr<memory::reference> target) const{
	target_->destruct(target);
}

std::shared_ptr<cminus::memory::reference> cminus::type::proxy::get_default_value() const{
	return target_->get_default_value();
}

std::size_t cminus::type::proxy::get_size() const{
	return target_->get_size();
}

std::size_t cminus::type::proxy::get_memory_size() const{
	return target_->get_memory_size();
}

bool cminus::type::proxy::is_exact(const object &target) const{
	return target_->is_exact(target);
}

int cminus::type::proxy::get_score(const object &target) const{
	return target_->get_score(target);
}

std::shared_ptr<cminus::memory::reference> cminus::type::proxy::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return target_->cast(data, target_type, type);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::proxy::get_evaluator() const{
	return target_->get_evaluator();
}

std::shared_ptr<cminus::evaluator::object> cminus::type::proxy::get_initializer() const{
	return target_->get_initializer();
}

std::shared_ptr<cminus::type::object> cminus::type::proxy::remove_ref(std::shared_ptr<object> self) const{
	return ((self.get() == this) ? self : target_->remove_ref(nullptr));
}

std::shared_ptr<cminus::type::object> cminus::type::proxy::remove_const(std::shared_ptr<object> self) const{
	return ((self.get() == this) ? self : target_->remove_const(nullptr));
}

std::shared_ptr<cminus::type::object> cminus::type::proxy::convert_auto(std::shared_ptr<object> target) const{
	return target_->convert_auto(target);
}

cminus::type::object *cminus::type::proxy::get_non_proxy() const{
	return target_->get_non_proxy();
}

bool cminus::type::proxy::is_ref() const{
	return target_->is_ref();
}

bool cminus::type::proxy::is_const() const{
	return target_->is_const();
}

bool cminus::type::proxy::is_auto() const{
	return target_->is_auto();
}

bool cminus::type::proxy::is_explicit_auto() const{
	return target_->is_explicit_auto();
}

bool cminus::type::proxy::is_any() const{
	return target_->is_any();
}

bool cminus::type::proxy::is_explicit_any() const{
	return target_->is_explicit_any();
}

bool cminus::type::proxy::is_nan() const{
	return target_->is_nan();
}

bool cminus::type::proxy::is_explicit_nan() const{
	return target_->is_explicit_nan();
}

bool cminus::type::proxy::is_undefined() const{
	return target_->is_undefined();
}
