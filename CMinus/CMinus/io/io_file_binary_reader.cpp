#include "io_file_binary_reader.h"

cminus::io::file_binary_reader::file_binary_reader(const std::string &name)
	: stream_binary_reader(named_buffer_), named_buffer_(name, std::ios::binary){}

cminus::io::file_binary_reader::~file_binary_reader() = default;
