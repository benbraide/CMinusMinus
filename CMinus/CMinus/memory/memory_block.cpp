#include "../type/type_object.h"
#include "../runtime/runtime_object.h"

#include "memory_block.h"

cminus::memory::type_value::type_value(std::shared_ptr<type::object> value)
	: value_(value){}

cminus::memory::type_value::~type_value() = default;

std::shared_ptr<cminus::type::object> cminus::memory::type_value::get_value() const{
	return value_;
}

cminus::memory::block::block(std::size_t address, std::byte *buffer, std::size_t size)
	: address_(address), buffer_(buffer), size_(size){}

cminus::memory::block::block(std::size_t address, std::size_t size)
	: block(address, nullptr, size){}

cminus::memory::block::~block() = default;

std::size_t cminus::memory::block::get_position() const{
	return 0u;
}

std::size_t cminus::memory::block::get_size() const{
	return size_;
}

std::size_t cminus::memory::block::set(std::byte value, std::size_t size){
	if ((size = std::min(size_, size)) == 0u)
		return 0u;//Nothing to copy

	before_write_();
	std::memset(buffer_, static_cast<int>(value), size);

	return size;
}

std::size_t cminus::memory::block::write(const std::byte *buffer, std::size_t size){
	if ((size = std::min(size_, size)) == 0u)
		return 0u;//Nothing to copy

	before_write_();
	std::memcpy(buffer_, buffer, size);

	return size;
}

std::size_t cminus::memory::block::write(const io::binary_reader &buffer, std::size_t size){
	if ((size = std::min(size_, size)) == 0u)
		return 0u;//Nothing to copy

	before_write_();
	size = buffer.read(buffer_, size);

	if (auto block_buffer = dynamic_cast<const block *>(&buffer); block_buffer != nullptr)
		value_ = block_buffer->value_;

	return size;
}

std::size_t cminus::memory::block::read(std::byte *buffer, std::size_t size) const{
	if ((size = std::min(size_, size)) == 0u)
		return 0u;//Nothing to copy

	before_read_();
	std::memcpy(buffer, buffer_, size);

	return size;
}

std::size_t cminus::memory::block::read(io::binary_writer &buffer, std::size_t size) const{
	if ((size = std::min(size_, size)) == 0u)
		return 0u;//Nothing to copy

	before_read_();
	size = buffer.write(buffer_, size);

	if (auto block_buffer = dynamic_cast<block *>(&buffer); block_buffer != nullptr)
		block_buffer->value_ = value_;

	return size;
}

std::size_t cminus::memory::block::get_address() const{
	return address_;
}

std::byte *cminus::memory::block::get_data() const{
	return buffer_;
}

std::shared_ptr<cminus::memory::block> cminus::memory::block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? get_block_((address_ + value), ((buffer_ == nullptr) ? nullptr : (buffer_ + value)), (size_ - value)) : nullptr);
}

bool cminus::memory::block::contains(std::size_t address) const{
	return (address_ <= address && address < (address_ + size_));
}

bool cminus::memory::block::is_write_protected() const{
	return (buffer_ == nullptr);
}

bool cminus::memory::block::is_access_protected() const{
	return (buffer_ == nullptr);
}

bool cminus::memory::block::is_resizable() const{
	return false;
}

void cminus::memory::block::set_value(std::shared_ptr<memory::value> value){
	value_ = value;
}

std::shared_ptr<cminus::memory::value> cminus::memory::block::get_value() const{
	return value_;
}

void cminus::memory::block::before_write_() const{
	if (is_write_protected())
		throw exception::write_protected(address_);
}

void cminus::memory::block::before_read_() const{
	if (is_access_protected())
		throw exception::access_protected(address_);
}

std::shared_ptr<cminus::memory::block> cminus::memory::block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::block>(address, buffer, size);
}

cminus::memory::write_protected_block::write_protected_block(std::size_t address, std::byte *buffer, std::size_t size)
	: block(address, buffer, size){}

cminus::memory::write_protected_block::write_protected_block(std::size_t address, std::size_t size)
	: write_protected_block(address, nullptr, size){}

cminus::memory::write_protected_block::~write_protected_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::write_protected_block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? std::make_shared<cminus::memory::write_protected_block>((address_ + value), (buffer_ + value), (size_ - value)) : nullptr);
}

bool cminus::memory::write_protected_block::is_write_protected() const{
	if (block::is_write_protected())
		return true;

	if ((runtime::object::state & runtime::flags::system) != 0u)
		return false;

	return (runtime::object::system_block == nullptr || !runtime::object::system_block->contains(address_));
}

