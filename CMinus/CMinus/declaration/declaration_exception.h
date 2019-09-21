#pragma once

#include <exception>

namespace cminus::declaration::exception{
	enum class code{
		nil,
		initialization_required,
		function_redefinition,
		function_redeclaration
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
}
