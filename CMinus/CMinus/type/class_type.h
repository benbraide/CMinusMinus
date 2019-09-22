#pragma once

#include "../storage/storage_object.h"
#include "../declaration/declaration_object.h"

namespace cminus::type{
	class class_ : public type::object, public storage::object{
	public:
		using type_base = type::object;
		using storage_base = storage::object;

		struct base_type_info{
			unsigned int access;
			std::size_t address_offset;
			std::shared_ptr<class_> value;
		};

		struct member_variable_info{
			std::size_t address_offset;
			std::shared_ptr<declaration::variable> value;
		};

		class_(const std::string &name, storage_base *parent);

		virtual ~class_();

		virtual const std::string &get_name() const override;

		virtual storage_base *get_parent() const override;

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::shared_ptr<memory::reference> get_default_value(std::shared_ptr<type_base> self) const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const type_base &target) const override;

		virtual int get_score(const type_base &target) const override;

		virtual std::size_t compute_base_offset(const type_base &base_type) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;

		virtual std::shared_ptr<type_base> convert(conversion_type type, std::shared_ptr<type_base> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const type_base *arg = nullptr) const override;

		virtual bool add_base(unsigned int access, std::shared_ptr<class_> value);

		using storage_base::find;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, std::shared_ptr<memory::reference> context) const;

		virtual const member_variable_info *find_member(const std::string &name) const;

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const override;

		virtual void add_(std::shared_ptr<declaration::variable> entry, std::size_t address) override;

		virtual bool exists_(const std::string &name, entry_type type) const override;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name) const override;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name, std::shared_ptr<memory::reference> context) const;

		virtual const member_variable_info *find_member_(const std::string &name) const;

		std::list<member_variable_info> member_variables_;
		std::unordered_map<std::string, member_variable_info *> member_variables_map_;

		std::list<base_type_info> base_types_;
		std::unordered_map<std::string, base_type_info *> base_types_map_;

		std::size_t size_ = 0u;
		std::size_t member_size_ = 0u;
	};
}
