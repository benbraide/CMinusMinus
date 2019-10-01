#include "memory_object.h"

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<data_block<block>>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_write_protected_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<data_block<write_protected_block>>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_access_protected_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<data_block<access_protected_block>>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_protected_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<data_block<protected_block>>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_no_data_protected_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<protected_block>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::allocate_heap_block(std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_<data_block<heap_block>>(size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::reallocate_heap_block(std::size_t address, std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return reallocate_heap_block_(address, size);
}

void cminus::memory::object::deallocate_block(std::size_t address){
	std::lock_guard<std::shared_mutex> guard(lock_);
	deallocate_block_(address);
}

std::size_t cminus::memory::object::set(std::size_t destination_address, std::byte value, std::size_t size){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return set_(destination_address, value, size);
}

std::size_t cminus::memory::object::write(std::size_t destination_address, const std::byte *buffer, std::size_t size){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return write_(destination_address, buffer, size);
}

std::size_t cminus::memory::object::write(std::size_t destination_address, const io::binary_reader &buffer, std::size_t size){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return write_(destination_address, buffer, size);
}

std::size_t cminus::memory::object::write(std::size_t destination_address, std::size_t source_address, std::size_t size){
	return read(source_address, destination_address, size);
}

std::size_t cminus::memory::object::write_buffer(std::size_t destination_address, const char *buffer){
	return write_buffer(destination_address, buffer, std::strlen(buffer));
}

std::size_t cminus::memory::object::read(std::size_t source_address, std::byte *buffer, std::size_t size) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return read_(source_address, buffer, size);
}

std::size_t cminus::memory::object::read(std::size_t source_address, io::binary_writer &buffer, std::size_t size) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return read_(source_address, buffer, size);
}

std::size_t cminus::memory::object::read(std::size_t source_address, std::size_t destination_address, std::size_t size) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return read_(source_address, destination_address, size);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::get_block(std::size_t address) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return get_block_(address, nullptr);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::find_block(std::size_t address) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return find_block_(address, nullptr);
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::reallocate_heap_block_(std::size_t address, std::size_t size){
	if (size == 0u)
		throw exception::invalid_size();

	auto block = get_block_(address, nullptr);
	if (block == nullptr)
		throw exception::block_not_found(address);

	if (block->size_ == size)//No changes
		return block;

	if (dynamic_cast<heap_block *>(block.get()) == nullptr)
		throw exception::block_not_resizable(address);

	deallocate_block_(address);
	auto new_block = allocate_block_<data_block<heap_block>>(size);

	if (new_block != nullptr && new_block->address_ != 0u)
		new_block->write(*block, std::min(size, block->size_));
	else
		throw exception::allocation_failure();

	return new_block;
}

void cminus::memory::object::deallocate_block_(std::size_t address){
	auto it = blocks_.end();
	if (get_block_(address, &it) == nullptr || it == blocks_.end())
		throw exception::block_not_found(address);

	(*it)->before_read_();
	auto next_it = std::next(it);

	if (next_it != blocks_.end() && dynamic_cast<free_block *>(next_it->get()) != nullptr){//Merge with next
		(*it)->size_ += (*next_it)->size_;
		blocks_.erase(next_it);
	}

	if (it != blocks_.begin() && dynamic_cast<free_block *>((next_it = std::prev(it))->get()) != nullptr){//Merge with previous
		(*next_it)->size_ += (*it)->size_;
		blocks_.erase(it);
	}
	else if (next_address_ <= (address + (*it)->size_)){//Shrink
		next_address_ = address;
		blocks_.erase(it);
	}
	else//Overwrite block
		*it = std::make_shared<free_block>(address, (*it)->size_);
}

std::size_t cminus::memory::object::set_(std::size_t destination_address, std::byte value, std::size_t size){
	if (size == 0u)
		return size;

	auto destination_it = blocks_.end();
	auto destination_block = find_block_(destination_address, &destination_it);

	if (destination_block == nullptr || destination_it == blocks_.end())
		throw exception::block_not_found(destination_address);

	std::size_t set_size = 0u;
	while (set_size < size){
		set_size += destination_block->set(value, (size - set_size));
		if (++destination_it != blocks_.end())
			destination_block = *destination_it;
		else
			break;
	}

	return set_size;
}

std::size_t cminus::memory::object::write_(std::size_t destination_address, const std::byte *buffer, std::size_t size){
	if (size == 0u)
		return size;

	auto destination_it = blocks_.end();
	auto destination_block = find_block_(destination_address, &destination_it);

	if (destination_block == nullptr || destination_it == blocks_.end())
		throw exception::block_not_found(destination_address);

	std::size_t write_size = 0u;
	while (write_size < size){
		write_size += destination_block->write((buffer + write_size), (size - write_size));
		if (++destination_it != blocks_.end())
			destination_block = *destination_it;
		else
			break;
	}

	return write_size;
}

