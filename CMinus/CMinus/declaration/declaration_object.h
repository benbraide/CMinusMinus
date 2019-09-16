#pragma once

#include <string>

#include "../attribute/attribute_collection.h"

namespace cminus::declaration{
	class object{
	public:
		template <typename attributes_type>
		explicit object(const std::string &name, std::shared_ptr<type::object> type, const attributes_type &attributes)
			: name_(name), type_(type), attributes_(attributes){}

		virtual ~object();

		virtual const std::string &get_name() const;

		virtual std::shared_ptr<type::object> get_type() const;

		virtual const attribute::collection &get_attributes() const;

		virtual attribute::collection &get_attributes();

	protected:
		std::string name_;
		std::shared_ptr<type::object> type_;
		attribute::collection attributes_;
	};
}
