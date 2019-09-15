#include "memory_exception.h"

cminus::memory::exception::meta_no_address::~meta_no_address() = default;

std::size_t cminus::memory::exception::meta_no_address::get_address() const{
	return 0u;
}

cminus::memory::exception::meta_address::meta_address(const char *message, std::size_t address)
	: base(message), address_(address){}

cminus::memory::exception::meta_address::~meta_address() = default;

std::size_t cminus::memory::exception::meta_address::get_address() const{
	return address_;
}

cminus::memory::exception::unnamed::unnamed(code code, std::size_t address)
	: meta_address("Memory Exception", address), code_(code){}

cminus::memory::exception::unnamed::~unnamed() = default;

cminus::memory::exception::code cminus::memory::exception::unnamed::get_code() const{
	return code_;
}

cminus::memory::exception::out_of_address_space::out_of_address_space()
	: meta_no_address("Out of memory address space"){}

cminus::memory::exception::out_of_address_space::~out_of_address_space() = default;

cminus::memory::exception::code cminus::memory::exception::out_of_address_space::get_code() const{
	return code::out_of_address_space;
}

cminus::memory::exception::allocation_failure::allocation_failure()
	: meta_no_address("Failed to allocate memory block"){}

cminus::memory::exception::allocation_failure::~allocation_failure() = default;

cminus::memory::exception::code cminus::memory::exception::allocation_failure::get_code() const{
	return code::allocation_failure;
}

cminus::memory::exception::invalid_size::invalid_size()
	: meta_no_address("Invalid size specified"){}

cminus::memory::exception::invalid_size::~invalid_size() = default;

cminus::memory::exception::code cminus::memory::exception::invalid_size::get_code() const{
	return code::invalid_size;
}

cminus::memory::exception::access_protected::access_protected(std::size_t address)
	: meta_address("Memory address is access protected", address){}

cminus::memory::exception::access_protected::~access_protected() = default;

cminus::memory::exception::code cminus::memory::exception::access_protected::get_code() const{
	return code::access_protected;
}

cminus::memory::exception::write_protected::write_protected(std::size_t address)
	: meta_address("Memory address is write protected", address){}

cminus::memory::exception::write_protected::~write_protected() = default;

cminus::memory::exception::code cminus::memory::exception::write_protected::get_code() const{
	return code::write_protected;
}

cminus::memory::exception::block_not_found::block_not_found(std::size_t address)
	: meta_address("Memory address does not correspond to any existing block", address){}

cminus::memory::exception::block_not_found::~block_not_found() = default;

cminus::memory::exception::code cminus::memory::exception::block_not_found::get_code() const{
	return code::block_not_found;
}

cminus::memory::exception::block_misaligned::block_misaligned(std::size_t address)
	: meta_address("No existing memory block begin at the specified address", address){}

cminus::memory::exception::block_misaligned::~block_misaligned() = default;

cminus::memory::exception::code cminus::memory::exception::block_misaligned::get_code() const{
	return code::block_misaligned;
}

cminus::memory::exception::block_not_resizable::block_not_resizable(std::size_t address)
	: meta_address("Block at memory address cannot be resized", address){}

cminus::memory::exception::block_not_resizable::~block_not_resizable() = default;

cminus::memory::exception::code cminus::memory::exception::block_not_resizable::get_code() const{
	return code::block_not_resizable;
}