std::size_t cminus::memory::object::write_(std::size_t destination_address, const io::binary_reader &buffer, std::size_t size){
	if (size == 0u)
		return size;

	auto destination_it = blocks_.end();
	auto destination_block = find_block_(destination_address, &destination_it);

	if (destination_block == nullptr || destination_it == blocks_.end())
		throw exception::block_not_found(destination_address);

	std::size_t write_size = 0u, previous_write_size = 0u;
	while (write_size < size){
		write_size += destination_block->write(buffer, (size - write_size));
		if (previous_write_size < write_size && ++destination_it != blocks_.end()){
			destination_block = *destination_it;
			previous_write_size = write_size;
		}
		else
			break;
	}

	return write_size;
}

std::size_t cminus::memory::object::read_(std::size_t source_address, std::byte *buffer, std::size_t size) const{
	if (size == 0u)
		return size;

	auto source_it = blocks_.end();
	auto source_block = find_block_(source_address, &source_it);

	if (source_block == nullptr || source_it == blocks_.end())
		throw exception::block_not_found(source_address);

	std::size_t read_size = 0u;
	while (read_size < size){
		read_size += source_block->read((buffer + read_size), (size - read_size));
		if (++source_it != blocks_.end())
			source_block = *source_it;
		else
			break;
	}

	return read_size;
}

std::size_t cminus::memory::object::read_(std::size_t source_address, io::binary_writer &buffer, std::size_t size) const{
	if (size == 0u)
		return size;

	auto source_it = blocks_.end();
	auto source_block = find_block_(source_address, &source_it);

	if (source_block == nullptr || source_it == blocks_.end())
		throw exception::block_not_found(source_address);

	std::size_t read_size = 0u, previous_read_size = 0u;
	while (read_size < size){
		read_size += (*source_it)->read(buffer, (size - read_size));
		if (previous_read_size < read_size && ++source_it != blocks_.end()){
			source_block = *source_it;
			previous_read_size = read_size;
		}
		else
			break;
	}

	return read_size;
}

std::size_t cminus::memory::object::read_(std::size_t source_address, std::size_t destination_address, std::size_t size) const{
	if (size == 0u)
		return size;

	auto source_it = blocks_.end();
	auto source_block = find_block_(source_address, &source_it);

	if (source_block == nullptr || source_it == blocks_.end())
		throw exception::block_not_found(source_address);

	auto destination_it = blocks_.end();
	auto destination_block = find_block_(destination_address, &destination_it);

	if (destination_block == nullptr || destination_it == blocks_.end())
		throw exception::block_not_found(destination_address);

	std::size_t read_size = 0u, current_read_size = 0u;
	while (read_size < size){
		current_read_size = source_block->read(*destination_block, (size - read_size));
		if (size <= (read_size += current_read_size))
			break;

		if (!advance_source_and_destination_(source_block, source_it, destination_block, destination_it, current_read_size))
			break;
	}

	return read_size;
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::get_block_(std::size_t address, block_list_type::const_iterator *out_it) const{
	if (out_it != nullptr)
		*out_it = blocks_.end();

	if (blocks_.empty())
		return nullptr;

	for (auto it = blocks_.begin(); it != blocks_.end(); ++it){
		if ((*it)->address_ == address){
			if (out_it != nullptr)
				*out_it = it;

			return *it;
		}
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::block> cminus::memory::object::find_block_(std::size_t address, block_list_type::const_iterator *out_it) const{
	if (out_it != nullptr)
		*out_it = blocks_.end();

	if (blocks_.empty())
		return nullptr;

	std::size_t address_offset = 0u;
	for (auto it = blocks_.begin(); it != blocks_.end(); ++it){
		if ((*it)->address_ == address){
			if (out_it != nullptr)
				*out_it = it;

			return *it;
		}

		if ((*it)->address_ <= address && (address_offset = (address - (*it)->address_)) < (*it)->size_){
			if (out_it != nullptr)
				*out_it = it;

			return (*it)->get_offset_block(address_offset);
		}
	}

	return nullptr;
}

bool cminus::memory::object::advance_source_and_destination_(std::shared_ptr<block> &source, block_list_type::const_iterator &source_it,
	std::shared_ptr<block> &destination, block_list_type::const_iterator &destination_it, std::size_t value) const{
	if (source->size_ <= value){//Use next block
		if (++source_it == blocks_.end())
			return false;

		source = *source_it;
	}
	else//Advance offset
		source = source->get_offset_block(value);

	if (destination->size_ <= value){//Use next block
		if (++destination_it == blocks_.end())
			return false;

		destination = *destination_it;
	}
	else//Advance offset
		destination = destination->get_offset_block(value);

	return true;
}
