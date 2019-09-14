#pragma once

#include <fstream>

#include "io_stream_binary_reader.h"

namespace cminus::io{
	class file_binary_reader : public stream_binary_reader{
	public:
		explicit file_binary_reader(const std::string &name);

		virtual ~file_binary_reader();

	protected:
		std::ifstream named_buffer_;
	};
}
