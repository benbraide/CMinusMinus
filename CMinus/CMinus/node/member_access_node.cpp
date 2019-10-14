#include "../type/class_type.h"
#include "../storage/global_storage.h"

#include "member_access_node.h"

cminus::node::member_access::member_access(std::shared_ptr<object> left, std::shared_ptr<object> right)
	: left_(left), right_(right){}

cminus::node::member_access::~member_access() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::member_access::evaluate() const{
	auto target = left_->evaluate();
	if (target == nullptr)
		throw runtime::exception::bad_member_access_left();

	auto class_type = dynamic_cast<type::class_ *>(target->get_type().get());
	if (class_type == nullptr)
		throw runtime::exception::bad_member_access_left();

	runtime::value_guard<storage::object *> guard(runtime::object::current_storage, class_type);
	return right_->lookup(target, false);
}
