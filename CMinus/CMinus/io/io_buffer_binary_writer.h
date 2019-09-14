#pragma once

#include <algorithm>

#include "io_binary_writer.h"

namespace cminus::io{
	class buffer_binary_writer : public binary_writer{
	public:
		using binary_writer::write;

		buffer_binary_writer(std::byte *buffer, std::size_t size);

		virtual ~buffer_binary_writer();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

	protected:
		std::byte *buffer_;
		std::size_t size_;
		std::size_t position_ = 0u;
	};
}
