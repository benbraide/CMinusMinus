#pragma once

#include "type_object.h"

namespace cminus::type{
	class function : public object{
	public:
		explicit function(std::shared_ptr<object> return_type);

		function(std::shared_ptr<object> return_type, const std::vector<std::shared_ptr<object>> &parameter_types);

		function(std::shared_ptr<object> return_type, std::vector<std::shared_ptr<object>> &&parameter_types);

		virtual ~function();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual void add_parameter_type(std::shared_ptr<object> value);

	protected:
		virtual void compute_name_();

		virtual bool is_exact_return_type_(const function &function_target) const;

		virtual bool is_exact_parameter_types_(const function &function_target) const;

		std::shared_ptr<object> return_type_;
		std::vector<std::shared_ptr<object>> parameter_types_;
	};
}
