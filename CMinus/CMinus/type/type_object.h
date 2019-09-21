#pragma once

#include "../memory/memory_reference.h"
#include "../declaration/declaration_exception.h"

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
			auto_assignable,
			assignable,
			ancestor,
			offspring,
			compatible,
			class_compatible,
		};

		enum class query_type{
			nil,
			const_,
			ref,
			pointer,
			nullptr_,
			array_,
			function,
			variadic,
			primitive,
			scalar,
			class_,
			struct_,
			enum_,
			auto_,
			explicit_auto,
			nan,
			undefined,
			void_,
			numeric,
			floating_point,
			integral,
			unsigned_integral,
			string,
			boolean,
			byte,
			character,
			inferred,
			offspring_of,
			child_of,
			updatable,
			modified,
		};

		enum class conversion_type{
			nil,
			correct_ref_const,
			remove_ref,
			remove_const,
			remove_ref_const,
			remove_indirection,
			remove_pointer,
			infer,
			update,
			clone,
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

		virtual std::size_t compute_base_offset(const object &base_type) const;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const = 0;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const;

		virtual object *get_non_proxy() const;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const;

		virtual bool is(query_type type, const object *arg = nullptr) const;

		static int get_score_value(score_result_type score);

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const;

		std::string name_;
		storage::object *parent_;
	};
}
