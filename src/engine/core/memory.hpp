#pragma once

#include <engine/core/common.hpp>

#include <memory>
#include <new>

#include <cstring>
#include <cstdlib>

namespace Memory {
	template <typename T>
	using UniquePointer = std::unique_ptr<T>;

	template <typename T>
	using SharedPointer = std::shared_ptr<T>;

	template <typename T>
	using WeakPointer = std::weak_ptr<T>;

	template <typename T>
	class SharedCounter : public SharedPointer<T> {
		public:
			inline explicit SharedCounter(T* t)
				: SharedPointer<T>(t)
				, id(counter++) {}

			inline uint32 getID() const { return id; }
		private:
			static uint32 counter;
			
			const uint32 id;
	};

	FORCEINLINE void* malloc(size_t size) {
		return std::malloc(size);
	}

	FORCEINLINE void free(void* ptr) {
		std::free(ptr);
	}

	FORCEINLINE void* memcpy(void* dest, const void* src, uintptr amt) {
		return std::memcpy(dest, src, amt);
	}

	FORCEINLINE void* memmove(void* dest, const void* src, uintptr amt) {
		return std::memmove(dest, src, amt);
	}

	FORCEINLINE int32 memcmp(const void* dest, const void* src, uintptr amt) {
		return std::memcmp(dest, src, amt);
	}

	FORCEINLINE void* memset(void* dest, int value, uintptr amt) {
		return std::memset(dest, value, amt);
	}

	template <typename T, typename... Args>
	FORCEINLINE UniquePointer<T> make_unique(Args&&... args) {
		return std::make_unique<T>(args...);
	}

	template <typename T, typename... Args>
	FORCEINLINE SharedPointer<T> make_shared(Args&&... args) {
		return std::make_shared<T>(args...);
	}
};

template <typename T>
uint32 Memory::SharedCounter<T>::counter(0);
