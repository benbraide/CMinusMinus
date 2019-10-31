#pragma once

#include "../type/type_object.h"
#include "../node/memory_reference_node.h"
#include "../memory/memory_reference.h"

#include "declaration_object.h"

namespace cminus::declaration{
	class variable : public object{
	public:
		template <typename attributes_type>
		variable(const std::string &name, std::shared_ptr<type::object> type, const attributes_type &attributes, unsigned int flags, std::shared_ptr<memory::reference> initialization)
			: variable(name, type, attributes, flags, ((initialization == nullptr) ? nullptr : std::make_shared<node::memory_reference>(initialization))){}

		template <typename attributes_type>
		variable(const std::string &name, std::shared_ptr<type::object> type, const attributes_type &attributes, unsigned int flags, std::shared_ptr<node::object> initialization)
			: object(name, type, attributes, flags), initialization_(initialization){}

		virtual ~variable();

		virtual std::shared_ptr<node::object> get_initialization() const;

		virtual std::shared_ptr<memory::reference> evaluate(std::size_t address, std::shared_ptr<node::object> initialization) const;

		virtual std::shared_ptr<memory::reference> evaluate(std::size_t address, const std::vector<std::shared_ptr<memory::reference>> &initialization) const;

		virtual std::shared_ptr<memory::reference> evaluate(std::size_t address, std::shared_ptr<memory::reference> initialization) const;

		virtual std::shared_ptr<memory::reference> evaluate(std::size_t address) const;

		virtual std::shared_ptr<memory::reference> allocate_memory(std::size_t address, std::shared_ptr<type::object> initialization_type) const;

		virtual void initialize_memory(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> value) const;

		virtual void initialize_memory(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &value) const;

		virtual void initialize_memory(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const;

		virtual void initialize_memory(std::shared_ptr<memory::reference> target) const;

	protected:
		virtual std::shared_ptr<memory::reference> evaluate_(std::size_t address, const std::vector<std::shared_ptr<memory::reference>> &initialization) const;

		virtual void initialize_memory_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &value) const;

		std::shared_ptr<node::object> initialization_;
		mutable std::shared_ptr<memory::reference> static_value_;
	};
}
