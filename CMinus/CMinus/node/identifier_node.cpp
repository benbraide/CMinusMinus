#include "../type/class_type.h"
#include "../type/pointer_type.h"
#include "../type/modified_type.h"
#include "../storage/global_storage.h"

#include "identifier_node.h"

cminus::node::identifier::identifier(const std::string &value)
	: value_(value){}

cminus::node::identifier::~identifier() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::identifier::evaluate() const{
	return get_storage_(true)->find(value_, should_search_tree_());
}

std::shared_ptr<cminus::memory::reference> cminus::node::identifier::lookup(std::shared_ptr<memory::reference> context, bool search_tree) const{
	auto storage = get_storage_(search_tree);
	if (auto class_storage = dynamic_cast<type::class_ *>(storage); class_storage != nullptr)
		return class_storage->find(value_, context, (search_tree && should_search_tree_()));

	return storage->find(value_, (search_tree && should_search_tree_()));
}

std::string cminus::node::identifier::evaluate_as_name() const{
	return value_;
}

std::shared_ptr<cminus::type::object> cminus::node::identifier::evaluate_as_type(bool search_tree) const{
	return get_storage_(search_tree)->find_type(value_, (search_tree && should_search_tree_()));
}

cminus::storage::object *cminus::node::identifier::evaluate_as_storage(bool search_tree) const{
	return get_storage_(search_tree)->find_storage(value_, (search_tree && should_search_tree_()));
}

cminus::storage::object *cminus::node::identifier::get_storage_(bool search_tree) const{
	return runtime::object::current_storage;
}

bool cminus::node::identifier::should_search_tree_() const{
	return true;
}

cminus::node::scoped_identifier::scoped_identifier(std::shared_ptr<object> left, const std::string &value)
	: identifier(value), left_(left){}

cminus::node::scoped_identifier::~scoped_identifier() = default;

std::string cminus::node::scoped_identifier::evaluate_as_name() const{
	return "";
}

cminus::storage::object *cminus::node::scoped_identifier::get_storage_(bool search_tree) const{
	if (left_ == nullptr)
		return runtime::object::global_storage;

	auto storage = left_->evaluate_as_storage(search_tree);
	if (storage == nullptr)
		throw runtime::exception::bad_scope_left();

	return storage;
}

bool cminus::node::scoped_identifier::should_search_tree_() const{
	return false;
}

cminus::node::primtive_type::primtive_type(id_type id)
	: id_(id){}

cminus::node::primtive_type::~primtive_type() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::primtive_type::evaluate() const{
	auto value = evaluate_as_type(true);
	auto block = runtime::object::memory_object->get_block(runtime::object::memory_object->allocate_write_protected_block(sizeof(void *)));

	runtime::value_guard guard(runtime::object::system_block, block.get());
	block->write_scalar(value.get());

	return std::make_shared<memory::rval_reference>(block->get_address(), runtime::object::global_storage->get_cached_type(id_type::type));
}

std::shared_ptr<cminus::type::object> cminus::node::primtive_type::evaluate_as_type(bool search_tree) const{
	return runtime::object::global_storage->get_cached_type(id_);
}

cminus::storage::object *cminus::node::primtive_type::evaluate_as_storage(bool search_tree) const{
	auto value = evaluate_as_type(search_tree);
	return dynamic_cast<storage::object *>(value.get());
}

cminus::node::primtive_type::id_type cminus::node::primtive_type::get_id() const{
	return id_;
}

cminus::node::modified_type::modified_type(std::shared_ptr<object> value)
	: value_(value){}

cminus::node::modified_type::~modified_type() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::modified_type::evaluate() const{
	auto value = evaluate_as_type(true);
	auto block = runtime::object::memory_object->get_block(runtime::object::memory_object->allocate_write_protected_block(sizeof(void *)));

	runtime::value_guard guard(runtime::object::system_block, block.get());
	block->write_scalar(value.get());
	block->set_value(std::make_shared<memory::type_value>(value));

	return std::make_shared<memory::rval_reference>(block->get_address(), runtime::object::global_storage->get_cached_type(primtive_type::id_type::type));
}

std::shared_ptr<cminus::type::object> cminus::node::modified_type::evaluate_as_type(bool search_tree) const{
	auto value = value_->evaluate_as_type(search_tree);
	return evaluate_as_type_(value);
}

std::shared_ptr<cminus::node::object> cminus::node::modified_type::get_value() const{
	return value_;
}

cminus::node::pointer_type::pointer_type(std::shared_ptr<object> value, std::size_t magnitude)
	: modified_type(value), magnitude_(magnitude){}

cminus::node::pointer_type::~pointer_type() = default;

std::size_t cminus::node::pointer_type::get_magnitude() const{
	return magnitude_;
}

std::shared_ptr<cminus::type::object> cminus::node::pointer_type::evaluate_as_type_(std::shared_ptr<type::object> value) const{
	for (auto count = magnitude_; 0u < count; --count)
		value = std::make_shared<type::pointer_primitive>(value);
	return value;
}

cminus::node::const_type::~const_type() = default;

std::shared_ptr<cminus::type::object> cminus::node::const_type::evaluate_as_type_(std::shared_ptr<type::object> value) const{
	return std::make_shared<type::constant>(value);
}

cminus::node::ref_type::~ref_type() = default;

std::shared_ptr<cminus::type::object> cminus::node::ref_type::evaluate_as_type_(std::shared_ptr<type::object> value) const{
	return std::make_shared<type::ref>(value);
}

cminus::node::typename_::~typename_() = default;

std::shared_ptr<cminus::type::object> cminus::node::typename_::evaluate_as_type_(std::shared_ptr<type::object> value) const{
	return value;
}
