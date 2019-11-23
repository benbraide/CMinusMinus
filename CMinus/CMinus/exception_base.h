#pragma once

#include <memory>
#include <exception>

namespace cminus::memory{
	class reference;
}

namespace cminus{
	class exception_base : public std::exception{
	public:
		using exception::exception;

		virtual ~exception_base() = default;

		virtual std::shared_ptr<memory::reference> create_value() const = 0;
	};
}
