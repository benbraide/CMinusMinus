#pragma once

#include <string>
#include <typeinfo>

namespace cminus::io{
	class reader{
	public:
		virtual ~reader() = default;

		virtual std::size_t get_position() const = 0;

		virtual std::size_t get_size() const = 0;

		virtual std::size_t read(std::byte *buffer, std::size_t size) const = 0;

		template <typename target_type>
		bool can_read_scalar() const{
			return can_read_scalar_(sizeof(target_type), typeid(target_type));
		}

		template <typename target_type>
		target_type read_scalar() const{
			auto buffer = target_type();
			return ((read_scalar_((std::byte *)(&buffer), sizeof(target_type), typeid(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(target_type *buffer, std::size_t count) const{
			return read((std::byte *)buffer, (sizeof(target_type) * count));
		}

		virtual std::size_t read_buffer(char *buffer, std::size_t count) const{
			return read(reinterpret_cast<std::byte *>(buffer), count);
		}

		virtual std::size_t read_buffer(wchar_t *buffer, std::size_t count) const{
			return read(reinterpret_cast<std::byte *>(buffer), (sizeof(wchar_t) * count));
		}

	protected:
		virtual bool can_read_scalar_(std::size_t size, std::type_info &type_info) const{
			return (size <= (get_size() - get_position()));
		}

		virtual std::size_t read_scalar_(std::byte *buffer, std::size_t size, const std::type_info &type_info) const{
			return read(buffer, size);
		}
	};
}
