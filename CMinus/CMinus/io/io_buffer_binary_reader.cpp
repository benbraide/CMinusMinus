#include "io_buffer_binary_reader.h"

cminus::io::buffer_binary_reader::buffer_binary_reader(const std::byte *buffer, std::size_t size)
	: buffer_(buffer), size_(size){}

cminus::io::buffer_binary_reader::~buffer_binary_reader() = default;

std::size_t cminus::io::buffer_binary_reader::get_position() const{
	return position_;
}

std::size_t cminus::io::buffer_binary_reader::get_size() const{
	return size_;
}

std::size_t cminus::io::buffer_binary_reader::read(std::byte *buffer, std::size_t size){
	if ((size = std::min((size_ - position_), size)) == 0u)
		return 0u;//Nothing to copy

	std::memcpy(buffer, (buffer_ + position_), size);
	position_ += size;//Advance position

	return size;
}
