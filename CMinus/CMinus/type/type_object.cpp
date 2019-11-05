#include "../node/node_object.h"
#include "../storage/global_storage.h"

#include "../evaluator/initializer.h"
#include "../evaluator/evaluator_object.h"

#include "class_type.h"
#include "primitive_type.h"

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
	if (initialization != nullptr){
		std::vector<std::shared_ptr<memory::reference>> args;

		args.reserve(initialization->get_list_count());
		initialization->traverse_list([&](const node::object &entry){
			args.push_back(entry.evaluate());
		});

		construct_(target, args);
	}
	else//No initialization
		construct(target, std::vector<std::shared_ptr<memory::reference>>{});
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	if (initialization.empty()){
		if (auto default_value = get_default_value(); default_value == nullptr)
			construct_(target, std::vector<std::shared_ptr<memory::reference>>{});
		else
			construct_(target, std::vector<std::shared_ptr<memory::reference>>{ default_value });
	}
	else
		construct_(target, initialization);
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> initialization) const{
	if (initialization == nullptr)
		construct(target, std::vector<std::shared_ptr<memory::reference>>{});
	else
		construct_(target, std::vector<std::shared_ptr<memory::reference>>{ initialization });
}

void cminus::type::object::construct(std::shared_ptr<memory::reference> target) const{
	construct(target, std::vector<std::shared_ptr<memory::reference>>{});
}

void cminus::type::object::destruct(std::shared_ptr<memory::reference> target) const{}

std::shared_ptr<cminus::memory::reference> cminus::type::object::get_default_value() const{
	return runtime::object::global_storage->get_zero_value(*this);
}

std::size_t cminus::type::object::get_memory_size() const{
	return get_size();
}

bool cminus::type::object::is_exact(const object &target) const{
	return (target.remove_proxy() == this);
}

int cminus::type::object::get_score(const object &target, bool is_lval, bool is_const) const{
	auto is_ref_target = target.is_ref();
	auto is_const_target = target.is_const();

	if (is_ref_target && !is_const_target && (!is_lval || is_const))
		return get_score_value(score_result_type::nil);

	auto base_target = target.remove_const_ref();
	if (target.is<auto_primitive>() || base_target->can_be_inferred_from(*this))
		return get_score_value(score_result_type::inferable, ((is_const_target == is_const) ? 0 : -1));

	if (remove_const_ref()->is_exact(*base_target))
		return get_score_value(score_result_type::exact, ((is_const_target == is_const) ? 0 : -1));

	if (is_ref_target && !is_const_target)//No conversions
		return get_no_conversion_score_(target, is_lval, is_const);

	if (auto class_target = target.as<class_>(); class_target != nullptr && class_target->is_constructible_from(*this, is_lval, is_const))
		return get_score_value(score_result_type::class_compatible, ((is_const_target == is_const) ? 0 : -1));

	auto result = get_score_(target, is_lval, is_const);
	if (result == get_score_value(score_result_type::nil))
		return result;

	return (result + ((is_const_target == is_const) ? 0 : -1));
}

std::shared_ptr<cminus::evaluator::object> cminus::type::object::get_evaluator() const{
	return nullptr;
}

std::shared_ptr<cminus::evaluator::initializer> cminus::type::object::get_initializer() const{
	return runtime::object::global_storage->get_default_initializer();
}

std::shared_ptr<cminus::type::object> cminus::type::object::get_inferred(std::shared_ptr<object> target) const{
	return nullptr;
}

const cminus::type::object *cminus::type::object::remove_proxy() const{
	return this;
}

const cminus::type::object *cminus::type::object::remove_const_ref() const{
	return this;
}

std::shared_ptr<cminus::type::object> cminus::type::object::remove_const_ref(std::shared_ptr<object> self) const{
	return self;
}

bool cminus::type::object::can_be_inferred_from(const object &target) const{
	return false;
}

bool cminus::type::object::is_inferred() const{
	return false;
}

bool cminus::type::object::is_const() const{
	return false;
}

bool cminus::type::object::is_ref() const{
	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::type::object::copy_data(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type){
	auto copy = std::make_shared<memory::write_protected_rval_reference>(target_type->remove_const_ref(target_type));
	if (copy == nullptr)
		throw memory::exception::allocation_failure();

	runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
	copy->write(data);

	return copy;
}

int cminus::type::object::get_score_value(score_result_type score, int offset){
	switch (score){
	case score_result_type::exact:
		return (100 + offset);
	case score_result_type::inferable:
		return (80 + offset);
	case score_result_type::assignable:
		return (50 + offset);
	case score_result_type::compatible:
		return (30 + offset);
	case score_result_type::ancestor:
	case score_result_type::class_compatible:
		return (20 + offset);
	default:
		break;
	}

	return 0;
}

bool cminus::type::object::is_static_cast(cast_type type){
	switch (type){
	case cast_type::static_:
	case cast_type::static_ref:
	case cast_type::static_const_ref:
	case cast_type::static_rval:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::object::is_static_rval_cast(cast_type type){
	switch (type){
	case cast_type::static_:
	case cast_type::static_const_ref:
	case cast_type::static_rval:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::object::is_ref_cast(cast_type type){
	switch (type){
	case cast_type::static_ref:
	case cast_type::static_const_ref:
	case cast_type::dynamic_ref:
	case cast_type::dynamic_const_ref:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::object::is_non_const_ref_cast(cast_type type){
	switch (type){
	case cast_type::static_ref:
	case cast_type::dynamic_ref:
		return true;
	default:
		break;
	}

	return false;
}

bool cminus::type::object::is_valid_static_cast(cast_type type, bool is_lval, bool is_const){
	return (is_static_cast(type) && (!is_non_const_ref_cast(type) || (is_lval && !is_const)));
}

void cminus::type::object::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (!args.empty())
		get_initializer()->initialize(target, *args.rbegin());
	else
		throw declaration::exception::initialization_required();
}

int cminus::type::object::get_score_(const object &target, bool is_lval, bool is_const) const{
	return get_score_value(score_result_type::nil);
}

int cminus::type::object::get_no_conversion_score_(const object &target, bool is_lval, bool is_const) const{
	return get_score_value(score_result_type::nil);
}
