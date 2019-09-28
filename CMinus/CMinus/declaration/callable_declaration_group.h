#pragma once

#include "callable_declaration.h"

namespace cminus::declaration{
	class callable_group{
	public:
		virtual ~callable_group();

		virtual callable::id_type get_id() const = 0;

		virtual const std::string &get_name() const = 0;

		virtual storage::object *get_parent() const = 0;

		virtual std::size_t get_address() const = 0;

		virtual std::shared_ptr<type::object> get_type() const = 0;

		virtual void add(std::shared_ptr<callable> entry);

		virtual std::shared_ptr<callable> find(const type::object &target_type) const = 0;

		virtual std::shared_ptr<callable> find(const std::list<std::shared_ptr<memory::reference>> &args, std::size_t *count = nullptr) const = 0;

		virtual std::shared_ptr<memory::reference> call(const std::list<std::shared_ptr<memory::reference>> &args) const;

	protected:
		virtual void add_(std::shared_ptr<callable> entry) = 0;
	};
}
