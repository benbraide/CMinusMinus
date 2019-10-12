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
		class_context_->get_type(),
		attribute::collection::list_type{},
		nullptr
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

bool cminus::type::class_::is_constructible(std::shared_ptr<memory::reference> target) const{
	auto constructor = find_function_(get_name());
	if (constructor == nullptr || constructor->get_id() != declaration::callable::id_type::constructor)
		return false;

	try{
		return (constructor->find(dummy_context_, std::list<std::shared_ptr<memory::reference>>{ target }) != nullptr);
	}
	catch (const declaration::exception::ambiguous_function_call &){}

	return false;
}

void cminus::type::class_::destruct(std::shared_ptr<memory::reference> target) const{
	auto destructor = find_function_(("~" + get_name()));
	if (destructor == nullptr)
		throw declaration::exception::function_not_found();

	if (destructor->get_id() == declaration::callable::id_type::destructor)
		target = destructor->call(target, std::list<std::shared_ptr<memory::reference>>{});
	else
		throw runtime::exception::bad_destructor();
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::get_default_value(std::shared_ptr<type_base> self) const{
	return nullptr;
}

void cminus::type::class_::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	auto callable = find_function_(("ToString"));
	if (callable == nullptr)
		throw runtime::exception::not_supported();

	auto value = callable->call(data, {});
	if (value == nullptr)
		throw runtime::exception::not_supported();

	auto str_value = value->get_type()->cast(value, runtime::object::global_storage->get_string_type(), cast_type::rval_static);
	if (str_value == nullptr)
		throw runtime::exception::not_supported();

	auto str_data = runtime::object::global_storage->get_string_value(str_value);
	writer.write_buffer(str_data.data(), str_data.size());
}

std::size_t cminus::type::class_::get_size() const{
	return std::max(size_, static_cast<std::size_t>(1));
}

bool cminus::type::class_::is_exact(const type_base &target) const{
	return (target.get_non_proxy() == this);
}

int cminus::type::class_::get_score(const type_base &target) const{
	if (target.is(query_type::explicit_auto))
		return get_score_value(score_result_type::auto_assignable);

	auto class_target = dynamic_cast<class_ *>(target.get_non_proxy());
	if (class_target == nullptr)
		return get_score_value(score_result_type::nil);

	if (class_target == this)
		return get_score_value(score_result_type::exact);

	return get_score_value((compute_base_offset_(*class_target, 0u) == static_cast<std::size_t>(-1)) ? score_result_type::nil : score_result_type::ancestor);
}

std::size_t cminus::type::class_::compute_base_offset(const type_base &base_type) const{
	if (auto class_base_type = dynamic_cast<const class_ *>(base_type.get_non_proxy()); class_base_type != nullptr)
		return compute_base_offset_(*class_base_type, 0u);
	return static_cast<std::size_t>(-1);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

	if (auto callable = find_operator(*target_type); callable != nullptr)
		return callable->call(data, std::list<std::shared_ptr<memory::reference>>{});

	auto is_ref = target_type->is(query_type::ref);
	if (is_ref && !target_type->is(query_type::const_) && !data->is_lvalue())
		return nullptr;

	auto class_target_type = dynamic_cast<class_ *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (class_target_type == nullptr)
		return nullptr;

	std::size_t base_offset = 0u;
	if (class_target_type != this && (base_offset = compute_base_offset_(*class_target_type, 0u)) == static_cast<std::size_t>(-1))
		return nullptr;

	if (is_ref || type == cast_type::rval_static)//No copying
		return ((base_offset == 0u) ? data : data->apply_offset(base_offset, target_type->convert(conversion_type::remove_ref, target_type)));

	return runtime::object::global_storage->copy(data, target_type->convert(conversion_type::remove_ref, target_type));
}

bool cminus::type::class_::is(query_type type, const type_base *arg) const{
	if (type == query_type::offspring_of || type == query_type::child_of){
		if (auto class_arg = dynamic_cast<const class_ *>(arg); class_arg != nullptr)
			return is_base_type_(*class_arg, (type == query_type::offspring_of));
	}

	return (type == query_type::class_);
}

bool cminus::type::class_::is_accessible(unsigned int access) const{
	if ((runtime::object::state & runtime::flags::allow_access) != 0u)
		return true;

	auto class_storage = runtime::object::current_storage->get_first_of<class_>();
	if (class_storage == this)//Private access
		return true;

	if (class_storage == nullptr)//Public access
		return ((access & (declaration::flags::private_ | declaration::flags::protected_)) == 0u);

	auto computed_access = class_storage->get_base_type_access_(*this, true);
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
		if ((access & declaration::flags::private_) != 0u)
			access = declaration::flags::private_;
		else if ((access & declaration::flags::protected_) != 0u)
			access = declaration::flags::protected_;
		else
			access = declaration::flags::public_;

		base_types_.push_back(base_type_info{ access, size_, value });
		base_types_map_[value->get_name()] = &base_types_.back();
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
	std::lock_guard<std::mutex> guard(lock_);
	return exists_(target_type);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);

		auto context_type = context->get_type();
		auto non_const_ref_context_type = context_type->convert(conversion_type::remove_ref_const, context_type);

		if (auto entry = find_(name, ((non_const_ref_context_type->get_non_proxy() == this) ? context : nullptr), 0u); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(name, true);

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_(target_type, context, exact);
}

