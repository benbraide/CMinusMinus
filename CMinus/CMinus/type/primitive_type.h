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
		explicit primitive(const std::string &name);

		primitive(const std::string &name, storage::object *parent);

		virtual ~primitive();

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual evaluator::object::id_type get_evaluator_id() const;

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

	class no_storage_primitive : public primitive{
	public:
		using primitive::primitive;

		virtual ~no_storage_primitive();

		virtual std::shared_ptr<memory::reference> get_default_value() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;
	};

	class undefined_primitive : public no_storage_primitive{
	public:
		undefined_primitive();

		virtual ~undefined_primitive();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	class void_primitive : public no_storage_primitive{
	public:
		void_primitive();

		virtual ~void_primitive();
	};

	class boolean_primitive : public primitive{
	public:
		boolean_primitive();

		virtual ~boolean_primitive();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	class byte_primitive : public primitive{
	public:
		byte_primitive();

		virtual ~byte_primitive();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	class char_primitive : public primitive{
	public:
		char_primitive();

		virtual ~char_primitive();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	class wchar_primitive : public primitive{
	public:
		wchar_primitive();

		virtual ~wchar_primitive();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;
	};

	template <class target_type>
	struct get_nan;

	template <>
	struct get_nan<__int16>{
		static constexpr __int16 value(){
			return std::numeric_limits<__int16>::min();
		}
	};

	template <>
	struct get_nan<__int32>{
		static constexpr __int32 value(){
			return std::numeric_limits<__int32>::min();
		}
	};

	template <>
	struct get_nan<__int64>{
		static constexpr __int64 value(){
			return std::numeric_limits<__int64>::min();
		}
	};

	template <>
	struct get_nan<unsigned __int16>{
		static constexpr unsigned __int16 value(){
			return std::numeric_limits<unsigned __int16>::max();
		}
	};

	template <>
	struct get_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return std::numeric_limits<unsigned __int32>::max();
		}
	};

	template <>
	struct get_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return std::numeric_limits<unsigned __int64>::max();
		}
	};

	template <>
	struct get_nan<float>{
		static constexpr float value(){
			return std::numeric_limits<float>::min();
		}
	};

	template <>
	struct get_nan<double>{
		static constexpr double value(){
			return std::numeric_limits<double>::min();
		}
	};

	template <>
	struct get_nan<long double>{
		static constexpr long double value(){
			return std::numeric_limits<long double>::min();
		}
	};

	template <class target_type>
	struct get_max_non_nan;

	template <>
	struct get_max_non_nan<__int16>{
		static constexpr __int16 value(){
			return std::numeric_limits<__int16>::max();
		}
	};

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
	struct get_max_non_nan<unsigned __int16>{
		static constexpr unsigned __int16 value(){
			return static_cast<unsigned __int16>(get_nan<unsigned __int16>::value() - 1ui16);
		}
	};

	template <>
	struct get_max_non_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return (get_nan<unsigned __int32>::value() - 1ui32);
		}
	};

	template <>
	struct get_max_non_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return (get_nan<unsigned __int64>::value() - 1ui64);
		}
	};

	template <>
	struct get_max_non_nan<float>{
		static constexpr float value(){
			return std::numeric_limits<float>::max();
		}
	};

	template <>
	struct get_max_non_nan<double>{
		static constexpr double value(){
			return std::numeric_limits<double>::max();
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
	struct get_min_non_nan<__int16>{
		static constexpr __int16 value(){
			return static_cast<__int16>(get_nan<__int16>::value() + 1i16);
		}
	};

	template <>
	struct get_min_non_nan<__int32>{
		static constexpr __int32 value(){
			return (get_nan<__int32>::value() + 1i32);
		}
	};

	template <>
	struct get_min_non_nan<__int64>{
		static constexpr __int64 value(){
			return (get_nan<__int64>::value() + 1i64);
		}
	};

	template <>
	struct get_min_non_nan<unsigned __int16>{
		static constexpr unsigned __int16 value(){
			return std::numeric_limits<__int16>::min();
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
			return (get_nan<float>::value() + 1.0f);
		}
	};

	template <>
	struct get_min_non_nan<double>{
		static constexpr double value(){
			return (get_nan<double>::value() + 1.0);
		}
	};

	template <>
	struct get_min_non_nan<long double>{
		static constexpr long double value(){
			return (get_nan<long double>::value() + 1.0l);
		}
	};

	template <class target_type>
	struct numeric_cast{
		template <typename value_type>
		static constexpr target_type get(value_type value){
			return ((value == get_nan<value_type>::template value()) ? get_nan<target_type>::template value() : static_cast<target_type>(value));
		}
	};

	class number_primitive : public primitive{
	public:
		enum class state_type{
			small_number,
			number,
			big_number,
			small_integer,
			unsigned_small_integer,
			integer,
			unsigned_integer,
			big_integer,
			unsigned_big_integer,
			small_float,
			float_,
			big_float,
		};

		explicit number_primitive(state_type state);

		virtual ~number_primitive();

		virtual std::shared_ptr<memory::reference> get_default_value() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

		virtual bool is_integral() const;

		virtual bool is_unsigned_integral() const;

		virtual bool is_nan(const memory::reference &data) const;

		template <typename target_type>
		target_type get_nan() const{
			switch (state_){
			case state_type::small_integer:
				return static_cast<target_type>(type::get_nan<__int16>::value());
			case state_type::integer:
				return static_cast<target_type>(type::get_nan<__int32>::value());
			case state_type::big_integer:
				return static_cast<target_type>(type::get_nan<__int64>::value());
			case state_type::unsigned_small_integer:
				return static_cast<target_type>(type::get_nan<unsigned __int16>::value());
			case state_type::unsigned_integer:
				return static_cast<target_type>(type::get_nan<unsigned __int32>::value());
			case state_type::unsigned_big_integer:
				return static_cast<target_type>(type::get_nan<unsigned __int64>::value());
			case state_type::small_float:
				return static_cast<target_type>(type::get_nan<float>::value());
			case state_type::float_:
				return static_cast<target_type>(type::get_nan<double>::value());
			case state_type::big_float:
				return static_cast<target_type>(type::get_nan<long double>::value());
			default:
				break;
			}

			return type::get_nan<target_type>::template value();
		}

		template <typename target_type>
		target_type read_value(std::shared_ptr<memory::reference> data) const{
			switch (state_){
			case state_type::small_integer:
				return numeric_cast<target_type>::template get(data->read_scalar<__int16>());
			case state_type::integer:
				return numeric_cast<target_type>::template get(data->read_scalar<__int32>());
			case state_type::big_integer:
				return numeric_cast<target_type>::template get(data->read_scalar<__int64>());
			case state_type::unsigned_small_integer:
				return numeric_cast<target_type>::template get(data->read_scalar<unsigned __int16>());
			case state_type::unsigned_integer:
				return numeric_cast<target_type>::template get(data->read_scalar<unsigned __int32>());
			case state_type::unsigned_big_integer:
				return numeric_cast<target_type>::template get(data->read_scalar<unsigned __int64>());
			case state_type::small_float:
				return numeric_cast<target_type>::template get(data->read_scalar<float>());
			case state_type::float_:
				return numeric_cast<target_type>::template get(data->read_scalar<double>());
			case state_type::big_float:
				return numeric_cast<target_type>::template get(data->read_scalar<long double>());
			default:
				break;
			}

			return type::get_nan<target_type>::template value();
		}

		virtual std::string get_string_value(std::shared_ptr<memory::reference> data) const;

		virtual state_type get_state() const;

		virtual state_type get_precedence(const number_primitive &target) const;

	protected:
		virtual int get_score_(const object &target, bool is_lval, bool is_const) const override;

		template <typename target_type>
		bool is_nan_(const memory::reference &data) const{
			return (data.read_scalar<target_type>() == type::get_nan<target_type>::template value());
		}

		template <typename value_type>
		std::string get_string_value_(value_type value) const{
			return ((value == type::get_nan<value_type>::template value()) ? "NaN" : runtime::to_string<value_type>::template get(value));
		}

		state_type state_;
		std::size_t size_;
	};

	class function_primitive : public inferred<primitive>{
	public:
		using base_type = inferred<primitive>;

		function_primitive();

		virtual ~function_primitive();

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_inferred_from(const object &target) const override;
	};

	class function_return_primitive : public primitive{
	public:
		function_return_primitive();

		virtual ~function_return_primitive();
	};

	class auto_primitive : public inferred<primitive>{
	public:
		using base_type = inferred<primitive>;

		auto_primitive();

		virtual ~auto_primitive();

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_inferred_from(const object &target) const override;
	};
}
