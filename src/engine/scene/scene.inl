
template <typename DerivedScene>
Scene<DerivedScene>::Scene()
		: engine(nullptr)
		, running(false) {}

template <typename DerivedScene>
void Scene<DerivedScene>::load(BaseEngine& engine) {
	this->engine = &engine;
	static_cast<DerivedScene*>(this)->load();
}

template <typename DerivedScene>
void Scene<DerivedScene>::update(float deltaTime) {
	static_cast<DerivedScene*>(this)->update(deltaTime);
}

template <typename DerivedScene>
void Scene<DerivedScene>::render() {
	static_cast<DerivedScene*>(this)->render();
}

template <typename DerivedScene>
void Scene<DerivedScene>::unload() {
	stop();
	static_cast<DerivedScene*>(this)->unload();
}

template <typename DerivedScene>
void Scene<DerivedScene>::virtualUnload() {
	unload();
}

template <typename DerivedScene>
BaseEngine* Scene<DerivedScene>::getEngine() noexcept {
	return engine;
}

template <typename DerivedScene>
const BaseEngine* Scene<DerivedScene>::getEngine() const noexcept {
	return engine;
}

template <typename DerivedScene>
void Scene<DerivedScene>::run() {
	if (running) {
		return;
	}

	constexpr const float frameTime = 1.0 / 60.0;

	uint32 fpsCounter = 0;

	double lastTime = Time::getTime();
	double fpsTimeCounter = 0.0;
	double updateTimer = 1.0;

	double currentTime, passedTime;
	bool shouldRender;

	running = true;

	while (running) {
		currentTime = Time::getTime();
		passedTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeCounter += passedTime;
		updateTimer += passedTime;

		if (fpsTimeCounter >= 1.0) {
			// TODO: emit FPS

			fpsTimeCounter = 0.0;
			fpsCounter = 0;
		}

		shouldRender = false; // TODO: set equal to SHOULD_UNLOCK_FPS

		while (updateTimer >= frameTime) {
			engine->update(frameTime);

			if (!running) {
				break;
			}

			update(frameTime);
			engine->postUpdate(frameTime);

			updateTimer -= frameTime;
			shouldRender = true;
		}

		if (shouldRender) {
			render();
			engine->render();

			++fpsCounter;
		}
	}
}

template <typename DerivedScene>
void Scene<DerivedScene>::stop() {
	if (!running) {
		return;
	}

	running = false;
}

