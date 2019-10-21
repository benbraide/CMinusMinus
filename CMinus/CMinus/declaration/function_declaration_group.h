#pragma once

#include "callable_declaration_group.h"

namespace cminus::declaration{
	class function_group : public callable_group{
	public:
		using list_type = std::vector<std::shared_ptr<callable>>;

		function_group(callable::id_type id, const std::string &name, storage::object *parent, std::size_t address);

		virtual ~function_group();

		virtual callable::id_type get_id() const override;

		virtual const std::string &get_name() const override;

		virtual storage::object *get_parent() const override;

		virtual std::size_t get_address() const override;

		virtual std::shared_ptr<type::object> get_type() const override;

		virtual std::shared_ptr<callable> find(const type::object &target_type) const override;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const override;

		virtual void traverse_list(const std::function<void(std::shared_ptr<callable>)> &callback) override;

	protected:
		virtual void add_(std::shared_ptr<callable> entry) override;

		virtual list_type::const_iterator find_(const type::object &target_type) const;

		callable::id_type id_;
		std::string name_;
		storage::object *parent_;
		std::size_t address_;
		list_type entries_;
	};
}
