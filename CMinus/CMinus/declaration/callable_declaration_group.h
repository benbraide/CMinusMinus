#pragma once

#include "callable_declaration.h"

namespace cminus::declaration{
	class callable_group : public object{
	public:
		using object::object;

		virtual ~callable_group();

		virtual callable::id_type get_id() const = 0;

		virtual storage::object *get_parent() const = 0;

		virtual void set_address(std::size_t value) = 0;

		virtual std::size_t get_address() const = 0;

		virtual void add(std::shared_ptr<callable> entry);

		virtual std::shared_ptr<callable> find(const type::function &target_type) const = 0;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const = 0;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<callable::arg_info> &args) const;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<callable::arg_info> &args, std::size_t required_size) const = 0;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<std::shared_ptr<memory::reference>> &args) const;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const = 0;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<callable::arg_info> &args) const;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<callable::arg_info> &args, std::size_t required_size) const = 0;

		virtual void traverse_list(const std::function<void(std::shared_ptr<callable>)> &callback) = 0;

		virtual std::size_t get_count() const = 0;

		virtual std::shared_ptr<memory::reference> call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const;

		virtual std::shared_ptr<memory::reference> call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const;

	protected:
		virtual void add_(std::shared_ptr<callable> entry) = 0;
	};
}
