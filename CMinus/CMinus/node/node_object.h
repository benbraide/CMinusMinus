#pragma once

#include <functional>

#include "../memory/memory_reference.h"

namespace cminus::node{
	class object{
	public:
		virtual ~object();

		virtual std::size_t get_list_count() const;

		virtual void traverse_list(const std::function<void(const object &)> &callback) const;

		virtual std::shared_ptr<memory::reference> evaluate() const = 0;
	};
}
