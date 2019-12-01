#pragma once

#include "enum_type.h"

namespace cminus::type{
	class attribute_stage : public enum_{
	public:
		attribute_stage();

		virtual ~attribute_stage();
	};

	class attribute_result : public enum_{
	public:
		attribute_result();

		virtual ~attribute_result();
	};

	class exception : public enum_{
	public:
		enum class code_type{
			//Memory exceptions
			memory_allocation_failure,
			access_protected_memory,
			write_protected_memory,
			memory_block_not_found,
			memory_block_misaligned,
			memory_block_not_resizable,
			invalid_memory_size,
			incompatible_memory_types,
			uninitialized_memory,

			//Storage exceptions
			duplicate_entry,
			duplicate_base,
			entry_not_found,
			unnamed_entry,
			inaccessible_entry,
			no_member_context,

			//Declaration exceptions
			bad_declaration,
			declaration_initialization_required,
			function_redefinition,
			function_redeclaration,
			function_expected,
			constructor_expected,
			destructor_expected,
			function_not_found,
			function_not_defined,
			ambiguous_function_call,
			bad_parameter_list,
			bad_initialization_list,
			void_function_value_return,
			value_function_no_return,
			deleted_function_call,
			deleted_constructor_call,
			deleted_destructor_call,
			deleted_operator_call,

			//Evaluator exceptions
			invalid_type_in_expression,
			type_inference_failure,
			incompatible_right_operand,
			rval_assignment,
			constant_assignment,
			rval_ref_assignment,
			constant_ref_assignment,
			unsupported_operator,

			//Runtime exceptions
			invalid_return,
			invalid_break,
			invalid_continue,
			bad_constructor,
			bad_destructor,
			bad_constructor_initialization_list,
			bad_scope_left,
			bad_member_access_left,
			bad_member_pointer_access_left,
			bad_control_condition,
			out_of_range,
			construction_failure,
			copy_failure,
			feature_not_supported,
		};

		exception();

		virtual ~exception();
	};
}
