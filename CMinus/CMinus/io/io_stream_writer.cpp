#include "io_stream_writer.h"

cminus::io::stream_writer::stream_writer(std::ostream &buffer)
	: buffer_(buffer){}

cminus::io::stream_writer::~stream_writer() = default;

std::size_t cminus::io::stream_writer::get_position() const{
	return buffer_.tellp();
}

std::size_t cminus::io::stream_writer::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::stream_writer::set(std::byte value, std::size_t size){
	std::fill_n(std::ostreambuf_iterator(buffer_), size, static_cast<char>(value));
	buffer_ << std::flush;
	return size;
}

std::size_t cminus::io::stream_writer::write(const std::byte *buffer, std::size_t size){
	buffer_.write(reinterpret_cast<const char *>(buffer), size);
	buffer_ << std::flush;
	return size;
}

std::size_t cminus::io::stream_writer::write_scalar_(const std::byte *buffer, std::size_t size, const std::type_info &type_info){
	if (&type_info == &typeid(bool))
		buffer_ << *reinterpret_cast<const bool *>(buffer);
	else if (&type_info == &typeid(char))
		buffer_ << *reinterpret_cast<const char *>(buffer);
	else if (&type_info == &typeid(unsigned char))
		buffer_ << *reinterpret_cast<const unsigned char *>(buffer);
	else if (&type_info == &typeid(signed char))
		buffer_ << *reinterpret_cast<const signed char *>(buffer);
	else if (&type_info == &typeid(__int16))
		buffer_ << *reinterpret_cast<const __int16 *>(buffer);
	else if (&type_info == &typeid(unsigned __int16))
		buffer_ << *reinterpret_cast<const unsigned __int16 *>(buffer);
	else if (&type_info == &typeid(__int32))
		buffer_ << *reinterpret_cast<const __int32 *>(buffer);
	else if (&type_info == &typeid(unsigned __int32))
		buffer_ << *reinterpret_cast<const unsigned __int32 *>(buffer);
	else if (&type_info == &typeid(__int64))
		buffer_ << *reinterpret_cast<const __int64 *>(buffer);
	else if (&type_info == &typeid(unsigned __int64))
		buffer_ << *reinterpret_cast<const unsigned __int64 *>(buffer);
	else if (&type_info == &typeid(float))
		buffer_ << *reinterpret_cast<const float *>(buffer);
	else if (&type_info == &typeid(double))
		buffer_ << *reinterpret_cast<const double *>(buffer);
	else if (&type_info == &typeid(long double))
		buffer_ << *reinterpret_cast<const long double *>(buffer);
	else//Not supported
		size = 0u;

	if (0u < size)
		buffer_ << std::flush;

	return size;
}

void cminus::io::stream_writer::fill_(char value, std::size_t size){
	std::fill_n(std::ostreambuf_iterator(buffer_), size, value);
	buffer_ << std::flush;
}

cminus::io::wide_stream_writer::wide_stream_writer(std::wostream &buffer)
	: buffer_(buffer){}

cminus::io::wide_stream_writer::~wide_stream_writer() = default;

std::size_t cminus::io::wide_stream_writer::get_position() const{
	return buffer_.tellp();
}

std::size_t cminus::io::wide_stream_writer::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::wide_stream_writer::set(std::byte value, std::size_t size){
	std::fill_n(std::ostreambuf_iterator(buffer_), size, static_cast<char>(value));
	buffer_ << std::flush;
	return size;
}

std::size_t cminus::io::wide_stream_writer::write(const std::byte *buffer, std::size_t size){
	buffer_.write(reinterpret_cast<const wchar_t *>(buffer), (size / sizeof(wchar_t)));
	buffer_ << std::flush;
	return (size - (size % sizeof(wchar_t)));
}

std::size_t cminus::io::wide_stream_writer::write_buffer(const char *buffer, std::size_t count){
	for (std::size_t index = 0u; index < count; ++index)
		buffer_ << *(buffer + index);

	buffer_ << std::flush;
	return count;
}

std::size_t cminus::io::wide_stream_writer::write_buffer(const wchar_t *buffer, std::size_t count){
	buffer_.write(buffer, count);
	buffer_ << std::flush;
	return (sizeof(wchar_t) * count);
}

std::size_t cminus::io::wide_stream_writer::write_scalar_(const std::byte *buffer, std::size_t size, const std::type_info &type_info){
	if (&type_info == &typeid(bool))
		buffer_ << *reinterpret_cast<const bool *>(buffer);
	else if (&type_info == &typeid(char))
		buffer_ << *reinterpret_cast<const char *>(buffer);
	else if (&type_info == &typeid(unsigned char))
		buffer_ << *reinterpret_cast<const unsigned char *>(buffer);
	else if (&type_info == &typeid(signed char))
		buffer_ << *reinterpret_cast<const signed char *>(buffer);
	else if (&type_info == &typeid(wchar_t))
		buffer_ << *reinterpret_cast<const wchar_t *>(buffer);
	else if (&type_info == &typeid(__int16))
		buffer_ << *reinterpret_cast<const __int16 *>(buffer);
	else if (&type_info == &typeid(unsigned __int16))
		buffer_ << *reinterpret_cast<const unsigned __int16 *>(buffer);
	else if (&type_info == &typeid(__int32))
		buffer_ << *reinterpret_cast<const __int32 *>(buffer);
	else if (&type_info == &typeid(unsigned __int32))
		buffer_ << *reinterpret_cast<const unsigned __int32 *>(buffer);
	else if (&type_info == &typeid(__int64))
		buffer_ << *reinterpret_cast<const __int64 *>(buffer);
	else if (&type_info == &typeid(unsigned __int64))
		buffer_ << *reinterpret_cast<const unsigned __int64 *>(buffer);
	else if (&type_info == &typeid(float))
		buffer_ << *reinterpret_cast<const float *>(buffer);
	else if (&type_info == &typeid(double))
		buffer_ << *reinterpret_cast<const double *>(buffer);
	else if (&type_info == &typeid(long double))
		buffer_ << *reinterpret_cast<const long double *>(buffer);
	else//Not supported
		size = 0u;

	if (0u < size)
		buffer_ << std::flush;

	return size;
}

void cminus::io::wide_stream_writer::fill_(wchar_t value, std::size_t size){
	std::fill_n(std::ostreambuf_iterator(buffer_), size, value);
	buffer_ << std::flush;
}
