#include "../storage/global_storage.h"
#include "../declaration/variable_declaration.h"
#include "../declaration/special_function_declaration.h"

#include "proxy_type.h"
#include "modified_type.h"
#include "class_type.h"

cminus::type::class_::class_(const std::string &name, storage_base *parent)
	: type_base(name, parent){
	class_context_ = std::make_shared<memory::rval_reference>(0u, std::make_shared<type::proxy>(*this));
	dummy_context_ = std::make_shared<memory::reference>(
		0u,
		std::make_shared<type::proxy>(*this),
		attribute::collection::list_type{},
		nullptr
	);
}

cminus::type::class_::~class_(){
	destroy_entries_();
}

const std::string &cminus::type::class_::get_name() const{
	return type_base::get_name();
}

cminus::storage::object *cminus::type::class_::get_parent() const{
	return type_base::get_parent();
}

bool cminus::type::class_::is_constructible(std::shared_ptr<memory::reference> target) const{
	auto constructor = get_constructor_();
	return (constructor != nullptr && constructor->find(std::list<std::shared_ptr<memory::reference>>{ dummy_context_, target }) != nullptr);
}

void cminus::type::class_::destruct(std::shared_ptr<memory::reference> target) const{
	if (auto destructor = get_destructor_(); destructor != nullptr)
		target = destructor->call(std::list<std::shared_ptr<memory::reference>>{ target });
	else
		throw runtime::exception::bad_destructor();
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::get_default_value(std::shared_ptr<type_base> self) const{
	return nullptr;
}

std::size_t cminus::type::class_::get_size() const{
	return ((size_ == 0u) ? 1u : size_);
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
	if (auto class_base_type = dynamic_cast<const class_ *>(&base_type); class_base_type != nullptr)
		return compute_base_offset_(*class_base_type, 0u);
	return static_cast<std::size_t>(-1);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

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
		base_types_map_[value->get_name()] = &*base_types_.rbegin();
		size_ += value->size_;
	}
	else
		throw storage::exception::duplicate_base();
}

void cminus::type::class_::add_default_functions(bool add_on_empty){
	if (!add_on_empty || storage_base::find(get_name(), false) == nullptr){//No constructor defined
		try{
			add(std::make_shared<declaration::default_constructor>(*this), 0u);
		}
		catch (const declaration::exception::base &){}

		try{
			add(std::make_shared<declaration::copy_constructor>(*this), 0u);
		}
		catch (const declaration::exception::base &){}
	}

	if (storage_base::find(("~" + get_name()), false) == nullptr){//No destructor defined
		try{
			add(std::make_shared<declaration::default_destructor>(*this), 0u);
		}
		catch (const declaration::exception::base &){}
	}
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (auto entry = find_(name, ((context->get_type().get() == this) ? context : nullptr), 0u); entry != nullptr || !search_tree)
			return entry;
	}

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(name, true);

	return nullptr;
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_non_static_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);

	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_non_static_member(const std::string &name, std::shared_ptr<memory::reference> context) const{
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

bool cminus::type::class_::is_base_type(const class_ &target, bool search_hierarchy) const{
	return is_base_type_(target, search_hierarchy);
}

const std::list<cminus::type::class_::member_variable_info> &cminus::type::class_::get_member_variables() const{
	return member_variables_;
}

const std::list<cminus::type::class_::base_type_info> &cminus::type::class_::get_base_types() const{
	return base_types_;
}

void cminus::type::class_::construct_(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &args) const{
	auto constructor = get_constructor_();
	if (constructor == nullptr)
		throw runtime::exception::bad_constructor();

	auto computed_args = args;
	computed_args.insert(computed_args.begin(), target);
	target = constructor->call(computed_args);
}

void cminus::type::class_::add_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	if (!entry->is(declaration::flags::static_)){
		auto &name = entry->get_name();
		auto entry_memory_size = entry->get_type()->get_memory_size();

		if (name.empty())
			throw storage::exception::unnamed_entry();

		member_variables_.push_back(member_variable_info{ member_size_, entry });
		member_variables_map_[name] = &*member_variables_.rbegin();

		member_size_ += entry_memory_size;
		size_ += entry_memory_size;

		for (auto &base_type : base_types_)//Adjust base offsets
			base_type.address_offset += entry_memory_size;
	}
	else//Static declaration
		storage_base::add_(entry, address);
}

bool cminus::type::class_::exists_(const std::string &name, entry_type type) const{
	return (member_variables_map_.find(name) != member_variables_map_.end() || base_types_map_.find(name) != base_types_map_.end() || storage_base::exists_(name, type));
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
	for (auto &entry : base_types_){
		if (entry.value.get() == &base_type)
			return (offset + entry.address_offset);

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

cminus::declaration::function_group_base *cminus::type::class_::find_function_(const std::string &name) const{
	auto it = functions_.find(get_name());
	if (it == functions_.end())
		return nullptr;

	auto function_entry = dynamic_cast<memory::function_reference *>(it->second.get());
	return ((function_entry == nullptr) ? nullptr : function_entry->get_entry());
}

cminus::declaration::constructor_group_base *cminus::type::class_::get_constructor_() const{
	return dynamic_cast<declaration::constructor_group_base *>(find_function_(get_name()));
}

cminus::declaration::destructor_group_base *cminus::type::class_::get_destructor_() const{
	return dynamic_cast<declaration::destructor_group_base *>(find_function_("~" + get_name()));
}
