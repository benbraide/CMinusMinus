#include "../storage/global_storage.h"

#include "system_enum_types.h"

cminus::type::attribute_stage::attribute_stage()
	: enum_("AttributeStage", runtime::object::current_storage){
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
	: enum_("AttributeResult", runtime::object::current_storage){
	add_("Nil");
	add_("Skip");
	add_("Prohibit");
	add_("Require");
	add_("Info");

	compile_();
}

cminus::type::attribute_result::~attribute_result() = default;

cminus::type::exception::exception()
	: enum_("Exception", runtime::object::current_storage){

	//Memory exceptions
	add_("MemoryAllocationFailure");
	add_("AccessProtectedMemory");
	add_("WriteProtectedMemory");
	add_("MemoryBlockNotFound");
	add_("MemoryBlockMisaligned");
	add_("MemoryBlockNotResizable");
	add_("InvalidMemorySize");
	add_("IncompatibleMemoryTypes");
	add_("UninitializedMemory");

	//Storage exceptions
	add_("DuplicateEntry");
	add_("DuplicateBase");
	add_("EntryNotFound");
	add_("UnnamedEntry");
	add_("InaccessibleEntry");
	add_("NoMemberContext");

	//Declaration exceptions
	add_("BadDeclaration");
	add_("DeclarationInitializationRequired");
	add_("FunctionRedefinition");
	add_("FunctionRedeclaration");
	add_("FunctionExpected");
	add_("ConstructorExpected");
	add_("DestructorExpected");
	add_("FunctionNotFound");
	add_("FunctionNotDefined");
	add_("AmbiguousFunctionCall");
	add_("BadParameterList");
	add_("BadInitializationList");
	add_("VoidFunctionValueReturn");
	add_("ValueFunctionNoReturn");
	add_("DeletedFunctionCall");
	add_("DeletedConstructorCall");
	add_("DeletedDestructorCall");
	add_("DeletedOperatorCall");

	//Evaluator exceptions
	add_("InvalidTypeInExpression");
	add_("TypeInferenceFailure");
	add_("IncompatibleRightOperand");
	add_("RvalAssignment");
	add_("ConstantAssignment");
	add_("RvalRefAssignment");
	add_("ConstantRefAssignment");
	add_("UnsupportedOperator");

	//Runtime exceptions
	add_("InvalidReturn");
	add_("InvalidBreak");
	add_("InvalidContinue");
	add_("BadConstructor");
	add_("BadDestructor");
	add_("BadConstructorInitializationList");
	add_("BadScopeLeft");
	add_("BadMemberAccessLeft");
	add_("BadMemberPointerAccessLeft");
	add_("BadControlCondition");
	add_("OutOfRange");
	add_("FeatureNoSupported");

	compile_();
}

cminus::type::exception::~exception() = default;
