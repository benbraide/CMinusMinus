#pragma once

#include "function_declaration_base.h"

namespace cminus::declaration{
	class function_group_base : public object{
	public:
		virtual ~function_group_base() = default;

		virtual std::size_t get_address() const = 0;

		virtual void add(std::shared_ptr<function_base> entry) = 0;
	};
}
