#include "storage_exception.h"

cminus::storage::exception::unnamed::unnamed(code code)
	: base("Unknown storage error"), code_(code){}

cminus::storage::exception::unnamed::~unnamed() = default;

cminus::storage::exception::code cminus::storage::exception::unnamed::get_code() const{
	return code_;
}

cminus::storage::exception::duplicate_entry::duplicate_entry()
	: base("An entry already with the specified name"){}

cminus::storage::exception::duplicate_entry::~duplicate_entry() = default;

cminus::storage::exception::code cminus::storage::exception::duplicate_entry::get_code() const{
	return code::duplicate_entry;
}

cminus::storage::exception::duplicate_base::duplicate_base()
	: base("Class is already in base list"){}

cminus::storage::exception::duplicate_base::~duplicate_base() = default;

cminus::storage::exception::code cminus::storage::exception::duplicate_base::get_code() const{
	return code::duplicate_base;
}

cminus::storage::exception::entry_not_found::entry_not_found()
	: base("An entry with the specified name was not found"){}

cminus::storage::exception::entry_not_found::~entry_not_found() = default;

cminus::storage::exception::code cminus::storage::exception::entry_not_found::get_code() const{
	return code::entry_not_found;
}

cminus::storage::exception::unnamed_entry::unnamed_entry()
	: base("A name is required for entry"){}

cminus::storage::exception::unnamed_entry::~unnamed_entry() = default;

cminus::storage::exception::code cminus::storage::exception::unnamed_entry::get_code() const{
	return code::unnamed_entry;
}

cminus::storage::exception::inaccessible_entry::inaccessible_entry()
	: base("Member is inaccessible from current scope"){}

cminus::storage::exception::inaccessible_entry::~inaccessible_entry() = default;

cminus::storage::exception::code cminus::storage::exception::inaccessible_entry::get_code() const{
	return code::inaccessible_entry;
}

cminus::storage::exception::no_member_context::no_member_context()
	: base("A non-static member requires a context"){}

cminus::storage::exception::no_member_context::~no_member_context() = default;

cminus::storage::exception::code cminus::storage::exception::no_member_context::get_code() const{
	return code::no_member_context;
}
