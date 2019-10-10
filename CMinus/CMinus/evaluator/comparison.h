#pragma once

#include "../type/primitive_type.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class comparison{
	public:
		virtual ~comparison();

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const = 0;

	protected:
		virtual object::memory_ptr_type pre_evaluate_(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;

		template <typename target_type>
		bool evaluate_(operators::id op, object::memory_ptr_type left, object::memory_ptr_type right) const{
			switch (op){
			case operators::id::less:
				return (left->read_scalar<target_type>() < right->read_scalar<target_type>());
			case operators::id::less_or_equal:
				return (left->read_scalar<target_type>() <= right->read_scalar<target_type>());
			case operators::id::equal:
				return (left->read_scalar<target_type>() == right->read_scalar<target_type>());
			case operators::id::not_equal:
				return (left->read_scalar<target_type>() != right->read_scalar<target_type>());
			case operators::id::greater_or_equal:
				return (left->read_scalar<target_type>() >= right->read_scalar<target_type>());
			case operators::id::greater:
				return (left->read_scalar<target_type>() > right->read_scalar<target_type>());
			default:
				break;
			}

			return false;
		}

		template <typename target_type>
		int compare_(object::memory_ptr_type left, object::memory_ptr_type right) const{
			auto left_value = left->read_scalar<target_type>(), right_value = right->read_scalar<target_type>();
			if (left_value < right_value)
				return -1;

			if (right_value < left_value)
				return 1;

			return 0;
		}

		template <typename target_type>
		bool evaluate_numeric_(operators::id op, object::memory_ptr_type left, object::memory_ptr_type right) const{
			auto left_is_nan = left->is_nan(), right_is_nan = left->is_nan();
			if (left_is_nan || right_is_nan){
				switch (op){
				case operators::id::less_or_equal:
				case operators::id::equal:
				case operators::id::greater_or_equal:
					return (left_is_nan == right_is_nan);
				case operators::id::not_equal:
					return (left_is_nan != right_is_nan);
				default:
					return false;
				}
			}

			return evaluate_<target_type>(op, left, right);
		}

		template <typename target_type>
		int compare_numeric_(object::memory_ptr_type left, object::memory_ptr_type right) const{
			auto left_is_nan = left->is_nan(), right_is_nan = left->is_nan();
			if (left_is_nan || right_is_nan)
				return ((left_is_nan == right_is_nan) ? 0 : -1);

			return compare_<target_type>(left, right);
		}

		bool evaluate_string_(operators::id op, const std::string_view &left, const std::string_view &right) const;

		int compare_string_(const std::string_view &left, const std::string_view &right) const;

		virtual object::memory_ptr_type create_value_(bool value) const;

		virtual object::memory_ptr_type create_compare_value_(int value) const;
	};

	template <class target_type>
	class generic_comparison : public comparison{
	public:
		virtual ~generic_comparison() = default;

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const override{
			auto left_type = left_value->get_type(), right_type = right_value->get_type();
			if (left_type == nullptr || right_type == nullptr)
				throw exception::invalid_type();

			auto left_number_type = dynamic_cast<type::number_primitive *>(left_type->get_non_proxy());
			if (left_number_type == nullptr)
				throw exception::unsupported_op();

			attribute::read_guard left_read_guard(left_value, nullptr);
			attribute::read_guard right_read_guard(right_value, nullptr);

			auto compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static);
			if (compatible_right_value == nullptr)
				throw exception::incompatible_rval();

			return create_value_(comparison::evaluate_<target_type>(op, left_value, compatible_right_value));
		}

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
			auto right_value = pre_evaluate_(op, left_value, right);
			return ((right_value == nullptr) ? nullptr : evaluate(op, left_value, right_value));
		}
	};

	class numeric_comparison : public comparison{
	public:
		virtual ~numeric_comparison();

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const override;

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;
	};

	class pointer_comparison : public comparison{
	public:
		virtual ~pointer_comparison();

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const override;

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;
	};
}
