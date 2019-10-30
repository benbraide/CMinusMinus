#pragma once

#include <string>

#include "../evaluator/evaluator_exception.h"
#include "../attribute/attribute_collection.h"

#include "declaration_exception.h"

namespace cminus::declaration{
	struct flags{
		static const unsigned int nil						= (0u << 0x0000u);
		static const unsigned int static_					= (1u << 0x0000u);
		static const unsigned int tls						= (1u << 0x0001u);//Thread-local storage
		static const unsigned int private_					= (1u << 0x0002u);
		static const unsigned int protected_				= (1u << 0x0003u);
		static const unsigned int public_					= (1u << 0x0004u);
		static const unsigned int const_					= (1u << 0x0005u);//Applicable to functions
		static const unsigned int rval						= (1u << 0x0006u);//Applicable to function return types
		static const unsigned int immediate					= (1u << 0x0007u);
	};

	class object{
	public:
		template <typename attributes_type>
		object(const std::string &name, std::shared_ptr<type::object> type, const attributes_type &attributes, unsigned int flags)
			: name_(name), type_(type), attributes_(attributes), flags_(flags){}

		object(const std::string &name, std::shared_ptr<type::object> type);

		virtual ~object();

		virtual const std::string &get_name() const;

		virtual std::shared_ptr<type::object> get_type() const;

		virtual const attribute::collection &get_attributes() const;

		virtual attribute::collection &get_attributes();

		virtual unsigned int get_flags() const;

		virtual bool is(unsigned int flag, bool all = true) const;

	protected:
		std::string name_;
		std::shared_ptr<type::object> type_;
		attribute::collection attributes_;
		unsigned int flags_;
	};
}
