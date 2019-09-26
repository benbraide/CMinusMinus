#pragma once

#include <string_view>

#include "../type/primitive_type.h"
#include "../evaluator/evaluator_object.h"

#include "storage_object.h"

namespace cminus::storage{
	class global : public named_object{
	public:
		enum class evaluator_type{
			nil,
			undefined,
			boolean,
			byte,
			character,
			floating_point,
			integral,
			nan,
			pointer,
			class_,
			string,
		};

		enum class cached_type{
			nil,
			undefined,
			void_,
			bool_,
			byte_,
			char_,
			wchar_,
			function,
			string,
		};

		global();

		virtual ~global();

		virtual std::shared_ptr<type::object> get_cached_type(cached_type type) const;

		virtual std::shared_ptr<type::object> get_cached_type(const type::object &type) const;

		virtual std::shared_ptr<evaluator::object> get_evaluator_for(const type::object &target_type) const;

		virtual std::shared_ptr<evaluator::initializer> get_default_initializer() const;

		virtual std::shared_ptr<memory::reference> copy(std::shared_ptr<memory::reference> value, std::shared_ptr<type::object> target_type = nullptr) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(const type::object &type) const;

		virtual std::shared_ptr<memory::reference> get_zero_value(std::shared_ptr<type::object> type) const;

		virtual std::shared_ptr<memory::reference> create_string(const std::string_view &value) const;

		virtual std::string_view get_string_value(std::shared_ptr<memory::reference> value) const;

		virtual std::shared_ptr<memory::reference> get_constant_value(bool value) const;

	protected:
		std::unordered_map<evaluator_type, std::shared_ptr<evaluator::object>> evaluators_;
		std::shared_ptr<evaluator::initializer> default_initializer_;
	};
}
