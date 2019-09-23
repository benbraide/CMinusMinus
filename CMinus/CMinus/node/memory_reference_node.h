#pragma once

#include "node_object.h"

namespace cminus::node{
	class memory_reference : public object{
	public:
		explicit memory_reference(std::shared_ptr<cminus::memory::reference> value);

		virtual ~memory_reference();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<memory::reference> get_value() const;

	protected:
		std::shared_ptr<memory::reference> value_;
	};
}
