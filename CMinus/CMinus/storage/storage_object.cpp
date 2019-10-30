#include "../declaration/variable_declaration.h"
#include "../declaration/function_declaration_group.h"
#include "../declaration/special_function_declaration.h"

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

void cminus::storage::unnamed_object::add_entry(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing){
	lock_guard guard(*this);
	add_entry_(entry, value, check_existing);
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

void cminus::storage::unnamed_object::traverse_declarations(const std::function<void(const declaration_info &)> &callback) const{
	for (auto &info : declarations_)
		callback(info.second);
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

	declarations_.clear();
	for (auto it = entries_.rbegin(); it != entries_.rend(); ++it)
		*it = entry_info{};//Destroy object

	entries_.clear();
}

void cminus::storage::unnamed_object::acquire_lock_() const{
	mutex_.lock();
}

void cminus::storage::unnamed_object::release_lock_() const{
	mutex_.unlock();
}

void cminus::storage::unnamed_object::add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (!name.empty() && declarations_.find(name) != declarations_.end())
		throw exception::duplicate_entry();

	auto value = initialize_variable_(*entry, address);
	if (!name.empty())
		declarations_[name] = declaration_info{ entry, value };
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::initialize_variable_(declaration::variable &entry, std::size_t address){
	auto value = entry.evaluate(address);
	if (value == nullptr)
		throw memory::exception::allocation_failure();

	entries_.push_back(entry_info{ &entry, value });
	return value;
}

void cminus::storage::unnamed_object::add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address){
	auto &name = entry->get_name();
	if (auto existing_entry = declarations_.find(name); existing_entry != declarations_.end()){
		if (std::holds_alternative<declaration::callable_group *>(existing_entry->second.resolved))
			std::get<declaration::callable_group *>(existing_entry->second.resolved)->add(entry);
		else
			throw exception::duplicate_entry();
	}

	std::shared_ptr<memory::block> block;
	if (address == 0u)
		block = runtime::object::memory_object->allocate_block(sizeof(void *));
	else
		block = runtime::object::memory_object->find_block(address);

	if (block == nullptr || block->get_address() == 0u)
		throw memory::exception::allocation_failure();

	auto group = std::make_shared<declaration::function_group>(entry->get_id(), name, this, block->get_address());
	if (group == nullptr)
		throw memory::exception::allocation_failure();

	group->add(entry);
	declarations_[name] = declaration_info{ group, group.get() };
}

void cminus::storage::unnamed_object::add_attribute_(std::shared_ptr<attribute::object> entry){
	
}

void cminus::storage::unnamed_object::add_type_(std::shared_ptr<type::object> entry){

}

void cminus::storage::unnamed_object::add_storage_(std::shared_ptr<object> entry){

}

void cminus::storage::unnamed_object::add_entry_(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing){
	auto &name = entry->get_name();
	if (check_existing && !name.empty() && declarations_.find(name) != declarations_.end())
		throw exception::duplicate_entry();

	entries_.push_back(entry_info{ entry.get(), value });
	if (!name.empty())
		declarations_[name] = declaration_info{ entry, value };
}

bool cminus::storage::unnamed_object::exists_(const std::string &name) const{
	return (!name.empty() && !declarations_.empty() && declarations_.find(name) != declarations_.end());
}

bool cminus::storage::unnamed_object::exists_(operators::id id) const{
	return (find_operator_(id) != nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const{
	if (name.empty() || declarations_.empty())
		return nullptr;

	if (auto it = declarations_.find(name); it != declarations_.end()){
		if (!is_accessible(it->second.value->get_flags()))
			throw storage::exception::inaccessible_entry();
		return std::visit(resolved_declaration_type_visitor(*this, *it->second.value, context, address), it->second.resolved);
	}

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::find_(operators::id id, std::shared_ptr<memory::reference> context) const{
	if (auto entry = find_operator_(id); entry != nullptr)
		return std::make_shared<memory::member_reference>(*entry, context);
	return nullptr;
}

cminus::declaration::callable_group *cminus::storage::unnamed_object::find_operator_(operators::id id) const{
	if (auto it = declarations_.find(operators::convert_id_to_string(id)); it != declarations_.end() && std::holds_alternative<declaration::callable_group *>(it->second.resolved)){
		auto group = std::get<declaration::callable_group *>(it->second.resolved);
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

cminus::storage::unnamed_object::resolved_declaration_type_visitor::resolved_declaration_type_visitor(const unnamed_object &target, const declaration::object &decl, std::shared_ptr<memory::reference> context, std::size_t address)
	: target_(target), decl_(decl), context_(context), address_(address){}

std::shared_ptr<cminus::memory::reference> cminus::storage::unnamed_object::resolved_declaration_type_visitor::operator()(std::size_t val){
	if (context_ == nullptr)
		throw storage::exception::no_member_context();

	if (decl_.get_type()->is_ref())
		return std::make_shared<memory::indirect_member_reference>((address_ + val), decl_, context_);

	return std::make_shared<memory::member_reference>((address_ + val), decl_, context_);
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
	return nullptr;
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