cminus::declaration::callable_group *cminus::type::class_::find_operator(const type::object &target_type, bool exact) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_operator_(target_type, exact);
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_non_static_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);

	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_non_static_member(const std::string &name, std::shared_ptr<memory::reference> context) const{
	if (context == nullptr)
		return nullptr;

	auto entry = find_non_static_member(name);
	if (entry == nullptr)
		return nullptr;

	return std::make_shared<memory::reference>(
		(context->get_address() + entry->address_offset),
		entry->value->get_type(),
		entry->value->get_attributes().get_list(),
		context
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_static_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);

	if (auto it = named_entries_.find(name); it != named_entries_.end())
		return it->second;

	return nullptr;
}

cminus::declaration::callable_group *cminus::type::class_::find_function(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_function_(name);
}

bool cminus::type::class_::is_assignable_to(std::shared_ptr<type_base> target_type) const{
	return (find_operator(*target_type, false) != nullptr);
}

bool cminus::type::class_::is_base_type(const class_ &target, bool search_hierarchy) const{
	return is_base_type_(target, search_hierarchy);
}

const std::list<cminus::type::class_::member_variable_info> &cminus::type::class_::get_member_variables() const{
	return member_variables_;
}

const std::list<cminus::type::class_::base_type_info> &cminus::type::class_::get_base_types() const{
	return base_types_;
}

void cminus::type::class_::compile(){
	std::lock_guard<std::mutex> guard(lock_);
	compile_();
}

void cminus::type::class_::construct_(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &args) const{
	auto constructor = find_function_(get_name());
	if (constructor == nullptr)
		throw declaration::exception::function_not_found();

	if (constructor->get_id() != declaration::callable::id_type::constructor)
		throw runtime::exception::bad_constructor();

	target = constructor->call(target, args);
}

bool cminus::type::class_::add_(std::shared_ptr<declaration::object> entry, std::size_t address){
	auto function_entry = std::dynamic_pointer_cast<declaration::type_operator>(entry);
	if (function_entry == nullptr)
		return false;

	if (!is_compiling_ && ((entry->get_flags() & declaration::flags::immediate) == 0u)){
		add_callable_(function_entry, address);
		return true;
	}

	auto target_type = function_entry->get_target_type();
	if (auto existing_entry = find_operator_(*target_type, true); existing_entry == nullptr){//New entry
		std::shared_ptr<memory::block> block;
		if (address == 0u)
			block = runtime::object::memory_object->allocate_block(sizeof(void *));
		else
			block = runtime::object::memory_object->find_block(address);

		if (block == nullptr || block->get_address() == 0u)
			throw memory::exception::allocation_failure();

		auto group = std::make_shared<declaration::function_group>(function_entry->get_id(), entry->get_name(), this, block->get_address());
		if (group == nullptr)
			throw memory::exception::allocation_failure();

		group->add(function_entry);
		type_operators_.push_back(type_operator_info{ target_type, group });
	}
	else//Add to existing
		existing_entry->add(function_entry);

	return true;
}

void cminus::type::class_::add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	auto entry_type = entry->get_type();
	auto entry_memory_size = entry_type->get_memory_size();
	if (entry_memory_size == 0u && !entry_type->is(query_type::class_))
		throw declaration::exception::bad_declaration();

	if (!entry->is(declaration::flags::static_)){
		auto &name = entry->get_name();
		if (name.empty())
			throw storage::exception::unnamed_entry();

		member_variables_.push_back(member_variable_info{ member_size_, entry });
		member_variables_map_[name] = &*member_variables_.rbegin();

		member_size_ += entry_memory_size;
		size_ += entry_memory_size;

		for (auto &base_type : base_types_)//Adjust base offsets
			base_type.address_offset += entry_memory_size;
	}
	else if (!is_compiling_ && ((entry->get_flags() & declaration::flags::immediate) == 0u)){
		static_size_ += entry_memory_size;
		static_declarations_.push_back(entry);
		static_declarations_map_[entry->get_name()] = entry;
	}
	else//Static declaration
		storage_base::add_variable_(entry, address);
}

