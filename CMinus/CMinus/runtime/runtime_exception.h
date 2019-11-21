#pragma once

#include <string>
#include <memory>

#include "../exception_base.h"

namespace cminus::memory{
	class reference;
}

namespace cminus::runtime::exception{
	enum class code{
		nil,
		return_interrupt,
		control_interrupt,
		bad_constructor,
		bad_destructor,
		bad_constructor_init_list,
		bad_scope_left,
		bad_member_access_left,
		bad_pointer_member_access_left,
		bad_control_condition,
		out_of_range,
		not_supported,
	};

	class base : public exception_base{
	public:
		using exception_base::exception_base;

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

	class control_interrupt : public base{
	public:
		enum class value_type{
			nil,
			break_,
			continue_,
		};

		explicit control_interrupt(value_type value);

		virtual ~control_interrupt();

		virtual code get_code() const override;

		virtual value_type get_value() const;

	protected:
		value_type value_;
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

	class bad_control_condition : public base{
	public:
		bad_control_condition();

		virtual ~bad_control_condition();

		virtual code get_code() const override;
	};

	class out_of_range : public base{
	public:
		out_of_range();

		virtual ~out_of_range();

		virtual code get_code() const override;
	};

	class not_supported : public base{
	public:
		not_supported();

		virtual ~not_supported();

		virtual code get_code() const override;
	};
}
