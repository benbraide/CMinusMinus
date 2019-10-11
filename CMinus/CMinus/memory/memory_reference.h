#pragma once

#include "../runtime/runtime_object.h"
#include "../runtime/runtime_value_guard.h"
#include "../attribute/attribute_collection.h"

#include "memory_block.h"


namespace cminus::declaration{
	class callable_group;
}

namespace cminus::memory{
	class reference : public io::binary_writer, public io::binary_reader{
	public:
		template <typename attributes_type>
		reference(std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: context_(context), attributes_(attributes, type){
			init_type_(type);
			allocate_memory_();
		}

		explicit reference(std::shared_ptr<type::object> type);

		template <typename attributes_type>
		reference(std::size_t address, std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: context_(context), attributes_(attributes, type), address_(address){
			init_type_(type);
		}

		reference(std::size_t address, std::shared_ptr<type::object> type);

		virtual ~reference();

		virtual std::size_t get_position() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

		virtual std::size_t write(const io::binary_reader &buffer, std::size_t size);

		virtual std::size_t write(std::size_t source_address, std::size_t size);

		virtual std::size_t write_address(std::size_t value);

		virtual std::size_t write_ownership(std::shared_ptr<reference> target);

		virtual std::size_t read(std::byte *buffer, std::size_t size) const override;

		virtual std::size_t read(io::binary_writer &buffer, std::size_t size) const;

		virtual std::size_t read(std::size_t destination_address, std::size_t size) const;

		virtual void set_constructed_state();
		
		virtual bool is_constructed() const;

		virtual std::shared_ptr<type::object> get_type() const;

		virtual std::shared_ptr<type::object> get_decl_type() const;

		virtual std::shared_ptr<reference> apply_offset(std::size_t value, std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<reference> clone() const;

		virtual std::shared_ptr<reference> bound_context(std::shared_ptr<reference> value) const;

		virtual std::shared_ptr<reference> get_context() const;

		virtual const attribute::collection &get_attributes() const;

		virtual attribute::collection &get_attributes();

		virtual std::size_t get_address() const;

		virtual std::size_t get_indirect_address() const;

		virtual bool is_lvalue() const;

		virtual bool is_const() const;

		virtual bool is_nan() const;

	protected:
		virtual void init_type_(std::shared_ptr<type::object> type);

		virtual void allocate_memory_();

		virtual std::shared_ptr<block> allocate_block_() const;

		virtual std::size_t get_memory_size_() const;

		virtual void destruct_();

		std::shared_ptr<type::object> type_;
		std::shared_ptr<reference> context_;

		attribute::collection attributes_;
		std::size_t address_ = 0u;

		bool is_lvalue_ = true;
		bool is_constructed_ = false;
		std::function<void()> deallocator_;
	};

	class undefined_reference : public reference{
	public:
		explicit undefined_reference(std::shared_ptr<reference> context);

		virtual ~undefined_reference();
	};

	class function_reference : public reference{
	public:
		function_reference(declaration::callable_group &entry, std::shared_ptr<reference> context);

		function_reference(declaration::callable_group &entry, std::shared_ptr<type::object> type, std::shared_ptr<reference> context);

		function_reference(std::size_t address, std::shared_ptr<type::object> type, std::shared_ptr<reference> context);

		virtual ~function_reference();

		virtual declaration::callable_group *get_entry() const;
	};

	class indirect_reference : public reference{
	public:
		template <typename attributes_type>
		indirect_reference(std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: reference(0u, type, attributes, context){
			allocate_memory_();
		}

		explicit indirect_reference(std::shared_ptr<type::object> type);

		template <typename attributes_type>
		indirect_reference(std::size_t address, std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: reference(address, type, attributes, context){}

		indirect_reference(std::size_t address, std::shared_ptr<type::object> type);

		virtual ~indirect_reference();

		virtual std::size_t write_address(std::size_t value) override;

		virtual std::size_t write_ownership(std::shared_ptr<reference> target) override;

		virtual std::shared_ptr<type::object> get_decl_type() const override;

		virtual std::size_t get_address() const override;

	protected:
		virtual std::size_t get_memory_size_() const override;

		std::shared_ptr<reference> owned_;
	};

	class rval_reference : public reference{
	public:
		explicit rval_reference(std::shared_ptr<type::object> type);

		rval_reference(std::size_t address, std::shared_ptr<type::object> type);

		virtual ~rval_reference();
	};

	class write_protected_rval_reference : public rval_reference{
	public:
		explicit write_protected_rval_reference(std::shared_ptr<type::object> type);

		virtual ~write_protected_rval_reference();

	protected:
		virtual std::shared_ptr<block> allocate_block_() const override;
	};

	template <class value_type>
	class scalar_reference : public write_protected_rval_reference{
	public:
		scalar_reference(std::shared_ptr<type::object> type, value_type value)
			: write_protected_rval_reference(type){
			runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
			runtime::object::memory_object->write_scalar(address_, value);
		}

		virtual ~scalar_reference() = default;

		virtual bool is_const() const override{
			return true;
		}
	};
}
