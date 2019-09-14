#include "io_file_reader.h"

cminus::io::file_reader::file_reader(const std::string &name)
	: stream_reader(file_buffer_), file_buffer_(name){}

cminus::io::file_reader::~file_reader() = default;
