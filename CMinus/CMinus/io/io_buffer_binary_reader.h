#pragma once

#include <algorithm>

#include "io_binary_reader.h"

namespace cminus::io{
	class buffer_binary_reader : public binary_reader{
	public:
		using binary_reader::read;

		buffer_binary_reader(const std::byte *buffer, std::size_t size);

		virtual ~buffer_binary_reader();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t read(std::byte *buffer, std::size_t size) override;

	protected:
		const std::byte *buffer_;
		std::size_t size_;
		std::size_t position_ = 0u;
	};
}
