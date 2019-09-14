#pragma once

#include <fstream>

#include "io_stream_writer.h"

namespace cminus::io{
	class file_writer : public stream_writer{
	public:
		explicit file_writer(const std::string &name);

		virtual ~file_writer();

	protected:
		std::ofstream file_buffer_;
	};
}
