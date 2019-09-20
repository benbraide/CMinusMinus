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

	protected:
		template <typename value_type>
		std::shared_ptr<memory::reference> convert_value_to_number_(value_type value, std::shared_ptr<object> target_type) const{
			if (target_type->is(query_type::unsigned_integral)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<unsigned __int32>>(target_type->convert(conversion_type::clone, target_type), static_cast<unsigned __int32>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<unsigned __int64>>(target_type->convert(conversion_type::clone, target_type), static_cast<unsigned __int64>(value));
				default:
					break;
				}
			}
			else if (target_type->is(query_type::integral)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<__int32>>(target_type->convert(conversion_type::clone, target_type), static_cast<__int32>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<__int64>>(target_type->convert(conversion_type::clone, target_type), static_cast<__int64>(value));
				default:
					break;
				}
			}
			else if (target_type->is(query_type::floating_point)){
				switch (target_type->get_size()){
				case 4u:
					return std::make_shared<memory::scalar_reference<float>>(target_type->convert(conversion_type::clone, target_type), static_cast<float>(value));
				case 8u:
					return std::make_shared<memory::scalar_reference<long double>>(target_type->convert(conversion_type::clone, target_type), static_cast<long double>(value));
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
			unsigned_integer,
			real,
			nan,
		};

		explicit number_primitive(state_type state = state_type::integer, std::size_t size = sizeof(int));

		virtual ~number_primitive();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

	protected:
		template <typename target_type>
		target_type read_value_(std::shared_ptr<memory::reference> data) const{
			if (state_ == state_type::integer){
				switch (size_){
				case 4u:
					return static_cast<target_type>(data->read_scalar<__int32>());
				case 8u:
					return static_cast<target_type>(data->read_scalar<__int64>());
				default:
					break;
				}
			}
			else if (state_ == state_type::unsigned_integer){
				switch (size_){
				case 4u:
					return static_cast<target_type>(data->read_scalar<unsigned __int32>());
				case 8u:
					return static_cast<target_type>(data->read_scalar<unsigned __int64>());
				default:
					break;
				}
			}
			else if (state_ == state_type::real){
				switch (size_){
				case 4u:
					return static_cast<target_type>(data->read_scalar<float>());
				case 8u:
					return static_cast<target_type>(data->read_scalar<long double>());
				default:
					break;
				}
			}

			return target_type();
		}

		mutable state_type state_;
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
}
