#pragma once

#include <engine/core/common.hpp>

#include <engine/scene/scene.hpp>

class CoreEngine final : public BaseEngine {
	public:
		CoreEngine() noexcept
				: currentScene(nullptr) {}

		template <typename SceneType>
		void load() noexcept {
			stop();

			SceneType* newScene = new SceneType(); // TODO: custom allocator?
			currentScene = static_cast<BaseScene*>(newScene);

			static_cast<Scene<SceneType>*>(newScene)->load(*this);
			newScene->run();
		}

		void stop() noexcept {
			if (currentScene) {
				currentScene->virtualUnload();
				delete currentScene; // TODO: custom alloactor

				currentScene = nullptr;
			}
		}

		virtual ~CoreEngine() {
			stop();
		}
	private:
		NULL_COPY_AND_ASSIGN(CoreEngine);

		BaseScene* currentScene;
};
