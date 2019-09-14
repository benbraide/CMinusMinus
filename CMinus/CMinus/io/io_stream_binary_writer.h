#pragma once

#include <ostream>

#include "io_binary_writer.h"

namespace cminus::io{
	class stream_binary_writer : public binary_writer{
	public:
		using binary_writer::write;

		explicit stream_binary_writer(std::ostream &buffer);

		virtual ~stream_binary_writer();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

	protected:
		std::ostream &buffer_;
	};
}
