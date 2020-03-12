#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <engine/resource/resource-fwd.hpp>

template <typename Loader, typename Resource>
class ResourceLoader {
	public:
		template <typename... Args>
		Memory::SharedPointer<Resource> get(Args&&... args) const {
			return static_cast<const Loader*>(this)
					->load(std::forward<Args>(args)...);
		}
	private:
		friend class ResourceCache<Resource>;
};

