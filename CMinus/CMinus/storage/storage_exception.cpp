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
