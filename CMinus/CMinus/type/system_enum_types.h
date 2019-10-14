#pragma once

#include "enum_type.h"

namespace cminus::type{
	class compare_result : public enum_{
	public:
		compare_result();

		virtual ~compare_result();
	};

	class attribute_stage : public enum_{
	public:
		attribute_stage();

		virtual ~attribute_stage();
	};

	class attribute_result : public enum_{
	public:
		attribute_result();

		virtual ~attribute_result();
	};
}
