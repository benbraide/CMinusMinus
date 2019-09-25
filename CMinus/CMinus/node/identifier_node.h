#pragma once

#include "node_object.h"

namespace cminus::node{
	class identifier : public object{
	public:
		explicit identifier(const std::string &value);

		virtual ~identifier();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::string evaluate_as_name(bool update_current_storage) const override;

		virtual std::shared_ptr<type::object> evaluate_as_type() const override;

		virtual storage::object *evaluate_as_storage() const override;

	protected:
		virtual storage::object *get_storage_() const;

		virtual bool should_search_tree_() const;

		std::string value_;
	};

	class scoped_identifier : public identifier{
	public:
		scoped_identifier(std::shared_ptr<object> left, const std::string &value);

		virtual ~scoped_identifier();

		virtual std::string evaluate_as_name(bool update_current_storage) const override;

	protected:
		virtual storage::object *get_storage_() const override;

		virtual bool should_search_tree_() const override;

		std::shared_ptr<object> left_;
	};
}
