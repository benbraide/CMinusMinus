#pragma once

#include "pointer_type.h"
#include "modified_type.h"
#include "class_type.h"

namespace cminus::type{
	class string : public class_{
	public:
		string();

		virtual ~string();

		virtual bool is_constructible(std::shared_ptr<memory::reference> target) const override;

		virtual int get_score(const type_base &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;

		virtual bool is(query_type type, const type_base *arg = nullptr) const override;
	};
}
