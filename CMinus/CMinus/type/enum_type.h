#pragma once

#include "../storage/storage_object.h"
#include "../declaration/declaration_object.h"

namespace cminus::type{
	class enum_ : public type::object, public storage::unnamed_object{
	public:
		using type_base = type::object;
		using storage_base = storage::unnamed_object;

		enum_(const std::string &name, storage_base *parent);

		virtual ~enum_();

		virtual const std::string &get_name() const override;

		virtual storage::object *get_parent() const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const type_base &target) const override;

		virtual int get_score(const type_base &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;

		virtual bool is(query_type type, const type_base *arg = nullptr) const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add(const std::string &name) override;

		virtual void add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual std::shared_ptr<memory::reference> get_item(std::size_t index) const;

		virtual std::size_t get_item_index(const std::string &name) const;

	protected:
		virtual void del_(const std::string &name) override;

		virtual bool exists_(const std::string &name, entry_type type) const override;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name) const override;

		std::list<std::string> items_;
	};
}
