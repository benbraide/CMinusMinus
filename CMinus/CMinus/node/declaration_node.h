#pragma once

#include "../declaration/variable_declaration.h"

#include "node_object.h"

namespace cminus::node{
	class variable_declaration : public object{
	public:
		explicit variable_declaration(std::shared_ptr<declaration::variable> value);

		virtual ~variable_declaration();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<declaration::variable> get_decl_value() const;

	protected:
		std::shared_ptr<declaration::variable> value_;
	};
}
