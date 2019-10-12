#pragma once

#include "class_type.h"

namespace cminus::type{
	class iterator : public class_{
	public:
		enum class state_type{
			nil,
			reversed,
			const_,
			reversed_const,
		};

		explicit iterator(state_type state);

		virtual ~iterator();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

	protected:
		evaluator::object::id_type evaluator_type_;
	};
}
