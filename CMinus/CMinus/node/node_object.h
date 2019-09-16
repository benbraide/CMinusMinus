#pragma once

#include "../memory/memory_reference.h"

namespace cminus::node{
	class object{
	public:
		virtual ~object() = default;

		virtual std::shared_ptr<memory::reference> evaluate() const = 0;
	};
}
