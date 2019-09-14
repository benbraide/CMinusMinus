#pragma once

#include "io_writer.h"

namespace cminus::io{
	class binary_writer : public writer{
	public:
		virtual ~binary_writer() = default;
	};
}
