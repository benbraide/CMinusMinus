#pragma once

#include <list>
#include <shared_mutex>
#include <unordered_map>

#include "memory_block.h"

namespace cminus::memory{
	class object{
	public:
		using block_list_type = std::list<std::shared_ptr<block>>;

		object();

		std::size_t allocate_block(std::size_t size);

		std::size_t allocate_write_protected_block(std::size_t size);

		std::size_t allocate_access_protected_block(std::size_t size);

		std::size_t allocate_protected_block(std::size_t size);

		std::size_t allocate_no_data_protected_block(std::size_t size);

		std::size_t allocate_heap_block(std::size_t size);

		std::size_t reallocate_heap_block(std::size_t address, std::size_t size);

		void deallocate_block(std::size_t address);

		std::size_t set(std::size_t destination_address, std::byte value, std::size_t size);

		std::size_t write(std::size_t destination_address, const std::byte *buffer, std::size_t size);

		std::size_t write(std::size_t destination_address, const io::binary_reader &buffer, std::size_t size);

		std::size_t write(std::size_t destination_address, std::size_t source_address, std::size_t size);

		template <typename target_type>
		std::size_t write_scalar(std::size_t destination_address, const target_type &buffer){
			return write(destination_address, reinterpret_cast<const std::byte *>(&buffer), sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(std::size_t destination_address, const target_type *buffer, std::size_t count){
			return write(destination_address, (const std::byte *)buffer, (sizeof(target_type) * count));
		}

		std::size_t write_buffer(std::size_t destination_address, const char *buffer);

		std::size_t read(std::size_t source_address, std::byte *buffer, std::size_t size) const;

		std::size_t read(std::size_t source_address, io::binary_writer &buffer, std::size_t size) const;

		std::size_t read(std::size_t source_address, std::size_t destination_address, std::size_t size) const;

		template <typename target_type>
		target_type read_scalar(std::size_t destination_address) const{
			auto buffer = target_type();
			return ((read(destination_address, (std::byte *)(&buffer), sizeof(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(std::size_t destination_address, target_type *buffer, std::size_t count) const{
			return read(destination_address, (std::byte *)buffer, (sizeof(target_type) * count));
		}

		std::shared_ptr<block> get_block(std::size_t address) const;

		std::shared_ptr<block> find_block(std::size_t address) const;

		std::size_t get_next_address() const;

		std::size_t get_size(std::size_t address, bool find = false) const;

		bool is_write_protected(std::size_t address, bool find = false) const;

		bool is_access_protected(std::size_t address, bool find = false) const;

		bool is_resizable(std::size_t address, bool find = false) const;

	private:
		template <typename block_type>
		std::size_t allocate_block_(std::size_t size){
			if (size == 0u)
				throw exception::invalid_size();

			auto free_it = blocks_.begin();
			for (; free_it != blocks_.end(); ++free_it){
				if (auto free_block = dynamic_cast<memory::free_block *>(free_it->get()); free_block != nullptr && size <= free_block->size_)
					break;
			}

			std::size_t address = 0u;
			if (free_it != blocks_.end()){
				address = (*free_it)->address_;
				(*free_it)->size_ -= size;
				(*free_it)->address_ += size;
			}
			else{//Use next address
				if ((std::numeric_limits<std::size_t>::max() - size) < next_address_)
					throw exception::out_of_address_space();

				address = next_address_;
				next_address_ += size;
			}

			auto block = std::make_shared<block_type>(address, size);
			if (block == nullptr)
				throw exception::allocation_failure();

			if (free_it != blocks_.end()){//Insert
				if (0u < (*free_it)->size_){
					if (blocks_.size() < blocks_.max_size())
						blocks_.insert(free_it, block);
					else
						throw exception::out_of_address_space();
				}
				else//Free block is completely used
					*free_it = block;
			}
			else if (blocks_.size() < blocks_.max_size())//Append
				blocks_.push_back(block);
			else//Error
				throw exception::out_of_address_space();

			return block->address_;
		}

		std::size_t reallocate_heap_block_(std::size_t address, std::size_t size);

		void deallocate_block_(std::size_t address);

		std::size_t set_(std::size_t destination_address, std::byte value, std::size_t size);

		std::size_t write_(std::size_t destination_address, const std::byte *buffer, std::size_t size);

		std::size_t write_(std::size_t destination_address, const io::binary_reader &buffer, std::size_t size);

		std::size_t read_(std::size_t source_address, std::byte *buffer, std::size_t size) const;

		std::size_t read_(std::size_t source_address, io::binary_writer &buffer, std::size_t size) const;

		std::size_t read_(std::size_t source_address, std::size_t destination_address, std::size_t size) const;

		std::shared_ptr<block> get_block_(std::size_t address, block_list_type::const_iterator *out_it) const;

		std::shared_ptr<block> find_block_(std::size_t address, block_list_type::const_iterator *out_it) const;

		bool advance_source_and_destination_(std::shared_ptr<block> &source, block_list_type::const_iterator &source_it, std::shared_ptr<block> &destination,
			block_list_type::const_iterator &destination_it, std::size_t value) const;

		std::size_t next_address_ = 0u;
		block_list_type blocks_;
		mutable std::shared_mutex lock_;
	};
}
