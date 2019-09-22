#pragma once

#include <exception>

namespace cminus::declaration::exception{
	enum class code{
		nil,
		initialization_required,
		function_redefinition,
		function_redeclaration,
		function_not_found,
		function_not_defined,
		ambiguous_function_call,
		bad_parameter_list,
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

	class initialization_required : public base{
	public:
		initialization_required();

		virtual ~initialization_required();

		virtual code get_code() const override;
	};

	class function_redefinition : public base{
	public:
		function_redefinition();

		virtual ~function_redefinition();

		virtual code get_code() const override;
	};

	class function_redeclaration : public base{
	public:
		function_redeclaration();

		virtual ~function_redeclaration();

		virtual code get_code() const override;
	};

	class function_not_found : public base{
	public:
		function_not_found();

		virtual ~function_not_found();

		virtual code get_code() const override;
	};

	class function_not_defined : public base{
	public:
		function_not_defined();

		virtual ~function_not_defined();

		virtual code get_code() const override;
	};

	class ambiguous_function_call : public base{
	public:
		ambiguous_function_call();

		virtual ~ambiguous_function_call();

		virtual code get_code() const override;
	};

	class bad_parameter_list : public base{
	public:
		bad_parameter_list();

		virtual ~bad_parameter_list();

		virtual code get_code() const override;
	};
}
