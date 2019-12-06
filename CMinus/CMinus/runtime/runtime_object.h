#pragma once

#include "../memory/memory_object.h"

#include "runtime_exception.h"

namespace cminus::memory{
	class reference;
}

namespace cminus::storage{
	class object;
	class global;
}

namespace cminus::runtime{
	struct flags{
		static const unsigned int nil						= (0u << 0x0000u);
		static const unsigned int system					= (1u << 0x0000u);
		static const unsigned int kernel					= (1u << 0x0001u);
		static const unsigned int allow_access				= (1u << 0x0002u);
		static const unsigned int silent_write				= (1u << 0x0003u);
		static const unsigned int silent_read				= (1u << 0x0004u);
		static const unsigned int silent_io					= (1u << 0x0005u);
		static const unsigned int ignore_rval_ref			= (1u << 0x0006u);
		static const unsigned int ignore_const_ref			= (1u << 0x0007u);
		static const unsigned int ignore_constructible		= (1u << 0x0008u);
	};

	struct object{
		static memory::object *memory_object;
		static thread_local unsigned int state;

		static storage::global *global_storage;
		static thread_local storage::object *current_storage;

		static thread_local std::shared_ptr<memory::reference> current_exception;
		static thread_local memory::block *system_block;
	};
}
