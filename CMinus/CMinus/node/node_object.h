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

		virtual std::string evaluate_as_name(bool update_current_storage) const;

		virtual std::shared_ptr<type::object> evaluate_as_type() const;

		virtual storage::object *evaluate_as_storage() const;
	};
}
