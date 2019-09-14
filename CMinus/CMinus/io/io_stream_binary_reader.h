#pragma once

#include <istream>

#include "io_binary_reader.h"

namespace cminus::io{
	class stream_binary_reader : public binary_reader{
	public:
		explicit stream_binary_reader(std::istream &buffer);

		virtual ~stream_binary_reader();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t read(std::byte *buffer, std::size_t size) override;

	protected:
		std::istream &buffer_;
	};
}
