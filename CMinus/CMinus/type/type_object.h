#pragma once

#include "../memory/memory_reference.h"

namespace cminus::memory{
	class reference;
}

namespace cminus::node{
	class object;
}

namespace cminus::evaluator{
	class object;
	class initializer;
}

namespace cminus::type{
	enum class cast_type{
		nil,
		static_,
		rval_static,
		reinterpret,
		dynamic,
	};

	class object{
	public:
		enum class score_result_type{
			nil,
			exact,
			assignable,
			ancestor,
			offspring,
			compatible,
			class_compatible,
			widened,
			shortened,
			too_widened,
			too_shortened,
		};

		object(const std::string &name, storage::object *parent);

		virtual ~object();

		virtual const std::string &get_name() const;

		virtual std::string get_qname() const;

		virtual storage::object *get_parent() const;

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const;

		virtual void construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const;

		virtual void construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const;

		virtual void construct(std::shared_ptr<memory::reference> target) const;

		virtual void destruct(std::shared_ptr<memory::reference> target) const;

		virtual std::shared_ptr<memory::reference> get_default_value() const;

		virtual std::size_t get_size() const = 0;

		virtual std::size_t get_memory_size() const;

		virtual bool is_exact(const object &target) const;

		virtual int get_score(const object &target) const = 0;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const = 0;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const;

		virtual std::shared_ptr<object> remove_ref(std::shared_ptr<object> self = nullptr) const;

		virtual std::shared_ptr<object> remove_const(std::shared_ptr<object> self = nullptr) const;

		virtual std::shared_ptr<object> convert_auto(std::shared_ptr<object> target) const;

		virtual void set_nan_state(bool value);

		virtual object *get_non_proxy() const;

		virtual bool is_ref() const;

		virtual bool is_const() const;

		virtual bool is_auto() const;

		virtual bool is_explicit_auto() const;

		virtual bool is_any() const;

		virtual bool is_explicit_any() const;

		virtual bool is_nan() const;

		virtual bool is_explicit_nan() const;

		virtual bool is_undefined() const;

		static int get_score_value(score_result_type score);

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const;

		std::string name_;
		storage::object *parent_;
	};
}
