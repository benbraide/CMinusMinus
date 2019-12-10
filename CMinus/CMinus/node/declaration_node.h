#pragma once

#include "../declaration/variable_declaration.h"

#include "node_object.h"

namespace cminus::node{
	class variable_declaration : public object{
	public:
		variable_declaration(const std::string &name, std::shared_ptr<object> type, std::shared_ptr<object> attributes, unsigned int flags, std::shared_ptr<object> initialization);

		virtual ~variable_declaration();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<declaration::variable> get_decl_value() const;

	protected:
		std::string name_;
		std::shared_ptr<object> type_;
		std::shared_ptr<object> attributes_;

		unsigned int flags_;
		std::shared_ptr<object> initialization_;
		mutable std::shared_ptr<declaration::variable> value_;
	};
}
