#pragma once

#include "../runtime/runtime_object.h"
#include "../runtime/runtime_value_guard.h"
#include "../attribute/attribute_collection.h"

#include "memory_block.h"

namespace cminus::declaration{
	class object;
	class callable_group;
}

namespace cminus::memory{
	class reference{
	public:
		explicit reference(std::shared_ptr<type::object> type);

		reference(std::size_t address, std::shared_ptr<type::object> type);

		virtual ~reference();

		virtual std::size_t initialize(std::shared_ptr<reference> value);

		virtual std::size_t write(std::shared_ptr<reference> value);

		virtual std::size_t write_address(std::size_t value);

		virtual std::size_t read(std::byte *buffer, std::size_t size) const;

		template <typename target_type>
		target_type read_scalar() const{
			auto buffer = target_type();
			read((std::byte *)(&buffer), sizeof(target_type));
			return buffer;
		}

		virtual void set_constructed_state();
		
		virtual bool is_constructed() const;

		virtual std::shared_ptr<type::object> get_type() const;

		virtual std::shared_ptr<reference> apply_offset(std::size_t value, std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<reference> clone() const;

		virtual std::shared_ptr<reference> get_context() const;

		virtual std::size_t get_address() const;

		virtual std::size_t get_indirect_address() const;

		virtual std::size_t get_size() const;

		virtual bool is_lvalue() const;

		virtual bool is_const() const;

		virtual bool is_nan() const;

	protected:
		virtual void allocate_memory_();

		virtual std::shared_ptr<block> allocate_block_() const;

		virtual std::size_t get_memory_size_() const;

		virtual void before_write_(std::shared_ptr<reference> value) const;

		virtual void after_write_(std::shared_ptr<reference> value) const;

		virtual void before_read_() const;

		virtual void after_read_() const;

		virtual void destruct_();

		std::shared_ptr<type::object> type_;
		std::size_t address_ = 0u;
		std::function<void()> deallocator_;

		bool is_lvalue_ = true;
		bool is_constructed_ = false;
	};

	class undefined_reference : public reference{
	public:
		undefined_reference();

		virtual ~undefined_reference();
	};

	class declared_reference : public reference{
	public:
		explicit declared_reference(const declaration::object &declaration);

		declared_reference(std::size_t address, const declaration::object &declaration);

		virtual ~declared_reference();

		virtual const declaration::object *get_declaration() const;

	protected:
		virtual void before_write_(std::shared_ptr<reference> value) const override;

		virtual void after_write_(std::shared_ptr<reference> value) const override;

		virtual void before_read_() const override;

		virtual void after_read_() const override;

		const declaration::object *declaration_;
	};

	class member_reference : public declared_reference{
	public:
		member_reference(std::size_t address, const declaration::object &declaration, std::shared_ptr<reference> context);

		member_reference(const declaration::callable_group &declaration, std::shared_ptr<reference> context);

		member_reference(const declaration::callable_group &declaration, std::shared_ptr<type::object> type, std::shared_ptr<reference> context);

		virtual ~member_reference();

		virtual std::size_t get_indirect_address() const override;

		virtual std::shared_ptr<reference> get_context() const override;

		virtual bool is_lvalue() const override;

		virtual bool is_const() const override;

	protected:
		std::shared_ptr<reference> context_;
	};

	class indirect_reference : public declared_reference{
	public:
		explicit indirect_reference(const declaration::object &declaration);

		indirect_reference(std::size_t address, const declaration::object &declaration);

		virtual ~indirect_reference();

		virtual std::size_t initialize(std::shared_ptr<reference> value) override;

		virtual std::size_t write_address(std::size_t value) override;

		virtual std::size_t get_address() const override;

	protected:
		virtual std::size_t get_memory_size_() const override;

		std::shared_ptr<reference> owned_;
	};

	class indirect_member_reference : public indirect_reference{
	public:
		indirect_member_reference(std::size_t address, const declaration::object &declaration, std::shared_ptr<reference> context);

		virtual ~indirect_member_reference();

		virtual std::size_t get_indirect_address() const override;

		virtual std::shared_ptr<reference> get_context() const override;

		virtual bool is_lvalue() const override;

		virtual bool is_const() const override;

	protected:
		std::shared_ptr<reference> context_;
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
