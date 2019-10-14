#pragma once

#include "../type/primitive_type.h"
#include "../storage/global_storage.h"

#include "node_object.h"

namespace cminus::node{
	template <class value_type>
	class literal_ : public object{
	public:
		explicit literal_(value_type value)
			: value_(value){}

		virtual ~literal_() = default;

		virtual std::shared_ptr<memory::reference> evaluate() const override{
			return std::make_shared<memory::scalar_reference<value_type>>(storage::get_cached_type<value_type>::template type(), value_);
		}

	protected:
		value_type value_;
	};
}
