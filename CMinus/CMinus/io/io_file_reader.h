#pragma once

#include <fstream>

#include "io_stream_reader.h"

namespace cminus::io{
	class file_reader : public stream_reader{
	public:
		explicit file_reader(const std::string &name);

		virtual ~file_reader();

	protected:
		std::ifstream file_buffer_;
	};
}
