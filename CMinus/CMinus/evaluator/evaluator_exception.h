#pragma once

#include <string>
#include <exception>

namespace cminus::evaluator::exception{
	enum class code{
		nil,
		invalid_type,
		inferred_type,
		incompatible_rval,
		rval_assignment,
		const_assignment,
		rval_ref_assignment,
		const_ref_assignment,
		unsupported_op,
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

	class invalid_type : public base{
	public:
		invalid_type();

		virtual ~invalid_type();

		virtual code get_code() const override;
	};

	class inferred_type : public base{
	public:
		inferred_type();

		virtual ~inferred_type();

		virtual code get_code() const override;
	};

	class incompatible_rval : public base{
	public:
		incompatible_rval();

		virtual ~incompatible_rval();

		virtual code get_code() const override;
	};

	class rval_assignment : public base{
	public:
		rval_assignment();

		virtual ~rval_assignment();

		virtual code get_code() const override;
	};

	class const_assignment : public base{
	public:
		const_assignment();

		virtual ~const_assignment();

		virtual code get_code() const override;
	};

	class rval_ref_assignment : public base{
	public:
		rval_ref_assignment();

		virtual ~rval_ref_assignment();

		virtual code get_code() const override;
	};

	class const_ref_assignment : public base{
	public:
		const_ref_assignment();

		virtual ~const_ref_assignment();

		virtual code get_code() const override;
	};

	class unsupported_op : public base{
	public:
		unsupported_op();

		virtual ~unsupported_op();

		virtual code get_code() const override;
	};
}
