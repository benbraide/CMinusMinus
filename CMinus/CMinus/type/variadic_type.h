#pragma once

#include "type_object.h"

namespace cminus::type{
	class variadic : public object{
	public:
		explicit variadic(std::shared_ptr<object> base_type);

		virtual ~variadic();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

	protected:
		std::shared_ptr<object> base_type_;
	};
}
