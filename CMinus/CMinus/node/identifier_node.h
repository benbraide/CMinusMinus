#pragma once

#include "node_object.h"

namespace cminus::node{
	class identifier : public object{
	public:
		explicit identifier(const std::string &value);

		virtual ~identifier();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<memory::reference> lookup(std::shared_ptr<memory::reference> context, bool search_tree) const override;

		virtual std::string evaluate_as_name() const override;

		virtual std::shared_ptr<type::object> evaluate_as_type(bool search_tree) const override;

		virtual storage::object *evaluate_as_storage(bool search_tree) const override;

	protected:
		virtual storage::object *get_storage_(bool search_tree) const;

		virtual bool should_search_tree_() const;

		std::string value_;
	};

	class scoped_identifier : public identifier{
	public:
		scoped_identifier(std::shared_ptr<object> left, const std::string &value);

		virtual ~scoped_identifier();

		virtual std::string evaluate_as_name() const override;

	protected:
		virtual storage::object *get_storage_(bool search_tree) const override;

		virtual bool should_search_tree_() const override;

		std::shared_ptr<object> left_;
	};
}
