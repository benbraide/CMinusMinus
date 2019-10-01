#pragma once

#include "../runtime/string_conversions.h"
#include "../evaluator/evaluator_object.h"

#include "type_object.h"

namespace cminus::type{
	enum class boolean_constant : char{
		false_,
		true_,
		indeterminate,
	};

	class primitive : public object{
	public:
		enum class id_type : char{
			nil,
			void_,
			bool_,
			byte_,
			char_,
			wchar_,
		};

		explicit primitive(const std::string &name);

		virtual ~primitive();

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual evaluator::object::id_type get_evaluator_id() const;

		static std::string convert_id_to_string(id_type value);

		template <typename value_type>
		static std::shared_ptr<memory::reference> convert_value_to_number(value_type value, std::shared_ptr<object> target_type){
			if (target_type->is(query_type::unsigned_integral)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<unsigned __int32>>(target_type, static_cast<unsigned __int32>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<unsigned __int64>>(target_type, static_cast<unsigned __int64>(value));
				default:
					break;
				}
			}
			else if (target_type->is(query_type::integral)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<__int32>>(target_type, static_cast<__int32>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<__int64>>(target_type, static_cast<__int64>(value));
				default:
					break;
				}
			}
			else if (target_type->is(query_type::floating_point)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<float>>(target_type, static_cast<float>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<long double>>(target_type, static_cast<long double>(value));
				default:
					break;
				}
			}

			return nullptr;
		}
	};

	class undefined_primitive : public primitive{
	public:
		undefined_primitive();

		virtual ~undefined_primitive();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;
	};

	template <primitive::id_type id, object::query_type query, std::size_t size, evaluator::object::id_type evaluator_id>
	class strict_primitive : public primitive{
	public:
		strict_primitive()
			: primitive(convert_id_to_string(id)){}

		virtual ~strict_primitive() = default;

		virtual std::size_t get_size() const override{
			return size;
		}

		virtual bool is_exact(const object &target) const override{
			return (dynamic_cast<const strict_primitive *>(&target) != nullptr);
		}

		virtual bool is(query_type type, const object *arg = nullptr) const override{
			return (type == query || primitive::is(type, arg));
		}

		virtual evaluator::object::id_type get_evaluator_id() const override{
			return evaluator_id;
		}
	};

	using void_primitive = strict_primitive<primitive::id_type::void_, object::query_type::void_, 0u, evaluator::object::id_type::nil>;
	using bool_primitive = strict_primitive<primitive::id_type::bool_, object::query_type::boolean, sizeof(boolean_constant), evaluator::object::id_type::boolean>;
	using byte_primitive = strict_primitive<primitive::id_type::byte_, object::query_type::byte, 1u, evaluator::object::id_type::byte>;

	using char_primitive = strict_primitive<primitive::id_type::char_, object::query_type::character, sizeof(char), evaluator::object::id_type::byte>;
	using wchar_primitive = strict_primitive<primitive::id_type::wchar_, object::query_type::character, sizeof(wchar_t), evaluator::object::id_type::byte>;

	struct numeric_constants{
		static const __int32 integer_nan_value = std::numeric_limits<__int32>::min();
		static const __int64 long_integer_nan_value = std::numeric_limits<__int64>::min();

		static const unsigned __int32 unsigned_integer_nan_value = std::numeric_limits<unsigned __int32>::max();
		static const unsigned __int64 unsigned_long_integer_nan_value = std::numeric_limits<unsigned __int64>::max();

		static const float real_nan_value;
		static const long double long_real_nan_value;
	};

	template <class target_type>
	struct get_nan;

	template <>
	struct get_nan<__int32>{
		static constexpr __int32 value(){
			return numeric_constants::integer_nan_value;
		}
	};

	template <>
	struct get_nan<__int64>{
		static constexpr __int64 value(){
			return numeric_constants::long_integer_nan_value;
		}
	};

