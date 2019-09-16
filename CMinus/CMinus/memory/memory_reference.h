#pragma once

#include "../runtime/runtime_object.h"
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

		virtual std::size_t set(std::byte value, std::size_t size) override;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) override;

		virtual std::size_t write(const io::binary_reader &buffer, std::size_t size);

		virtual std::size_t write(std::size_t source_address, std::size_t size);

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

	protected:
		virtual void allocate_memory_();

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

		virtual ~function_reference();
	};
}
