#pragma once

#include "list_node.h"

namespace cminus::node{
	class block : public object{
	public:
		explicit block(const std::vector<std::shared_ptr<object>> &statements);

		explicit block(std::vector<std::shared_ptr<object>> &&statements);

		virtual ~block();

		virtual std::size_t get_list_count() const override;

		virtual void traverse_list(const std::function<void(const object &)> &callback) const override;

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		std::vector<std::shared_ptr<object>> statements_;
	};

	class init_list : public list{
	public:
		explicit init_list(const std::vector<std::shared_ptr<object>> &value);

		explicit init_list(std::vector<std::shared_ptr<object>> &&value);

		virtual ~init_list();
	};

	class param_list : public list{
	public:
		explicit param_list(const std::vector<std::shared_ptr<object>> &value);

		explicit param_list(std::vector<std::shared_ptr<object>> &&value);

		virtual ~param_list();
	};

	class index_list : public list{
	public:
		explicit index_list(const std::vector<std::shared_ptr<object>> &value);

		explicit index_list(std::vector<std::shared_ptr<object>> &&value);

		virtual ~index_list();
	};
}
