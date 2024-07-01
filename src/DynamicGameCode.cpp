


#include "core/engine.cpp"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 540

#define NATIVE_GAME_WIDTH 240
#define NATIVE_GAME_HEIGHT 180

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

enum GameMode {
    GM_GOBLINS,
    GM_ROAD_LESS_TRAVELLED,
    GM_COUNT
};


struct GameState {
    Shader colorShader;
    Shader textureShader;
    GLuint player_texture;
    GLuint goblins_menu_texture;
    GLuint less_traveled_bg_texture;
    GLuint text_texture;
    GLuint diverge_texture;
    GLuint forest_texture;
    GLuint std_texture;
    GLuint endings_texture;
    v2i tex_dimensions;
    i32 dialogue_line = -1;
    Mix_Chunk *dialogue;
    Mix_Music *travel_music;

    Mix_Chunk *dialogue_streams[13];

    TTF_Font *font;

    GameMode mode;

    enum {
        POEM_SCROLL,
        CHOICE_DIALOGUE,
        CHOICE,
        CHOICE_LESS_TRAVELED,
        CHOICE_MORE_TRAVELED,

        GOOD_ENDING,
        BAD_ENDING,
        
        STATE_COUNT
    } travelled_state;
    float text_scroll_amount=0;


    glm::mat4 projection;
    FightChara player;
};

global_variable GameState *game_state=nullptr;

char robert_frost_poem[] = "TWO ROADS DIVERGED IN A YELLOW WOOD,\n\
AND SORRY I COULD NOT TRAVEL BOTH\n\
AND BE ONE TRAVELER, LONG I STOOD\n\
AND LOOKED DOWN ONE AS FAR AS I COULD\n\
TO WHERE IT BENT IN THE UNDERGROWTH;\n\
\n\
THEN TOOK THE OTHER, AS JUST AS FAIR,\n\
AND HAVING PERHAPS THE BETTER CLAIM,\n\
BECAUSE IT WAS GRASSY AND WANTED WEAR;\n\
THOUGH AS FOR THAT THE PASSING THERE\n\
HAD WORN THEM REALLY ABOUT THE SAME,\n\
\n\
AND BOTH THAT MORNING EQUALLY LAY\n\
IN LEAVES NO STEP HAD TRODDEN BLACK.\n\
OH, I KEPT THE FIRST FOR ANOTHER DAY!\n\
YET KNOWING HOW WAY LEADS ON TO WAY,\n\
I DOUBTED IF I SHOULD EVER COME BACK.\n\
\n\
I SHALL BE TELLING THIS WITH A SIGH\n\
SOMEWHERE AGES AND AGES HENCE:\n\
TWO ROADS DIVERGED IN A WOOD, AND I— \n\
I TOOK THE ONE LESS TRAVELED BY,\n\
AND THAT HAS MADE ALL THE DIFFERENCE.";

