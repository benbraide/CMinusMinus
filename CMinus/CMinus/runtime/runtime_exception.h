#pragma once

#include <string>
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
		bad_constructor_init_list,
		bad_scope_left,
		bad_member_access_left,
		bad_pointer_member_access_left,
		out_of_range,
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

		unnamed(code code, const char *message);

		unnamed(code code, const std::string &message);

		virtual ~unnamed();

		virtual const char *what() const override;

		virtual code get_code() const override;

	protected:
		code code_;
		std::string message_;
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

	class bad_constructor_init_list : public base{
	public:
		bad_constructor_init_list();

		virtual ~bad_constructor_init_list();

		virtual code get_code() const override;
	};

	class bad_scope_left : public base{
	public:
		bad_scope_left();

		virtual ~bad_scope_left();

		virtual code get_code() const override;
	};

	class bad_member_access_left : public base{
	public:
		bad_member_access_left();

		virtual ~bad_member_access_left();

		virtual code get_code() const override;
	};

	class bad_pointer_member_access_left : public base{
	public:
		bad_pointer_member_access_left();

		virtual ~bad_pointer_member_access_left();

		virtual code get_code() const override;
	};

	class out_of_range : public base{
	public:
		out_of_range();

		virtual ~out_of_range();

		virtual code get_code() const override;
	};
}
