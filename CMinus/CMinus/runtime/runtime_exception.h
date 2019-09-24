#pragma once

#include <memory>
#include <exception>

namespace cminus::memory{
	class reference;
}

namespace cminus::runtime::exception{
	enum class code{
		nil,
		return_interrupt,
		bad_constructor,
		bad_destructor,
	};

	class base : public std::exception{
	public:
		using exception::exception;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class unnamed : public base{
	public:
		explicit unnamed(code code);

		virtual ~unnamed();

		virtual code get_code() const override;

	protected:
		code code_;
	};

	class return_interrupt : public base{
	public:
		explicit return_interrupt(std::shared_ptr<memory::reference> value);

		virtual ~return_interrupt();

		virtual code get_code() const override;

		virtual std::shared_ptr<memory::reference> get_value() const;

	protected:
		std::shared_ptr<memory::reference> value_;
	};

	class void_return_interrupt : public return_interrupt{
	public:
		void_return_interrupt();

		virtual ~void_return_interrupt();
	};

	class bad_constructor : public base{
	public:
		bad_constructor();

		virtual ~bad_constructor();

		virtual code get_code() const override;
	};

	class bad_destructor : public base{
	public:
		bad_destructor();

		virtual ~bad_destructor();

		virtual code get_code() const override;
	};
}
