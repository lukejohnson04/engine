
#include "core/engine.cpp"

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 256


struct FightChara {
    v2 pos={0,0};
    enum {
        ATTACK_LOADING,
        ATTACK_LOADED,
        ATTACK_NONE
    } attack_state = ATTACK_NONE;
    i32 health=100;
    f32 attack_load_timer=0.f;
    f32 damage_timer=0.f;
    bool damaged=false;
    f32 rotation=0;
    i32 frame=0;
};


struct GameState {
    Shader colorShader;
    Shader textureShader;
    GLuint player_texture;

    glm::mat4 projection;
    FightChara player;
};

global_variable GameState *game_state=nullptr;

void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    game_state->colorShader = CreateShader("../src/shaders/color.vert","../src/shaders/color.frag");
    game_state->textureShader = CreateShader("../src/shaders/texture.vert","../src/shaders/texture.frag");
    game_state->projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);

    game_state->player = {};

    glGenTextures(1,&game_state->player_texture);
    GL_load_texture(game_state->player_texture,"res/imgs/fightchara.png");
    
    // load all textures
    memory->is_initialized = true;
    std::cout << "Initialized game memory\n";
}


// Luck
// Strength
// Charisma
// Intelligence


#if defined __cplusplus
extern "C"
#endif
GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    game_state = (GameState*)game_memory->permanentStorage;
    input = n_input;
    if (!input) {
        return;
    }    
    //ImGui::SetCurrentContext(imgui_context);

    if (game_memory->is_initialized == false) {
        InitializeGameMemory(game_memory);
    }

    // <----------------------->
    //       UPDATE BEGIN
    // <----------------------->
    {
        auto player = &game_state->player;
        const int speed = (int)(350*timestep);
        if (input->is_pressed[SDL_SCANCODE_D]) {
            game_state->player.pos.x += speed;
        } if (input->is_pressed[SDL_SCANCODE_A]) {
            game_state->player.pos.x -= speed;
        } if (input->is_pressed[SDL_SCANCODE_W]) {
            game_state->player.pos.y -= speed;
        } if (input->is_pressed[SDL_SCANCODE_S]) {
            game_state->player.pos.y += speed;
        }

        const float rot_speed = PI*1.25f;
        if (input->is_pressed[SDL_SCANCODE_LEFT]) {
            game_state->player.rotation -= rot_speed*timestep;
        } if (input->is_pressed[SDL_SCANCODE_RIGHT]) {
            game_state->player.rotation += rot_speed*timestep;
        }

        if (game_state->player.rotation >= PI*2) {
            game_state->player.rotation -= PI*2;
        } else if (game_state->player.rotation < 0) {
            game_state->player.rotation += PI*2;
        }
        if (input->just_pressed[SDL_SCANCODE_LCTRL]) {
            if (player->attack_state == FightChara::ATTACK_NONE) {
                player->attack_state = FightChara::ATTACK_LOADING;
                player->attack_load_timer = 0.f;
            } else if (player->attack_state == FightChara::ATTACK_LOADED) {
                player->attack_state = FightChara::ATTACK_NONE;
                /*
                Entity *wave = create_entity();
                float rot = sprite.transform.rotation;
                wave->Create(Entity::FIGHT_LOVE_WAVE_ATTACK);
                wave->vel = angle_to_vec(rot-PI/2).mult(-6.f);
                wave->sprite.transform.rotation = rot - PI*2;
                wave->SetPosition(transform.pos);

                // LoadWaveAttack(wave,transform.pos,rot-PI/2);
                player->attack_state = FightCharaProps::ATTACK_NONE;
                player->attack_load_timer = 0.f;
                */
            }
        }
        if (player->attack_state == FightChara::ATTACK_LOADING) {
            player->attack_load_timer += timestep;
            if (player->attack_load_timer >= 1.5f) {
                player->attack_state = FightChara::ATTACK_LOADED;
            }
        } if (player->attack_state == FightChara::ATTACK_LOADED) {
            player->frame = 1;
        } else {
            player->frame = 0;
        }
    }
    // <----------------------->
    //       RENDER BEGIN
    // <----------------------->
    
    glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    UseShader(&game_state->colorShader);
    glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
    glUniform4f(game_state->colorShader.Uniform("color"),1.0f,1.0f,0.0f,1.0f);
    GL_DrawRect({100,100,100,100});
    GL_DrawRect({400,100,100,100});


    UseShader(&game_state->textureShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game_state->player_texture);
    glUniform1i(game_state->textureShader.Uniform("_texture"),0);
    glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,1.0f,1.0f);
    glUniformMatrix4fv(game_state->textureShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));

    glm::mat4 model = glm::mat4(1.0f);
    if (game_state->player.rotation != 0) {
        glm::vec2 pos = glm::vec2(game_state->player.pos.x,game_state->player.pos.y);
        float angleRadians = game_state->player.rotation;
        // Calculate the center of the object for rotation
        glm::vec2 size = glm::vec2(64,64);
        glm::vec2 center = pos + size * 0.5f;
        model = glm::translate(model, glm::vec3(center, 0.0f)); // Move pivot to center
        model = glm::rotate(model, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
        model = glm::translate(model, glm::vec3(-center, 0.0f)); // Move pivot back
    }

    glUniformMatrix4fv(game_state->textureShader.Uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

    GL_DrawTexture({game_state->player.frame*32,0,32,32},{(int)game_state->player.pos.x,(int)game_state->player.pos.y,64,64});



    /*

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debug");
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    
    ImGui::End();
    ImGui::Render();
    
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    */
    SDL_GL_SwapWindow(window);
}

#if defined __cplusplus
extern "C"
#endif
GAME_HANDLE_EVENT(GameHandleEvent) {
    if (e.sdl_event.type == SDL_QUIT) {
        *running = false;
        e.handled = true;
        return;
    }
    if (game_state == nullptr) {
        return;
    }
}

#ifdef GAMEDISABLED
#include <SDL.h>
#if defined __cplusplus
extern "C"
#endif
extern GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    std::cout << 1235;
}

#if defined __cplusplus
extern "C"
#endif
extern GAME_HANDLE_EVENT(GameHandleEvent) {
}
#endif
