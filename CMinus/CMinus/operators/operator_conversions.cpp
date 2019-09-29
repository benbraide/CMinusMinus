#include "operator_conversions.h"

std::string cminus::operators::convert_id_to_string(id value){
	switch (value){
	case id::comma:
		return ",";
	case id::assignment:
		return "=";
	case id::compound_plus:
		return "+=";
	case id::compound_minus:
		return "-=";
	case id::compound_times:
		return "/*";
	case id::compound_divide:
		return "/=";
	case id::compound_modulus:
		return "%=";
	case id::compound_bitwise_or:
		return "|=";
	case id::compound_bitwise_xor:
		return "^=";
	case id::compound_bitwise_and:
		return "&=";
	case id::compound_left_shift:
		return "<<=";
	case id::compound_right_shift:
		return ">>=";
	case id::plus:
		return "+";
	case id::minus:
		return "-";
	case id::times:
		return "*";
	case id::divide:
		return "/";
	case id::modulus:
		return "%";
	case id::bitwise_or:
		return "|";
	case id::bitwise_xor:
		return "^";
	case id::bitwise_and:
		return "&";
	case id::left_shift:
		return "<<";
	case id::right_shift:
		return ">>";
	case id::bitwise_inverse:
		return "~";
	case id::ternary:
		return "?:";
	case id::relational_or:
		return "||";
	case id::relational_and:
		return "&&";
	case id::like:
		return "~=";
	case id::equal:
		return "==";
	case id::explicit_equal:
		return "===";
	case id::not_equal:
		return "!=";
	case id::explicit_not_equal:
		return "!==";
	case id::less:
		return "<";
	case id::less_or_equal:
		return "<=";
	case id::greater_or_equal:
		return ">=";
	case id::greater:
		return ">";
	case id::spaceship:
		return "<=>";
	case id::new_:
		return "new";
	case id::delete_:
		return "delete";
	case id::sizeof_:
		return "sizeof";
	case id::typeof:
		return "typeof";
	case id::relational_not:
		return "!";
	case id::increment:
		return "++";
	case id::decrement:
		return "--";
	case id::member_access:
		return ".";
	case id::member_pointer_access:
		return "->";
	case id::scope_resolution:
		return "::";
	case id::call:
		return "()";
	case id::index:
		return "[]";
	default:
		break;
	}

	return "";
}
