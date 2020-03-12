#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <engine/resource/resource-fwd.hpp>

#include <utility>

template <typename Resource>
class ResourceHandle {
	public:
		ResourceHandle() noexcept = default;

		const Resource& get() const noexcept { return *resource; }

		Resource& get() noexcept {
			return const_cast<Resource&>(std::as_const(*this).get());
		}

		operator const Resource & () const noexcept { return get(); }
		operator Resource & () noexcept { return get(); }

		const Resource& operator *() const noexcept { return get(); }
		Resource& operator *() noexcept { return get(); }

		const Resource* operator->() const noexcept { return resource.get(); }

		Resource* operator->() noexcept {
			return const_cast<Resource*>(std::as_const(*this).operator->());
		}

		explicit operator bool() const {
			return static_cast<bool>(resource);
		}
	private:
		Memory::SharedPointer<Resource> resource;

		ResourceHandle(Memory::SharedPointer<Resource> res) noexcept
				: resource{std::move(res)} {}

		friend class ResourceCache<Resource>;
};

