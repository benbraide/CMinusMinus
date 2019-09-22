#pragma once

#include "../memory/memory_reference.h"
#include "../storage/storage_object.h"

#include "function_declaration_group_base.h"

namespace cminus::declaration{
	class function_group : public function_group_base{
	public:
		using map_type = std::unordered_map<function_base *, std::shared_ptr<function_base>>;

		function_group(const std::string &name, storage::object *parent, std::size_t address);

		virtual ~function_group();

		virtual const std::string &get_name() const override;

		virtual storage::object *get_parent() const override;

		virtual std::size_t get_address() const override;

		virtual std::shared_ptr<type::object> get_type() const override;

		virtual void add(std::shared_ptr<function_base> entry) override;

		virtual std::shared_ptr<function_base> find(const type::object &target_type) const;

		virtual std::shared_ptr<function_base> find(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t *count = nullptr) const;

		virtual std::shared_ptr<memory::reference> call(const std::vector<std::shared_ptr<memory::reference>> &args) const;

	protected:
		virtual map_type::const_iterator find_(const type::object &target_type) const;

		std::string name_;
		storage::object *parent_;
		std::size_t address_;
		map_type entries_;
	};
}