	template <>
	struct get_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return numeric_constants::unsigned_integer_nan_value;
		}
	};

	template <>
	struct get_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return numeric_constants::unsigned_long_integer_nan_value;
		}
	};

	template <>
	struct get_nan<float>{
		static constexpr float value(){
			return numeric_constants::real_nan_value;
		}
	};

	template <>
	struct get_nan<long double>{
		static constexpr long double value(){
			return numeric_constants::long_real_nan_value;
		}
	};

	template <class target_type>
	struct get_max_non_nan;

	template <>
	struct get_max_non_nan<__int32>{
		static constexpr __int32 value(){
			return std::numeric_limits<__int32>::max();
		}
	};

	template <>
	struct get_max_non_nan<__int64>{
		static constexpr __int64 value(){
			return std::numeric_limits<__int64>::max();
		}
	};

	template <>
	struct get_max_non_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return (numeric_constants::unsigned_integer_nan_value - 1ui32);
		}
	};

	template <>
	struct get_max_non_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return (numeric_constants::unsigned_long_integer_nan_value - 1ui64);
		}
	};

	template <>
	struct get_max_non_nan<float>{
		static constexpr float value(){
			return std::numeric_limits<float>::max();
		}
	};

	template <>
	struct get_max_non_nan<long double>{
		static constexpr long double value(){
			return std::numeric_limits<long double>::max();
		}
	};

	template <class target_type>
	struct get_min_non_nan;

	template <>
	struct get_min_non_nan<__int32>{
		static constexpr __int32 value(){
			return (numeric_constants::integer_nan_value + 1i32);
		}
	};

	template <>
	struct get_min_non_nan<__int64>{
		static constexpr __int64 value(){
			return (numeric_constants::long_integer_nan_value + 1i64);
		}
	};

	template <>
	struct get_min_non_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return std::numeric_limits<unsigned __int32>::min();
		}
	};

	template <>
	struct get_min_non_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return std::numeric_limits<unsigned __int64>::min();
		}
	};

	template <>
	struct get_min_non_nan<float>{
		static constexpr float value(){
			return (numeric_constants::real_nan_value + 1.0f);
		}
	};

	template <>
	struct get_min_non_nan<long double>{
		static constexpr long double value(){
			return (numeric_constants::long_real_nan_value + 1.0l);
		}
	};

	class number_primitive : public primitive{
	public:
		enum class state_type{
			nil,
			integer,
			long_integer,
			unsigned_integer,
			unsigned_long_integer,
			real,
			long_real,
		};

		explicit number_primitive(state_type state);

		virtual ~number_primitive();

		virtual std::shared_ptr<memory::reference> get_default_value(std::shared_ptr<object> self) const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

		virtual bool is_nan(const memory::reference &data) const;

		template <typename target_type>
		target_type get_nan() const{
			switch (state_){
			case state_type::integer:
				return static_cast<target_type>(type::get_nan<__int32>::value());
			case state_type::long_integer:
				return static_cast<target_type>(type::get_nan<__int64>::value());
			case state_type::unsigned_integer:
				return static_cast<target_type>(type::get_nan<unsigned __int32>::value());
			case state_type::unsigned_long_integer:
				return static_cast<target_type>(type::get_nan<unsigned __int64>::value());
			case state_type::real:
				return static_cast<target_type>(type::get_nan<float>::value());
			case state_type::long_real:
				return static_cast<target_type>(type::get_nan<long double>::value());
			default:
				break;
			}

			return target_type();
		}

		template <typename target_type>
		target_type read_value(std::shared_ptr<memory::reference> data) const{
			switch (state_){
			case state_type::integer:
				return cast_value<target_type>(data->read_scalar<__int32>());
			case state_type::long_integer:
				return cast_value<target_type>(data->read_scalar<__int64>());
			case state_type::unsigned_integer:
				return cast_value<target_type>(data->read_scalar<unsigned __int32>());
			case state_type::unsigned_long_integer:
				return cast_value<target_type>(data->read_scalar<unsigned __int64>());
			case state_type::real:
				return cast_value<target_type>(data->read_scalar<float>());
			case state_type::long_real:
				return cast_value<target_type>(data->read_scalar<long double>());
			default:
				break;
			}

			return target_type();
		}

		template <typename target_type, typename value_type>
		target_type cast_value(value_type from) const{
			return ((from == type::get_nan<value_type>::template value()) ? type::get_nan<target_type>::template value() : static_cast<target_type>(from));
		}

		virtual std::string get_string_value(std::shared_ptr<memory::reference> data) const;

		virtual state_type get_state() const;

		virtual bool has_precedence_over(const number_primitive &target) const;

	protected:
		template <typename target_type>
		bool is_nan_(const memory::reference &data) const{
			return (data.read_scalar<__int32>() == type::get_nan<target_type>::template value());
		}

		template <typename value_type>
		std::string get_string_value_(value_type value) const{
			return ((value == type::get_nan<value_type>::template value()) ? "NaN" : runtime::to_string<value_type>::template get(value));
		}

		state_type state_;
		std::size_t size_;
	};

	class function_primitive : public primitive{
	public:
		function_primitive();

		virtual ~function_primitive();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	class auto_primitive : public primitive{
	public:
		auto_primitive();

		virtual ~auto_primitive();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;
	};
}
