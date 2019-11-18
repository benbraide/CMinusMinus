#pragma once

#include <algorithm>
#include <string_view>

#include "../storage/global_storage.h"
#include "../storage/specialized_storage.h"

#include "string_function_declarations.h"

namespace cminus::declaration::string{
	class helper{
	public:
		enum class allocation_type{
			nil,
			extend,
			shrink,
			compute,
		};

		struct essential_info{
			std::shared_ptr<memory::reference> data;
			std::shared_ptr<memory::reference> size;
			std::size_t data_address;
			std::size_t size_value;
		};

		struct member_info{
			std::shared_ptr<memory::reference> data;
			std::shared_ptr<memory::reference> size;
		};

		struct address_info{
			std::size_t data;
			std::size_t size;
		};

		struct data_address_size_value_info{
			std::size_t data;
			std::size_t size;
		};

		struct value_info{
			char *data;
			std::size_t size;
		};

		static void retrieve_info(essential_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(member_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(address_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(data_address_size_value_info &info, std::shared_ptr<memory::reference> context);

		static void retrieve_info(value_info &info, std::shared_ptr<memory::reference> context);

		static std::shared_ptr<memory::reference> retrieve_data(std::shared_ptr<memory::reference> context);

		static std::shared_ptr<memory::reference> retrieve_size(std::shared_ptr<memory::reference> context);

		static std::size_t allocate_block(std::size_t buffer_size, allocation_type how, std::size_t split_index, std::shared_ptr<memory::reference> context,
			bool write_protected = false, std::size_t *allocated_address = nullptr);

		static void assign(const char *buffer, std::size_t buffer_size, bool compute, bool fill, std::shared_ptr<memory::reference> context);

		static void insert(const char *buffer, std::size_t buffer_size, std::size_t offset, bool fill, std::shared_ptr<memory::reference> context);

		static void erase(std::size_t buffer_size, std::size_t offset, std::shared_ptr<memory::reference> context);

		static std::size_t find(const char *buffer, std::size_t buffer_size, std::size_t offset, std::shared_ptr<memory::reference> context, bool is_last);

		static std::size_t find(std::shared_ptr<memory::reference> other, std::size_t pos, std::size_t size, std::size_t offset, std::shared_ptr<memory::reference> context, bool is_last);

		template <typename... args_types>
		static std::shared_ptr<memory::reference> get_found(args_types &&... args){
			return std::make_shared<memory::scalar_reference<std::size_t>>(
				runtime::object::global_storage->get_size_type(),
				find(std::forward<args_types>(args)...)
			);
		}

		static char *read_data(const std::string &name, std::shared_ptr<memory::reference> context);

		template <typename target_type>
		static target_type read_value(const std::string &name, std::shared_ptr<memory::reference> context){
			if (context != nullptr){
				runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::allow_access));
				return runtime::object::global_storage->get_raw_string_type()->find(name, context, false)->read_scalar<target_type>();
			}
			
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

	class data_def : public data{
	public:
		using data::data;

		virtual ~data_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class begin_def : public begin{
	public:
		using begin::begin;

		virtual ~begin_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class end_def : public end{
	public:
		using end::end;

		virtual ~end_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class at_def : public at{
	public:
		using at::at;

		virtual ~at_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class find_copy_def : public find_copy{
	public:
		using find_copy::find_copy;

		virtual ~find_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class find_sub_copy_def : public find_sub_copy{
	public:
		using find_sub_copy::find_sub_copy;

		virtual ~find_sub_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class find_buffer_def : public find_buffer{
	public:
		using find_buffer::find_buffer;

		virtual ~find_buffer_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class find_sized_buffer_def : public find_sized_buffer{
	public:
		using find_sized_buffer::find_sized_buffer;

		virtual ~find_sized_buffer_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class find_single_def : public find_single{
	public:
		using find_single::find_single;

		virtual ~find_single_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class get_sub_def : public get_sub{
	public:
		using get_sub::get_sub;

		virtual ~get_sub_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class resize_def : public resize{
	public:
		using resize::resize;

		virtual ~resize_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class erase_def : public erase{
	public:
		using erase::erase;

		virtual ~erase_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class clear_def : public clear{
	public:
		using clear::clear;

		virtual ~clear_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class swap_def : public swap{
	public:
		using swap::swap;

		virtual ~swap_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class assign_copy_def : public assign_copy{
	public:
		using assign_copy::assign_copy;

		virtual ~assign_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class assign_sub_copy_def : public assign_sub_copy{
	public:
		using assign_sub_copy::assign_sub_copy;

		virtual ~assign_sub_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class assign_buffer_def : public assign_buffer{
	public:
		using assign_buffer::assign_buffer;

		virtual ~assign_buffer_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class assign_fill_def : public assign_fill{
	public:
		using assign_fill::assign_fill;

		virtual ~assign_fill_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class insert_copy_def : public insert_copy{
	public:
		using insert_copy::insert_copy;

		virtual ~insert_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class insert_sub_copy_def : public insert_sub_copy{
	public:
		using insert_sub_copy::insert_sub_copy;

		virtual ~insert_sub_copy_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class insert_buffer_def : public insert_buffer{
	public:
		using insert_buffer::insert_buffer;

		virtual ~insert_buffer_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class insert_fill_def : public insert_fill{
	public:
		using insert_fill::insert_fill;

		virtual ~insert_fill_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	class index_operator_def : public index_operator{
	public:
		using index_operator::index_operator;

		virtual ~index_operator_def();

	protected:
		virtual void evaluate_body_() const override;
	};

	template <class base_type>
	class operator_def : public base_type{
	public:
		using base_type::base_type;

		virtual ~operator_def() = default;

	protected:
		virtual void evaluate_body_() const override{
			throw runtime::exception::return_interrupt(runtime::object::global_storage->get_evaluator(evaluator::object::id_type::string)->evaluate_binary(
				base_type::get_op(),
				runtime::object::current_storage->get_first_of<storage::class_member>()->get_context(),
				runtime::object::current_storage->find("other", true)
			));
		}
	};
}
