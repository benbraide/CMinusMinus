#pragma once

#include "../attribute/attribute_collection.h"

#include "variable_declaration.h"
#include "callable_declaration.h"

namespace cminus::declaration{
	class function : public callable{
	public:
		using parameter_list_type = std::vector<std::shared_ptr<variable>>;

		template <typename attributes_type>
		function(const std::string &name, storage::object *parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: callable(name, nullptr, attributes, (flags & ~declaration::flags::tls)), parent_(parent){
			init_(return_type);
		}

		virtual ~function();

		virtual id_type get_id() const override;

		virtual storage::object *get_parent() const override;

		using callable::add_parameter;

		virtual void add_parameter(std::shared_ptr<variable> value) override;

		virtual bool is_defined() const override;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const override;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::vector<arg_info> &args, std::size_t required_size) const override;

		virtual int get_args_score(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const override;

		virtual int get_args_score(const std::vector<arg_info> &args, std::size_t required_size) const override;

	protected:
		virtual std::shared_ptr<memory::reference> call_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const override;

		virtual void init_(std::shared_ptr<type::object> return_type);

		virtual int get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const;

		virtual void copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const;

		virtual void copy_args_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const;

		virtual void evaluate_body_() const;

		virtual std::shared_ptr<memory::reference> copy_return_value_(std::shared_ptr<memory::reference> value) const;

		virtual int get_meta_score_(std::shared_ptr<memory::reference> context, std::size_t args_count, parameter_list_type::const_iterator &it) const;

		virtual int get_meta_score_(std::size_t args_count) const;

		template <typename list_type>
		int get_args_score_(const list_type &args, parameter_list_type::const_iterator param_it, int lowest_rank_score, std::size_t required_size) const{
			auto arg_it = args.begin();
			auto current_rank_score = lowest_rank_score;

			std::shared_ptr<type::object> variadic_base_type, param_type, arg_type;
			for (; arg_it != args.end(); ++arg_it){
				if (variadic_base_type == nullptr){
					if (param_it == parameter_list_.end())
						break;

					param_type = (*param_it)->get_type();
					if (auto variadic_type = param_type->as<type::variadic>(); variadic_type != nullptr)
						param_type = variadic_base_type = variadic_type->get_base_type();

					++param_it;
				}

				if ((current_rank_score = get_arg_score_(param_type, *arg_it)) < lowest_rank_score){
					if (current_rank_score == type::object::get_score_value(type::object::score_result_type::nil))
						return current_rank_score;
					lowest_rank_score = current_rank_score;
				}
			}

			for (; param_it != parameter_list_.end(); ++param_it){//Check unmatched parameters
				if ((*param_it)->get_initialization() == nullptr && !(*param_it)->get_type()->is<type::variadic>())
					return type::object::get_score_value(type::object::score_result_type::nil);
			}

			return ((arg_it == args.end() || required_size <= static_cast<std::size_t>(std::distance(args.begin(), arg_it))) ? lowest_rank_score : type::object::get_score_value(type::object::score_result_type::nil));
		}

		virtual int get_arg_score_(std::shared_ptr<type::object> param_type, const arg_info &info) const;

		virtual int get_arg_score_(std::shared_ptr<type::object> param_type, std::shared_ptr<memory::reference> arg) const;

		virtual std::size_t get_args_count_(std::shared_ptr<memory::reference> context, std::size_t args_count) const;

		storage::object *parent_;
		std::shared_ptr<variable> return_declaration_;
		parameter_list_type parameter_list_;

		std::size_t min_arg_count_ = 0u;
		std::size_t max_arg_count_ = 0u;
	};

	class defined_function : public function{
	public:
		using function::function;

		virtual ~defined_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_function : public function{
	public:
		using function::function;

		virtual ~external_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};
}
