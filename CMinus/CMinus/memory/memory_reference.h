#pragma once

#include "../runtime/runtime_object.h"
#include "../runtime/runtime_value_guard.h"

#include "../attribute/attribute_collection.h"
#include "../declaration/function_declaration_group_base.h"

#include "memory_block.h"

namespace cminus::memory{
	class reference : public io::binary_writer, public io::binary_reader{
	public:
		template <typename attributes_type>
		reference(std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: type_(type), context_(context), attributes_(attributes, type){
			allocate_memory_();
		}

		template <typename attributes_type>
		reference(std::size_t address, std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: type_(type), context_(context), attributes_(attributes, type), address_(address){}

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

		virtual std::shared_ptr<type::object> get_type() const;

		virtual std::shared_ptr<reference> apply_offset(std::size_t value, std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<reference> clone() const;

		virtual std::shared_ptr<reference> bound_context(std::shared_ptr<reference> value) const;

		virtual std::shared_ptr<reference> get_context() const;

		virtual const attribute::collection &get_attributes() const;

		virtual attribute::collection &get_attributes();

		virtual std::size_t get_address() const;

		virtual bool is_lvalue() const;

		virtual bool is_const() const;

	protected:
		virtual void allocate_memory_();

		virtual std::shared_ptr<block> allocate_block_() const;

		virtual std::size_t get_memory_size_() const;

		std::shared_ptr<type::object> type_;
		std::shared_ptr<reference> context_;

		attribute::collection attributes_;
		std::size_t address_ = 0u;

		bool is_lvalue_ = true;
		std::function<void()> deallocator_;
	};

	class undefined_reference : public reference{
	public:
		explicit undefined_reference(std::shared_ptr<reference> context);

		virtual ~undefined_reference();
	};

	class function_reference : public reference{
	public:
		function_reference(declaration::function_group_base &entry, std::shared_ptr<reference> context);

		function_reference(std::size_t address, std::shared_ptr<type::object> type, std::shared_ptr<reference> context);

		virtual ~function_reference();

		virtual const declaration::function_group_base *get_entry() const;
	};

	class indirect_reference : public reference{
	public:
		template <typename attributes_type>
		indirect_reference(std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: reference(0u, type, attributes, context){
			allocate_memory_();
		}

		template <typename attributes_type>
		indirect_reference(std::size_t address, std::shared_ptr<type::object> type, const attributes_type &attributes, std::shared_ptr<reference> context)
			: reference(address, type, attributes, context){}

		virtual ~indirect_reference();

		virtual std::size_t write_address(std::size_t value) override;

		virtual std::size_t write_ownership(std::shared_ptr<reference> target) override;

		virtual std::size_t get_address() const override;

	protected:
		std::shared_ptr<reference> owned_;
	};

	class rval_reference : public reference{
	public:
		explicit rval_reference(std::shared_ptr<type::object> type);

		rval_reference(std::size_t address, std::shared_ptr<type::object> type);

		virtual ~rval_reference();
	};

	template <class value_type>
	class scalar_reference : public rval_reference{
	public:
		scalar_reference(std::shared_ptr<type::object> type, value_type value)
			: rval_reference(0u, type){
			allocate_memory_();
			runtime::value_guard guard(runtime::object::is_system, true);
			runtime::object::memory_object->write_scalar(address_, value);
		}

		virtual ~scalar_reference() = default;

	protected:
		virtual std::shared_ptr<block> allocate_block_() const override{
			return runtime::object::memory_object->allocate_write_protected_block(get_memory_size_());
		}
	};
}
