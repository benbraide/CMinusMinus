#include "../type/proxy_type.h"
#include "../type/class_type.h"
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
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);
	std::unordered_map<std::string, std::shared_ptr<node::object>> init_list;

	std::string computed_name;
	for (auto &init_item : init_list_){
		if (auto name = init_item.key->evaluate_as_name(false); !name.empty())
			computed_name = name;
		else if (auto class_item = dynamic_cast<type::class_ *>(init_item.key->evaluate_as_storage()); class_item != nullptr && class_parent->is_base_type(*class_item, false))
			computed_name = class_item->get_name();
		else
			throw runtime::exception::bad_constructor_init_list();

		if (init_list.find(computed_name) == init_list.end())
			init_list[computed_name] = init_item.initialization;
		else
			throw runtime::exception::bad_constructor_init_list();
	}

	auto self = runtime::object::current_storage->find("self", false);
	for (auto &base_type : class_parent->get_base_types()){
		if (auto it = init_list.find(base_type.value->get_name()); it != init_list.end()){
			base_type.value->construct(
				self->apply_offset(base_type.address_offset, base_type.value),
				it->second
			);
		}
		else{//Construct default
			base_type.value->construct(
				self->apply_offset(base_type.address_offset, base_type.value)
			);
		}
	}

	for (auto &member_variable : reinterpret_cast<type::class_ *>(parent_)->get_member_variables()){
		if (auto it = init_list.find(member_variable.value->get_name()); it != init_list.end()){
			member_variable.value->get_type()->construct(
				class_parent->find(member_variable.value->get_name(), self, false),
				it->second
			);
		}
		else{//Construct default
			member_variable.value->get_type()->construct(
				class_parent->find(member_variable.value->get_name(), self, false)
			);
		}
	}

	function::evaluate_body_();
}

cminus::declaration::defined_constructor::~defined_constructor() = default;

void cminus::declaration::defined_constructor::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_constructor::get_definition() const{
	return definition_;
}

cminus::declaration::external_constructor::~external_constructor() = default;

void cminus::declaration::external_constructor::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_constructor::get_definition() const{
	return nullptr;
}

cminus::declaration::default_constructor::default_constructor(type::class_ &parent)
	: external_constructor(parent){}

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

	for (auto &base_type : class_parent->get_base_types()){//Default construct base types
		base_type.value->construct(
			self->apply_offset(base_type.address_offset, base_type.value)
		);
	}

	for (auto &member_variable : class_parent->get_member_variables()){//Default construct member variables
		member_variable.value->get_type()->construct(
			class_parent->find(member_variable.value->get_name(), self, false)
		);
	}
}

cminus::declaration::copy_constructor::copy_constructor(type::class_ &parent)
	: external_constructor(parent){
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
	for (auto &base_type : class_parent->get_base_types()){//Copy construct base types
		base_type.value->construct(
			self->apply_offset(base_type.address_offset, base_type.value),
			other->apply_offset(base_type.address_offset, base_type.value)
		);
	}

	for (auto &member_variable : class_parent->get_member_variables()){//Copy construct member variables
		member_variable.value->get_type()->construct(
			class_parent->find(member_variable.value->get_name(), self, false),
			class_parent->find(member_variable.value->get_name(), other, false)
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
	for (auto it = member_variables.rbegin(); it != member_variables.rend(); ++it){
		it->value->get_type()->destruct(
			class_parent->find(it->value->get_name(), self, false)
		);
	}

	auto &base_types = class_parent->get_base_types();
	for (auto it = base_types.rbegin(); it != base_types.rend(); ++it)
		it->value->destruct(self->apply_offset(it->address_offset, it->value));
}

cminus::declaration::defined_destructor::~defined_destructor() = default;

void cminus::declaration::defined_destructor::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_destructor::get_definition() const{
	return definition_;
}

cminus::declaration::external_destructor::~external_destructor() = default;

void cminus::declaration::external_destructor::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_destructor::get_definition() const{
	return nullptr;
}

cminus::declaration::default_destructor::default_destructor(type::class_ &parent)
	: external_destructor(parent){}

cminus::declaration::default_destructor::~default_destructor() = default;

bool cminus::declaration::default_destructor::is_defined() const{
	return true;
}
