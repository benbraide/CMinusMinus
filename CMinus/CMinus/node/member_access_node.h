#pragma once

#include "node_object.h"

namespace cminus::node{
	class member_access : public object{
	public:
		member_access(std::shared_ptr<object> left, std::shared_ptr<object> right);

		virtual ~member_access();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		std::shared_ptr<object> left_;
		std::shared_ptr<object> right_;
	};
}
