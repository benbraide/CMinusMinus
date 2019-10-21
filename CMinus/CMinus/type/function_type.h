#pragma once

#include "primitive_type.h"

namespace cminus::type{
	class function : public primitive{
	public:
		function(bool is_constant, std::shared_ptr<object> return_type);

		function(std::shared_ptr<object> return_type, const std::list<std::shared_ptr<object>> &parameter_types);

		function(std::shared_ptr<object> return_type, std::list<std::shared_ptr<object>> &&parameter_types);

		virtual ~function();

		virtual std::string get_qname() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual void add_parameter_type(std::shared_ptr<object> value);

		virtual bool is_exact_parameter_types(const function &target) const;

		virtual bool is_exact_return_type(const function &function_target) const;

	protected:
		virtual void compute_name_();

		bool is_constant_;
		std::shared_ptr<object> return_type_;
		std::list<std::shared_ptr<object>> parameter_types_;
		std::string qname_;
	};
}
