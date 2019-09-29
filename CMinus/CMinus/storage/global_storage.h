#pragma once

#include <string_view>

#include "../type/primitive_type.h"
#include "../evaluator/evaluator_object.h"

#include "storage_object.h"

namespace cminus::storage{
	class global : public named_object{
	public:
		enum class cached_type{
			nil,
			undefined,
			void_,
			bool_,
			byte_,
			char_,
			wchar_,
			number,
			integer,
			long_integer,
			unsigned_integer,
			unsigned_long_integer,
			real,
			long_real,
			function,
			nullptr_,
			string,
		};

		global();

		virtual ~global();

		virtual std::shared_ptr<type::object> get_cached_type(cached_type type) const;

		virtual std::shared_ptr<type::object> get_cached_type(const type::object &type) const;

		virtual std::shared_ptr<evaluator::object> get_evaluator(evaluator::object::id_type type) const;

		virtual std::shared_ptr<evaluator::initializer> get_default_initializer() const;

		virtual std::shared_ptr<memory::reference> copy(std::shared_ptr<memory::reference> value, std::shared_ptr<type::object> target_type = nullptr) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(const type::object &type) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<memory::reference> create_string(const std::string_view &value) const;

		virtual std::string_view get_string_value(std::shared_ptr<memory::reference> value) const;

		virtual std::shared_ptr<memory::reference> get_boolean_value(type::boolean_constant value) const;

		virtual std::shared_ptr<memory::reference> get_boolean_value(bool value) const;

		virtual std::shared_ptr<memory::reference> get_nullptr_value() const;

		virtual std::shared_ptr<memory::reference> get_undefined_value() const;

	protected:
		std::unordered_map<cached_type, std::shared_ptr<type::object>> cached_types_;
		std::unordered_map<evaluator::object::id_type, std::shared_ptr<evaluator::object>> evaluators_;
		std::shared_ptr<evaluator::initializer> default_initializer_;

		std::unordered_map<type::boolean_constant, std::shared_ptr<memory::reference>> boolean_values_;
		std::shared_ptr<memory::reference> nullptr_value_;
		std::shared_ptr<memory::reference> undefined_value_;
	};
}
