#include "../storage/global_storage.h"
#include "../declaration/variable_declaration.h"
#include "../declaration/function_declaration_group.h"
#include "../declaration/special_function_declaration.h"

#include "proxy_type.h"
#include "modified_type.h"
#include "class_type.h"

cminus::type::class_::class_(const std::string &name, storage_base *parent)
	: type_base(name, parent){
	class_context_ = std::make_shared<memory::rval_reference>(0u, std::make_shared<type::proxy>(*this));
	dummy_context_ = std::make_shared<memory::reference>(
		0u,
		class_context_->get_type()
	);
}

cminus::type::class_::~class_(){
	destroy_entries_();
	if (static_address_ != 0u){
		try{
			runtime::object::memory_object->deallocate_block(static_address_);
		}
		catch (...){}
		static_address_ = 0u;
	}
}

const std::string &cminus::type::class_::get_name() const{
	return type_base::get_name();
}

std::string cminus::type::class_::get_qname() const{
	return type_base::get_qname();
}

cminus::storage::object *cminus::type::class_::get_parent() const{
	return type_base::get_parent();
}

void cminus::type::class_::destruct(std::shared_ptr<memory::reference> target) const{
	auto destructor = find_function_(("~" + get_name()));
	if (destructor == nullptr)
		throw declaration::exception::function_not_found();

	if (destructor->get_id() == declaration::callable::id_type::destructor)
		destructor->call(target, {});
	else
		throw runtime::exception::bad_destructor();
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::get_default_value() const{
	return nullptr;
}

void cminus::type::class_::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	auto callable = find_function_(("ToString"));
	if (callable == nullptr)
		throw runtime::exception::not_supported();

	auto value = callable->call(data, {});
	if (value == nullptr)
		throw runtime::exception::not_supported();

	auto str_value = value->get_type()->cast(value, runtime::object::global_storage->get_string_type(), cast_type::static_rval);
	if (str_value == nullptr)
		throw runtime::exception::not_supported();

	auto str_data = runtime::object::global_storage->get_string_value(str_value);
	writer.write_buffer(str_data.data(), str_data.size());
}

std::size_t cminus::type::class_::get_size() const{
	return std::max(size_, static_cast<std::size_t>(1));
}

std::size_t cminus::type::class_::compute_base_offset(const class_ &base_type) const{
	return compute_base_offset_(base_type, 0u);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::class_::get_evaluator() const{
	return runtime::object::global_storage->get_evaluator(evaluator::object::id_type::class_);
}

bool cminus::type::class_::is_default_constructible(bool ignore_callable) const{
	if (!ignore_callable){//Search for function
		if (auto constructor = find_function_(get_name()); constructor != nullptr && constructor->get_id() == declaration::callable::id_type::constructor){
			try{
				if (auto matched = constructor->find_by_args(std::vector<declaration::callable::arg_info>{}); matched != nullptr && dynamic_cast<declaration::deleted_constructor *>(matched.get()) == nullptr)
					return true;
			}
			catch (...){}
		}

		return false;
	}

	for (auto &base_type : base_types_){
		if (!base_type.value->is_default_constructible(false))
			return false;
	}

	for (auto &info : entries_){
		if (dynamic_cast<declaration::variable *>(info.decl.get()) == nullptr || info.decl->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.value))
			continue;

		if (!info.decl->get_type()->is_default_constructible(false))
			return false;
	}

	return true;
}

bool cminus::type::class_::is_copy_constructible(bool ignore_callable) const{
	if (!ignore_callable){//Search for function
		if (auto constructor = find_function_(get_name()); constructor != nullptr && constructor->get_id() == declaration::callable::id_type::constructor){
			try{
				if (auto matched = constructor->find_by_args({ declaration::callable::arg_info{ this, true, false } }); matched != nullptr && dynamic_cast<declaration::deleted_constructor *>(matched.get()) == nullptr)
					return true;
			}
			catch (...){}
		}

		return false;
	}

	for (auto &base_type : base_types_){
		if (!base_type.value->is_copy_constructible(false))
			return false;
	}

	for (auto &info : entries_){
		if (dynamic_cast<declaration::variable *>(info.decl.get()) == nullptr || info.decl->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.value))
			continue;

		if (!info.decl->get_type()->is_copy_constructible(false))
			return false;
	}

	return true;
}

