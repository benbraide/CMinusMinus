#include "../type/type_object.h"
#include "../storage/global_storage.h"
#include "../declaration/function_declaration_group_base.h"

#include "memory_reference.h"

cminus::memory::reference::~reference(){
	if (deallocator_ != nullptr){
		deallocator_();
		deallocator_ = nullptr;
	}
}

std::size_t cminus::memory::reference::get_position() const{
	return 0u;
}

std::size_t cminus::memory::reference::get_size() const{
	return ((type_ == nullptr) ? 0u : type_->get_size());
}

std::size_t cminus::memory::reference::set(std::byte value, std::size_t size){
	return runtime::object::memory_object->set(get_address(), value, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::write(const std::byte *buffer, std::size_t size){
	return runtime::object::memory_object->write(get_address(), buffer, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::write(const io::binary_reader &buffer, std::size_t size){
	return runtime::object::memory_object->write(get_address(), buffer, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::write(std::size_t source_address, std::size_t size){
	return runtime::object::memory_object->write(get_address(), source_address, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::write_address(std::size_t value){
	return 0u;
}

std::size_t cminus::memory::reference::write_ownership(std::shared_ptr<reference> target){
	return 0u;
}

std::size_t cminus::memory::reference::read(std::byte *buffer, std::size_t size) const{
	return runtime::object::memory_object->read(get_address(), buffer, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::read(io::binary_writer &buffer, std::size_t size) const{
	return runtime::object::memory_object->read(get_address(), buffer, std::min(type_->get_size(), size));
}

std::size_t cminus::memory::reference::read(std::size_t destination_address, std::size_t size) const{
	return runtime::object::memory_object->read(get_address(), destination_address, std::min(type_->get_size(), size));
}

std::shared_ptr<cminus::type::object> cminus::memory::reference::get_type() const{
	return type_;
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::apply_offset(std::size_t value, std::shared_ptr<type::object> type) const{
	auto target_type = ((type == nullptr) ? type_ : type);
	auto entry = std::make_shared<memory::reference>(
		(get_address() + value),
		target_type->convert(type::object::conversion_type::remove_indirection, target_type),
		attributes_.get_list(),
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
	return std::make_shared<memory::reference>(address_, type_, attributes_.get_list(), value);
}

std::shared_ptr<cminus::memory::reference> cminus::memory::reference::get_context() const{
	return context_;
}

const cminus::attribute::collection &cminus::memory::reference::get_attributes() const{
	return attributes_;
}

cminus::attribute::collection &cminus::memory::reference::get_attributes(){
	return attributes_;
}

std::size_t cminus::memory::reference::get_address() const{
	return address_;
}

bool cminus::memory::reference::is_lvalue() const{
	return (is_lvalue_ && (context_ == nullptr || context_->is_lvalue()));
}

bool cminus::memory::reference::is_const() const{
	return (type_->is(type::object::query_type::const_) || (context_ != nullptr && context_->is_const()));
}

void cminus::memory::reference::allocate_memory_(){
	auto block = allocate_block_();
	if (block == nullptr || (address_ = block->get_address()) == 0u)
		throw memory::exception::allocation_failure();

	deallocator_ = [this](){
		if (type_ != nullptr)
			type_->destruct(clone());

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

cminus::memory::undefined_reference::undefined_reference(std::shared_ptr<reference> context)
	: reference(0u, nullptr, attribute::collection::list_type{}, context){}

cminus::memory::undefined_reference::~undefined_reference() = default;

cminus::memory::function_reference::function_reference(declaration::function_group_base &entry, std::shared_ptr<reference> context)
	: reference(entry.get_address(), entry.get_type(), attribute::collection::list_type{}, context){}

cminus::memory::function_reference::function_reference(declaration::function_group_base &entry, std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: reference(entry.get_address(), ((type == nullptr) ? entry.get_type() : type), attribute::collection::list_type{}, context){}

cminus::memory::function_reference::function_reference(std::size_t address, std::shared_ptr<type::object> type, std::shared_ptr<reference> context)
	: reference(address, type, attribute::collection::list_type{}, context){}

cminus::declaration::function_group_base *cminus::memory::function_reference::get_entry() const{
	return read_scalar<declaration::function_group_base *>();
}

cminus::memory::function_reference::~function_reference() = default;

cminus::memory::indirect_reference::~indirect_reference(){
	if (deallocator_ != nullptr){
		deallocator_();
		deallocator_ = nullptr;
	}
}

std::size_t cminus::memory::indirect_reference::write_address(std::size_t value){
	return runtime::object::memory_object->write_scalar(address_, value);
}

std::size_t cminus::memory::indirect_reference::write_ownership(std::shared_ptr<reference> target){
	if (target == nullptr || owned_ != nullptr || write_address(target->get_address()) == 0u)
		return 0u;

	owned_ = target;
	type_ = type_->convert(type::object::conversion_type::remove_ref, type_);

	return type_->get_size();
}

std::size_t cminus::memory::indirect_reference::get_address() const{
	return runtime::object::memory_object->read_scalar<std::size_t>(address_);
}

cminus::memory::rval_reference::rval_reference(std::shared_ptr<type::object> type)
	: reference(0u, type->convert(type::object::conversion_type::remove_ref_const, type), attribute::collection::list_type{}, nullptr){
	is_lvalue_ = false;
}

cminus::memory::rval_reference::rval_reference(std::size_t address, std::shared_ptr<type::object> type)
	: reference(address, type->convert(type::object::conversion_type::remove_ref_const, type), attribute::collection::list_type{}, nullptr){
	is_lvalue_ = false;
}

cminus::memory::rval_reference::~rval_reference() = default;
