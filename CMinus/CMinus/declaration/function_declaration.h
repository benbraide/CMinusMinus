#pragma once

#include "../type/function_type.h"
#include "../attribute/attribute_collection.h"

#include "variable_declaration.h"
#include "function_declaration_base.h"

namespace cminus::declaration{
	class function : public function_base{
	public:
		template <typename attributes_type>
		function(const std::string &name, storage::object *parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: function_base(name, nullptr, attributes, flags), parent_(parent){
			return_declaration_ = std::make_shared<variable>(
				"",											//Name
				return_type,								//Type
				attribute::collection::list_type{},			//Attributes
				flags::nil,									//Flags
				nullptr										//Initialization
			);

			type_ = std::make_shared<type::function>(return_type);
		}

		virtual ~function();

		virtual storage::object *get_parent() const override;

		virtual std::shared_ptr<type::object> get_type() const override;

		virtual void add_parameter(std::shared_ptr<object> value) override;

		virtual bool is_defined() const override;

		virtual int get_score(const std::vector<std::shared_ptr<memory::reference>> &args) const override;

	protected:
		friend class function_group;

		virtual std::shared_ptr<memory::reference> call_(const std::vector<std::shared_ptr<memory::reference>> &args) const override;

		virtual void copy_args_(const std::vector<std::shared_ptr<memory::reference>> &args) const;

		virtual void evaluate_body_() const;

		virtual std::shared_ptr<memory::reference> copy_return_value_(std::shared_ptr<memory::reference> value) const;

		storage::object *parent_;
		std::shared_ptr<type::function> type_;

		std::shared_ptr<object> return_declaration_;
		std::vector<std::shared_ptr<object>> parameter_list_;

		std::size_t min_arg_count_ = 0u;
		std::size_t max_arg_count_ = 0u;
	};
}
