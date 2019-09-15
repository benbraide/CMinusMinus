#pragma once

#include <exception>

namespace cminus::memory::exception{
	enum class code{
		nil,
		out_of_address_space,
		allocation_failure,
		access_protected,
		write_protected,
		block_not_found,
		block_misaligned,
		block_not_resizable,
		invalid_size,
		incompatible_types,
		uninitialized_memory,
	};

	class base : public std::exception{
	public:
		using exception::exception;

		virtual ~base() = default;

		virtual code get_code() const = 0;

		virtual std::size_t get_address() const = 0;
	};

	class meta_no_address : public base{
	public:
		using base::base;

		virtual ~meta_no_address();

		virtual std::size_t get_address() const override;
	};

	class meta_address : public base{
	public:
		meta_address(const char *message, std::size_t address);

		virtual ~meta_address();

		virtual std::size_t get_address() const override;

	protected:
		std::size_t address_;
	};

	class unnamed : public meta_address{
	public:
		unnamed(code code, std::size_t address);

		virtual ~unnamed();

		virtual code get_code() const override;

	protected:
		code code_;
	};

	class out_of_address_space : public meta_no_address{
	public:
		out_of_address_space();

		virtual ~out_of_address_space();

		virtual code get_code() const override;
	};

	class allocation_failure : public meta_no_address{
	public:
		allocation_failure();

		virtual ~allocation_failure();

		virtual code get_code() const override;
	};

	class invalid_size : public meta_no_address{
	public:
		invalid_size();

		virtual ~invalid_size();

		virtual code get_code() const override;
	};

	class access_protected : public meta_address{
	public:
		explicit access_protected(std::size_t address);

		virtual ~access_protected();

		virtual code get_code() const override;
	};

	class write_protected : public meta_address{
	public:
		explicit write_protected(std::size_t address);

		virtual ~write_protected();

		virtual code get_code() const override;
	};

	class block_not_found : public meta_address{
	public:
		explicit block_not_found(std::size_t address);

		virtual ~block_not_found();

		virtual code get_code() const override;
	};

	class block_misaligned : public meta_address{
	public:
		explicit block_misaligned(std::size_t address);

		virtual ~block_misaligned();

		virtual code get_code() const override;
	};

	class block_not_resizable : public meta_address{
	public:
		explicit block_not_resizable(std::size_t address);

		virtual ~block_not_resizable();

		virtual code get_code() const override;
	};
}
