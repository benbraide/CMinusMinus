#include "../storage/global_storage.h"
#include "../storage/specialized_storage.h"

#include "special_function_declaration.h"

cminus::declaration::member_function::~member_function() = default;

cminus::declaration::callable::id_type cminus::declaration::member_function::get_id() const{
	return id_type::member;
}

int cminus::declaration::member_function::get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	if (is(flags::static_) || context_declaration_ == nullptr)
		return type::object::get_score_value(type::object::score_result_type::exact);

	auto context_type = context->get_type();
	return get_arg_score_(context_declaration_->get_type(), arg_info{
		context_type->remove_const_ref(),
		true,
		context->is_const()
	});
}

void cminus::declaration::member_function::copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	if (context_declaration_ == nullptr)
		return;

	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::ignore_rval_ref));
	if (auto entry = context_declaration_->evaluate(0u, context); entry != nullptr)
		runtime::object::current_storage->get_first_of<storage::class_member>()->set_context(entry);
	else
		throw memory::exception::allocation_failure();
}

std::size_t cminus::declaration::member_function::get_args_count_(std::shared_ptr<memory::reference> context, std::size_t args_count) const{
	return args_count;
}

void cminus::declaration::member_function::init_context_(type::class_ &parent){
	if (is(flags::static_))//Context not required
		return;

	context_declaration_ = std::make_shared<variable>(
		"self",
		runtime::object::global_storage->get_ref_type(std::make_shared<type::proxy>(parent), is(flags::const_)),
		attribute::collection::list_type{},
		flags::nil,
		std::shared_ptr<node::object>()
	);

	dynamic_cast<type::function *>(type_.get())->add_parameter_type(context_declaration_->get_type());
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

void cminus::declaration::constructor::copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::ignore_const_ref));
	member_function::copy_context_(context, it);
}

void cminus::declaration::constructor::evaluate_body_() const{
	auto class_parent = reinterpret_cast<type::class_ *>(parent_);
	std::unordered_map<std::string, std::shared_ptr<node::object>> init_list;

	std::string computed_name;
	for (auto &init_item : init_list_){
		if (auto name = init_item.key->evaluate_as_name(); !name.empty())
			computed_name = name;
		else if (auto class_item = dynamic_cast<type::class_ *>(init_item.key->evaluate_as_storage(false)); class_item != nullptr && class_parent->is_base_type(*class_item, false))
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

	parent_->traverse_declarations([&](const storage::object::declaration_info &info){//Initialize members
		if (info.value->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.resolved))
			return;

		std::shared_ptr<memory::reference> instance;
		if (info.value->get_type()->is_ref())
			instance = std::make_shared<memory::indirect_member_reference>(std::get<std::size_t>(info.resolved), *info.value, self);
		else
			instance = std::make_shared<memory::member_reference>(std::get<std::size_t>(info.resolved), *info.value, self);

		if (instance == nullptr)
			throw memory::exception::allocation_failure();

		if (auto it = init_list.find(info.value->get_name()); it != init_list.end())
			info.value->get_type()->construct(instance, it->second);
		else//Construct default
			info.value->get_type()->construct(instance);
	});

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

	parent_->traverse_declarations([&](const storage::object::declaration_info &info){//Initialize members
		if (info.value->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.resolved))
			return;

		std::shared_ptr<memory::reference> instance, other_instance;
		if (info.value->get_type()->is_ref()){
			instance = std::make_shared<memory::indirect_member_reference>(std::get<std::size_t>(info.resolved), *info.value, self);
			other_instance = std::make_shared<memory::indirect_member_reference>(std::get<std::size_t>(info.resolved), *info.value, other);
		}
		else{
			instance = std::make_shared<memory::member_reference>(std::get<std::size_t>(info.resolved), *info.value, self);
			other_instance = std::make_shared<memory::member_reference>(std::get<std::size_t>(info.resolved), *info.value, other);
		}

		if (instance != nullptr || other_instance == nullptr)
			info.value->get_type()->construct(instance, other);
		else
			throw memory::exception::allocation_failure();
	});
}

cminus::declaration::destructor::~destructor() = default;

cminus::declaration::callable::id_type cminus::declaration::destructor::get_id() const{
	return id_type::destructor;
}

void cminus::declaration::destructor::copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const{
	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::ignore_const_ref));
	member_function::copy_context_(context, it);
}

void cminus::declaration::destructor::evaluate_body_() const{
	function::evaluate_body_();

	auto self = runtime::object::current_storage->find("self", false);
	parent_->traverse_entries([&](const storage::object::entry_info &info){//Destruct members
		if (info.decl == nullptr || info.decl->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.value) || !info.decl->get_type()->is<type::class_>())
			return;

		auto instance = std::make_shared<memory::member_reference>(std::get<std::size_t>(info.value), *info.decl, self);
		if (instance == nullptr)
			throw memory::exception::allocation_failure();

		info.decl->get_type()->destruct(instance);
	}, true);

	auto &base_types = dynamic_cast<type::class_ *>(parent_)->get_base_types();
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
	return id_type::type_operator;
}

std::shared_ptr<cminus::type::object> cminus::declaration::type_operator::get_target_type() const{
	return target_type_;
}

void cminus::declaration::type_operator::insert_target_type_(){
	dynamic_cast<type::function *>(type_.get())->add_parameter_type(target_type_);
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
