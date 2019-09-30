#pragma once

#include "special_function_declaration.h"

namespace cminus::declaration::string{
	class default_constructor : public declaration::default_constructor{
	public:
		using declaration::default_constructor::default_constructor;

		virtual ~default_constructor();
	};

	class copy_constructor : public declaration::copy_constructor{
	public:
		using declaration::copy_constructor::copy_constructor;

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
		using default_destructor::default_destructor;

		virtual ~destructor() = default;
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
}
