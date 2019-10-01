#include "../storage/specialized_storage.h"

#include "special_function_declaration.h"

cminus::declaration::member_function::~member_function() = default;

cminus::declaration::callable::id_type cminus::declaration::member_function::get_id() const{
	return id_type::member;
}

int cminus::declaration::member_function::get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	if (context == nullptr)
		return type::object::get_score_value(is(flags::static_) ? type::object::score_result_type::exact : type::object::score_result_type::nil);

	auto context_type = context->get_type();
	auto context_class_type = dynamic_cast<type::class_ *>(context_type->get_non_proxy());

	if (context_class_type == nullptr)
		return type::object::get_score_value(type::object::score_result_type::nil);

	auto context_is_const = context->is_const();
	auto this_context_is_const = context_type_->is(type::object::query_type::const_);

	if (context_is_const && !this_context_is_const)
		return type::object::get_score_value(type::object::score_result_type::nil);

	auto non_const_ref_this_context_type = context_type_->convert(type::object::conversion_type::remove_ref_const, context_type_);
	auto base_offset = context_class_type->compute_base_offset(*non_const_ref_this_context_type);

	if (base_offset == static_cast<std::size_t>(-1))
		return type::object::get_score_value(type::object::score_result_type::nil);

	auto score = type::object::get_score_value((base_offset == 0u) ? type::object::score_result_type::exact : type::object::score_result_type::ancestor);
	return ((context_is_const == this_context_is_const) ? score : (score - 1));
}

void cminus::declaration::member_function::copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	auto entry = std::make_shared<memory::indirect_reference>(context_type_, attribute::collection::list_type{}, nullptr);
	if (entry == nullptr)
		throw memory::exception::allocation_failure();

	entry->write_address(context->get_address());
	runtime::object::current_storage->add_entry("self", entry);
	runtime::object::current_storage->get_first_of<storage::class_member>()->set_context(entry);
}

std::size_t cminus::declaration::member_function::get_args_count_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	return args.size();
}

void cminus::declaration::member_function::init_context_(type::class_ &parent){
	if (is(flags::static_))//Context not required
		return;

	context_type_ = std::make_shared<type::proxy>(parent);
	context_type_ = std::make_shared<type::ref>(context_type_);

	if (is(flags::const_))//Constant function -- Make context type constant
		context_type_ = std::make_shared<type::constant>(context_type_);

	type_->add_parameter_type(context_type_);
}

cminus::declaration::defined_member_function::~defined_member_function() = default;

void cminus::declaration::defined_member_function::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_member_function::get_definition() const{
	return definition_;
}

cminus::declaration::external_member_function::~external_member_function() = default;

void cminus::declaration::external_member_function::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_member_function::get_definition() const{
	return nullptr;
}

bool cminus::declaration::external_member_function::is_defined() const{
	return true;
}

cminus::declaration::constructor::~constructor() = default;

cminus::declaration::callable::id_type cminus::declaration::constructor::get_id() const{
	return id_type::constructor;
}

void cminus::declaration::constructor::add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization){
	init_list_.push_back(init_info{ key, initialization });
}

void cminus::declaration::constructor::evaluate_body_() const{
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);
	std::unordered_map<std::string, std::shared_ptr<node::object>> init_list;

	std::string computed_name;
	for (auto &init_item : init_list_){
		if (auto name = init_item.key->evaluate_as_name(); !name.empty())
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

bool cminus::declaration::external_constructor::is_defined() const{
	return true;
}

cminus::declaration::default_constructor::~default_constructor() = default;

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

cminus::declaration::copy_constructor::~copy_constructor() = default;

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

cminus::declaration::destructor::~destructor() = default;

cminus::declaration::callable::id_type cminus::declaration::destructor::get_id() const{
	return id_type::destructor;
}

void cminus::declaration::destructor::evaluate_body_() const{
	function::evaluate_body_();

	auto self = runtime::object::current_storage->find("self", false);
	auto class_parent = dynamic_cast<type::class_ *>(parent_);

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

bool cminus::declaration::external_destructor::is_defined() const{
	return true;
}

cminus::declaration::default_destructor::~default_destructor() = default;

cminus::declaration::operator_::~operator_() = default;

cminus::declaration::callable::id_type cminus::declaration::operator_::get_id() const{
	return id_type::operator_;
}

cminus::operators::id cminus::declaration::operator_::get_op() const{
	return id_;
}

cminus::declaration::defined_operator::~defined_operator() = default;

void cminus::declaration::defined_operator::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_operator::get_definition() const{
	return definition_;
}

cminus::declaration::external_operator::~external_operator() = default;

void cminus::declaration::external_operator::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_operator::get_definition() const{
	return nullptr;
}

bool cminus::declaration::external_operator::is_defined() const{
	return true;
}

cminus::declaration::member_operator::~member_operator() = default;

cminus::declaration::callable::id_type cminus::declaration::member_operator::get_id() const{
	return id_type::operator_;
}

cminus::operators::id cminus::declaration::member_operator::get_op() const{
	return id_;
}

cminus::declaration::defined_member_operator::~defined_member_operator() = default;

void cminus::declaration::defined_member_operator::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_member_operator::get_definition() const{
	return definition_;
}

cminus::declaration::external_member_operator::~external_member_operator() = default;

void cminus::declaration::external_member_operator::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_member_operator::get_definition() const{
	return nullptr;
}

bool cminus::declaration::external_member_operator::is_defined() const{
	return true;
}

cminus::declaration::type_operator::~type_operator() = default;

cminus::declaration::callable::id_type cminus::declaration::type_operator::get_id() const{
	return id_type::operator_;
}

std::shared_ptr<cminus::type::object> cminus::declaration::type_operator::get_target_type() const{
	return target_type_;
}

cminus::declaration::defined_type_operator::~defined_type_operator() = default;

void cminus::declaration::defined_type_operator::define(std::shared_ptr<node::object> definition){
	if (definition_ == nullptr)
		definition_ = definition;
	else
		throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::defined_type_operator::get_definition() const{
	return definition_;
}

cminus::declaration::external_type_operator::~external_type_operator() = default;

void cminus::declaration::external_type_operator::define(std::shared_ptr<node::object> definition){
	throw exception::function_redefinition();
}

std::shared_ptr<cminus::node::object> cminus::declaration::external_type_operator::get_definition() const{
	return nullptr;
}

bool cminus::declaration::external_type_operator::is_defined() const{
	return true;
}
