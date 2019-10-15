#pragma once

#include <exception>

namespace cminus{
	class exception_base : public std::exception{
	public:
		using exception::exception;

		virtual ~exception_base() = default;
	};
}
