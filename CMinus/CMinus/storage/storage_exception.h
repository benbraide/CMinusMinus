#pragma once

#include <exception>

namespace cminus::storage::exception{
	enum class code{
		nil,
		duplicate_entry,
		entry_not_found,
		unnamed_entry,
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

	class duplicate_entry : public base{
	public:
		duplicate_entry();

		virtual ~duplicate_entry();

		virtual code get_code() const override;
	};

	class entry_not_found : public base{
	public:
		entry_not_found();

		virtual ~entry_not_found();

		virtual code get_code() const override;
	};

	class unnamed_entry : public base{
	public:
		unnamed_entry();

		virtual ~unnamed_entry();

		virtual code get_code() const override;
	};
}
