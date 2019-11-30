#pragma once

#include "../evaluator/initializer.h"
#include "../evaluator/evaluator_object.h"

#include "type_object.h"

namespace cminus::type{
	class proxy : public object{
	public:
		proxy(const object &target);

		virtual ~proxy();

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target) const override;

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::shared_ptr<memory::reference> get_default_value() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target, bool is_lval, bool is_const) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const override;

		virtual const object *remove_proxy() const override;

		virtual const object *remove_const_ref() const override;

		virtual std::shared_ptr<object> remove_const_ref(std::shared_ptr<object> self) const override;

		virtual bool is_default_constructible(bool ignore_callable = false) const override;

		virtual bool is_copy_constructible(bool ignore_callable = false) const override;

		virtual bool is_copy_assignable(bool ignore_callable = false) const override;

		virtual bool can_be_iterated() const override;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual bool is_const() const override;

		virtual bool is_ref() const override;

	protected:
		const object *target_;
	};
}
