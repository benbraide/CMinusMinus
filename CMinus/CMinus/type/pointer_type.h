#pragma once

#include "primitive_type.h"

namespace cminus::type{
	class pointer : public primitive{
	public:
		explicit pointer(object &base_type);

	protected:
		object *base_type_;
	};
}
