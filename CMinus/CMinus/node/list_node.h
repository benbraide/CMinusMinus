#pragma once

#include "../storage/global_storage.h"

#include "node_object.h"

namespace cminus::node{
	class list : public object{
	public:
		explicit list(const std::vector<std::shared_ptr<object>> &value);

		explicit list(std::vector<std::shared_ptr<object>> &&value);

		virtual ~list();

		virtual std::size_t get_list_count() const override;

		virtual void traverse_list(const std::function<void(const object &)> &callback) const override;

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual void evaluate_as_list(std::vector<std::shared_ptr<memory::reference>> &value_list) const override;

	protected:
		std::vector<std::shared_ptr<object>> value_;
	};

	class range : public object{
	public:
		range(std::shared_ptr<object> start, std::shared_ptr<object> end);

		virtual ~range();

		virtual std::size_t get_list_count() const override;

		virtual void traverse_list(const std::function<void(const object &)> &callback) const override;

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual void evaluate_as_list(std::vector<std::shared_ptr<memory::reference>> &value_list) const override;

	protected:
		template <typename target_type>
		void evaluate_as_list_(target_type start_value, target_type end_value, std::vector<std::shared_ptr<memory::reference>> &value_list) const{
			auto is_ascending = (start_value < end_value);
			target_type distance = ((is_ascending ? (end_value - start_value) : (start_value - end_value)) + static_cast<target_type>(1));

			value_list.reserve(value_list.size() + distance);
			while (is_ascending ? (start_value <= end_value) : (end_value <= start_value)){
				value_list.push_back(std::make_shared<memory::scalar_reference<target_type>>(storage::get_cached_type<target_type>::template type(), start_value));
				if (is_ascending)
					start_value += static_cast<target_type>(1);
				else//Descending
					start_value -= static_cast<target_type>(1);
			}
		}

		std::shared_ptr<object> start_;
		std::shared_ptr<object> end_;
	};
}