void cminus::type::class_::add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address){
	if (!is_compiling_ && ((entry->get_flags() & declaration::flags::immediate) == 0u)){
		auto &name = entry->get_name();
		if (auto it = deferred_callables_map_.find(name); it == deferred_callables_map_.end()){//New entry
			auto group = std::make_shared<declaration::function_group>(entry->get_id(), entry->get_name(), this, 0u);
			if (group == nullptr)
				throw memory::exception::allocation_failure();

			group->add(entry);
			static_size_ += sizeof(void *);

			deferred_callables_.push_back(group);
			deferred_callables_map_[name] = group;
		}
		else
			it->second->add(entry);
	}
	else
		storage_base::add_callable_(entry, address);
}

void cminus::type::class_::add_operator_(std::shared_ptr<declaration::operator_> entry, std::size_t address){
	if (!is_compiling_ && ((entry->get_flags() & declaration::flags::immediate) == 0u))
		add_callable_(entry, address);
	else
		storage_base::add_callable_(entry, address);
}

bool cminus::type::class_::exists_(const std::string &name, entry_type type) const{
	return (member_variables_map_.find(name) != member_variables_map_.end() || base_types_map_.find(name) != base_types_map_.end() ||
		static_declarations_map_.find(name) != static_declarations_map_.end() || storage_base::exists_(name, type));

	if (type == entry_type::function)
		return false;

	return (deferred_callables_map_.find(name) != deferred_callables_map_.end());
}

bool cminus::type::class_::exists_(const type::object &target_type) const{
	return (find_operator_(target_type, true) != nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name) const{
	return find_(name, nullptr, 0u);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address_offset) const{
	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end()){
		if (context == nullptr)
			throw storage::exception::no_member_context();

		if (!is_accessible(it->second->value->get_flags()))
			throw storage::exception::inaccessible_entry();

		return std::make_shared<memory::reference>(
			(address_offset + context->get_address() + it->second->address_offset),
			it->second->value->get_type(),
			it->second->value->get_attributes().get_list(),
			context
		);
	}

	if (auto entry = storage_base::find_(name); entry != nullptr){
		if (auto function_entry = dynamic_cast<memory::function_reference *>(entry.get()); function_entry != nullptr){//Bind context
			return std::make_shared<memory::function_reference>(
				*function_entry->get_entry(),
				function_entry->get_type(),
				((context == nullptr) ? class_context_ : context)
			);
		}

		if (auto declaration_entry = declarations_.find(name); declaration_entry != declarations_.end() && !is_accessible(declaration_entry->second->get_flags()))
			throw storage::exception::inaccessible_entry();

		return entry;
	}

	for (auto &base_type : base_types_){
		if (auto entry = base_type.value->find_(name, context, (address_offset + base_type.address_offset)); entry != nullptr)
			return entry;
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact) const{
	if (auto entry = find_operator_(target_type, exact); entry != nullptr){
		return std::make_shared<memory::function_reference>(
			*entry,
			entry->get_type(),
			((context == nullptr) ? class_context_ : context)
		);
	}

	return nullptr;
}

cminus::declaration::callable_group *cminus::type::class_::find_operator_(const type::object &target_type, bool exact) const{
	for (auto &entry : type_operators_){
		if (exact && entry.key->is_exact(target_type))
			return entry.group.get();

		if (!exact && entry.key->get_score(target_type) != get_score_value(score_result_type::nil))
			return entry.group.get();
	}

	return nullptr;
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
	auto it = functions_.find(name);
	return ((it == functions_.end()) ? nullptr : it->second.get());
}

void cminus::type::class_::compile_(){
	if (static_size_ == 0u)
		return;

	auto block = runtime::object::memory_object->allocate_block(static_size_);
	if (block == nullptr || (static_address_ = block->get_address()) == 0u)
		throw memory::exception::allocation_failure();

	is_compiling_ = true;
	auto current_address = static_address_;

	for (auto entry : static_declarations_){
		add_variable_(entry, current_address);
		current_address += entry->get_type()->get_memory_size();
	}

	for (auto entry : deferred_callables_){
		entry->traverse_list([&](std::shared_ptr<declaration::callable> entry){
			if (add_(entry, current_address))
				return;//Handled

			if (auto function_entry = std::dynamic_pointer_cast<declaration::operator_>(entry); function_entry != nullptr)
				return add_operator_(function_entry, current_address);

			if (auto function_entry = std::dynamic_pointer_cast<declaration::callable>(entry); function_entry != nullptr)
				return add_callable_(function_entry, current_address);

			throw declaration::exception::bad_declaration();
		});

		current_address += sizeof(void *);
	}

	static_declarations_.clear();
	static_declarations_map_.clear();

	deferred_callables_.clear();
	deferred_callables_map_.clear();

	static_size_ = 0;
	is_compiling_ = false;
}
