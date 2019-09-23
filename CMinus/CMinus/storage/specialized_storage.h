#pragma once

#include "../declaration/function_declaration_base.h"

#include "storage_object.h"

namespace cminus::storage{
	class class_member : public object{
	public:
		class_member(const declaration::function_base &owner, std::shared_ptr<memory::reference> context);

		virtual ~class_member();

		virtual const std::string &get_name() const override;

		virtual object *get_parent() const override;

		virtual std::shared_ptr<memory::reference> get_context() const;

		virtual const declaration::function_base &get_owner() const;

	protected:
		const declaration::function_base &owner_;
		std::shared_ptr<memory::reference> context_;
	};
}
