#include "../storage/global_storage.h"

#include "compare_result_type.h"

cminus::type::compare_result::compare_result()
	: enum_("CompareResultType", runtime::object::global_storage){
	add_("NotEqual");
	add_("Less");
	add_("Equal");
	add_("Greater");

	compile_();
}

cminus::type::compare_result::~compare_result() = default;
