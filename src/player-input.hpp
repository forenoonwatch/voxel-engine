#pragma once

class Registry;
class Input;

struct PlayerInputComponent {
    bool forward;
    bool back;
    bool left;
    bool right;
    bool up;
    bool down;

    bool rightMouse;

    float mouseDeltaY;
    float mouseDeltaX;
};

void update_player_input(Registry& registry, Input& input);
