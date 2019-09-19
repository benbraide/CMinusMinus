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

		virtual bool is(query_type type) const override;

		static std::string convert_id_to_string(id_type value);
	};

	template <primitive::id_type id, std::size_t size>
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
	};

	using void_primitive = strict_primitive<primitive::id_type::void_, 0u>;
	using bool_primitive = strict_primitive<primitive::id_type::bool_, 1u>;
	using byte_primitive = strict_primitive<primitive::id_type::byte_, 1u>;

	using char_primitive = strict_primitive<primitive::id_type::char_, sizeof(char)>;
	using wchar_primitive = strict_primitive<primitive::id_type::wchar_, sizeof(wchar_t)>;

	class number_primitive : public primitive{
	public:
		enum class state_type{
			nil,
			integer,
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

		virtual bool is(query_type type) const override;

	protected:
		template <typename target_type>
		target_type read_value_(std::shared_ptr<memory::reference> data) const{
			if (state_ == state_type::integer){
				switch (size_){
				case 1u:
					return static_cast<target_type>(data->read_scalar<__int8>());
				case 2u:
					return static_cast<target_type>(data->read_scalar<__int16>());
				case 4u:
					return static_cast<target_type>(data->read_scalar<__int32>());
				case 8u:
					return static_cast<target_type>(data->read_scalar<__int16>());
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

	class nan_primitive : public primitive{
	public:

	};
}
