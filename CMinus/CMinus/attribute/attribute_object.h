#pragma once

#include <string>
#include <memory>

#include "attribute_options.h"

namespace cminus::storage{
	class object;
}

namespace cminus::type{
	class object;
}

namespace cminus::memory{
	class reference;
}

namespace cminus::attribute{
	class object{
	public:
		object(const std::string &name, storage::object *parent);

		virtual ~object();

		virtual const std::string &get_name() const;

		virtual std::string get_qname() const;

		virtual storage::object *get_parent() const;

		virtual bool is_primitive() const;

		virtual bool is_same(const object &other) const;

		virtual bool applies_to_type(const type::object &target_type) const;

		virtual std::string get_message() const;

		virtual query_result query(stage_type stage, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg) const = 0;

		static query_result handle_query_result(stage_type stage, const object &attr, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg);

		static query_result handle_query_result(query_result result, const object &target, std::shared_ptr<memory::reference> arg);

	protected:
		std::string name_;
		storage::object *parent_;
	};
}
