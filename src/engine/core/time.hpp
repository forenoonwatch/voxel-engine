#pragma once

#include <engine/core/common.hpp>
#include <engine/core/singleton.hpp>

class Time final : public Singleton<Time> {
	public:
		Time();

		static FORCEINLINE double getTime() noexcept {
			return getInstance().getTimeInternal();
		}
		
		static void sleep(double time) noexcept;
	private:
		double start;

		double getTimeInternal() const noexcept;
};

