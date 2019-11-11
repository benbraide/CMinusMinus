#pragma once

#include <memory>
#include <algorithm>

#include "../io/io_binary_writer.h"
#include "../io/io_binary_reader.h"

#include "memory_exception.h"

namespace cminus::memory{
	class object;

	class block : public io::binary_writer, public io::binary_reader{
	public:
		block(std::size_t address, std::byte *buffer, std::size_t size);

		block(std::size_t address, std::size_t size);

		virtual ~block();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

		virtual std::size_t write(const io::binary_reader &buffer, std::size_t size);

		virtual std::size_t read(std::byte *buffer, std::size_t size) const override;

		virtual std::size_t read(io::binary_writer &buffer, std::size_t size) const;

		virtual std::size_t get_address() const;

		virtual std::byte *get_data() const;

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const;

		virtual bool is_write_protected() const;

		virtual bool is_access_protected() const;

		virtual bool is_resizable() const;

	protected:
		friend class object;

		virtual void before_write_() const;

		virtual void before_read_() const;

		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const;

		std::size_t address_;
		std::byte *buffer_;
		std::size_t size_;
	};

	class write_protected_block : public block{
	public:
		write_protected_block(std::size_t address, std::byte *buffer, std::size_t size);

		write_protected_block(std::size_t address, std::size_t size);

		virtual ~write_protected_block();

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const override;

		virtual bool is_write_protected() const;

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	class access_protected_block : public block{
	public:
		access_protected_block(std::size_t address, std::byte *buffer, std::size_t size);

		access_protected_block(std::size_t address, std::size_t size);

		virtual ~access_protected_block();

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const override;

		virtual bool is_access_protected() const;

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	class protected_block : public block{
	public:
		protected_block(std::size_t address, std::byte *buffer, std::size_t size);

		protected_block(std::size_t address, std::size_t size);

		virtual ~protected_block();

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const override;

		virtual bool is_write_protected() const;

		virtual bool is_access_protected() const;

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	class null_block : public protected_block{
	public:
		null_block(std::size_t address, std::byte *buffer, std::size_t size);

		null_block(std::size_t address, std::size_t size);

		virtual ~null_block();

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const override;

		virtual bool is_write_protected() const;

		virtual bool is_access_protected() const;

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	class free_block : public protected_block{
	public:
		free_block(std::size_t address, std::byte *buffer, std::size_t size);

		free_block(std::size_t address, std::size_t size);

		virtual ~free_block();

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	class heap_block : public block{
	public:
		heap_block(std::size_t address, std::byte *buffer, std::size_t size);

		heap_block(std::size_t address, std::size_t size);

		virtual ~heap_block();

		virtual std::shared_ptr<block> get_offset_block(std::size_t value) const override;

		virtual bool is_resizable() const override;

	protected:
		virtual std::shared_ptr<block> get_block_(std::size_t address, std::byte *buffer, std::size_t size) const override;
	};

	template <class base_type>
	class data_block : public base_type{
	public:
		data_block(std::size_t address, std::size_t size)
			: base_type(address, nullptr, size){
			if (0u < base_type::size_){
				data_ = std::make_unique<std::byte[]>(base_type::size_);
				base_type::buffer_ = data_.get();
			}
		}

		virtual ~data_block() = default;

	protected:
		std::unique_ptr<std::byte[]> data_;
	};

	template <>
	class data_block<free_block>;

	template <class base_type>
	class data_block<data_block<base_type>>;
}
