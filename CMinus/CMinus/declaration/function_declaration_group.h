#pragma once

#include "callable_declaration_group.h"

namespace cminus::declaration{
	class function_group : public callable_group{
	public:
		using list_type = std::list<std::shared_ptr<callable>>;

		function_group(callable::id_type id, const std::string &name, storage::object *parent, std::size_t address);

		virtual ~function_group();

		virtual callable::id_type get_id() const override;

		virtual storage::object *get_parent() const override;

		virtual void set_address(std::size_t value) override;

		virtual std::size_t get_address() const override;

		virtual std::shared_ptr<callable> find(const type::function &target_type) const override;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const override;

		virtual std::shared_ptr<callable> find(std::shared_ptr<memory::reference> context, const std::vector<callable::arg_info> &args, std::size_t required_size) const override;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const override;

		virtual std::shared_ptr<callable> find_by_args(const std::vector<callable::arg_info> &args, std::size_t required_size) const override;

		virtual void traverse_list(const std::function<void(std::shared_ptr<callable>)> &callback) override;

		virtual std::size_t get_count() const override;

	protected:
		virtual void add_(std::shared_ptr<callable> entry) override;

		virtual list_type::const_iterator find_(const type::function &target_type) const;

		template <typename list_type>
		std::shared_ptr<callable> find_(std::shared_ptr<memory::reference> context, const list_type &args, std::size_t required_size) const{
			auto highest_rank_score = type::object::get_score_value(type::object::score_result_type::nil), current_rank_score = highest_rank_score;

			std::size_t match_count = 0u;
			std::shared_ptr<callable> highest_ranked = nullptr;

			for (auto entry : entries_){
				if (highest_rank_score < (current_rank_score = entry->get_score(context, args, required_size))){
					highest_rank_score = current_rank_score;
					highest_ranked = entry;
					match_count = 1u;
				}
				else if (highest_rank_score == current_rank_score)
					++match_count;
			}

			if (1u < match_count && highest_ranked != nullptr)
				throw exception::ambiguous_function_call();

			return highest_ranked;
		}

		template <typename list_type>
		std::shared_ptr<callable> find_by_args_(const list_type &args, std::size_t required_size) const{
			auto highest_rank_score = type::object::get_score_value(type::object::score_result_type::nil), current_rank_score = highest_rank_score;

			std::size_t match_count = 0u;
			std::shared_ptr<callable> highest_ranked = nullptr;

			for (auto entry : entries_){
				if (highest_rank_score < (current_rank_score = entry->get_args_score(args, required_size))){
					highest_rank_score = current_rank_score;
					highest_ranked = entry;
					match_count = 1u;
				}
				else if (highest_rank_score == current_rank_score)
					++match_count;
			}

			if (1u < match_count && highest_ranked != nullptr)
				throw exception::ambiguous_function_call();

			return highest_ranked;
		}

		callable::id_type id_;
		storage::object *parent_;
		std::size_t address_;
		list_type entries_;
	};
}
