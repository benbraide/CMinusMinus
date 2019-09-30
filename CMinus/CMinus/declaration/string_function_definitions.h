#pragma once

#include <algorithm>
#include <string_view>

#include "string_function_declarations.h"

namespace cminus::declaration::string{
	class helper{
	public:
		enum class allocation_type{
			nil,
			extend,
			shrink,
		};

		struct member_info{
			std::shared_ptr<memory::reference> data;
			std::shared_ptr<memory::reference> size;
		};

		struct address_info{
			std::size_t data;
			std::size_t size;
		};

		struct data_address_siz_value_info{
			std::size_t data;
			std::size_t size;
		};

		struct value_info{
			char *data;
			std::size_t size;
		};

		static void retrieve_info(member_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(address_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(data_address_siz_value_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(value_info &info, std::shared_ptr<memory::reference> context);

		static std::size_t allocate_block(std::size_t buffer_size, allocation_type how, std::size_t split_index);

		static void assign(const char *buffer, std::size_t buffer_size, bool fill);

		static void insert(const char *buffer, std::size_t buffer_size, std::size_t offset, bool fill);

		static void erase(std::size_t buffer_size, std::size_t offset);

		static std::size_t find_buffer(const char *buffer, std::size_t buffer_size, std::size_t offset);

		template <typename target_type>
		static target_type read_value(const std::string &name){
			return runtime::object::current_storage->find(name, true)->read_scalar<target_type>();
		}
	};

	class default_constructor_def : public default_constructor{
	public:
		using default_constructor::default_constructor;

		virtual ~default_constructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class copy_constructor_def : public copy_constructor{
	public:
		using copy_constructor::copy_constructor;

		virtual ~copy_constructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class sub_copy_constructor_def : public sub_copy_constructor{
	public:
		using sub_copy_constructor::sub_copy_constructor;

		virtual ~sub_copy_constructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class buffer_constructor_def : public buffer_constructor{
	public:
		using buffer_constructor::buffer_constructor;

		virtual ~buffer_constructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class fill_constructor_def : public fill_constructor{
	public:
		using fill_constructor::fill_constructor;

		virtual ~fill_constructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class destructor_def : public destructor{
	public:
		using destructor::destructor;

		virtual ~destructor_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class empty_def : public empty{
	public:
		using empty::empty;

		virtual ~empty_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class size_def : public size{
	public:
		using size::size;

		virtual ~size_def();

	protected:
		virtual void evaluate_body_() const override;
	};
}
