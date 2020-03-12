#pragma once

#include <engine/core/common.hpp>
#include <engine/core/time.hpp>

#include <engine/scene/base-engine.hpp>

class BaseScene {
	public:
		virtual void virtualUnload() = 0;

		virtual ~BaseScene() = default;
};

template <typename DerivedScene>
class Scene : public BaseScene {
	public:
		Scene();

		void load(BaseEngine& engine);

		void update(float deltaTime);
		void render();

		void unload();

		virtual void virtualUnload() override final;
		
		virtual ~Scene() = default;
	protected:
		BaseEngine* getEngine() noexcept;
		const BaseEngine* getEngine() const noexcept;
	private:
		BaseEngine* engine;
		bool running;

		void run();
		void stop();

		friend class CoreEngine;
};

#include "engine/scene/scene.inl"

