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

std::shared_ptr<cminus::evaluator::object> cminus::storage::global::get_evaluator_for(const type::object &target_type) const{
	return nullptr;
}

std::shared_ptr<cminus::evaluator::initializer> cminus::storage::global::get_default_initializer() const{
	return default_initializer_;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::copy(std::shared_ptr<memory::reference> value) const{
	auto reference = std::make_shared<memory::rval_reference>(value->get_type());
	if (reference != nullptr){
		auto value_type = value->get_type();
		value_type->construct(value_type, reference, value);
	}

	return reference;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::global::get_zero_value(std::shared_ptr<type::object> type) const{
	if (type->get_size() == 0u)
		return nullptr;

	auto reference = std::make_shared<memory::rval_reference>(type);
	if (reference != nullptr)
		reference->set(std::byte(0), type->get_size());

	return reference;
}
