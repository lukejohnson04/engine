
#pragma once

struct InputState {
    bool is_pressed[256];
    bool just_pressed[256];
    bool just_released[256];

    bool mouse_just_pressed;
    bool mouse_just_released;
    bool mouse_pressed;

    i32 mouseXMotion,mouseYMotion;
};

void PollEvents(InputState *state, game_handle_event* callback, bool *running);
v2i GetMousePosition();


InputState *input=nullptr;

void PollEvents(InputState *state, game_handle_event *callback, bool *running) {
    memset(state->just_pressed, 0, 256*sizeof(bool));
    memset(state->just_released, 0, 256*sizeof(bool));
    state->mouse_just_pressed=false;
    state->mouse_just_released=false;
    state->mouseXMotion=0;
    state->mouseYMotion=0;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        //ImGui_ImplSDL2_ProcessEvent(&e);
        //auto &io = ImGui::GetIO();

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            state->just_pressed[e.key.keysym.scancode] = true;
            state->is_pressed[e.key.keysym.scancode] = true;
        } else if (e.type == SDL_KEYUP) {
            state->just_released[e.key.keysym.scancode] = true;
            state->is_pressed[e.key.keysym.scancode] = false;
        } 
        //if (io.WantCaptureMouse) {
        //    continue;
        //}
        
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            state->mouse_pressed = true;
            state->mouse_just_pressed = true;
        } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            state->mouse_pressed = false;
            state->mouse_just_released = true;
        } else if (e.type == SDL_MOUSEMOTION) {
            state->mouseXMotion = e.motion.xrel;
            state->mouseYMotion = e.motion.yrel;
        }

        Event proc_event;
        proc_event.handled = false;
        proc_event.sdl_event = e;
        
        callback(proc_event, running);
    }
}

v2i GetMousePosition() {
    int mouse_X, mouse_Y;
    SDL_GetMouseState(&mouse_X, &mouse_Y);
    return {mouse_X, mouse_Y};
}
