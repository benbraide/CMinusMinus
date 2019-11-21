#pragma once

#include "../declaration/callable_declaration.h"

#include "storage_object.h"

namespace cminus::storage{
	class class_member : public unnamed_object{
	public:
		explicit class_member(const declaration::callable &owner);

		virtual ~class_member();

		virtual const std::string &get_name() const override;

		virtual object *get_parent() const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const override;

		virtual void set_context(std::shared_ptr<memory::reference> value);

		virtual std::shared_ptr<memory::reference> get_context() const;

		virtual const declaration::callable &get_owner() const;

	protected:
		const declaration::callable &owner_;
		std::shared_ptr<memory::reference> context_;
	};

	class control : public named_object{
	public:
		control();

		virtual ~control();

		virtual void init_inner();

		virtual void uninit_inner();

	protected:
		object *previous_ = nullptr;
		std::size_t inner_offset_ = 0u;
	};
}
