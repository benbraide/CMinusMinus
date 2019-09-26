#pragma once

#include "type_object.h"

namespace cminus::type{
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

	template <primitive::id_type id, object::query_type query, std::size_t size>
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
	};

	using void_primitive = strict_primitive<primitive::id_type::void_, object::query_type::void_, 0u>;
	using bool_primitive = strict_primitive<primitive::id_type::bool_, object::query_type::boolean, 1u>;
	using byte_primitive = strict_primitive<primitive::id_type::byte_, object::query_type::byte, 1u>;

	using char_primitive = strict_primitive<primitive::id_type::char_, object::query_type::character, sizeof(char)>;
	using wchar_primitive = strict_primitive<primitive::id_type::wchar_, object::query_type::character, sizeof(wchar_t)>;

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

		virtual bool is_nan(const memory::reference &data) const;

		template <typename target_type>
		target_type get_nan() const{
			switch (state_){
			case state_type::integer:
				return static_cast<target_type>(integer_nan_value);
			case state_type::long_integer:
				return static_cast<target_type>(long_integer_nan_value);
			case state_type::unsigned_integer:
				return static_cast<target_type>(unsigned_integer_nan_value);
			case state_type::unsigned_long_integer:
				return static_cast<target_type>(unsigned_long_integer_nan_value);
			case state_type::real:
				return static_cast<target_type>(real_nan_value);
			case state_type::long_real:
				return static_cast<target_type>(long_real_nan_value);
			default:
				break;
			}

			return target_type();
		}

		template <typename target_type>
		target_type read_value(std::shared_ptr<memory::reference> data) const{
			switch (state_){
			case state_type::integer:
				return static_cast<target_type>(data->read_scalar<__int32>());
			case state_type::long_integer:
				return static_cast<target_type>(data->read_scalar<__int64>());
			case state_type::unsigned_integer:
				return static_cast<target_type>(data->read_scalar<unsigned __int32>());
			case state_type::unsigned_long_integer:
				return static_cast<target_type>(data->read_scalar<unsigned __int64>());
			case state_type::real:
				return static_cast<target_type>(data->read_scalar<float>());
			case state_type::long_real:
				return static_cast<target_type>(data->read_scalar<long double>());
			default:
				break;
			}

			return target_type();
		}

		virtual state_type get_state() const;

		virtual bool has_precedence_over(const number_primitive &target) const;

		static const __int32 integer_nan_value = std::numeric_limits<__int32>::min();
		static const __int64 long_integer_nan_value = std::numeric_limits<__int64>::min();

		static const unsigned __int32 unsigned_integer_nan_value = std::numeric_limits<unsigned __int32>::max();
		static const unsigned __int64 unsigned_long_integer_nan_value = std::numeric_limits<unsigned __int64>::max();

		static const float real_nan_value;
		static const long double long_real_nan_value;

	protected:
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

	template <class target_type>
	struct get_nan;

	template <>
	struct get_nan<__int32>{
		static constexpr __int32 value(){
			return number_primitive::integer_nan_value;
		}
	};

	template <>
	struct get_nan<__int64>{
		static constexpr __int64 value(){
			return number_primitive::long_integer_nan_value;
		}
	};

	template <>
	struct get_nan<unsigned __int32>{
		static constexpr unsigned __int32 value(){
			return number_primitive::unsigned_integer_nan_value;
		}
	};

	template <>
	struct get_nan<unsigned __int64>{
		static constexpr unsigned __int64 value(){
			return number_primitive::unsigned_long_integer_nan_value;
		}
	};

	template <>
	struct get_nan<float>{
		static constexpr float value(){
			return number_primitive::real_nan_value;
		}
	};

	template <>
	struct get_nan<long double>{
		static constexpr long double value(){
			return number_primitive::long_real_nan_value;
		}
	};
}
