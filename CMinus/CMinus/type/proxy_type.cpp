#include "proxy_type.h"

cminus::type::proxy::proxy(const object &target)
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

void cminus::type::proxy::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	target_->print_value(writer, data);
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

int cminus::type::proxy::get_score(const object &target, bool is_lval, bool is_const) const{
	return target_->get_score(target, is_lval, is_const);
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

const cminus::type::object *cminus::type::proxy::remove_proxy() const{
	return target_->remove_proxy();
}

const cminus::type::object *cminus::type::proxy::remove_const_ref() const{
	return target_->remove_const_ref();
}

std::shared_ptr<cminus::type::object> cminus::type::proxy::remove_const_ref(std::shared_ptr<object> self) const{
	return target_->remove_const_ref(self);
}

bool cminus::type::proxy::is_default_constructible(bool ignore_callable) const{
	return target_->is_default_constructible(ignore_callable);
}

bool cminus::type::proxy::is_copy_constructible(bool ignore_callable) const{
	return target_->is_copy_constructible(ignore_callable);
}

bool cminus::type::proxy::is_copy_assignable(bool ignore_callable) const{
	return target_->is_copy_assignable(ignore_callable);
}

bool cminus::type::proxy::is_inferred() const{
	return target_->is_inferred();
}

bool cminus::type::proxy::is_const() const{
	return target_->is_const();
}

bool cminus::type::proxy::is_ref() const{
	return target_->is_ref();
}
