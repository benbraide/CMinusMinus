#include "../type/string_type.h"
#include "../type/primitive_type.h"
#include "../type/modified_type.h"

#include "../node/literal_node.h"
#include "../storage/global_storage.h"

#include "variable_declaration.h"
#include "string_function_declarations.h"

std::shared_ptr<cminus::type::object> cminus::declaration::string::misc_helper::get_string_ref_type(type::class_ &base, bool is_const){
	std::shared_ptr<type::object> value = std::make_shared<type::proxy>(base);

	value = std::make_shared<type::ref>(value);
	if (is_const)
		value = std::make_shared<type::constant>(value);

	return value;
}

std::shared_ptr<cminus::node::object> cminus::declaration::string::misc_helper::get_zero_or_nan_size_arg(bool is_zero){
	return (is_zero ? get_zero_size_arg() : get_nan_size_arg());
}

std::shared_ptr<cminus::node::object> cminus::declaration::string::misc_helper::get_zero_size_arg(){
	return std::make_shared<node::literal_<unsigned __int64>>(0u);
}

std::shared_ptr<cminus::node::object> cminus::declaration::string::misc_helper::get_nan_size_arg(){
	return std::make_shared<node::literal_<unsigned __int64>>(type::get_nan<unsigned __int64>::value());
}

std::shared_ptr<cminus::node::object> cminus::declaration::string::misc_helper::get_null_char_arg(){
	return std::make_shared<node::literal_<char>>('\0');
}

cminus::declaration::string::default_constructor::default_constructor(type::class_ &parent)
	: base_type(parent, attribute::collection::list_type{}, flags::nil){}

cminus::declaration::string::default_constructor::~default_constructor() = default;

cminus::declaration::string::copy_constructor::copy_constructor(type::class_ &parent)
	: base_type(parent, attribute::collection::list_type{}, flags::nil){}

cminus::declaration::string::copy_constructor::~copy_constructor() = default;

cminus::declaration::string::sub_copy_constructor::sub_copy_constructor(type::class_ &parent)
	: copy_constructor(parent, attribute::collection::list_type{}, flags::nil){
	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::sub_copy_constructor::~sub_copy_constructor() = default;

cminus::declaration::string::buffer_constructor::buffer_constructor(type::class_ &parent)
	: external_constructor(parent, attribute::collection::list_type{}, flags::nil){
	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		runtime::object::global_storage->get_char_pointer_type(true),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::buffer_constructor::~buffer_constructor() = default;

cminus::declaration::string::fill_constructor::fill_constructor(type::class_ &parent)
	: external_constructor(parent, attribute::collection::list_type{}, flags::nil){
	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"fill",																	//Name
		runtime::object::global_storage->get_char_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::fill_constructor::~fill_constructor() = default;

cminus::declaration::string::destructor::destructor(type::class_ &parent)
	: default_destructor(parent, attribute::collection::list_type{}, flags::nil){}

cminus::declaration::string::destructor::~destructor() = default;

cminus::declaration::string::empty::empty(type::class_ &parent)
	: external_member_function("IsEmpty", parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_boolean_type()){}

cminus::declaration::string::empty::~empty() = default;

cminus::declaration::string::size::size(type::class_ &parent)
	: external_member_function("GetSize", parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){}

cminus::declaration::string::size::~size() = default;

cminus::declaration::string::data::data(type::class_ &parent, bool is_const)
	: external_member_function("GetData", parent, attribute::collection::list_type{}, (is_const ? flags::const_ : flags::nil), runtime::object::global_storage->get_char_pointer_type(is_const)){}

cminus::declaration::string::data::~data() = default;

cminus::declaration::string::begin::begin(type::class_ &parent, bool is_const)
	: external_member_function("Begin", parent, attribute::collection::list_type{}, (is_const ? flags::const_ : flags::nil), runtime::object::global_storage->get_char_pointer_type(is_const)){}

cminus::declaration::string::begin::~begin() = default;

cminus::declaration::string::end::end(type::class_ & parent, bool is_const)
	: external_member_function("End", parent, attribute::collection::list_type{}, (is_const ? flags::const_ : flags::nil), runtime::object::global_storage->get_char_pointer_type(is_const)){}

cminus::declaration::string::end::~end() = default;

cminus::declaration::string::at::at(type::class_ &parent, bool is_const)
	: external_member_function("GetChar", parent, attribute::collection::list_type{}, (is_const ? flags::const_ : flags::nil), runtime::object::global_storage->get_char_ref_type(is_const)){
	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::at::~at() = default;

cminus::declaration::string::find_copy::find_copy(type::class_ &parent, bool is_last)
	: external_member_function((is_last ? "FindLast" : "Find"), parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){
	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_zero_or_nan_size_arg(!is_last)							//Initialization
	));
}

cminus::declaration::string::find_copy::~find_copy() = default;

cminus::declaration::string::find_sub_copy::find_sub_copy(type::class_ &parent, bool is_last)
	: external_member_function((is_last ? "FindLast" : "Find"), parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){
	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_zero_or_nan_size_arg(!is_last)							//Initialization
	));
}

