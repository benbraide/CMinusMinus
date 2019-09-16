#pragma once

#include <string>

#include "../attribute/attribute_collection.h"

namespace cminus::declaration{
	class object{
	public:
		virtual ~object();

		virtual const std::string &get_name() const;

		virtual const attribute::collection &get_attributes() const;

		virtual attribute::collection &get_attributes();
	};
}