std::shared_ptr<cminus::memory::block> cminus::memory::write_protected_block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::write_protected_block>(address, buffer, size);
}

cminus::memory::access_protected_block::access_protected_block(std::size_t address, std::byte *buffer, std::size_t size)
	: block(address, buffer, size){}

cminus::memory::access_protected_block::access_protected_block(std::size_t address, std::size_t size)
	: access_protected_block(address, nullptr, size){}

cminus::memory::access_protected_block::~access_protected_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::access_protected_block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? std::make_shared<cminus::memory::access_protected_block>((address_ + value), (buffer_ + value), (size_ - value)) : nullptr);
}

bool cminus::memory::access_protected_block::is_access_protected() const{
	if (block::is_access_protected())
		return true;

	if ((runtime::object::state & runtime::flags::system) != 0u)
		return false;

	return (runtime::object::system_block == nullptr || !runtime::object::system_block->contains(address_));
}

std::shared_ptr<cminus::memory::block> cminus::memory::access_protected_block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::access_protected_block>(address, buffer, size);
}

cminus::memory::protected_block::protected_block(std::size_t address, std::byte *buffer, std::size_t size)
	: block(address, buffer, size){}

cminus::memory::protected_block::protected_block(std::size_t address, std::size_t size)
	: protected_block(address, nullptr, size){}

cminus::memory::protected_block::~protected_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::protected_block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? std::make_shared<cminus::memory::protected_block>((address_ + value), (buffer_ + value), (size_ - value)) : nullptr);
}

bool cminus::memory::protected_block::is_write_protected() const{
	if (block::is_write_protected())
		return true;

	if ((runtime::object::state & runtime::flags::system) != 0u)
		return false;

	return (runtime::object::system_block == nullptr || !runtime::object::system_block->contains(address_));
}

bool cminus::memory::protected_block::is_access_protected() const{
	if (block::is_access_protected())
		return true;

	if ((runtime::object::state & runtime::flags::system) != 0u)
		return false;

	return (runtime::object::system_block == nullptr || !runtime::object::system_block->contains(address_));
}

std::shared_ptr<cminus::memory::block> cminus::memory::protected_block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::protected_block>(address, buffer, size);
}

cminus::memory::null_block::null_block(std::size_t address, std::byte *buffer, std::size_t size)
	: protected_block(address, buffer, size){}

cminus::memory::null_block::null_block(std::size_t address, std::size_t size)
	: null_block(address, nullptr, size){}

cminus::memory::null_block::~null_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::null_block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? std::make_shared<cminus::memory::null_block>((address_ + value), (buffer_ + value), (size_ - value)) : nullptr);
}

bool cminus::memory::null_block::is_write_protected() const{
	return ((runtime::object::state & runtime::flags::kernel) == 0u || block::is_write_protected());
}

bool cminus::memory::null_block::is_access_protected() const{
	return ((runtime::object::state & runtime::flags::kernel) == 0u || block::is_access_protected());
}

std::shared_ptr<cminus::memory::block> cminus::memory::null_block::get_block_(std::size_t address, std::byte * buffer, std::size_t size) const{
	return std::make_shared<memory::null_block>(address, buffer, size);
}

cminus::memory::free_block::free_block(std::size_t address, std::byte *buffer, std::size_t size)
	: free_block(address, size){}

cminus::memory::free_block::free_block(std::size_t address, std::size_t size)
	: protected_block(address, nullptr, size){}

cminus::memory::free_block::~free_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::free_block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::free_block>(address, size);
}

cminus::memory::heap_block::heap_block(std::size_t address, std::byte *buffer, std::size_t size)
	: block(address, buffer, size){}

cminus::memory::heap_block::heap_block(std::size_t address, std::size_t size)
	: heap_block(address, nullptr, size){}

cminus::memory::heap_block::~heap_block() = default;

std::shared_ptr<cminus::memory::block> cminus::memory::heap_block::get_offset_block(std::size_t value) const{
	return ((value < size_) ? std::make_shared<cminus::memory::heap_block>((address_ + value), (buffer_ + value), (size_ - value)) : nullptr);
}

bool cminus::memory::heap_block::is_resizable() const{
	return true;
}

std::shared_ptr<cminus::memory::block> cminus::memory::heap_block::get_block_(std::size_t address, std::byte *buffer, std::size_t size) const{
	return std::make_shared<memory::heap_block>(address, buffer, size);
}
