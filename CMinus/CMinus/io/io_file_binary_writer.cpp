#include "io_file_binary_writer.h"

cminus::io::file_binary_writer::file_binary_writer(const std::string &name)
	: stream_binary_writer(named_buffer_), named_buffer_(name, std::ios::binary){}

cminus::io::file_binary_writer::~file_binary_writer() = default;