bool cminus::type::class_::can_be_iterated() const{
	return (find_function_("begin") != nullptr && find_function_("end") != nullptr);
}

bool cminus::type::class_::is_copy_assignable(bool ignore_callable) const{
	if (!ignore_callable){//Search for function
		if (auto constructor = find_operator_(operators::id::assignment); constructor != nullptr && constructor->get_id() == declaration::callable::id_type::operator_){
			try{
				if (auto matched = constructor->find_by_args({ declaration::callable::arg_info{ this, true, false } }); matched != nullptr && dynamic_cast<declaration::deleted_operator *>(matched.get()) == nullptr)
					return true;
			}
			catch (...){}
		}

		return false;
	}

	for (auto &base_type : base_types_){
		if (!base_type.value->is_copy_assignable(false))
			return false;
	}

	for (auto &info : entries_){
		if (dynamic_cast<declaration::variable *>(info.decl.get()) == nullptr || info.decl->is(declaration::flags::static_) || !std::holds_alternative<std::size_t>(info.value))
			continue;

		if (!info.decl->get_type()->is_copy_assignable(false))
			return false;
	}

	return true;
}

bool cminus::type::class_::is_accessible(unsigned int access) const{
	if ((runtime::object::state & runtime::flags::allow_access) != 0u)
		return true;

	auto current_class_storage = runtime::object::current_storage->get_first_of<class_>();
	for (auto class_storage = current_class_storage; class_storage != nullptr; class_storage = class_storage->get_first_of<class_>()){
		if (class_storage == this)//Private access
			return true;
	}

	if (current_class_storage == nullptr)//Public access
		return ((access & (declaration::flags::private_ | declaration::flags::protected_)) == 0u);

	auto computed_access = current_class_storage->get_base_type_access_(*this, true);
	if ((computed_access & declaration::flags::private_) != 0u)
		access = declaration::flags::private_;
	else if ((computed_access & declaration::flags::protected_) != 0u && access != declaration::flags::private_)
		access = declaration::flags::protected_;

	if (computed_access == declaration::flags::nil)//Public access
		return ((access & (declaration::flags::private_ | declaration::flags::protected_)) == 0u);

	return ((access & declaration::flags::private_) == 0u);//Protected access
}

void cminus::type::class_::add_base(unsigned int access, std::shared_ptr<class_> value){
	if (!is_base_type_(*value, false)){
		base_types_.push_back(base_type_info{ access, size_, value });
		size_ += value->size_;
	}
	else
		throw storage::exception::duplicate_base();
}

void cminus::type::class_::add_default_functions(bool add_on_empty){
	if (!add_on_empty || storage_base::find(get_name(), false) == nullptr){//No constructor defined
		try{
			add(std::make_shared<declaration::default_constructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), 0u);
		}
		catch (const declaration::exception::base &){}

		try{
			add(std::make_shared<declaration::copy_constructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), 0u);
		}
		catch (const declaration::exception::base &){}
	}

	if (storage_base::find(("~" + get_name()), false) == nullptr){//No destructor defined
		try{
			add(std::make_shared<declaration::default_destructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), 0u);
		}
		catch (const declaration::exception::base &){}
	}
}

bool cminus::type::class_::exists(const type::object &target_type) const{
	lock_guard guard(*this);
	return exists_(target_type);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_(name, ((context->get_type()->remove_const_ref() == this) ? context : nullptr), 0u); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(name, true);

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact) const{
	lock_guard guard(*this);
	return find_(target_type, context, exact);
}

