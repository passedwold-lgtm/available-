#pragma once
#include <vector>
#include <cmath>


// --- Global Variables ---
struct Vars_t {
    //tab0
    bool ESP_Box = true, ESP_Name = true, ESP_Distance = true;
    float ESP_MaxDistance = 100.0f;
    //tab1
    bool AIMBOT_Aimbot = true, AIMBOT_AimbotKey = true;
    float AIMBOT_AimbotKeyCode = 0.0f;
    //tab2
    bool MEMORY_Memory = true, MEMORY_MemoryKey = true;
    float MEMORY_MemoryKeyCode = 0.0f;

} Vars;

std::vector<void *> players;

class game_sdk_t {
public:
    void init();

    // void* (*transform)(void *);
    // void* (*camera)();
    
    // Vector3 (*position)(void *);

    // Vector3 (*world_to_viewport)(void *, Vector3);
    
};
game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init() {
    // this->position = (Vector3 (*)(void *))getRealOffset(ENCRYPTOFFSET("0xFFFFFFFF"));
    // this->transform = (void* (*)(void *))getRealOffset(ENCRYPTOFFSET("0xFFFFFFFF"));
    // this->camera = (void* (*)())getRealOffset(ENCRYPTOFFSET("0xFFFFFFFF"));
    // this->world_to_viewport = (Vector3 (*)(void *, Vector3))getRealOffset(ENCRYPTOFFSET("0xFFFFFFFF"));

}

void esp_draw() {
    // if (!Vars.ESP_Box) return;
    // for (void *player : players) {
    //     Vector3 pos = game_sdk->position(player);
    //     Vector3 screen = game_sdk->world_to_viewport(player, pos);
    //     draw_box(screen, 10, 10, 10);
    // }
}

void render_lop() {
    // esp_draw();

}
