#include "io_stream_binary_writer.h"

cminus::io::stream_binary_writer::stream_binary_writer(std::ostream &buffer)
	: buffer_(buffer){}

cminus::io::stream_binary_writer::~stream_binary_writer() = default;

std::size_t cminus::io::stream_binary_writer::get_position() const{
	return buffer_.tellp();
}

std::size_t cminus::io::stream_binary_writer::get_size() const{
	return static_cast<std::size_t>(-1);
}

std::size_t cminus::io::stream_binary_writer::set(std::byte value, std::size_t size){
	std::fill_n(std::ostreambuf_iterator(buffer_), size, static_cast<char>(value));
	return size;
}

std::size_t cminus::io::stream_binary_writer::write(const std::byte *buffer, std::size_t size){
	buffer_.write(reinterpret_cast<const char *>(buffer), size);
	return size;
}