cminus::declaration::callable *cminus::type::class_::find_operator(const type::object &target_type, bool exact, declaration::callable_group **group) const{
	lock_guard guard(*this);
	return find_operator_(target_type, exact, group);
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_non_static_member(const std::string &name) const{
	lock_guard guard(*this);

	/*if (auto it = member_variables_map_.find(name); it != member_variables_map_.end())
		return it->second;*/

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_non_static_member(const std::string &name, std::shared_ptr<memory::reference> context) const{
	if (context == nullptr)
		return nullptr;

	auto entry = find_non_static_member(name);
	if (entry == nullptr)
		return nullptr;

	return std::make_shared<memory::member_reference>(
		(context->get_address() + entry->address_offset),
		*entry->value,
		context
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_static_member(const std::string &name) const{
	lock_guard guard(*this);

	/*if (auto it = named_entries_.find(name); it != named_entries_.end())
		return it->second;*/

	return nullptr;
}

cminus::declaration::callable_group *cminus::type::class_::find_function(const std::string &name) const{
	lock_guard guard(*this);
	return find_function_(name);
}

bool cminus::type::class_::is_constructible_from(const type_base &target_type, bool is_lval, bool is_const) const{
	auto constructor = find_function_(get_name());
	if (constructor == nullptr)
		return false;

	try{
		runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::ignore_constructible));
		return (constructor->find_by_args({ declaration::callable::arg_info{ &target_type, is_lval, is_const } }) != nullptr);
	}
	catch (const declaration::exception::ambiguous_function_call &){}

	return false;
}

bool cminus::type::class_::is_assignable_to(std::shared_ptr<type_base> target_type) const{
	return (find_operator(*target_type, false) != nullptr);
}

bool cminus::type::class_::is_base_type(const class_ &target, bool search_hierarchy) const{
	return is_base_type_(target, search_hierarchy);
}

const std::list<cminus::type::class_::base_type_info> &cminus::type::class_::get_base_types() const{
	return base_types_;
}

void cminus::type::class_::compile(){
	lock_guard guard(*this);
	compile_();
}

void cminus::type::class_::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	auto constructor = find_function_(get_name());
	if (constructor == nullptr)
		throw declaration::exception::function_not_found();

	if (constructor->get_id() == declaration::callable::id_type::constructor)
		constructor->call(target, args);
	else
		throw runtime::exception::bad_constructor();
}

int cminus::type::class_::get_score_(const type_base &target, bool is_lval, bool is_const) const{
	if (find_operator(target, false) != nullptr)
		return get_score_value(score_result_type::class_compatible);
	return get_no_conversion_score_(target, is_lval, is_const);
}

int cminus::type::class_::get_no_conversion_score_(const type_base &target, bool is_lval, bool is_const) const{
	auto class_target = target.as<class_>();
	if (class_target == nullptr)
		return get_score_value(score_result_type::nil);

	if (is_base_type(*class_target, true))
		return get_score_value(score_result_type::class_compatible);

	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (!is_static_cast(type))
		return nullptr;

	if (auto callable = find_operator(*target_type); callable != nullptr)
		return callable->call(data, {});

	auto data_is_lval = data->is_lvalue();
	if (!is_valid_static_cast(type, data_is_lval, data->is_const()))
		return nullptr;

	auto class_target_type = target_type->as<class_>();
	if (class_target_type == nullptr)
		return nullptr;

	auto offset = compute_base_offset_(*class_target_type, 0u);
	if (offset == static_cast<std::size_t>(-1))
		return nullptr;

	if (offset == 0u)
		return ((type == cast_type::static_) ? copy_data(data, target_type) : data);

	return ((!data_is_lval || type != cast_type::static_) ? data->apply_offset(offset, target_type) : copy_data(data->apply_offset(offset, target_type), target_type));
}

void cminus::type::class_::acquire_lock_() const{}

void cminus::type::class_::release_lock_() const{}

void cminus::type::class_::add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (name.empty())
		throw storage::exception::unnamed_entry();

	auto entry_memory_size = entry->get_type()->get_memory_size();
	if (entry_memory_size == 0u)
		throw declaration::exception::bad_declaration();

	if (!entry->is(declaration::flags::static_)){
		entries_.push_back(entry_info{ entry, member_size_ });
		mapped_entries_[name] = &entries_.back();

		member_size_ += entry_memory_size;
		size_ += entry_memory_size;

		for (auto &base_type : base_types_)//Adjust base offsets
			base_type.address_offset += entry_memory_size;
	}
	else{//Static member
		entries_.push_back(entry_info{ entry, static_size_ });
		mapped_entries_[name] = &entries_.back();
		static_size_ += entry_memory_size;
	}
}

