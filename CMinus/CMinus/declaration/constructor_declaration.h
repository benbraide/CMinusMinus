#pragma once

#include "../type/class_type.h"

#include "function_declaration.h"

namespace cminus::declaration{
	class constructor : public function{
	public:
		explicit constructor(const type::class_ &parent);

		virtual ~constructor();

	protected:
		virtual std::shared_ptr<memory::reference> call_(const std::list<std::shared_ptr<memory::reference>> &args) const override;
	};
}
