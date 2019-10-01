#pragma once

#include "special_function_declaration.h"

namespace cminus::declaration::string{
	class misc_helper{
	public:
		static std::shared_ptr<type::object> get_string_ref_type(type::class_ &base, bool is_const);

		static std::shared_ptr<node::object> get_zero_or_nan_size_arg(bool is_zero);

		static std::shared_ptr<node::object> get_zero_size_arg();

		static std::shared_ptr<node::object> get_nan_size_arg();

		static std::shared_ptr<node::object> get_null_char_arg();
	};

	class default_constructor : public declaration::default_constructor{
	public:
		using base_type = declaration::default_constructor;

		explicit default_constructor(type::class_ &parent);

		virtual ~default_constructor();
	};

	class copy_constructor : public declaration::copy_constructor{
	public:
		using base_type = declaration::copy_constructor;

		explicit copy_constructor(type::class_ &parent);

		virtual ~copy_constructor();
	};

	class sub_copy_constructor : public declaration::copy_constructor{
	public:
		explicit sub_copy_constructor(type::class_ &parent);

		virtual ~sub_copy_constructor();
	};

	class buffer_constructor : public external_constructor{
	public:
		explicit buffer_constructor(type::class_ &parent);

		virtual ~buffer_constructor();
	};

	class fill_constructor : public external_constructor{
	public:
		explicit fill_constructor(type::class_ &parent);

		virtual ~fill_constructor();
	};

	class destructor : public default_destructor{
	public:
		explicit destructor(type::class_ &parent);

		virtual ~destructor();
	};

	class empty : public external_member_function{
	public:
		explicit empty(type::class_ &parent);

		virtual ~empty();
	};

	class size : public external_member_function{
	public:
		explicit size(type::class_ &parent);

		virtual ~size();
	};

	class data : public external_member_function{
	public:
		data(type::class_ &parent, bool is_const);

		virtual ~data();
	};

	class begin : public external_member_function{
	public:
		begin(type::class_ &parent, bool is_const);

		virtual ~begin();
	};

	class end : public external_member_function{
	public:
		end(type::class_ &parent, bool is_const);

		virtual ~end();
	};

	class at : public external_member_function{
	public:
		at(type::class_ &parent, bool is_const);

		virtual ~at();
	};

	class find_copy : public external_member_function{
	public:
		find_copy(type::class_ &parent, bool is_last);

		virtual ~find_copy();
	};

	class find_sub_copy : public external_member_function{
	public:
		find_sub_copy(type::class_ &parent, bool is_last);

		virtual ~find_sub_copy();
	};

	class find_buffer : public external_member_function{
	public:
		find_buffer(type::class_ &parent, bool is_last);

		virtual ~find_buffer();
	};

	class find_sized_buffer : public external_member_function{
	public:
		find_sized_buffer(type::class_ &parent, bool is_last);

		virtual ~find_sized_buffer();
	};

	class find_single : public external_member_function{
	public:
		find_single(type::class_ &parent, bool is_last);

		virtual ~find_single();
	};

	class get_sub : public external_member_function{
	public:
		explicit get_sub(type::class_ &parent);

		virtual ~get_sub();
	};
}
