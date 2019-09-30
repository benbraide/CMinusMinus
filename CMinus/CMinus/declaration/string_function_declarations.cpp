#include "../type/string_type.h"
#include "../type/primitive_type.h"
#include "../type/modified_type.h"
#include "../storage/global_storage.h"

#include "variable_declaration.h"
#include "string_function_declarations.h"

cminus::declaration::string::default_constructor::~default_constructor() = default;

cminus::declaration::string::copy_constructor::~copy_constructor() = default;

cminus::declaration::string::sub_copy_constructor::sub_copy_constructor(type::class_ &parent)
	: copy_constructor(parent){
	auto unsigned_long_integral_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer);

	std::shared_ptr<memory::reference> empty_initialization;
	auto max_non_nan = std::make_shared<memory::scalar_reference<unsigned __int64>>(
		unsigned_long_integral_type,
		type::get_max_non_nan<unsigned __int64>::value()
	);

	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		unsigned_long_integral_type,											//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		empty_initialization													//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		unsigned_long_integral_type,											//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		max_non_nan																//Initialization
	));
}

cminus::declaration::string::sub_copy_constructor::~sub_copy_constructor() = default;

cminus::declaration::string::buffer_constructor::buffer_constructor(type::class_ &parent)
	: external_constructor(parent){
	auto char_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::char_);
	auto unsigned_long_integral_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer);

	auto const_char_type = std::make_shared<type::constant>(char_type);
	auto const_char_pointer_type = std::make_shared<type::pointer_primitive>(char_type);

	std::shared_ptr<memory::reference> empty_initialization;
	auto max_non_nan = std::make_shared<memory::scalar_reference<unsigned __int64>>(
		unsigned_long_integral_type,
		type::get_max_non_nan<unsigned __int64>::value()
	);

	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		const_char_pointer_type,												//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		empty_initialization													//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		unsigned_long_integral_type,											//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		max_non_nan																//Initialization
	));
}

cminus::declaration::string::buffer_constructor::~buffer_constructor() = default;

cminus::declaration::string::fill_constructor::fill_constructor(type::class_ &parent)
	: external_constructor(parent){
	auto char_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::char_);
	auto unsigned_long_integral_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer);

	std::shared_ptr<memory::reference> empty_initialization;
	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		unsigned_long_integral_type,											//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		empty_initialization													//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"fill",																	//Name
		char_type,																//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		empty_initialization													//Initialization
	));
}

cminus::declaration::string::fill_constructor::~fill_constructor() = default;

cminus::declaration::string::empty::empty(type::class_ &parent)
	: external_function("Empty", &parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_cached_type(storage::global::cached_type::bool_)){}

cminus::declaration::string::empty::~empty() = default;

cminus::declaration::string::size::size(type::class_ &parent)
	: external_function("Size", &parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer)){}

cminus::declaration::string::size::~size() = default;
