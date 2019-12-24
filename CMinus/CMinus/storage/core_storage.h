#pragma once

#include "../declaration/core_functions.h"

#include "global_storage.h"
#include "compiled_storage.h"

namespace cminus::storage{
	class core : public compiled{
	public:
		core();

		virtual ~core();
	};
}
