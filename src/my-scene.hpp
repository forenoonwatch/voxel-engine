#pragma once

#include <engine/scene/scene.hpp>

class RenderTarget;
class ChunkManager;
class UniformBuffer;

class MyScene final : public Scene<MyScene> {
    public:
        void load();

        void update(float deltaTime);
        void render();

        void unload();
        
        virtual ~MyScene() = default;
    private:
        RenderTarget* screen;
        ChunkManager* chunkManager;
        UniformBuffer* cameraBuffer;
};
