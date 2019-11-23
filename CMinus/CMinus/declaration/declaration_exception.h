#pragma once

#include <string>

#include "../exception_base.h"

namespace cminus::declaration::exception{
	enum class code{
		nil,
		bad_declaration,
		initialization_required,
		function_redefinition,
		function_redeclaration,
		function_expected,
		constructor_expected,
		destructor_expected,
		function_not_found,
		function_not_defined,
		ambiguous_function_call,
		bad_parameter_list,
		bad_init_list,
		void_function_value_return,
		value_function_no_return,
		invalid_function_return,
		deleted_function_call,
		deleted_constructor_call,
		deleted_destructor_call,
		deleted_operator_call,
	};

	class base : public exception_base{
	public:
		using exception_base::exception_base;

		virtual ~base();

		virtual std::shared_ptr<memory::reference> create_value() const override;

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

	class bad_declaration : public base{
	public:
		bad_declaration();

		virtual ~bad_declaration();

		virtual code get_code() const override;
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

	class function_expected : public base{
	public:
		function_expected();

		virtual ~function_expected();

		virtual code get_code() const override;
	};

	class constructor_expected : public base{
	public:
		constructor_expected();

		virtual ~constructor_expected();

		virtual code get_code() const override;
	};

	class destructor_expected : public base{
	public:
		destructor_expected();

		virtual ~destructor_expected();

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

	class bad_init_list : public base{
	public:
		bad_init_list();

		virtual ~bad_init_list();

		virtual code get_code() const override;
	};

	class void_function_value_return : public base{
	public:
		void_function_value_return();

		virtual ~void_function_value_return();

		virtual code get_code() const override;
	};

	class value_function_no_return : public base{
	public:
		value_function_no_return();

		virtual ~value_function_no_return();

		virtual code get_code() const override;
	};

	class invalid_function_return : public base{
	public:
		invalid_function_return();

		virtual ~invalid_function_return();

		virtual code get_code() const override;
	};

	class deleted_function_call : public base{
	public:
		deleted_function_call();

		virtual ~deleted_function_call();

		virtual code get_code() const override;
	};

	class deleted_constructor_call : public base{
	public:
		deleted_constructor_call();

		virtual ~deleted_constructor_call();

		virtual code get_code() const override;
	};

	class deleted_destructor_call : public base{
	public:
		deleted_destructor_call();

		virtual ~deleted_destructor_call();

		virtual code get_code() const override;
	};

	class deleted_operator_call : public base{
	public:
		deleted_operator_call();

		virtual ~deleted_operator_call();

		virtual code get_code() const override;
	};
}
