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
		static_ref,
		static_const_ref,
		static_rval,
		reinterpret,
		dynamic,
		dynamic_ref,
		dynamic_const_ref,
	};

	class object{
	public:
		enum class score_result_type{
			nil,
			exact,
			inferable,
			assignable,
			ancestor,
			offspring,
			compatible,
			class_compatible,
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

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const = 0;

		virtual std::size_t get_size() const = 0;

		virtual std::size_t get_memory_size() const;

		virtual bool is_exact(const object &target) const;

		virtual int get_score(const object &target, bool is_lval, bool is_const) const;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const = 0;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const;

		virtual std::shared_ptr<evaluator::initializer> get_initializer() const;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const;

		virtual const object *remove_proxy() const;

		virtual const object *remove_const_ref() const;

		virtual std::shared_ptr<object> remove_const_ref(std::shared_ptr<object> self) const;

		virtual bool is_default_constructible(bool ignore_callable = false) const;

		virtual bool is_copy_constructible(bool ignore_callable = false) const;

		virtual bool is_copy_assignable(bool ignore_callable = false) const;

		virtual bool can_be_iterated() const;

		virtual bool can_be_inferred_from(const object &target) const;

		virtual bool is_inferred() const;

		virtual bool is_const() const;

		virtual bool is_ref() const;

		template <typename target_type>
		bool is(bool as_base = true) const{
			return (as<target_type>(as_base) != nullptr);
		}

		template <typename target_type>
		const target_type *as(bool as_base = true) const{
			return dynamic_cast<const target_type *>(as_base ? remove_const_ref() : remove_proxy());
		}

		static std::shared_ptr<memory::reference> copy_data(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type);

		static int get_score_value(score_result_type score, int offset = 0);

		static bool is_static_cast(cast_type type);

		static bool is_static_rval_cast(cast_type type);

		static bool is_ref_cast(cast_type type);

		static bool is_non_const_ref_cast(cast_type type);

		static bool is_valid_static_cast(cast_type type, bool is_lval, bool is_const);

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const;

		virtual int get_score_(const object &target, bool is_lval, bool is_const) const;

		virtual int get_no_conversion_score_(const object &target, bool is_lval, bool is_const) const;

		std::string name_;
		storage::object *parent_;
	};

	template <class base_type>
	class inferred : public base_type{
	public:
		using base_type::base_type;

		virtual ~inferred() = default;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override{
			throw runtime::exception::not_supported();
		}

		virtual std::size_t get_size() const override{
			return 0u;
		}

		virtual int get_score(const object &target, bool is_lval, bool is_const) const override{
			return object::get_score_value(object::score_result_type::nil);
		}

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override{
			return nullptr;
		}

		virtual bool is_inferred() const override{
			return true;
		}
	};
}
