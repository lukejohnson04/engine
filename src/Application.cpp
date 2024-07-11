#pragma once
#include <string>

struct SDL_Window;
struct SDL_Surface;
struct Event;

struct Application {
    void Run(std::string start_room="");
    void OnEvent(Event e);

    SDL_Window *window=nullptr;
    SDL_Surface *screenSurface=nullptr;

    bool running=true;
    bool imgui_render=true;
    bool debug_mode=false;
};


#include <windows.h>

#ifdef DEBUG
int main() {
#else
int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd) {
#endif
    Application app = Application();
    app.Run("");

    return 0;
}


#include <iostream>
#include "core/engine.cpp"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 540


struct win32_game_code {
    HMODULE dll;
    FILETIME LastWriteTime;
    game_update_and_render *UpdateAndRender;
    game_handle_event *HandleEvent;
    bool is_valid=false;
};

GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) {}
GAME_HANDLE_EVENT(GameHandleEventStub) {}

inline FILETIME Win32GetLastWriteTime(char *filename) {
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE) {
        LastWriteTime = FindData.ftLastWriteTime;
    }
    FindClose(FindHandle);
    return LastWriteTime;
}

internal void LoadGameCode(win32_game_code *game_code) {
    char *sourceDLL = "DynamicGameCode.dll";
    char *tempDLL = "TempGameCode.dll";

    game_code->LastWriteTime = Win32GetLastWriteTime(sourceDLL);
    CopyFile(sourceDLL, tempDLL, FALSE);
    game_code->dll = LoadLibrary(tempDLL);
    if (game_code->dll) {
        game_code->UpdateAndRender = (game_update_and_render*)GetProcAddress(game_code->dll, "GameUpdateAndRender");
        game_code->HandleEvent = (game_handle_event*)GetProcAddress(game_code->dll, "GameHandleEvent");
        game_code->is_valid = game_code->UpdateAndRender && game_code->HandleEvent;
    }

    if (game_code->is_valid == false) {
        game_code->UpdateAndRender = GameUpdateAndRenderStub;
        game_code->HandleEvent = GameHandleEventStub;
        std::cout << "Failed to load game code!\n";
    }
}


internal void UnloadGameCode(win32_game_code *game_code) {
    if (game_code->dll) {
        FreeLibrary(game_code->dll);
        game_code->dll = 0;
    }

    game_code->is_valid = false;
    game_code->UpdateAndRender = GameUpdateAndRenderStub;
    game_code->HandleEvent = GameHandleEventStub;
    game_code->LastWriteTime = {};
}

global_variable win32_game_code global_game_code;

// input processes events, and then calls this function
void Application::OnEvent(Event e) {
    if (e.sdl_event.type == SDL_QUIT) {
        running = false;
        e.handled = true;
        return;
    }
}

global_variable int global_clock=0;
global_variable int last_frame_time=0;
internal void sleep_till_next_update() {
    int dl_time = (int)(last_frame_time + FRAME_TIME_MS - SDL_GetTicks());
    SDL_Delay(dl_time > 0 ? dl_time : 0);
    global_clock = SDL_GetTicks();
    last_frame_time = global_clock;
}

void Application::Run(std::string start_room) {
    bool vsync = false;
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_VIDEO_OPENGL) < 0) {
        printf( "SDL could not initialize! SDL Error: %s\r\n", SDL_GetError() );
        return;
    }    
    
    if (TTF_Init() != 0) {
        printf("Error: %s\n", TTF_GetError()); 
        return;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Error: %s\n", IMG_GetError());
        return;
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);  // Use OpenGL 3.x
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  // Version 3.3, for example
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    window = SDL_CreateWindow("Hello",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_OPENGL);
    
    printf("Created window\n");

    if (window == nullptr) {
        printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
    }
    
    SDL_GLContext glContext;

    glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    SDL_GL_MakeCurrent(window, glContext);
    
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum err = glGetError();
    
    if (err != GL_NO_ERROR) {
        std::string error;
        switch(err) {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
 
        std::cout << "OPENGL ERROR: " << error << std::endl;
    }

    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    screenSurface = SDL_GetWindowSurface(window);

    std::cout << "Loading game code.\n";
    LoadGameCode(&global_game_code);
    std::cout << "Reserving game memory.\n";
    GameMemory game_memory = {};
    game_memory.size = Megabytes(256);
    game_memory.permanentStorage = VirtualAlloc(0, game_memory.size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (game_memory.permanentStorage == nullptr) {
        std::cout << "Error: Failed to allocate game memory!\n";
        return;
    }

    std::cout << "Succesfully reserved game memory.\n";

    input = new InputState();

    
    /*
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, sdl_renderer);
    ImGui_ImplSDLRenderer2_Init(sdl_renderer);
    */
    
    // poll events
    float load_timer=0;
    while (running) {
        // check for dll reload
        FILETIME new_WriteTime = Win32GetLastWriteTime("DynamicGameCode.dll");
        if (CompareFileTime(&new_WriteTime, &global_game_code.LastWriteTime) != 0) {
            UnloadGameCode(&global_game_code);
            LoadGameCode(&global_game_code);
            printf("Reloaded game code\n");
        }
        // events
        PollEvents(input, global_game_code.HandleEvent, &running);
        global_game_code.UpdateAndRender(window, input, &game_memory, nullptr, FRAME_TIME);
        sleep_till_next_update();
    }

    //ImGui_ImplSDLRenderer2_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
}
