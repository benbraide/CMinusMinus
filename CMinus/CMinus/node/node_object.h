#pragma once

#include <functional>

#include "../memory/memory_reference.h"

namespace cminus::node{
	class object{
	public:
		virtual ~object();

		virtual std::size_t get_list_count() const;

		virtual void traverse_list(const std::function<void(const object &)> &callback) const;

		virtual std::shared_ptr<memory::reference> evaluate() const = 0;

		virtual std::shared_ptr<memory::reference> lookup(std::shared_ptr<memory::reference> context, bool search_tree) const;

		virtual std::string evaluate_as_name() const;

		virtual std::shared_ptr<type::object> evaluate_as_type(bool search_tree) const;

		virtual storage::object *evaluate_as_storage(bool search_tree) const;
	};
}
