#include "io_file_writer.h"

cminus::io::file_writer::file_writer(const std::string &name)
	: stream_writer(file_buffer_), file_buffer_(name){}

cminus::io::file_writer::~file_writer() = default;
