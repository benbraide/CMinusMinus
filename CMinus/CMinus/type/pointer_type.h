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

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

	protected:
		std::shared_ptr<object> base_type_;
	};
}
