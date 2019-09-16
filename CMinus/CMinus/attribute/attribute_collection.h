#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include "attribute_object.h"

namespace cminus::attribute{
	class collection{
	public:
		using list_type = std::vector<std::shared_ptr<object>>;
		using map_type = std::unordered_map<object *, std::shared_ptr<object>>;

		explicit collection(std::shared_ptr<type::object> target_type = nullptr);

		explicit collection(const list_type &list, std::shared_ptr<type::object> target_type = nullptr);

		explicit collection(const map_type &list, std::shared_ptr<type::object> target_type = nullptr);

		explicit collection(map_type &&list, std::shared_ptr<type::object> target_type = nullptr);

		virtual void set_target_type(std::shared_ptr<type::object> value);

		virtual std::shared_ptr<type::object> get_target_type() const;

		void add(std::shared_ptr<object> value);

		void remove(const object &target);

		void remove(const std::string &qname);

		void remove_primitive(const std::string &name);

		void clear();

		bool has(const object &target) const;

		bool has(const std::string &qname) const;

		bool has_primitive(const std::string &name) const;

		void traverse(const std::function<void(std::shared_ptr<object>)> &callback) const;

		void traverse_all(const std::function<void(std::shared_ptr<object>)> &callback) const;

		query_result query(stage_type stage, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg) const;

		const map_type &get_list() const;

	private:
		map_type list_;
		std::shared_ptr<type::object> target_type_;
	};
}
