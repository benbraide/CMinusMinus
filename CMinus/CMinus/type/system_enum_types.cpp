#include "../storage/global_storage.h"

#include "system_enum_types.h"

cminus::type::compare_result::compare_result()
	: enum_("CompareResult", runtime::object::global_storage){
	add_("NotEqual");
	add_("Less");
	add_("Equal");
	add_("Greater");

	compile_();
}

cminus::type::compare_result::~compare_result() = default;

cminus::type::attribute_stage::attribute_stage()
	: enum_("AttributeStage", runtime::object::global_storage){
	add_("Nil");
	add_("BeforeDeclaration");
	add_("AfterDeclaration");
	add_("AfterLookup");
	add_("BeforeAddressRead");
	add_("AfterAddressRead");
	add_("BeforeRead");
	add_("AfterRead");
	add_("BeforeWrite");
	add_("AfterWrite");
	add_("BeforeDelete");
	add_("AfterDelete");
	add_("BeforeInheritance");
	add_("AfterInheritance");

	compile_();
}

cminus::type::attribute_stage::~attribute_stage() = default;

cminus::type::attribute_result::attribute_result()
	: enum_("AttributeResult", runtime::object::global_storage){
	add_("Nil");
	add_("Skip");
	add_("Prohibit");
	add_("Require");
	add_("Info");

	compile_();
}

cminus::type::attribute_result::~attribute_result() = default;
