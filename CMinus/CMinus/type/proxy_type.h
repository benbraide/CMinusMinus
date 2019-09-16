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

		virtual void construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const override;

		virtual void construct(std::shared_ptr<memory::reference> target) const override;

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::shared_ptr<memory::reference> get_default_value() const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const override;

		virtual std::shared_ptr<object> remove_ref(std::shared_ptr<object> self = nullptr) const override;

		virtual std::shared_ptr<object> remove_const(std::shared_ptr<object> self = nullptr) const override;

		virtual std::shared_ptr<object> convert_auto(std::shared_ptr<object> target) const override;

		virtual void set_nan_state(bool value) override;

		virtual object *get_non_proxy() const override;

		virtual bool is_ref() const override;

		virtual bool is_const() const override;

		virtual bool is_auto() const override;

		virtual bool is_explicit_auto() const override;

		virtual bool is_any() const override;

		virtual bool is_explicit_any() const override;

		virtual bool is_nan() const override;

		virtual bool is_explicit_nan() const override;

		virtual bool is_undefined() const override;

	protected:
		object *target_;
	};
}