cminus::declaration::string::find_sub_copy::~find_sub_copy() = default;

cminus::declaration::string::find_buffer::find_buffer(type::class_ &parent, bool is_last)
	: external_member_function((is_last ? "FindLast" : "Find"), parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		runtime::object::global_storage->get_char_pointer_type(true),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_zero_or_nan_size_arg(!is_last)							//Initialization
	));
}

cminus::declaration::string::find_buffer::~find_buffer() = default;

cminus::declaration::string::find_sized_buffer::find_sized_buffer(type::class_ &parent, bool is_last)
	: external_member_function((is_last ? "FindLast" : "Find"), parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		runtime::object::global_storage->get_char_pointer_type(true),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::find_sized_buffer::~find_sized_buffer() = default;

cminus::declaration::string::find_single::find_single(type::class_ &parent, bool is_last)
	: external_member_function((is_last ? "FindLast" : "Find"), parent, attribute::collection::list_type{}, flags::const_, runtime::object::global_storage->get_size_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"target",																//Name
		runtime::object::global_storage->get_char_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_zero_or_nan_size_arg(!is_last)							//Initialization
	));
}

cminus::declaration::string::find_single::~find_single() = default;

cminus::declaration::string::get_sub::get_sub(type::class_ &parent)
	: external_member_function("GetSub", parent, attribute::collection::list_type{}, flags::const_, std::make_shared<type::proxy>(parent)){
	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_zero_size_arg()										//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::get_sub::~get_sub() = default;

cminus::declaration::string::resize::resize(type::class_ &parent)
	: external_member_function("Resize", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"fill",																	//Name
		runtime::object::global_storage->get_char_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_null_char_arg()										//Initialization
	));
}

cminus::declaration::string::resize::~resize() = default;

cminus::declaration::string::erase::erase(type::class_ &parent)
	: external_member_function("Erase", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::erase::~erase() = default;

cminus::declaration::string::clear::clear(type::class_ &parent)
	: external_member_function("Clear", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){}

cminus::declaration::string::clear::~clear() = default;

cminus::declaration::string::swap::swap(type::class_ &parent)
	: external_member_function("Swap", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, false),						//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::swap::~swap() = default;

cminus::declaration::string::assign_copy::assign_copy(type::class_ &parent, bool append)
	: external_member_function((append ? "Append" : "Assign"), parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::assign_copy::~assign_copy() = default;

cminus::declaration::string::assign_sub_copy::assign_sub_copy(type::class_ &parent, bool append)
	: external_member_function((append ? "Append" : "Assign"), parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::assign_sub_copy::~assign_sub_copy() = default;

cminus::declaration::string::assign_buffer::assign_buffer(type::class_ &parent, bool append)
	: external_member_function((append ? "Append" : "Assign"), parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		runtime::object::global_storage->get_char_pointer_type(true),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::assign_buffer::~assign_buffer() = default;

cminus::declaration::string::assign_fill::assign_fill(type::class_ &parent, bool append)
	: external_member_function((append ? "Append" : "Assign"), parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"fill",																	//Name
		runtime::object::global_storage->get_char_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::assign_fill::~assign_fill() = default;

cminus::declaration::string::insert_copy::insert_copy(type::class_ &parent)
	: external_member_function("Insert", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::insert_copy::~insert_copy() = default;

cminus::declaration::string::insert_sub_copy::insert_sub_copy(type::class_ & parent)
	: external_member_function("Insert", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<variable>(
		"other",																//Name
		misc_helper::get_string_ref_type(parent, true),							//Type
		attribute::collection::list_type{},										//Attributes
		flags::nil,																//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"position",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::insert_sub_copy::~insert_sub_copy() = default;

cminus::declaration::string::insert_buffer::insert_buffer(type::class_ & parent)
	: external_member_function("Insert", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"data",																	//Name
		runtime::object::global_storage->get_char_pointer_type(true),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		misc_helper::get_nan_size_arg()											//Initialization
	));
}

cminus::declaration::string::insert_buffer::~insert_buffer() = default;

cminus::declaration::string::insert_fill::insert_fill(type::class_ & parent)
	: external_member_function("Insert", parent, attribute::collection::list_type{}, flags::nil, runtime::object::global_storage->get_void_type()){
	add_parameter(std::make_shared<declaration::variable>(
		"offset",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"size",																	//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));

	add_parameter(std::make_shared<declaration::variable>(
		"fill",																	//Name
		runtime::object::global_storage->get_char_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::nil,												//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	));
}

cminus::declaration::string::insert_fill::~insert_fill() = default;
