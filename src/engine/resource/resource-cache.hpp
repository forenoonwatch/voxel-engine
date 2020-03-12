#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <engine/core/singleton.hpp>

#include <engine/core/hash-map.hpp>

#include <engine/resource/resource-handle.hpp>
#include <engine/resource/resource-loader.hpp>
#include <engine/resource/resource-fwd.hpp>

#include <utility>

template <typename Resource>
class ResourceCache final : public Singleton<ResourceCache<Resource>> {
	public:
		ResourceCache() = default;
		ResourceCache(ResourceCache&&) = default;
		
		ResourceCache& operator=(ResourceCache&&) = default;

		template <typename Loader, typename... Args>
		ResourceHandle<Resource> load(const uint32 id, Args&&... args) {
			ResourceHandle<Resource> resource {};

			if (auto it = resources.find(id); it == resources.cend()) {
				if (auto instance = Loader{}.get(std::forward<Args>(args)...);
						instance) {
					resources[id] = instance;
					resource = std::move(instance);
				}
			}
			else {
				resource = it->second;
			}

			return resource;
		}

		template <typename Loader, typename... Args>
		ResourceHandle<Resource> reload(const uint32 id, Args&&... args) {
			return (discard(id),
					load<Loader>(id, std::forward<Args>(args)...));
		}

		template <typename Loader, typename... Args>
		ResourceHandle<Resource> temp(Args&&... args) const {
			return { Loader{}.get(std::forward<Args>(args)...) };
		}

		ResourceHandle<Resource> handle(const uint32 id) const {
			auto it = resources.find(id);
			return { it == resources.end() ? nullptr : it->second };
		}

		void clear() noexcept { resources.clear(); }

		bool contains(const uint32 id) const noexcept {
			return (resources.find(id) != resources.cend());
		}

		void discard(const uint32 id) const noexcept {
			if (auto it = resources.find(id); it != resources.end()) {
				resources.erase(it);
			}
		}

		// TODO: foreach

		size_t size() const noexcept { return resources.size(); }

		bool empty() const noexcept { return resources.empty(); }
	private:
		HashMap<uint32, Memory::SharedPointer<Resource>> resources;
};