void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    game_state->colorShader = CreateShader("../src/shaders/color.vert","../src/shaders/color.frag");
    game_state->textureShader = CreateShader("../src/shaders/texture.vert","../src/shaders/texture.frag");
    game_state->projection = glm::ortho(0.0f, static_cast<float>(NATIVE_GAME_WIDTH), static_cast<float>(NATIVE_GAME_HEIGHT), 0.0f, -1.0f, 1.0f);
    game_state->mode = GM_ROAD_LESS_TRAVELLED;
    game_state->travelled_state = GameState::POEM_SCROLL;

    game_state->player = {};

    glGenTextures(1,&game_state->player_texture);
    GL_load_texture(game_state->player_texture,"res/imgs/guy.png");
    glGenTextures(1,&game_state->goblins_menu_texture);
    GL_load_texture(game_state->goblins_menu_texture,"res/imgs/goblins_menu.png");
    glGenTextures(1,&game_state->less_traveled_bg_texture);
    GL_load_texture(game_state->less_traveled_bg_texture,"res/imgs/less_travelled.png");
    glGenTextures(1,&game_state->diverge_texture);
    GL_load_texture(game_state->diverge_texture,"res/imgs/diverge.png");
    glGenTextures(1,&game_state->std_texture);
    GL_load_texture(game_state->std_texture,"res/imgs/std.png");
    glGenTextures(1,&game_state->endings_texture);
    GL_load_texture(game_state->endings_texture,"res/imgs/endings.png");
    glGenTextures(1,&game_state->forest_texture);
    GL_load_texture(game_state->forest_texture,"res/imgs/forest.png");


    game_state->font = TTF_OpenFont("res/fonts/Alkhemikal.ttf",16);
    if (game_state->font == nullptr) {
        printf("ERROR: failed to load res/fonts/Alkhemikal.ttf!\n");
        printf("Error: %s\n", TTF_GetError()); 
    }
    //TTF_SetFontOutline(game_state->font,2);
    glGenTextures(1,&game_state->text_texture);
    generate_text(game_state->text_texture,game_state->font,robert_frost_poem,COLOR_WHITE,220);
    int miplevel=0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);

    game_state->dialogue = Mix_LoadWAV("res/sound/traveled_monologue.mp3");
    game_state->travel_music = Mix_LoadMUS("res/sound/travel_music.mp3");

    for (i32 num=0;num<13;num++) {
        game_state->dialogue_streams[num] = Mix_LoadWAV(("res/sound/traveled_dialogue" + std::to_string(num+1) + ".mp3").c_str());
    }
    
    Mix_VolumeChunk(game_state->dialogue,128);
    Mix_PlayChannel(-1,game_state->dialogue,0);
    Mix_PlayMusic(game_state->travel_music,-1);
    Mix_VolumeMusic(18);
    
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
        if (game_state->mode == GM_GOBLINS) {

            
        } else if (game_state->mode == GM_ROAD_LESS_TRAVELLED) {
            //if (game_state->travelled_state == GameState::POEM_SCROLL || game_state->travelled_state == GameState::CHOICE_MORE_TRAVELED || game_state->travelled_state == GameState::BAD_ENDING) {
                game_state->text_scroll_amount += timestep;
                //}
        }
    }
    // <----------------------->
    //       RENDER BEGIN
    // <----------------------->
    
    glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /*
    UseShader(&game_state->colorShader);
    glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
    glUniform4f(game_state->colorShader.Uniform("color"),1.0f,1.0f,0.0f,1.0f);
    GL_DrawRect({100,100,100,100});
    GL_DrawRect({400,100,100,100});
    */

    if (game_state->mode == GM_GOBLINS) {
        UseShader(&game_state->textureShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,game_state->goblins_menu_texture);
        glUniform1i(game_state->textureShader.Uniform("_texture"),0);
        glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,1.0f,1.0f);
        glUniformMatrix4fv(game_state->textureShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(game_state->textureShader.Uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

        GL_DrawTexture({0,0,160,120},{0,0,160,120});
    } else if (game_state->mode == GM_ROAD_LESS_TRAVELLED) {
        UseShader(&game_state->textureShader);
        glActiveTexture(GL_TEXTURE0);
        glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,1.0f,1.0f);
        glUniformMatrix4fv(game_state->textureShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(game_state->textureShader.Uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

        if (game_state->travelled_state == GameState::POEM_SCROLL) {
            glBindTexture(GL_TEXTURE_2D,game_state->diverge_texture);
            glUniform1i(game_state->textureShader.Uniform("_texture"),0);
            GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

            float scroll_amount=game_state->text_scroll_amount;
            const float scroll_speed = 10;
            int text_y_dest = NATIVE_GAME_HEIGHT + scroll_speed*2 - int(scroll_amount * scroll_speed);
            float full_time = (NATIVE_GAME_HEIGHT + scroll_speed*2 + game_state->tex_dimensions.y)/scroll_speed + 1.0f;
            if (scroll_amount >= full_time) {
                float fade_timestep = scroll_amount-full_time;
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                float fade_len = 5.0f;
                float fade = fade_timestep/fade_len;
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MIN(1.0f,fade));
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if (fade_timestep - fade_len >= 2.0f) {
                    game_state->travelled_state = GameState::CHOICE_DIALOGUE;
                }
            } else {
                glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);        
                GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,text_y_dest,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
            }
        } else if (game_state->travelled_state == GameState::CHOICE_DIALOGUE) {
            glBindTexture(GL_TEXTURE_2D,game_state->less_traveled_bg_texture);
            glUniform1i(game_state->textureShader.Uniform("_texture"),0);
            GL_DrawTexture({0,0,160,120},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});


            char dialogue[13][128] = {
                "Oh, the beauty of life, once again marred by a tricky crossroad.",
                //"A world awaits, of great joys yet great horrors.",
                "For without the work of the devil, the choice shall lie clear.",
                "On the left, a path through the woods.",
                "Shall we stop and smell the flowers? Enjoy the magic of nature.",
                "Is it as it seems, a boring patch with nothing of interest?",
                "There must be a reason the path is less travelled, correct?",
                "But perhaps the traveledness of the path...",
                "Is not such a reflecion of the path, but of the travelers.",
                "Is the path poor? Or are the travellers weak?",
                "To jump to either conclusion is not the winning strategy",
                "For the winningest strategy of all is to draw your own conclusions",
                //"If not to stop and smell the flowers, there is the other path",
                "On the right, the path leads to a town with blackjack and hookers.",
                "Choose wisely."
            };

            if (input->just_pressed[SDL_SCANCODE_RETURN] || game_state->dialogue_line==-1) {
                game_state->dialogue_line++;
                if (game_state->dialogue_line >= 13) {
                    game_state->travelled_state = GameState::CHOICE;
                } else {
                    generate_text(game_state->text_texture,game_state->font,dialogue[game_state->dialogue_line],COLOR_WHITE,220);
                    int miplevel=0;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                    Mix_PlayChannel(0,game_state->dialogue_streams[game_state->dialogue_line],0);
                }
            }
            glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
            glUniform1i(game_state->textureShader.Uniform("_texture"),0);        
            GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,20,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
        } else if (game_state->travelled_state == GameState::CHOICE) {
            UseShader(&game_state->textureShader);
            glBindTexture(GL_TEXTURE_2D,game_state->less_traveled_bg_texture);
            glUniform1i(game_state->textureShader.Uniform("_texture"),0);

            iRect left_rect = {42,38,53,86};
            iRect right_rect = {157,55,52,48};

            v2i mpos = GetMousePosition();
            mpos.x = (int)(((float)mpos.x) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
            mpos.y = (int)(((float)mpos.y) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
            i32 frame=0;

            if (rect_contains_point(left_rect,mpos)) {
                frame = 1;
                if (input->mouse_just_pressed) {
                    game_state->travelled_state = GameState::CHOICE_LESS_TRAVELED;
                    game_state->text_scroll_amount=0;
                    game_state->dialogue_line=-1;
                }
            } else if (rect_contains_point(right_rect,mpos)) {
                frame = 2;
                if (input->mouse_just_pressed) {
                    game_state->travelled_state = GameState::CHOICE_MORE_TRAVELED;
                    game_state->text_scroll_amount=0;
                    game_state->dialogue_line=-1;
                }
            }
            GL_DrawTexture({frame*160,0,160,120},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        } else if (game_state->travelled_state == GameState::CHOICE_LESS_TRAVELED) {
            static enum {
                FADE_OUT,
                FADE_TEXT,
                FADE_IN,
                SCENE_TEXT,
                ENDING_FADE_OUT
            }  scene_state=FADE_OUT;
            if (scene_state == FADE_OUT) {
                glBindTexture(GL_TEXTURE_2D,game_state->less_traveled_bg_texture);
                GL_DrawTexture({0,0,160,120},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

                float fade_timestep = game_state->text_scroll_amount;
                float fade_time = 3.f;

                Mix_VolumeMusic(MAX(int(18.f * (1.0f - (fade_timestep/fade_time))),0));
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MIN(1.0f,fade_timestep/fade_time));
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if (fade_timestep >= fade_time + 1.0f) {
                    scene_state = FADE_TEXT;
                }
            } else if (scene_state == ENDING_FADE_OUT) {
                glBindTexture(GL_TEXTURE_2D,game_state->forest_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                float fade_timestep = game_state->text_scroll_amount;
                float fade_time = 3.f;
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MIN(1.0f,fade_timestep/fade_time));
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if (fade_timestep >= fade_time + 1.0f) {
                    game_state->travelled_state = GameState::GOOD_ENDING;
                    game_state->text_scroll_amount=0;
                    
                    generate_text(game_state->text_texture,game_state->font,"Retry",COLOR_WHITE,220);
                    int miplevel=0;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                }
                
            } else if (scene_state == FADE_TEXT) {
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,1.0f);
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if ((input->just_pressed[SDL_SCANCODE_RETURN] || game_state->dialogue_line==-1)) {
                    game_state->dialogue_line++;

                    char dialogue[11][128] = {
                        "And so you came across the path.",
                        "Coy yet confident in the decision, you travelled.",
                        "And travelled... and walked and walked until you thought you could not walk any more",
                        "\"I Wonder what's going on in the town right now?\", you wondered.",
                        "The money, the dollars, the blackjack...",
                        "You considered a speedy retreat. Perhaps the path more taken is taken more for good reason!",
                        "But of course, the devil will tempt you at every twist and turn",
                        "For even a boring pathway will be full of temptation. And you found the strength to deny it.",
                        "You found a way to ejoy the journey. Smelling the flowers, enjoying the vibrations of nature.",
                        "Gods little gifts just kept on giving...",
                        "But it appeared God had one gift left, patiently waiting."
                    };
                    
                    if (game_state->dialogue_line >= 11) {
                        scene_state = FADE_IN;
                        game_state->text_scroll_amount=0;
                    } else {
                        generate_text(game_state->text_texture,game_state->font,dialogue[game_state->dialogue_line],COLOR_WHITE,220);
                        int miplevel=0;
                        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                        //Mix_PlayChannel(0,game_state->dialogue_streams[game_state->dialogue_line],0);
                    }
                }
                UseShader(&game_state->textureShader);
                glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);        
                GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,20,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
            } else if (scene_state == FADE_IN || scene_state == SCENE_TEXT) {
                glBindTexture(GL_TEXTURE_2D,game_state->forest_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

                if (scene_state == FADE_IN) {
                    float fade_timestep = game_state->text_scroll_amount - 3.0f;
                    float fade_time = 1.f;

                    UseShader(&game_state->colorShader);
                    glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                    glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MAX(0.0f,1.0f - fade_timestep/fade_time));
                    GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                    if (fade_timestep >= fade_time + 1.0f) {
                        scene_state = SCENE_TEXT;
                        game_state->dialogue_line=-1;
                    }
                } else {
                    if (input->just_pressed[SDL_SCANCODE_RETURN] || game_state->dialogue_line==-1) {
                        game_state->dialogue_line++;
                        char dialogue[8][128] = {
                            "A great journey, a great voyage, lengthy trail, nature awaits.",
                            "Mana from the heavens above, a lady dropped down from the gates.",
                            "Another world, another time, another path gave different gifts.",
                            "For the heavens gave a perfect lady, untainted if you get my drift",
                            "A lady less taken, a lady less travelled, a perfect gift from God.",
                            "You can't start a family with a lady who's been on someone elses rod",
                            "The path more taken, the path more men have travelled, the more men that will walk through your home",
                            "The path less taken, a lady untainted, will let a family grow around it like a sourdough starter."
                        };
                        if (game_state->dialogue_line >= 6) {
                            scene_state = ENDING_FADE_OUT;
                            game_state->text_scroll_amount=0;
                        } else {
                            generate_text(game_state->text_texture,game_state->font,dialogue[game_state->dialogue_line],COLOR_WHITE,220);
                            int miplevel=0;
                            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                            //Mix_PlayChannel(0,game_state->dialogue_streams[game_state->dialogue_line],0);
                        }
                    } if (scene_state != ENDING_FADE_OUT) {
                        glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                        glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                        GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,20,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
                    }
                }
            }

        } else if (game_state->travelled_state == GameState::CHOICE_MORE_TRAVELED) {
            static enum {
                FADE_OUT,
                FADE_TEXT,
                FADE_IN,
                SCENE_TEXT,
                ENDING_FADE_OUT
            }  scene_state=FADE_OUT;
            if (scene_state == FADE_OUT) {
                glBindTexture(GL_TEXTURE_2D,game_state->less_traveled_bg_texture);
                GL_DrawTexture({0,0,160,120},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

                float fade_timestep = game_state->text_scroll_amount;
                float fade_time = 3.f;

                Mix_VolumeMusic(MAX(int(18.f * (1.0f - (fade_timestep/fade_time))),0));
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MIN(1.0f,fade_timestep/fade_time));
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if (fade_timestep >= fade_time + 1.0f) {
                    scene_state = FADE_TEXT;
                }
            } else if (scene_state == ENDING_FADE_OUT) {
                glBindTexture(GL_TEXTURE_2D,game_state->std_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                float fade_timestep = game_state->text_scroll_amount;
                float fade_time = 3.f;
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MIN(1.0f,fade_timestep/fade_time));
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if (fade_timestep >= fade_time + 1.0f) {
                    game_state->travelled_state = GameState::BAD_ENDING;
                    game_state->text_scroll_amount=0;
                    
                    generate_text(game_state->text_texture,game_state->font,"Retry",COLOR_WHITE,220);
                    int miplevel=0;
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                }
                
            } else if (scene_state == FADE_TEXT) {
                UseShader(&game_state->colorShader);
                glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,1.0f);
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                if ((input->just_pressed[SDL_SCANCODE_RETURN] || game_state->dialogue_line==-1)) {
                    game_state->dialogue_line++;

                    char dialogue[9][128] = {
                        "And so you arrived in the town. If only you had the time to take both paths!",
                        "Ah yes, the more popular choice must be the greater.",
                        "But is this true? let us see.",
                        "And so you came to pass, and played some blackjack, and slept with some hookers.",
                        "The path more taken, the option more choesn, the option more used.",
                        "Money well spent, time well spent, apparently a choice well made.",
                        "But two weeks later...",
                        "At your yearly checkup....",
                        "A small little issue begged to differ..."
                    };
                    if (game_state->dialogue_line >= 9) {
                        scene_state = FADE_IN;
                        game_state->text_scroll_amount=0;
                    } else {
                        generate_text(game_state->text_texture,game_state->font,dialogue[game_state->dialogue_line],COLOR_WHITE,220);
                        int miplevel=0;
                        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                        //Mix_PlayChannel(0,game_state->dialogue_streams[game_state->dialogue_line],0);
                    }
                }
                UseShader(&game_state->textureShader);
                glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);        
                GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,20,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
            } else if (scene_state == FADE_IN || scene_state == SCENE_TEXT) {
                glBindTexture(GL_TEXTURE_2D,game_state->std_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

                if (scene_state == FADE_IN) {
                    float fade_timestep = game_state->text_scroll_amount - 3.0f;
                    float fade_time = 1.f;

                    UseShader(&game_state->colorShader);
                    glUniformMatrix4fv(game_state->colorShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
                    glUniform4f(game_state->colorShader.Uniform("color"),0.0f,0.0f,0.0f,MAX(0.0f,1.0f - fade_timestep/fade_time));
                    GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                    if (fade_timestep >= fade_time + 1.0f) {
                        scene_state = SCENE_TEXT;
                        game_state->dialogue_line=-1;
                    }
                } else {
                    if (input->just_pressed[SDL_SCANCODE_RETURN] || game_state->dialogue_line==-1) {
                        game_state->dialogue_line++;
                        char dialogue[6][128] = {
                            "And so it became clear, a joyous time, and a pleasant ride later",
                            "That the path more traveled was traveled more.",
                            "For the destination is wonderous, but the mark of each traveler remains clear",
                            "The choice went out the way it came, gone with the wind, leaving no trace except for the accute mark of gonorrhea",
                            "Proving once and for the all, the one more traveled and taken...",
                            "Is always worse than the virgin daughter your neighbor sold to your father for six yaks"
                        };
                        if (game_state->dialogue_line >= 6) {
                            scene_state = ENDING_FADE_OUT;
                            game_state->text_scroll_amount=0;
                        } else {
                            generate_text(game_state->text_texture,game_state->font,dialogue[game_state->dialogue_line],COLOR_WHITE,220);
                            int miplevel=0;
                            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &game_state->tex_dimensions.x);
                            glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &game_state->tex_dimensions.y);
                            //Mix_PlayChannel(0,game_state->dialogue_streams[game_state->dialogue_line],0);
                        }
                    } if (scene_state != ENDING_FADE_OUT) {
                        glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                        glUniform1i(game_state->textureShader.Uniform("_texture"),0);
                        GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},{10,20,game_state->tex_dimensions.x,game_state->tex_dimensions.y});
                    }
                }
            }
        } else if (game_state->travelled_state == GameState::BAD_ENDING || game_state->travelled_state == GameState::GOOD_ENDING) {

            glBindTexture(GL_TEXTURE_2D,game_state->endings_texture);
            glUniform1i(game_state->textureShader.Uniform("_texture"),0);
            iRect rect={0,0,240,180};
            if (game_state->travelled_state == GameState::GOOD_ENDING) {
                rect.x = 240;
            }
            GL_DrawTexture(rect,{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
            if (game_state->text_scroll_amount >= 2.0f) {

                bool hover = false;
                iRect button_rect = {NATIVE_GAME_WIDTH/2-(game_state->tex_dimensions.x/2),120,game_state->tex_dimensions.x,game_state->tex_dimensions.y};
                v2i mpos = GetMousePosition();
                mpos.x = (int)(((float)mpos.x) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
                mpos.y = (int)(((float)mpos.y) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
                
                if (rect_contains_point(button_rect,mpos)) {
                    hover = true;
                    if (input->mouse_just_pressed) {
                        game_state->travelled_state = GameState::CHOICE;
                        game_state->text_scroll_amount=0;
                        game_state->dialogue_line=-1;
                    }
                }
                glBindTexture(GL_TEXTURE_2D,game_state->text_texture);
                glUniform1i(game_state->textureShader.Uniform("_texture"),0);


                if (hover) {
                    glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,0.0f,1.0f);
                } else {
                    glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,1.0f,1.0f);
                }
                
                GL_DrawTexture({0,0,game_state->tex_dimensions.x,game_state->tex_dimensions.y},button_rect);
            }
        }
    }

    /*
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

    GL_DrawTexture({game_state->player.frame*16,0,16,16},{(int)game_state->player.pos.x,(int)game_state->player.pos.y,16,16});
    */



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
}

#if defined __cplusplus
extern "C"
#endif
extern GAME_HANDLE_EVENT(GameHandleEvent) {
}
#endif
