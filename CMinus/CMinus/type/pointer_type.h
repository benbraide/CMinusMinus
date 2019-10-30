#pragma once

#include "primitive_type.h"

namespace cminus::type{
	class pointer_primitive : public primitive{
	public:
		explicit pointer_primitive(std::shared_ptr<object> base_type);

		virtual ~pointer_primitive();

		virtual std::string get_qname() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual std::shared_ptr<object> get_base_type() const;

		virtual bool is_nullptr() const;

	protected:
		virtual int get_score_(const object &target, bool is_lval, bool is_const) const override;

		std::shared_ptr<object> base_type_;
	};
}
