#include "../node/node_object.h"
#include "../storage/storage_object.h"

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

void cminus::type::object::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> initialization) const{
	if (initialization == nullptr)
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{});
	else
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ initialization->evaluate() });
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	construct_(target, initialization);
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const{
	if (initialization == nullptr)
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{});
	else
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ initialization });
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target) const{
	if (auto default_value = get_default_value(); default_value == nullptr)
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{});
	else//Use default value
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ default_value });
}

void cminus::type::object::destruct(std::shared_ptr<memory::reference> target) const{}

std::shared_ptr<cminus::memory::reference> cminus::type::object::get_default_value() const{
	return nullptr;
}

std::size_t cminus::type::object::get_memory_size() const{
	return get_size();
}

bool cminus::type::object::is_exact(const object &target) const{
	return (&target == this || get_score(target) == get_score_value(score_result_type::exact));
}

std::shared_ptr<cminus::evaluator::object> cminus::type::object::get_initializer() const{
	return get_evaluator();
}

int cminus::type::object::get_score_value(score_result_type score){
	switch (score){
	case score_result_type::exact:
		return 100;
	case score_result_type::assignable:
		return 90;
	case score_result_type::ancestor:
	case score_result_type::widened:
		return 80;
	case score_result_type::too_widened:
		return 70;
	case score_result_type::shortened:
		return 60;
	case score_result_type::too_shortened:
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

std::shared_ptr<cminus::type::object> cminus::type::object::remove_ref(std::shared_ptr<object> self) const{
	return ((self.get() == this) ? self : std::make_shared<proxy>(*const_cast<object *>(this)));
}

std::shared_ptr<cminus::type::object> cminus::type::object::remove_const(std::shared_ptr<object> self) const{
	return ((self.get() == this) ? self : std::make_shared<proxy>(*const_cast<object *>(this)));
}

std::shared_ptr<cminus::type::object> cminus::type::object::convert_auto(std::shared_ptr<object> target) const{
	return nullptr;
}

cminus::type::object *cminus::type::object::get_non_proxy() const{
	return const_cast<object *>(this);
}

bool cminus::type::object::is_ref() const{
	return false;
}

bool cminus::type::object::is_const() const{
	return false;
}

bool cminus::type::object::is_auto() const{
	return false;
}

bool cminus::type::object::is_explicit_auto() const{
	return false;
}

bool cminus::type::object::is_any() const{
	return false;
}

bool cminus::type::object::is_explicit_any() const{
	return false;
}

bool cminus::type::object::is_nan() const{
	return false;
}

bool cminus::type::object::is_explicit_nan() const{
	return false;
}

bool cminus::type::object::is_undefined() const{
	return false;
}

void cminus::type::object::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{

}
