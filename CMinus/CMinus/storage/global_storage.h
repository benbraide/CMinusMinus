#pragma once

#include <string_view>

#include "../type/primitive_type.h"
#include "../node/identifier_node.h"
#include "../evaluator/evaluator_object.h"

#include "storage_object.h"

namespace cminus::type{
	class string;
}

namespace cminus::storage{
	class global : public named_object{
	public:
		using cached_type = node::primtive_type::id_type;

		global();

		virtual ~global();

		virtual object *get_core_storage() const;

		virtual std::shared_ptr<type::object> get_cached_type(cached_type type) const;

		virtual std::shared_ptr<type::object> get_cached_type(const type::object &type) const;

		virtual std::shared_ptr<type::object> get_pointer_type(std::shared_ptr<type::object> base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_pointer_type(cached_type base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_reversed_pointer_type(std::shared_ptr<type::object> base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_reversed_pointer_type(cached_type base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_ref_type(std::shared_ptr<type::object> base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_ref_type(cached_type base_type, bool is_const) const;

		virtual std::shared_ptr<type::object> get_auto_pointer_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_auto_ref_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_char_pointer_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_reversed_char_pointer_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_char_ref_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_string_ref_type(bool is_const) const;

		virtual std::shared_ptr<type::object> get_auto_type() const;

		virtual std::shared_ptr<type::object> get_void_type() const;

		virtual std::shared_ptr<type::object> get_boolean_type() const;

		virtual std::shared_ptr<type::object> get_char_type() const;

		virtual std::shared_ptr<type::object> get_int_type() const;

		virtual std::shared_ptr<type::object> get_size_type() const;

		virtual std::shared_ptr<type::object> get_string_type() const;

		virtual type::string *get_raw_string_type() const;

		virtual std::shared_ptr<evaluator::object> get_evaluator(evaluator::object::id_type type) const;

		virtual std::shared_ptr<evaluator::initializer> get_default_initializer() const;

		virtual std::shared_ptr<memory::reference> copy(std::shared_ptr<memory::reference> value, std::shared_ptr<type::object> target_type = nullptr) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(const type::object &type) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(cached_type type) const;

		virtual std::shared_ptr<memory::reference> create_string(const char *value, bool lval = false) const;

		virtual std::shared_ptr<memory::reference> create_string(const std::string &value, bool lval = false) const;

		virtual std::shared_ptr<memory::reference> create_string(const std::string_view &value, bool lval = false) const;

		virtual std::shared_ptr<memory::reference> create_string(const std::string_view &first, const std::string_view &second, bool lval = false) const;

		virtual std::string_view get_string_value(std::shared_ptr<memory::reference> value) const;

		virtual std::size_t read_enum_value(std::shared_ptr<memory::reference> value) const;

		virtual std::shared_ptr<memory::reference> get_enum_value(std::shared_ptr<type::object> target_type, std::size_t value) const;

		virtual std::shared_ptr<memory::reference> get_enum_value(cached_type type, std::size_t value) const;

		virtual std::shared_ptr<memory::reference> get_boolean_value(type::boolean_constant value) const;

		virtual std::shared_ptr<memory::reference> get_boolean_value(bool value) const;

		virtual std::shared_ptr<memory::reference> get_nullptr_value() const;

		virtual std::shared_ptr<memory::reference> get_undefined_value() const;

		virtual bool compare_enum(const type::object &type, std::shared_ptr<memory::reference> left, std::shared_ptr<memory::reference> right) const;

	protected:
		std::unordered_map<cached_type, std::shared_ptr<type::object>> cached_types_;
		std::unordered_map<type::boolean_constant, std::shared_ptr<memory::reference>> boolean_values_;

		std::unordered_map<evaluator::object::id_type, std::shared_ptr<evaluator::object>> evaluators_;
		std::shared_ptr<evaluator::initializer> default_initializer_;

		std::shared_ptr<memory::reference> nullptr_value_;
		std::shared_ptr<memory::reference> undefined_value_;

		std::shared_ptr<object> core_storage_;
	};

	template <class target_type>
	struct get_cached_type;

	template <>
	struct get_cached_type<void>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::void_);
		}
	};

	template <>
	struct get_cached_type<std::byte>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::byte_);
		}
	};

	template <>
	struct get_cached_type<char>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::char_);
		}
	};

	template <>
	struct get_cached_type<wchar_t>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::wchar_);
		}
	};

	template <>
	struct get_cached_type<bool>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::bool_);
		}
	};

	template <>
	struct get_cached_type<type::boolean_constant>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::bool_);
		}
	};

	template <>
	struct get_cached_type<__int16>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::small_integer);
		}
	};

	template <>
	struct get_cached_type<__int32>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::integer);
		}
	};

	template <>
	struct get_cached_type<__int64>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::big_integer);
		}
	};

	template <>
	struct get_cached_type<unsigned __int16>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::unsigned_small_integer);
		}
	};

	template <>
	struct get_cached_type<unsigned __int32>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::unsigned_integer);
		}
	};

	template <>
	struct get_cached_type<unsigned __int64>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::unsigned_big_integer);
		}
	};

	template <>
	struct get_cached_type<float>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::small_float);
		}
	};

	template <>
	struct get_cached_type<double>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::float_);
		}
	};

	template <>
	struct get_cached_type<long double>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::big_float);
		}
	};

	template <>
	struct get_cached_type<attribute::stage_type>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::attribute_stage);
		}
	};

	template <>
	struct get_cached_type<attribute::query_result>{
		static std::shared_ptr<type::object> type(){
			return runtime::object::global_storage->get_cached_type(global::cached_type::attribute_result);
		}
	};
}
