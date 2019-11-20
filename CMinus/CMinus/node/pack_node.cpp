#include "pack_node.h"

cminus::node::block::block(const std::vector<std::shared_ptr<object>> &statements)
	: statements_(statements){}

cminus::node::block::block(std::vector<std::shared_ptr<object>> &&statements)
	: statements_(std::move(statements)){}

cminus::node::block::~block() = default;

std::size_t cminus::node::block::get_list_count() const{
	return statements_.size();
}

void cminus::node::block::traverse_list(const std::function<void(const object &)> &callback) const{
	for (auto statement : statements_)
		callback(*statement);
}

std::shared_ptr<cminus::memory::reference> cminus::node::block::evaluate() const{
	for (auto statement : statements_)
		statement->evaluate();
	return nullptr;
}

cminus::node::init_list::init_list(const std::vector<std::shared_ptr<object>> &value)
	: list(value){}

cminus::node::init_list::init_list(std::vector<std::shared_ptr<object>> &&value)
	: list(std::move(value)){}

cminus::node::init_list::~init_list() = default;

cminus::node::param_list::param_list(const std::vector<std::shared_ptr<object>> &value)
	: list(value){}

cminus::node::param_list::param_list(std::vector<std::shared_ptr<object>> &&value)
	: list(std::move(value)){}

cminus::node::param_list::~param_list() = default;

cminus::node::index_list::index_list(const std::vector<std::shared_ptr<object>> &value)
	: list(value){}

cminus::node::index_list::index_list(std::vector<std::shared_ptr<object>> &&value)
	: list(std::move(value)){}

cminus::node::index_list::~index_list() = default;
