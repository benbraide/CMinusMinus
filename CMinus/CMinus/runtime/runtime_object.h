#pragma once

#include "../memory/memory_object.h"

#include "runtime_exception.h"

namespace cminus::storage{
	class object;
	class global;
}

namespace cminus::runtime{
	struct flags{
		static const unsigned int nil						= (0u << 0x0000u);
		static const unsigned int system					= (1u << 0x0000u);
		static const unsigned int allow_access				= (1u << 0x0001u);
		static const unsigned int silent_write				= (1u << 0x0002u);
		static const unsigned int silent_read				= (1u << 0x0003u);
		static const unsigned int silent_io					= (1u << 0x0004u);
	};

	struct object{
		static memory::object *memory_object;
		static thread_local unsigned int state;

		static storage::global *global_storage;
		static thread_local storage::object *current_storage;
	};
}
