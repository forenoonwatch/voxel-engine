#include "player-input.hpp"

#include <engine/ecs/ecs.hpp>
#include <engine/application/input.hpp>

void update_player_input(Registry& registry, Input& input) {
    registry.view<PlayerInputComponent>().each([input](auto& pic) {
        pic.forward = input.is_key_down(Input::KEY_W);
        pic.back = input.is_key_down(Input::KEY_S);
        pic.left = input.is_key_down(Input::KEY_A);
        pic.right = input.is_key_down(Input::KEY_D);
        pic.up = input.is_key_down(Input::KEY_E);
        pic.down = input.is_key_down(Input::KEY_Q);

        pic.rightMouse = input.is_mouse_down(Input::MOUSE_BUTTON_RIGHT);

        pic.mouseDeltaY = input.getMouseDeltaY();
        pic.mouseDeltaX = input.getMouseDeltaX();
    });
}
