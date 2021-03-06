#include "../declaration/variable_declaration.h"
#include "../declaration/function_declaration_group.h"
#include "../declaration/special_function_declaration.h"

#include "global_storage.h"
#include "storage_object.h"

cminus::storage::unnamed_object::~unnamed_object(){
	destroy_entries_();
}

std::string cminus::storage::unnamed_object::get_qname() const{
	return ((get_parent() == nullptr) ? get_name() : (get_parent()->get_qname() + "::" + get_name()));
}

void cminus::storage::unnamed_object::add(std::shared_ptr<declaration::object> entry, std::size_t address){
	lock_guard guard(*this);
	if (auto variable_entry = std::dynamic_pointer_cast<declaration::variable>(entry); variable_entry != nullptr)
		return add_variable_(variable_entry, address);

	if (auto function_entry = std::dynamic_pointer_cast<declaration::callable>(entry); function_entry != nullptr)
		return add_callable_(function_entry, address);
}

void cminus::storage::unnamed_object::add_entry(std::shared_ptr<declaration::object> decl, std::shared_ptr<memory::reference> value, bool check_existing){
	lock_guard guard(*this);
	add_entry_(decl, value, check_existing);
}

bool cminus::storage::unnamed_object::exists(const std::string &name) const{
	lock_guard guard(*this);
	return (!name.empty() && exists_(name));
}

