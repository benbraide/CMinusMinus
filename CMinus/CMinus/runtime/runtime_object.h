#pragma once

#include "../memory/memory_object.h"

#include "runtime_exception.h"

namespace cminus::storage{
	class object;
	class global;
}

namespace cminus::runtime{
	struct object{
		static memory::object *memory_object;
		static thread_local bool is_system;

		static storage::global *global_storage;
		static thread_local storage::object *current_storage;
	};
}
