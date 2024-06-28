struct InputState;
struct ImGuiContext;
struct SDL_Renderer;

struct Event {
    SDL_Event sdl_event;
    bool handled=0;
};

#define GAME_UPDATE_AND_RENDER(name) void name(SDL_Window *window, InputState *n_input, GameMemory *game_memory, ImGuiContext *imgui_context, float timestep)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define GAME_HANDLE_EVENT(name) void name(Event e, bool *running)
typedef GAME_HANDLE_EVENT(game_handle_event);
