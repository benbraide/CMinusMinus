#pragma once

#include "../memory/memory_object.h"

namespace cminus::runtime{
	struct object{
		static memory::object *memory_object;
		static thread_local bool is_system;
	};
}
