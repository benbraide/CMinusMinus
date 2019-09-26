#include "../type/primitive_type.h"
#include "../evaluator/initializer.h"

#include "global_storage.h"

cminus::storage::global::global()
	: named_object("", nullptr){
	runtime::object::global_storage = this;
	runtime::object::current_storage = this;

	default_initializer_ = std::make_shared<evaluator::initializer>();
}

cminus::storage::global::~global(){
	destroy_entries_();
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_cached_type(cached_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::global::get_cached_type(const type::object &type) const{
	return nullptr;
}

std::shared_ptr<cminus::evaluator::object> cminus::storage::global::get_evaluator_for(const type::object &target_type) const{
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
		reference->set(std::byte(0), type->get_size());

	return reference;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::create_string(const std::string_view &value) const{
	return nullptr;
}

std::string_view cminus::storage::global::get_string_value(std::shared_ptr<memory::reference> value) const{
	return std::string_view();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_constant_value(bool value) const{
	return nullptr;
}
