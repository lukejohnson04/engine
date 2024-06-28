#pragma once

struct DialoguePlayerState {
    // std::string text;
    bool isDisplayingChoices = false;

    Sprite player_sprite;
    text_object text;
    // SDL_Texture* textTexture = nullptr;
    int textWidth = 0;
    int textHeight = 0;
    float time_accumulator = 0;

    TTF_Font* font=nullptr;

    bool isPlaying = false;
    bool isUpdating = false;

    bool recorder_menu_open = false;
    text_object recorder_options[4];
    u8 recorder_option_count=0;
    u8 selection=0;
    Sprite recorder_icon;
    Sprite selector_icon;

};

namespace DialoguePlayer {
	internal_function void InitializeState(DialoguePlayerState *n_state);
	internal_function void Update(DialoguePlayerState &dialogue_state, float timestep);
	internal_function void Render(DialoguePlayerState &dialogue_state);
	internal_function void SetText(DialoguePlayerState &dialogue_state, char *n_text);
};
