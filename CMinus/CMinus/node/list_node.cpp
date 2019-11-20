#include "list_node.h"

cminus::node::list::list(const std::vector<std::shared_ptr<object>> &value)
	: value_(value){}

cminus::node::list::list(std::vector<std::shared_ptr<object>> &&value)
	: value_(std::move(value)){}

cminus::node::list::~list() = default;

std::size_t cminus::node::list::get_list_count() const{
	return value_.size();
}

void cminus::node::list::traverse_list(const std::function<void(const object &)> &callback) const{
	for (auto entry : value_)
		callback(*entry);
}

std::shared_ptr<cminus::memory::reference> cminus::node::list::evaluate() const{
	std::vector<std::shared_ptr<memory::reference>> value_list;
	evaluate_as_list(value_list);
	return (value_list.empty() ? nullptr : value_list.back());
}

void cminus::node::list::evaluate_as_list(std::vector<std::shared_ptr<memory::reference>> &value_list) const{
	if (value_.empty())
		return;

	value_list.reserve(value_list.size() + value_.size());
	for (auto entry : value_)
		entry->evaluate_as_list(value_list);
}

cminus::node::range::range(std::shared_ptr<object> start, std::shared_ptr<object> end)
	: start_(start), end_(end){}

cminus::node::range::~range() = default;

std::size_t cminus::node::range::get_list_count() const{
	return 2u;
}

void cminus::node::range::traverse_list(const std::function<void(const object &)> &callback) const{
	callback(*start_);
	callback(*end_);
}

std::shared_ptr<cminus::memory::reference> cminus::node::range::evaluate() const{
	std::vector<std::shared_ptr<memory::reference>> value_list;
	evaluate_as_list(value_list);
	return (value_list.empty() ? nullptr : value_list.back());
}

void cminus::node::range::evaluate_as_list(std::vector<std::shared_ptr<memory::reference>> &value_list) const{
	auto start_value = start_->evaluate();
	if (start_value == nullptr)
		throw evaluator::exception::unsupported_op();

	auto start_value_type = start_value->get_type()->as<type::number_primitive>();
	if (start_value_type == nullptr || !start_value_type->is_integral())
		throw evaluator::exception::unsupported_op();

	auto end_value = end_->evaluate();
	if (end_value == nullptr)
		throw evaluator::exception::unsupported_op();

	auto end_value_type = end_value->get_type()->as<type::number_primitive>();
	if (end_value_type == nullptr || !end_value_type->is_integral())
		throw evaluator::exception::unsupported_op();

	switch (start_value_type->get_precedence(*end_value_type)){
	case type::number_primitive::state_type::small_integer:
		evaluate_as_list_(start_value_type->read_value<__int16>(start_value), end_value_type->read_value<__int16>(end_value), value_list);
		break;
	case type::number_primitive::state_type::integer:
		evaluate_as_list_(start_value_type->read_value<__int32>(start_value), end_value_type->read_value<__int32>(end_value), value_list);
		break;
	case type::number_primitive::state_type::big_integer:
		evaluate_as_list_(start_value_type->read_value<__int64>(start_value), end_value_type->read_value<__int64>(end_value), value_list);
		break;
	case type::number_primitive::state_type::unsigned_small_integer:
		evaluate_as_list_(start_value_type->read_value<unsigned __int16>(start_value), end_value_type->read_value<unsigned __int16>(end_value), value_list);
		break;
	case type::number_primitive::state_type::unsigned_integer:
		evaluate_as_list_(start_value_type->read_value<unsigned __int32>(start_value), end_value_type->read_value<unsigned __int32>(end_value), value_list);
		break;
	case type::number_primitive::state_type::unsigned_big_integer:
		evaluate_as_list_(start_value_type->read_value<unsigned __int64>(start_value), end_value_type->read_value<unsigned __int64>(end_value), value_list);
		break;
	default:
		throw evaluator::exception::unsupported_op();
		break;
	}
}
