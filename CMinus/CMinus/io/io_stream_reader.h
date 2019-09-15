#pragma once

#include <istream>

#include "io_reader.h"

namespace cminus::io{
	class stream_reader : public reader{
	public:
		using char_type = char;

		explicit stream_reader(std::istream &buffer);

		virtual ~stream_reader();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t read(std::byte *buffer, std::size_t size) const override;

	protected:
		virtual bool can_read_scalar_(std::size_t size, std::type_info &type_info) const override;

		virtual std::size_t read_scalar_(std::byte *buffer, std::size_t size, const std::type_info &type_info) const override;

		std::istream &buffer_;
	};

	class wide_stream_reader : public reader{
	public:
		using char_type = wchar_t;

		using reader::read_buffer;

		explicit wide_stream_reader(std::wistream &buffer);

		virtual ~wide_stream_reader();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t read(std::byte *buffer, std::size_t size) const override;

		virtual std::size_t read_buffer(char *buffer, std::size_t count) const override;

		virtual std::size_t read_buffer(wchar_t *buffer, std::size_t count) const override;

	protected:
		virtual bool can_read_scalar_(std::size_t size, std::type_info &type_info) const override;

		virtual std::size_t read_scalar_(std::byte *buffer, std::size_t size, const std::type_info &type_info) const override;

		std::wistream &buffer_;
	};
}
