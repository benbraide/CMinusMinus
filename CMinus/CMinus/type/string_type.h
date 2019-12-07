#pragma once

#include "pointer_type.h"
#include "modified_type.h"
#include "class_type.h"
#include "primitive_type.h"

namespace cminus::type{
	class string : public class_{
	public:
		string();

		virtual ~string();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual bool is_constructible_from(const type_base &target_type, bool is_lval, bool is_const) const override;

		virtual bool is_forward_traversable() const override;

		virtual bool is_reverse_traversable() const override;

	protected:
		virtual int get_score_(const type_base &target, bool is_lval, bool is_const) const override;

		virtual std::shared_ptr<memory::reference> cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;
	};
}
