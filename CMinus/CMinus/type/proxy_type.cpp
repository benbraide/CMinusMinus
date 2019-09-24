#include "proxy_type.h"

cminus::type::proxy::proxy(object &target)
	: object(target.get_name(), target.get_parent()), target_(&target){}

cminus::type::proxy::~proxy() = default;

bool cminus::type::proxy::is_constructible(std::shared_ptr<memory::reference> target) const{
	return target_->is_constructible(target);
}

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const{
	target_->construct(target, initialization);
}

void cminus::type::proxy::construct(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &initialization) const{
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

std::shared_ptr<cminus::memory::reference> cminus::type::proxy::get_default_value(std::shared_ptr<object> self) const{
	return target_->get_default_value(self);
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

std::size_t cminus::type::proxy::compute_base_offset(const object &base_type) const{
	return target_->compute_base_offset(base_type);
}

std::shared_ptr<cminus::memory::reference> cminus::type::proxy::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return target_->cast(data, target_type, type);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::proxy::get_evaluator() const{
	return target_->get_evaluator();
}

std::shared_ptr<cminus::evaluator::initializer> cminus::type::proxy::get_initializer() const{
	return target_->get_initializer();
}

std::shared_ptr<cminus::type::object> cminus::type::proxy::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	return target_->convert(type, self_or_other);
}

cminus::type::object *cminus::type::proxy::get_non_proxy() const{
	return target_->get_non_proxy();
}

bool cminus::type::proxy::is(query_type type, const object *arg) const{
	return target_->is(type, arg);
}
