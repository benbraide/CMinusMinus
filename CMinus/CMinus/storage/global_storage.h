#pragma once

#include "../type/primitive_type.h"
#include "../evaluator/evaluator_object.h"

#include "storage_object.h"

namespace cminus::storage{
	class global : public object{
	public:
		enum class evaluator_type{
			nil,
			undefined,
			boolean,
			byte,
			character,
			floating_point,
			integral,
			nan,
			pointer,
			class_,
			string,
		};

		global();

		virtual ~global();

		virtual std::shared_ptr<evaluator::object> get_evaluator_for(const type::object &target_type) const;

		virtual std::shared_ptr<evaluator::initializer> get_default_initializer() const;

	protected:
		std::unordered_map<evaluator_type, std::shared_ptr<evaluator::object>> evaluators_;
		std::shared_ptr<evaluator::initializer> default_initializer_;
	};
}
