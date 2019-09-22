#include "../node/node_object.h"
#include "../storage/global_storage.h"

#include "../evaluator/initializer.h"
#include "../evaluator/evaluator_object.h"

#include "type_object.h"
#include "proxy_type.h"

cminus::type::object::object(const std::string &name, storage::object *parent)
	: name_(name), parent_(parent){}

cminus::type::object::~object() = default;

const std::string &cminus::type::object::get_name() const{
	return name_;
}

std::string cminus::type::object::get_qname() const{
	return ((parent_ == nullptr) ? name_ : (parent_->get_qname() + "::" + name_));
}

cminus::storage::object *cminus::type::object::get_parent() const{
	return parent_;
}

void cminus::type::object::construct(std::shared_ptr<object> self, std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const{
	if (initialization != nullptr){
		std::vector<std::shared_ptr<memory::reference>> args;
		args.reserve(initialization->get_list_count());

		initialization->traverse_list([&](const node::object &entry){
			args.push_back(entry.evaluate());
		});

		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ args });
	}
	else//No initialization
		construct(self, target, std::vector<std::shared_ptr<memory::reference>>{});
}

void cminus::type::object::construct(std::shared_ptr<object> self, std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	if (initialization.empty()){
		if (auto default_value = get_default_value(self); default_value == nullptr)
			construct_(target, std::vector<std::shared_ptr<memory::reference>>{});
		else
			construct_(target, std::vector<std::shared_ptr<memory::reference>>{ default_value });
	}
	else
		construct_(target, initialization);
}

void cminus::type::object::construct(std::shared_ptr<object> self, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const{
	if (initialization == nullptr)
		construct(self, target, std::vector<std::shared_ptr<memory::reference>>{});
	else
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ initialization });
}

void cminus::type::object::construct(std::shared_ptr<object> self, std::shared_ptr<memory::reference> target) const{
	construct(self, target, std::vector<std::shared_ptr<memory::reference>>{});
}

void cminus::type::object::destruct(std::shared_ptr<memory::reference> target) const{}

std::shared_ptr<cminus::memory::reference> cminus::type::object::get_default_value(std::shared_ptr<object> self) const{
	return runtime::object::global_storage->get_zero_value(convert(conversion_type::clone, self));
}

std::size_t cminus::type::object::get_memory_size() const{
	return get_size();
}

bool cminus::type::object::is_exact(const object &target) const{
	return (target.get_non_proxy() == this || (parent_ == target.get_parent() && name_ == target.get_name()));
}

std::size_t cminus::type::object::compute_base_offset(const object &base_type) const{
	return static_cast<std::size_t>(-1);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::object::get_evaluator() const{
	return runtime::object::global_storage->get_evaluator_for(*this);
}

std::shared_ptr<cminus::evaluator::initializer> cminus::type::object::get_initializer() const{
	return runtime::object::global_storage->get_default_initializer();
}

int cminus::type::object::get_score_value(score_result_type score){
	switch (score){
	case score_result_type::exact:
		return 100;
	case score_result_type::auto_assignable:
		return 80;
	case score_result_type::ancestor:
		return 60;
	case score_result_type::assignable:
		return 50;
	case score_result_type::compatible:
		return 30;
	case score_result_type::class_compatible:
		return 20;
	default:
		break;
	}

	return 0;
}

cminus::type::object *cminus::type::object::get_non_proxy() const{
	return const_cast<object *>(this);
}

std::shared_ptr<cminus::type::object> cminus::type::object::convert(conversion_type type, std::shared_ptr<object> self_or_other) const{
	return ((self_or_other.get() == this) ? self_or_other : std::make_shared<proxy>(*const_cast<object *>(this)));
}

bool cminus::type::object::is(query_type type, const object *arg) const{
	return (type == query_type::scalar);
}

void cminus::type::object::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (!args.empty())
		get_initializer()->initialize(target, *args.rbegin());
	else
		throw declaration::exception::initialization_required();
}
