#include "../type/proxy_type.h"
#include "../type/modified_type.h"

#include "variable_declaration.h"
#include "special_function_declaration.h"

cminus::declaration::constructor::constructor(type::class_ &parent)
	: function(parent.get_name(), &parent, attribute::collection::list_type{}, flags::nil, nullptr){}

cminus::declaration::constructor::~constructor() = default;

void cminus::declaration::constructor::add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization){
	init_list_.push_back(init_info{ key, initialization });
}

void cminus::declaration::constructor::evaluate_body_() const{
	std::unordered_map<std::string, std::shared_ptr<node::object>> member_init_list;

	auto self = runtime::object::current_storage->find("self", false);
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);

	for (auto &member_variable : reinterpret_cast<type::class_ *>(parent_)->get_member_variables()){
		if (auto it = member_init_list.find(member_variable.value->get_name()); it != member_init_list.end()){
			member_variable.value->get_type()->construct(
				class_parent->find(member_variable.value->get_name(), self),
				it->second
			);
		}
		else{//Construct default
			member_variable.value->get_type()->construct(
				class_parent->find(member_variable.value->get_name(), self)
			);
		}
	}

	function::evaluate_body_();
}

cminus::declaration::default_constructor::default_constructor(type::class_ &parent)
	: constructor(parent){}

cminus::declaration::default_constructor::~default_constructor() = default;

void cminus::declaration::default_constructor::add_parameter(std::shared_ptr<variable> value){
	throw exception::bad_parameter_list();
}

bool cminus::declaration::default_constructor::is_defined() const{
	return true;
}

void cminus::declaration::default_constructor::add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization){
	throw exception::bad_init_list();
}

void cminus::declaration::default_constructor::evaluate_body_() const{
	auto self = runtime::object::current_storage->find("self", false);
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);

	for (auto &member_variable : class_parent->get_member_variables()){//Default construct member variables
		member_variable.value->get_type()->construct(
			class_parent->find(member_variable.value->get_name(), self)
		);
	}
}

cminus::declaration::copy_constructor::copy_constructor(type::class_ &parent)
	: constructor(parent){
	std::shared_ptr<type::object> other_type = std::make_shared<type::proxy>(parent);

	other_type = std::make_shared<type::ref>(other_type);
	other_type = std::make_shared<type::constant>(other_type);

	add_parameter(std::make_shared<variable>(
		"other",								//Name
		other_type,								//Type
		attribute::collection::list_type{},		//Attributes
		flags::nil,								//Flags
		nullptr									//Initialization
	));
}

cminus::declaration::copy_constructor::~copy_constructor() = default;

void cminus::declaration::copy_constructor::add_parameter(std::shared_ptr<variable> value){
	if (2u <= parameter_list_.size())
		throw exception::bad_parameter_list();
}

bool cminus::declaration::copy_constructor::is_defined() const{
	return true;
}

void cminus::declaration::copy_constructor::add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization){
	throw exception::bad_init_list();
}

void cminus::declaration::copy_constructor::evaluate_body_() const{
	auto self = runtime::object::current_storage->find("self", false);
	auto other = runtime::object::current_storage->find("other", false);

	auto class_parent = reinterpret_cast<type::class_ *>(parent_);
	for (auto &member_variable : class_parent->get_member_variables()){//Copy construct member variables
		member_variable.value->get_type()->construct(
			class_parent->find(member_variable.value->get_name(), self),
			class_parent->find(member_variable.value->get_name(), other)
		);
	}
}

cminus::declaration::destructor::destructor(type::class_ &parent)
	: function(("~" + parent.get_name()), &parent, attribute::collection::list_type{}, flags::nil, nullptr){}

cminus::declaration::destructor::~destructor() = default;

void cminus::declaration::destructor::add_parameter(std::shared_ptr<variable> value){
	throw exception::bad_parameter_list();
}

void cminus::declaration::destructor::evaluate_body_() const{
	function::evaluate_body_();

	auto self = runtime::object::current_storage->find("self", false);
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);

	auto &member_variables = class_parent->get_member_variables();
	for (auto it = member_variables.rbegin(); it != member_variables.rend(); ++it){//Default construct member variables
		(*it).value->get_type()->destruct(
			class_parent->find(it->value->get_name(), self)
		);
	}
}

cminus::declaration::default_destructor::default_destructor(type::class_ &parent)
	: destructor(parent){}

cminus::declaration::default_destructor::~default_destructor() = default;

bool cminus::declaration::default_destructor::is_defined() const{
	return true;
}