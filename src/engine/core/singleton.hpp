#pragma once

template <typename T>
class Singleton {
	public:
		static inline T& getInstance() { return instance; }
	private:
		static inline T instance;
};

