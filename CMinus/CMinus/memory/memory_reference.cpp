#include "../type/primitive_type.h"
#include "../type/modified_type.h"

#include "../storage/global_storage.h"
#include "../declaration/callable_declaration_group.h"

#include "memory_reference.h"

cminus::memory::reference::reference(std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: type_(type), context_(context){
	allocate_memory_();
}

cminus::memory::reference::reference(std::shared_ptr<type::object> type)
	: reference(type, nullptr){}

cminus::memory::reference::reference(std::size_t address, std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: type_(type), context_(context), address_(address){}

cminus::memory::reference::reference(std::size_t address, std::shared_ptr<type::object> type)
	: reference(address, type, nullptr){}

cminus::memory::reference::~reference(){
	destruct_();
}

std::size_t cminus::memory::reference::initialize(std::shared_ptr<reference> value){
	return write(value);
}

std::size_t cminus::memory::reference::write(std::shared_ptr<reference> value){
	before_write_(value);
	value->before_read_();
	auto result = runtime::object::memory_object->write(get_address(), value->get_address(), std::min(get_size(), value->get_size()));

	value->after_read_();
	after_write_(value);

	return result;
}

std::size_t cminus::memory::reference::write_address(std::size_t value){
	return 0u;
}

std::size_t cminus::memory::reference::read(std::byte *buffer, std::size_t size) const{
	before_read_();
	auto result = runtime::object::memory_object->read(get_address(), buffer, std::min(type_->get_size(), size));
	after_read_();

	return result;
}

void cminus::memory::reference::set_constructed_state(){
	is_constructed_ = true;
}

bool cminus::memory::reference::is_constructed() const{
	return is_constructed_;
}

std::shared_ptr<cminus::type::object> cminus::memory::reference::get_type() const{
	return type_;
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::apply_offset(std::size_t value, std::shared_ptr<type::object> type) const{
	auto target_type = ((type == nullptr) ? type_ : type);
	auto entry = std::make_shared<memory::reference>(
		(get_address() + value),
		target_type->convert(type::object::conversion_type::remove_indirection, target_type),
		context_
	);

	if (entry != nullptr)//Copy l-value state
		entry->is_lvalue_ = is_lvalue_;

	return entry;
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::clone() const{
	return apply_offset(0u, nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::bound_context(std::shared_ptr<reference> value) const{
	return std::make_shared<memory::reference>(address_, type_, value);
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::get_context() const{
	return context_;
}

std::size_t cminus::memory::reference::get_address() const{
	return address_;
}

std::size_t cminus::memory::reference::get_indirect_address() const{
	return address_;
}

std::size_t cminus::memory::reference::get_size() const{
	return ((type_ == nullptr) ? 0u : type_->get_size());
}

bool cminus::memory::reference::is_lvalue() const{
	return (is_lvalue_ && (context_ == nullptr || context_->is_lvalue()));
}

bool cminus::memory::reference::is_const() const{
	return (type_->is(type::object::query_type::const_) || (context_ != nullptr && context_->is_const()));
}

bool cminus::memory::reference::is_nan() const{
	auto number_type = dynamic_cast<type::number_primitive *>(type_->convert(type::object::conversion_type::remove_ref_const, type_)->get_non_proxy());
	return (number_type != nullptr && number_type->is_nan(*this));
}

void cminus::memory::reference::allocate_memory_(){
	auto block = allocate_block_();
	if (block == nullptr || (address_ = block->get_address()) == 0u)
		throw memory::exception::allocation_failure();

	deallocator_ = [this](){
		if (address_ != 0u){
			runtime::object::memory_object->deallocate_block(address_);
			address_ = 0u;
		}
	};
}

std::shared_ptr<cminus::memory::block> cminus::memory::reference::allocate_block_() const{
	return runtime::object::memory_object->allocate_block(get_memory_size_());
}

std::size_t cminus::memory::reference::get_memory_size_() const{
	return ((type_ == nullptr) ? 0u : type_->get_memory_size());
}

void cminus::memory::reference::before_write_(std::shared_ptr<reference> value) const{}

void cminus::memory::reference::after_write_(std::shared_ptr<reference> value) const{}

void cminus::memory::reference::before_read_() const{}

void cminus::memory::reference::after_read_() const{}

void cminus::memory::reference::destruct_(){
	if (is_constructed_ && type_ != nullptr){
		is_constructed_ = false;
		type_->destruct(clone());
	}

	if (deallocator_ != nullptr){
		deallocator_();
		deallocator_ = nullptr;
	}
}

cminus::memory::undefined_reference::undefined_reference(std::shared_ptr<reference> context)
	: reference(0u, nullptr, context){}

cminus::memory::undefined_reference::~undefined_reference() = default;

cminus::memory::declared_reference::declared_reference(const declaration::object &declaration, std::shared_ptr<reference> context)
	: reference(declaration.get_type(), context), declaration_(&declaration){}

cminus::memory::declared_reference::declared_reference(std::size_t address, const declaration::object &declaration, std::shared_ptr<reference> context)
	: reference(address, declaration.get_type(), context), declaration_(&declaration){}

cminus::memory::declared_reference::~declared_reference(){
	destruct_();
}

const cminus::declaration::object *cminus::memory::declared_reference::get_declaration() const{
	return declaration_;
}

void cminus::memory::declared_reference::before_write_(std::shared_ptr<reference> value) const{

}

void cminus::memory::declared_reference::after_write_(std::shared_ptr<reference> value) const{

}

void cminus::memory::declared_reference::before_read_() const{

}

void cminus::memory::declared_reference::after_read_() const{

}

cminus::memory::function_reference::function_reference(declaration::callable_group &entry, std::shared_ptr<reference> context)
	: reference(entry.get_address(), entry.get_type(), context){}

cminus::memory::function_reference::function_reference(declaration::callable_group &entry, std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: reference(entry.get_address(), ((type == nullptr) ? entry.get_type() : type), context){}

cminus::memory::function_reference::function_reference(std::size_t address, std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: reference(address, type, context){}

cminus::declaration::callable_group *cminus::memory::function_reference::get_entry() const{
	return read_scalar<declaration::callable_group *>();
}

cminus::memory::function_reference::~function_reference() = default;

cminus::memory::indirect_reference::indirect_reference(const declaration::object &declaration, std::shared_ptr<reference> context)
	: declared_reference(0u, declaration, context){
	allocate_memory_();
}

cminus::memory::indirect_reference::indirect_reference(std::size_t address, const declaration::object &declaration, std::shared_ptr<reference> context)
	: declared_reference(address, declaration, context){}

cminus::memory::indirect_reference::~indirect_reference(){
	destruct_();
}

std::size_t cminus::memory::indirect_reference::initialize(std::shared_ptr<reference> value){
	if (value == nullptr || owned_ != nullptr || write_address(value->get_address()) == 0u)
		return 0u;

	if (!value->is_lvalue())
		owned_ = value;

	return type_->get_size();
}

std::size_t cminus::memory::indirect_reference::write_address(std::size_t value){
	return runtime::object::memory_object->write_scalar(address_, value);
}

std::size_t cminus::memory::indirect_reference::get_address() const{
	return runtime::object::memory_object->read_scalar<std::size_t>(address_);
}

std::size_t cminus::memory::indirect_reference::get_memory_size_() const{
	return ((type_ == nullptr) ? 0u : sizeof(void *));
}

cminus::memory::rval_reference::rval_reference(std::shared_ptr<type::object> type)
	: reference(type, nullptr){
	is_lvalue_ = false;
}

cminus::memory::rval_reference::rval_reference(std::size_t address, std::shared_ptr<type::object> type)
	: reference(address, type, nullptr){
	is_lvalue_ = false;
}

cminus::memory::rval_reference::~rval_reference() = default;

cminus::memory::write_protected_rval_reference::write_protected_rval_reference(std::shared_ptr<type::object> type)
	: rval_reference(0u, type){
	allocate_memory_();
}

std::shared_ptr<cminus::memory::block> cminus::memory::write_protected_rval_reference::allocate_block_() const{
	return runtime::object::memory_object->allocate_write_protected_block(get_memory_size_());
}

cminus::memory::write_protected_rval_reference::~write_protected_rval_reference() = default;
