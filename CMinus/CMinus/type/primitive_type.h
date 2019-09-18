#pragma once

#include "type_object.h"

namespace cminus::type{
	class primitive : public object{
	public:
		enum class id_type : char{
			nil,
			nullptr_,
			bool_,
			byte_,
			char_,
			wchar_,
			int16_,
			uint16_,
			int32_,
			uint32_,
			int64_,
			uint64_,
			int128_,
			uint128_,
			float_,
			double_,
			ldouble,
			nan_,
			void_,
			function,
			auto_,
			string,
		};

		using object::object;

		virtual ~primitive();

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual bool is(query_type type) const override;
	};
}
