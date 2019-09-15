#include "io_stream_reader.h"

cminus::io::stream_reader::stream_reader(std::istream &buffer)
	: buffer_(buffer){}

cminus::io::stream_reader::~stream_reader() = default;

std::size_t cminus::io::stream_reader::get_position() const{
	return buffer_.tellg();
}

std::size_t cminus::io::stream_reader::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::stream_reader::read(std::byte *buffer, std::size_t size) const{
	buffer_.read(reinterpret_cast<char *>(buffer), size);
	return size;
}

bool cminus::io::stream_reader::can_read_scalar_(std::size_t size, std::type_info &type_info) const{
	return (type_info != typeid(wchar_t));
}

std::size_t cminus::io::stream_reader::read_scalar_(std::byte *buffer, std::size_t size, const std::type_info &type_info) const{
	if (&type_info == &typeid(bool))
		buffer_ >> *reinterpret_cast<bool *>(buffer);
	else if (&type_info == &typeid(char))
		buffer_ >> *reinterpret_cast<char *>(buffer);
	else if (&type_info == &typeid(unsigned char))
		buffer_ >> *reinterpret_cast<unsigned char *>(buffer);
	else if (&type_info == &typeid(signed char))
		buffer_ >> *reinterpret_cast<signed char *>(buffer);
	else if (&type_info == &typeid(__int16))
		buffer_ >> *reinterpret_cast<__int16 *>(buffer);
	else if (&type_info == &typeid(unsigned __int16))
		buffer_ >> *reinterpret_cast<unsigned __int16 *>(buffer);
	else if (&type_info == &typeid(__int32))
		buffer_ >> *reinterpret_cast<__int32 *>(buffer);
	else if (&type_info == &typeid(unsigned __int32))
		buffer_ >> *reinterpret_cast<unsigned __int32 *>(buffer);
	else if (&type_info == &typeid(__int64))
		buffer_ >> *reinterpret_cast<__int64 *>(buffer);
	else if (&type_info == &typeid(unsigned __int64))
		buffer_ >> *reinterpret_cast<unsigned __int64 *>(buffer);
	else if (&type_info == &typeid(float))
		buffer_ >> *reinterpret_cast<float *>(buffer);
	else if (&type_info == &typeid(double))
		buffer_ >> *reinterpret_cast<double *>(buffer);
	else if (&type_info == &typeid(long double))
		buffer_ >> *reinterpret_cast<long double *>(buffer);
	else//Not supported
		size = 0u;

	return size;
}

cminus::io::wide_stream_reader::wide_stream_reader(std::wistream &buffer)
	: buffer_(buffer){}

cminus::io::wide_stream_reader::~wide_stream_reader() = default;

std::size_t cminus::io::wide_stream_reader::get_position() const{
	return buffer_.tellg();
}

std::size_t cminus::io::wide_stream_reader::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::wide_stream_reader::read(std::byte *buffer, std::size_t size) const{
	buffer_.read(reinterpret_cast<wchar_t *>(buffer), (size / sizeof(wchar_t)));
	return (size - (size % sizeof(wchar_t)));
}

std::size_t cminus::io::wide_stream_reader::read_buffer(char *buffer, std::size_t count) const{
	return 0u;
}

std::size_t cminus::io::wide_stream_reader::read_buffer(wchar_t *buffer, std::size_t count) const{
	buffer_.read(buffer, count);
	return (sizeof(wchar_t) * count);
}

bool cminus::io::wide_stream_reader::can_read_scalar_(std::size_t size, std::type_info &type_info) const{
	return (type_info != typeid(char) && type_info != typeid(unsigned char) && type_info != typeid(signed char));
}

std::size_t cminus::io::wide_stream_reader::read_scalar_(std::byte *buffer, std::size_t size, const std::type_info &type_info) const{
	if (&type_info == &typeid(bool))
		buffer_ >> *reinterpret_cast<bool *>(buffer);
	else if (&type_info == &typeid(wchar_t))
		buffer_ >> *reinterpret_cast<wchar_t *>(buffer);
	else if (&type_info == &typeid(__int16))
		buffer_ >> *reinterpret_cast<__int16 *>(buffer);
	else if (&type_info == &typeid(unsigned __int16))
		buffer_ >> *reinterpret_cast<unsigned __int16 *>(buffer);
	else if (&type_info == &typeid(__int32))
		buffer_ >> *reinterpret_cast<__int32 *>(buffer);
	else if (&type_info == &typeid(unsigned __int32))
		buffer_ >> *reinterpret_cast<unsigned __int32 *>(buffer);
	else if (&type_info == &typeid(__int64))
		buffer_ >> *reinterpret_cast<__int64 *>(buffer);
	else if (&type_info == &typeid(unsigned __int64))
		buffer_ >> *reinterpret_cast<unsigned __int64 *>(buffer);
	else if (&type_info == &typeid(float))
		buffer_ >> *reinterpret_cast<float *>(buffer);
	else if (&type_info == &typeid(double))
		buffer_ >> *reinterpret_cast<double *>(buffer);
	else if (&type_info == &typeid(long double))
		buffer_ >> *reinterpret_cast<long double *>(buffer);
	else//Not supported
		size = 0u;

	return size;
}