void cminus::type::class_::add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (auto it = mapped_entries_.find(name); it != mapped_entries_.end()){
		if (auto group = dynamic_cast<declaration::callable_group *>(it->second->decl.get()); group != nullptr)
			group->add(entry);
		else
			throw storage::exception::duplicate_entry();
	}
	else if (auto group = std::make_shared<declaration::function_group>(entry->get_id(), name, this, 0u); group != nullptr){
		group->add(entry);
		entries_.push_back(entry_info{ group, group.get() });
		mapped_entries_[name] = &entries_.back();
		function_size_ += sizeof(void *);
	}
	else
		throw memory::exception::allocation_failure();
}

bool cminus::type::class_::exists_(const type::object &target_type) const{
	return (find_operator_(target_type, true, nullptr) != nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const{
	if (name.empty())
		return nullptr;

	if (auto entry = storage_base::find_(name, context, address); entry != nullptr)
		return entry;

	for (auto &base_type : base_types_){
		if (auto entry = base_type.value->find_(name, context, (address + base_type.address_offset)); entry != nullptr)
			return entry;
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact) const{
	declaration::callable_group *group = nullptr;
	if (auto entry = find_operator_(target_type, exact, &group); entry != nullptr)
		return std::make_shared<memory::member_reference>(*group, entry->get_type(), context);
	return nullptr;
}

cminus::declaration::callable *cminus::type::class_::find_operator_(const type::object &target_type, bool exact, declaration::callable_group **group) const{
	declaration::type_operator *op = nullptr;
	declaration::callable_group *op_group = nullptr;

	for (auto &info : mapped_entries_){
		if (!std::holds_alternative<declaration::callable_group *>(info.second->value))
			continue;

		op_group = std::get<declaration::callable_group *>(info.second->value);
		if (op_group->get_id() != declaration::callable::id_type::type_operator)
			continue;

		op_group->traverse_list([&](std::shared_ptr<declaration::callable> entry){
			if (op != nullptr || (op = dynamic_cast<declaration::type_operator *>(entry.get())) == nullptr)
				return;

			auto op_target_type = op->get_target_type();
			if (exact && op_target_type->is_exact(target_type))
				return;

			if (!exact && op_target_type->get_score(target_type, op_target_type->is_ref(), op_target_type->is_const()) != get_score_value(score_result_type::nil))
				return;

			op = nullptr;
		});

		if (group != nullptr)
			*group = ((op == nullptr) ? nullptr : op_group);

		break;
	}

	return op;
}

cminus::storage::object *cminus::type::class_::find_storage_(const std::string &name) const{
	if (name == get_name())
		return const_cast<class_ *>(this);

	for (auto &base_type : base_types_){
		if (auto value = base_type.value->find_storage_(name); value != nullptr)
			return value;
	}

	return storage_base::find_storage_(name);
}

cminus::type::class_ *cminus::type::class_::find_base_type_(const std::string &name, bool search_hierarchy) const{
	for (auto &base_type : base_types_){
		if (base_type.value->get_name() == name)
			return base_type.value.get();

		if (auto value = (search_hierarchy ? base_type.value->find_base_type_(name, true) : nullptr); value != nullptr)
			return value;
	}

	return nullptr;
}

std::size_t cminus::type::class_::compute_base_offset_(const class_ &base_type, std::size_t offset) const{
	if (&base_type == this)
		return offset;

	for (auto &entry : base_types_){
		if (auto value = entry.value->compute_base_offset_(base_type, (offset + entry.address_offset)); value != static_cast<std::size_t>(-1))
			return value;
	}

	return static_cast<std::size_t>(-1);
}

bool cminus::type::class_::is_base_type_(const class_ &target, bool search_hierarchy) const{
	return (find_base_type_(target.get_name(), search_hierarchy) != nullptr);
}

unsigned int cminus::type::class_::get_base_type_access_(const class_ &target, bool skip_immediate) const{
	auto highest_access = declaration::flags::nil;
	for (auto &base_type : base_types_){
		if (!skip_immediate){//Apply restrictions
			if ((base_type.access & declaration::flags::private_) != 0u)
				highest_access = declaration::flags::private_;
			else if ((base_type.access & declaration::flags::protected_) != 0u && highest_access != declaration::flags::private_)
				highest_access = declaration::flags::protected_;
		}

		if (auto access = base_type.value->get_base_type_access_(target, false); (access & declaration::flags::private_) != 0u)
			highest_access = declaration::flags::private_;
		else if ((access & declaration::flags::protected_) != 0u && highest_access != declaration::flags::private_)
			highest_access = declaration::flags::protected_;
	}

	return highest_access;
}

cminus::declaration::callable_group *cminus::type::class_::find_function_(const std::string &name) const{
	if (auto it = mapped_entries_.find(name); it != mapped_entries_.end() && std::holds_alternative<declaration::callable_group *>(it->second->value))
		return std::get<declaration::callable_group *>(it->second->value);
	return nullptr;
}

void cminus::type::class_::compile_(){
	if (static_size_ != 0u)
		static_address_ = runtime::object::memory_object->allocate_block(static_size_);

	auto has_constructor = (find_function_(get_name()) != nullptr);
	auto has_destructor = (find_function_(("~" + get_name())) != nullptr);
	auto has_assignment_operator = (find_operator_(operators::id::assignment) != nullptr);

	std::size_t additional_function_size = 0u;
	if (!has_constructor)
		additional_function_size += sizeof(void *);

	if (!has_destructor)
		additional_function_size += sizeof(void *);

	if (!has_assignment_operator)
		additional_function_size += sizeof(void *);

	declaration::variable *var_decl;
	declaration::callable_group *func_decl = nullptr;

	auto static_address = static_address_;
	auto function_address = function_address_ = runtime::object::memory_object->allocate_write_protected_block(function_size_ + additional_function_size);

	for (auto &decl : mapped_entries_){//Initialize static members
		if ((func_decl = dynamic_cast<declaration::callable_group *>(decl.second->decl.get())) != nullptr){
			runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
			runtime::object::memory_object->write_scalar(function_address, func_decl);

			func_decl->set_address(function_address);
			function_address += sizeof(void *);
		}
		else if ((var_decl = dynamic_cast<declaration::variable *>(decl.second->decl.get())) != nullptr && decl.second->decl->is(declaration::flags::static_)){
			decl.second->value = var_decl->evaluate(static_address);
			static_address += var_decl->get_type()->get_memory_size();
		}
	}

	if (!has_constructor){//Insert default constructors
		storage_base::add_callable_(std::make_shared<declaration::default_constructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), function_address);
		storage_base::add_callable_(std::make_shared<declaration::copy_constructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), function_address);
		function_address += sizeof(void *);
	}

	if (!has_destructor){//Insert default destructor
		storage_base::add_callable_(std::make_shared<declaration::default_destructor>(*this, attribute::collection::list_type{}, declaration::flags::nil), function_address);
		function_address += sizeof(void *);
	}

	if (!has_assignment_operator){//Insert default assignment operator
		storage_base::add_callable_(std::make_shared<declaration::copy_operator>(*this, attribute::collection::list_type{}, declaration::flags::nil), function_address);
		function_address += sizeof(void *);
	}
}
