#include "../type/class_type.h"
#include "../declaration/variable_declaration.h"

#include "specialized_storage.h"

cminus::storage::class_member::class_member(const declaration::callable &owner)
	: owner_(owner){}

cminus::storage::class_member::~class_member(){
	destroy_entries_();
}

const std::string &cminus::storage::class_member::get_name() const{
	return owner_.get_name();
}

cminus::storage::object *cminus::storage::class_member::get_parent() const{
	return owner_.get_parent();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_member::find(const std::string &name, bool search_tree) const{
	if (auto entry = unnamed_object::find(name, false); entry != nullptr || !search_tree)
		return entry;

	auto parent = get_parent();
	if (parent == nullptr)
		return nullptr;

	if (auto class_parent = dynamic_cast<type::class_ *>(parent); class_parent != nullptr)
		return class_parent->find(name, context_, true);

	return parent->find(name, true);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_member::get_context() const{
	return context_;
}

const cminus::declaration::callable &cminus::storage::class_member::get_owner() const{
	return owner_;
}

void cminus::storage::class_member::add_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	unnamed_object::add_(entry, address);
	if (entry->get_name() == "self")//Update context
		context_ = named_entries_["self"];
}

cminus::storage::class_wrapper::class_wrapper(std::shared_ptr<memory::reference> context)
	: context_(context){}

cminus::storage::class_wrapper::~class_wrapper() = default;

const std::string &cminus::storage::class_wrapper::get_name() const{
	return context_->get_type()->get_name();
}

std::string cminus::storage::class_wrapper::get_qname() const{
	return context_->get_type()->get_qname();
}

cminus::storage::object *cminus::storage::class_wrapper::get_parent() const{
	return dynamic_cast<object *>(context_->get_type().get());
}

void cminus::storage::class_wrapper::add(std::shared_ptr<declaration::object> entry, std::size_t address){}

void cminus::storage::class_wrapper::add(const std::string &name){}

void cminus::storage::class_wrapper::add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing){}

void cminus::storage::class_wrapper::del(const std::string &name){}

bool cminus::storage::class_wrapper::exists(const std::string &name, entry_type type) const{
	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_wrapper::find(const std::string &name, bool search_tree) const{
	if (auto class_parent = dynamic_cast<type::class_ *>(context_->get_type().get()); class_parent != nullptr)
		return class_parent->find(name, context_, false);
	return nullptr;
}

cminus::declaration::callable_group *cminus::storage::class_wrapper::find_operator(const std::string &name, bool search_tree) const{
	if (auto class_parent = dynamic_cast<type::class_ *>(context_->get_type().get()); class_parent != nullptr)
		return class_parent->find_operator(name, false);
	return nullptr;
}

cminus::declaration::callable_group *cminus::storage::class_wrapper::find_operator(operators::id id, bool search_tree) const{
	if (auto class_parent = dynamic_cast<type::class_ *>(context_->get_type().get()); class_parent != nullptr)
		return class_parent->find_operator(id, false);
	return nullptr;
}

std::shared_ptr<cminus::attribute::object> cminus::storage::class_wrapper::find_attribute(const std::string &name, bool search_tree) const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::class_wrapper::find_type(const std::string &name, bool search_tree) const{
	return nullptr;
}

cminus::storage::object *cminus::storage::class_wrapper::find_storage(const std::string &name, bool search_tree) const{
	auto class_parent = dynamic_cast<type::class_ *>(context_->get_type().get());
	if (class_parent == nullptr)
		return nullptr;

	auto entry = dynamic_cast<type::class_ *>(class_parent->find_storage(name, false));
	return ((entry != nullptr && (entry == class_parent || class_parent->is_base_type(*entry, true))) ? entry : nullptr);
}

bool cminus::storage::class_wrapper::is_accessible(unsigned int access) const{
	if (auto class_parent = dynamic_cast<type::class_ *>(context_->get_type().get()); class_parent != nullptr)
		return class_parent->is_accessible(access);
	return true;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_wrapper::get_context() const{
	return context_;
}
