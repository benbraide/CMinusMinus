#include "../type/list_type.h"
#include "../type/class_type.h"
#include "../storage/global_storage.h"

#include "core_functions.h"

cminus::declaration::core::core_function::core_function(const std::string &name, storage::object &parent, std::shared_ptr<type::object> return_type)
	: external_function(name, &parent, attribute::collection::list_type{}, flags::nil, return_type){}

cminus::declaration::core::core_function::~core_function() = default;

cminus::declaration::core::heap_alloc::heap_alloc(storage::object &parent)
	: core_function("HeapAlloc", parent, runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false)){
	add_parameter("size", runtime::object::global_storage->get_size_type());
}

cminus::declaration::core::heap_alloc::~heap_alloc() = default;

void cminus::declaration::core::heap_alloc::evaluate_body_() const{
	throw runtime::exception::return_interrupt(std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false),
		runtime::object::memory_object->allocate_heap_block(runtime::object::current_storage->find("size", true)->read_scalar<std::size_t>())
	));
}

cminus::declaration::core::heap_dealloc::heap_dealloc(storage::object & parent)
	: core_function("HeapDealloc", parent, runtime::object::global_storage->get_void_type()){
	add_parameter("target", runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false));
}

cminus::declaration::core::heap_dealloc::~heap_dealloc() = default;

void cminus::declaration::core::heap_dealloc::evaluate_body_() const{
	runtime::object::memory_object->deallocate_heap_block(runtime::object::current_storage->find("target", true)->read_scalar<std::size_t>());
}

cminus::declaration::core::heap_realloc::heap_realloc(storage::object & parent)
	: core_function("HeapRealloc", parent, runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false)){
	add_parameter("target", runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false));
	add_parameter("size", runtime::object::global_storage->get_size_type());
}

cminus::declaration::core::heap_realloc::~heap_realloc() = default;

void cminus::declaration::core::heap_realloc::evaluate_body_() const{
	auto address = runtime::object::current_storage->find("target", true)->read_scalar<std::size_t>();
	throw runtime::exception::return_interrupt(std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false),
		runtime::object::memory_object->reallocate_heap_block(address, runtime::object::current_storage->find("size", true)->read_scalar<std::size_t>())
	));
}

cminus::declaration::core::is_heap::is_heap(storage::object & parent)
	: core_function("IsHeap", parent, runtime::object::global_storage->get_boolean_type()){
	add_parameter("target", runtime::object::global_storage->get_pointer_type(storage::global::cached_type::void_, false));
}

cminus::declaration::core::is_heap::~is_heap() = default;

void cminus::declaration::core::is_heap::evaluate_body_() const{
	throw runtime::exception::return_interrupt(runtime::object::global_storage->get_boolean_value(
		runtime::object::memory_object->is_resizable(runtime::object::current_storage->find("target", true)->read_scalar<std::size_t>())
	));
}

cminus::declaration::core::begin::begin(storage::object & parent, bool is_constant_target)
	: core_function("Begin", parent, runtime::object::global_storage->get_auto_type()){
	add_parameter("target", runtime::object::global_storage->get_auto_ref_type(is_constant_target));
}

cminus::declaration::core::begin::~begin() = default;

void cminus::declaration::core::begin::evaluate_body_() const{
	auto target = runtime::object::current_storage->find("target", true);
	throw runtime::exception::return_interrupt(target->get_type()->begin(target));
}

cminus::declaration::core::rbegin::rbegin(storage::object & parent, bool is_constant_target)
	: core_function("ReverseBegin", parent, runtime::object::global_storage->get_auto_type()){
	add_parameter("target", runtime::object::global_storage->get_auto_ref_type(is_constant_target));
}

cminus::declaration::core::rbegin::~rbegin() = default;

void cminus::declaration::core::rbegin::evaluate_body_() const{
	auto target = runtime::object::current_storage->find("target", true);
	throw runtime::exception::return_interrupt(target->get_type()->rbegin(target));
}

cminus::declaration::core::end::end(storage::object & parent, bool is_constant_target)
	: core_function("End", parent, runtime::object::global_storage->get_auto_type()){
	add_parameter("target", runtime::object::global_storage->get_auto_ref_type(is_constant_target));
}

cminus::declaration::core::end::~end() = default;

void cminus::declaration::core::end::evaluate_body_() const{
	auto target = runtime::object::current_storage->find("target", true);
	throw runtime::exception::return_interrupt(target->get_type()->end(target));
}

cminus::declaration::core::rend::rend(storage::object & parent, bool is_constant_target)
	: core_function("ReverseEnd", parent, runtime::object::global_storage->get_auto_type()){
	add_parameter("target", runtime::object::global_storage->get_auto_ref_type(is_constant_target));
}

cminus::declaration::core::rend::~rend() = default;

void cminus::declaration::core::rend::evaluate_body_() const{
	auto target = runtime::object::current_storage->find("target", true);
	throw runtime::exception::return_interrupt(target->get_type()->rend(target));
}
