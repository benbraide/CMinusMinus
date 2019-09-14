#pragma once

#include <ostream>

#include "io_writer.h"

namespace cminus::io{
	class stream_writer : public writer{
	public:
		using char_type = char;

		using writer::write;

		explicit stream_writer(std::ostream &buffer);

		virtual ~stream_writer();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

	protected:
		virtual std::size_t write_scalar_(const std::byte *buffer, std::size_t size, const std::type_info &type_info) override;

		virtual void fill_(char value, std::size_t size);

		std::ostream &buffer_;
	};

	class wide_stream_writer : public writer{
	public:
		using char_type = wchar_t;

		using writer::write;
		using writer::write_buffer;

		explicit wide_stream_writer(std::wostream &buffer);

		virtual ~wide_stream_writer();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

		virtual std::size_t write_buffer(const char *buffer, std::size_t count) override;

		virtual std::size_t write_buffer(const wchar_t *buffer, std::size_t count) override;

	protected:
		virtual std::size_t write_scalar_(const std::byte *buffer, std::size_t size, const std::type_info &type_info) override;

		virtual void fill_(wchar_t value, std::size_t size);

		std::wostream &buffer_;
	};

	class tabbed_stream_writer_base{
	public:
		virtual ~tabbed_stream_writer_base() = default;

		virtual void write_tabs() = 0;

		virtual void set_tab_size(std::size_t value) = 0;

		virtual std::size_t tab() = 0;

		virtual std::size_t untab() = 0;

		virtual std::size_t get_tab_size() const = 0;

		virtual std::size_t get_tab_count() const = 0;
	};

	template <class stream_writer_type>
	class tabbed_stream_writer : public stream_writer_type, public tabbed_stream_writer_base{
	public:
		using char_type = typename stream_writer_type::char_type;

		template <typename stream_type>
		explicit tabbed_stream_writer(stream_type &stream)
			: stream_writer_type(stream){}

		virtual ~tabbed_stream_writer() = default;

		virtual void write_tabs() override{
			if (0u < (tab_size_ * tab_count_))
				stream_writer_type::fill_(fill_char_, (tab_size_ * tab_count_));
		}

		virtual void set_tab_size(std::size_t value) override{
			tab_size_ = value;
		}

		virtual std::size_t tab() override{
			return ++tab_count_;
		}

		virtual std::size_t untab() override{
			if (0u < tab_count_)
				--tab_count_;
			return tab_count_;
		}

		virtual std::size_t get_tab_size() const override{
			return tab_size_;
		}

		virtual std::size_t get_tab_count() const override{
			return tab_count_;
		}

		virtual void set_fill_char(char_type value){
			fill_char_ = value;
		}

		virtual char_type get_fill_char() const{
			return fill_char_;
		}

	protected:
		char_type fill_char_ = static_cast<char_type>(' ');
		std::size_t tab_size_ = 2u;
		std::size_t tab_count_ = 0u;
	};
}
