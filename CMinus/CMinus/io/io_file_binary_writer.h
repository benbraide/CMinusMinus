#pragma once

#include <fstream>

#include "io_stream_binary_writer.h"

namespace cminus::io{
	class file_binary_writer : public stream_binary_writer{
	public:
		explicit file_binary_writer(const std::string &name);

		virtual ~file_binary_writer();

	protected:
		std::ofstream named_buffer_;
	};
}
