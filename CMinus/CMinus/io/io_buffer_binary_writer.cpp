#include "io_buffer_binary_writer.h"

cminus::io::buffer_binary_writer::buffer_binary_writer(std::byte *buffer, std::size_t size)
	: buffer_(buffer), size_(size){}

cminus::io::buffer_binary_writer::~buffer_binary_writer() = default;

std::size_t cminus::io::buffer_binary_writer::get_position() const{
	return position_;
}

std::size_t cminus::io::buffer_binary_writer::get_size() const{
	return size_;
}

std::size_t cminus::io::buffer_binary_writer::set(std::byte value, std::size_t size){
	if ((size = std::min((size_ - position_), size)) == 0u)
		return 0u;//Nothing to copy

	std::memset((buffer_ + position_), static_cast<int>(value), size);
	position_ += size;//Advance position

	return size;
}

std::size_t cminus::io::buffer_binary_writer::write(const std::byte *buffer, std::size_t size){
	if ((size = std::min((size_ - position_), size)) == 0u)
		return 0u;//Nothing to copy

	std::memcpy((buffer_ + position_), buffer, size);
	position_ += size;//Advance position

	return size;
}
