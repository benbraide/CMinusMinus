#pragma once

#include "../evaluator/initializer.h"
#include "../evaluator/evaluator_object.h"

#include "type_object.h"

namespace cminus::type{
	class proxy : public object{
	public:
		proxy(object &target);

		virtual ~proxy();

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target) const override;

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::shared_ptr<memory::reference> get_default_value(std::shared_ptr<object> self) const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::size_t compute_base_offset(const object &base_type) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const override;

		virtual object *get_non_proxy() const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

	protected:
		object *target_;
	};
}
