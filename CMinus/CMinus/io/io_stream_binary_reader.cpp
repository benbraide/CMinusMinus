#include "io_stream_binary_reader.h"

cminus::io::stream_binary_reader::stream_binary_reader(std::istream &buffer)
	: buffer_(buffer){}

cminus::io::stream_binary_reader::~stream_binary_reader() = default;

std::size_t cminus::io::stream_binary_reader::get_position() const{
	return buffer_.tellg();
}

std::size_t cminus::io::stream_binary_reader::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::stream_binary_reader::read(std::byte *buffer, std::size_t size) const{
	buffer_.read(reinterpret_cast<char *>(buffer), size);
	return size;
}