bool cminus::storage::unnamed_object::exists(operators::id id) const{
	lock_guard guard(*this);
	return exists_(id);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find(const std::string &name, bool search_tree) const{
	return find(name, nullptr, search_tree);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_(name, context, 0u); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(name, context, true);

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find(operators::id id, bool search_tree) const{
	return find(id, nullptr, search_tree);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find(operators::id id, std::shared_ptr<memory::reference> context, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_(id, context); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(id, context, true);

	return nullptr;
}

cminus::declaration::callable_group *cminus::storage::unnamed_object::find_operator(operators::id id, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_operator_(id); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_operator(id, true);

	return nullptr;
}

std::shared_ptr<cminus::attribute::object> cminus::storage::unnamed_object::find_attribute(const std::string &name, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_attribute_(name); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_attribute(name, true);

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::unnamed_object::find_type(const std::string &name, bool search_tree) const{
	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_type_(name); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_type(name, true);

	return nullptr;
}

cminus::storage::object *cminus::storage::unnamed_object::find_storage(const std::string &name, bool search_tree) const{
	if (name.empty())
		return nullptr;

	{//Scoped lock
		lock_guard guard(*this);
		if (auto entry = find_storage_(name); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_storage(name, true);

	return nullptr;
}

bool cminus::storage::unnamed_object::is_accessible(unsigned int access) const{
	return true;
}

void cminus::storage::unnamed_object::traverse_entries(const std::function<void(const entry_info &)> &callback, bool reversed) const{
	if (reversed){
		for (auto it = entries_.rbegin(); it != entries_.rend(); ++it)
			callback(*it);
	}
	else{//No reverse
		for (auto &info : entries_)
			callback(info);
	}
}

void cminus::storage::unnamed_object::destroy_entries_(){
	if (entries_.empty())
		return;

	for (auto it = entries_.rbegin(); it != entries_.rend(); ++it){
		if (auto mem_entry = std::get_if<std::shared_ptr<memory::reference>>(&it->value); mem_entry != nullptr && (*mem_entry)->is_constructed())
			it->decl->get_type()->destruct(*mem_entry);
		it->value = '\0';//Destroy value
	}

	entries_.clear();
	mapped_entries_.clear();
}

void cminus::storage::unnamed_object::acquire_lock_() const{
	mutex_.lock();
}

void cminus::storage::unnamed_object::release_lock_() const{
	mutex_.unlock();
}

void cminus::storage::unnamed_object::add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (!name.empty() && mapped_entries_.find(name) != mapped_entries_.end())
		throw exception::duplicate_entry();

	auto value = initialize_variable_(entry, address);
	if (!name.empty())
		mapped_entries_[name] = &entries_.back();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::initialize_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	auto value = entry->evaluate(address);
	if (value == nullptr)
		throw memory::exception::allocation_failure();

	entries_.push_back(entry_info{ entry, value });
	return value;
}

void cminus::storage::unnamed_object::add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (auto existing_entry = mapped_entries_.find(name); existing_entry != mapped_entries_.end()){
		if (std::holds_alternative<declaration::callable_group *>(existing_entry->second->value))
			std::get<declaration::callable_group *>(existing_entry->second->value)->add(entry);
		else
			throw exception::duplicate_entry();

		return;
	}

	if (address == 0u)//Allocate block
		address = runtime::object::memory_object->allocate_write_protected_block(sizeof(void *));

	auto group = std::make_shared<declaration::function_group>(entry->get_id(), name, this, address);
	if (group == nullptr)
		throw memory::exception::allocation_failure();

	group->add(entry);
	entries_.push_back(entry_info{ group, group.get() });
	mapped_entries_[name] = &entries_.back();

	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
	runtime::object::memory_object->write_scalar(address, static_cast<declaration::callable_group *>(group.get()));
}

void cminus::storage::unnamed_object::add_attribute_(std::shared_ptr<attribute::object> entry){
	auto &name = entry->get_name();
	if (!name.empty() && mapped_entries_.find(name) != mapped_entries_.end())
		throw exception::duplicate_entry();

	entries_.push_back(entry_info{ nullptr, entry });
	if (!name.empty())
		mapped_entries_[name] = &entries_.back();
}

void cminus::storage::unnamed_object::add_type_(std::shared_ptr<type::object> entry){
	auto &name = entry->get_name();
	if (!name.empty() && mapped_entries_.find(name) != mapped_entries_.end())
		throw exception::duplicate_entry();

	entries_.push_back(entry_info{ nullptr, entry });
	if (!name.empty())
		mapped_entries_[name] = &entries_.back();
}

void cminus::storage::unnamed_object::add_storage_(std::shared_ptr<object> entry){
	auto &name = entry->get_name();
	if (!name.empty() && mapped_entries_.find(name) != mapped_entries_.end())
		throw exception::duplicate_entry();

	entries_.push_back(entry_info{ nullptr, entry });
	if (!name.empty())
		mapped_entries_[name] = &entries_.back();
}

void cminus::storage::unnamed_object::add_entry_(std::shared_ptr<declaration::object> decl, std::shared_ptr<memory::reference> value, bool check_existing){
	auto name = ((decl == nullptr) ? "" : decl->get_name());
	if (check_existing && !name.empty() && mapped_entries_.find(name) != mapped_entries_.end())
		throw exception::duplicate_entry();

	entries_.push_back(entry_info{ decl, value });
	if (!name.empty())
		mapped_entries_[name] = &entries_.back();
}

bool cminus::storage::unnamed_object::exists_(const std::string &name) const{
	return (!name.empty() && !mapped_entries_.empty() && mapped_entries_.find(name) != mapped_entries_.end());
}

bool cminus::storage::unnamed_object::exists_(operators::id id) const{
	return (find_operator_(id) != nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const{
	if (name.empty() || mapped_entries_.empty())
		return nullptr;

	if (auto it = mapped_entries_.find(name); it != mapped_entries_.end()){
		if (it->second->decl != nullptr && !is_accessible(it->second->decl->get_flags()))
			throw storage::exception::inaccessible_entry();
		return std::visit(resolved_declaration_type_visitor(*this, it->second->decl, context, address), it->second->value);
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find_(operators::id id, std::shared_ptr<memory::reference> context) const{
	if (auto entry = find_operator_(id); entry != nullptr)
		return std::make_shared<memory::member_reference>(*entry, context);
	return nullptr;
}

cminus::declaration::callable_group *cminus::storage::unnamed_object::find_operator_(operators::id id) const{
	if (auto it = mapped_entries_.find(operators::convert_id_to_string(id)); it != mapped_entries_.end() && std::holds_alternative<declaration::callable_group *>(it->second->value)){
		auto group = std::get<declaration::callable_group *>(it->second->value);
		return ((group->get_id() == declaration::callable::id_type::operator_) ? group : nullptr);
	}

	return nullptr;
}

std::shared_ptr<cminus::attribute::object> cminus::storage::unnamed_object::find_attribute_(const std::string &name) const{
	/*if (name.empty() || attributes_.empty())
		return nullptr;

	if (auto it = attributes_.find(name); it != attributes_.end())
		return it->second;*/

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::unnamed_object::find_type_(const std::string &name) const{
	/*if (name.empty() || types_.empty())
		return nullptr;

	if (auto it = types_.find(name); it != types_.end())
		return it->second;*/

	return nullptr;
}

cminus::storage::object *cminus::storage::unnamed_object::find_storage_(const std::string &name) const{
	/*if (name == get_name())
		return const_cast<unnamed_object *>(this);

	if (!storages_.empty()){
		if (auto it = storages_.find(name); it != storages_.end())
			return it->second.get();
	}*/

	return dynamic_cast<object *>(find_type_(name).get());
}

cminus::storage::named_object::named_object(const std::string &name, object *parent)
	: name_(name), parent_(parent){}

cminus::storage::named_object::~named_object(){
	destroy_entries_();
}

const std::string &cminus::storage::named_object::get_name() const{
	return name_;
}

cminus::storage::object *cminus::storage::named_object::get_parent() const{
	return parent_;
}

cminus::storage::unnamed_object::resolved_declaration_type_visitor::resolved_declaration_type_visitor(const unnamed_object &target, std::shared_ptr<declaration::object> decl, std::shared_ptr<memory::reference> context, std::size_t address)
	: target_(target), decl_(decl), context_(context), address_(address){}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::size_t val){
	if (decl_ == nullptr)
		throw declaration::exception::bad_declaration();

	if (context_ == nullptr)
		throw storage::exception::no_member_context();

	if (decl_->get_type()->is_ref())
		return std::make_shared<memory::indirect_member_reference>((address_ + val), *decl_, context_);

	return std::make_shared<memory::member_reference>((address_ + val), *decl_, context_);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::shared_ptr<memory::reference> val){
	return val;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(declaration::callable_group *val){
	return std::make_shared<memory::member_reference>(*val, context_);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::shared_ptr<attribute::object> val){
	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::shared_ptr<type::object> val){
	auto block = runtime::object::memory_object->get_block(runtime::object::memory_object->allocate_write_protected_block(sizeof(void *)));

	runtime::value_guard guard(runtime::object::system_block, block.get());
	block->write_scalar(val.get());

	return std::make_shared<memory::rval_reference>(block->get_address(), runtime::object::global_storage->get_cached_type(global::cached_type::type));
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::shared_ptr<object> val){
	return nullptr;
}

cminus::storage::unnamed_object::lock_guard::lock_guard(const unnamed_object &target)
	: target_(target){
	target_.acquire_lock_();
}

cminus::storage::unnamed_object::lock_guard::~lock_guard(){
	target_.release_lock_();
}
