#pragma once

#include "../memory/memory_reference.h"

namespace cminus::attribute{
	class guard{
	public:
		virtual ~guard();

	protected:
		std::function<void()> callback_;
	};

	class write_guard : public guard{
	public:
		write_guard(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg);

		virtual ~write_guard();
	};

	class read_guard : public guard{
	public:
		read_guard(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg);

		virtual ~read_guard();
	};
}
