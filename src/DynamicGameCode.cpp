
#include "core/engine.cpp"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 540

#define NATIVE_GAME_WIDTH 240
#define NATIVE_GAME_HEIGHT 180

v2i GetMousePositionIngame() {
    v2i mpos = GetMousePosition();
    mpos.x = (int)(((float)mpos.x) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
    mpos.y = (int)(((float)mpos.y) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
    return mpos;
}



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
    GM_THIRD_WHEEL,
    GM_DRIVING,
    GM_OBESE,
    GM_POLICE_LINEUP,
    GM_COUNT,
    GM_UNSELECTED
};

struct Car {
    v2 pos;
    float speed=0.f;
    double angle_body=0.;
    double turn=0.0;
    //float angle_wheels=0.f;
};

struct LineupPerson {
    struct {
        u8 num;
        v2i eyepos_1;
        v2i eyepos_2;
        v2i nose_pos;
        v2i mouth_pos;
        v2i earpos_1;
        v2i earpos_2;
        v2i body_pos;
        v2i hair_pos;
    } face;

    struct {
        u8 num;
        v2i head_pos;
        v2i armpos_1;
        v2i armpos_2;
        v2i legpos_1;
        v2i legpos_2;
    } body;

    struct {
        u8 num;
        v2i bodypos_1;
        v2i bodypos_2;
    } legs;

    struct {
        u8 num;
        v2i bodypos_1;
        v2i bodypos_2;
    } arms;

    struct {
        u8 num;
        v2i face_pos;
    } hair;
    
    u8 eye;
    u8 ear;
    u8 mouth;

    Color eye_color;
    Color skin_color;
    Color shirt_color;
    Color pant_color;
    Color hair_color;

    bool suspect;
};



struct GameState {
    Shader colorShader;
    Shader textureShader;
    Shader bodyShader;

    GLuint main_menu_buttons_texture;
    
    GLuint player_texture;
    GLuint goblins_menu_texture;
    GLuint less_traveled_bg_texture;
    GLuint text_texture;
    GLuint diverge_texture;
    GLuint forest_texture;
    GLuint std_texture;
    GLuint endings_texture;

    GLuint obese_menu_texture;
    GLuint obese_controls_texture;
    GLuint obese_credits_texture;
    

    GLuint car_texture;
    GLuint course_texture;
    GLuint arrow_texture;
    Car player_car;

    GLuint police_lineup_texture;
    GLuint bodyparts_texture;
    GLuint notepad_texture;
    GLuint buttons_texture;
    GLuint lineup_arrow_texture;
    GLuint newspaper_texture;
    GLuint lineup_main_menu_bg_texture;
    GLuint lineup_crime_bar_texture;
    camera_t lineup_camera;
    i32 current_target=0;

    enum {
        MAIN_MENU,
        GAMEPLAY
    } overall_game_state = MAIN_MENU;
    
    enum {
        LL_MAIN_MENU,
        BRIEFING,
        CHOOSING,
        NEWSPAPER
    };
    i32 lineup_state;

    enum {
        LL_JAM,
        LL_MELONS,
        LL_BLOODY
    };
    i32 lineup_level=-1;

    bool lineup_choice_made=false;
    bool newspaper_fadeout=false;
    float fade_timer=0.f;
    float lineup_newspaper_timer=0.f;
    
    LineupPerson people[12];
    Mix_Chunk *lineup_dialogue_streams[6];
    Mix_Chunk *lineup_gotit;

    Mix_Chunk *lineup_incorrect_generic_streams[3];
    Mix_Chunk *lineup_incorrect_jam_streams[5];
    Mix_Chunk *lineup_incorrect_melon_streams[8];

    i32 lineup_dialogue_line=0;

    camera_t camera;    

    v2i tex_dimensions;
    i32 dialogue_line = -1;
    Mix_Chunk *dialogue;
    Mix_Music *travel_music;
    Mix_Chunk *obese_breathing;

    Mix_Chunk *dialogue_streams[13];

    TTF_Font *font;

    GameMode mode;

    enum {
        OB_MAIN_MENU,
        OB_CONTROLS,
        OB_CREDITS,
        OB_GAMEPLAY
    } obese_state=OB_MAIN_MENU;

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
TWO ROADS DIVERGED IN A WOOD, AND\n\
I TOOK THE ONE LESS TRAVELED BY,\n\
AND THAT HAS MADE ALL THE DIFFERENCE.";

v2i face_dimensions = {48,48};
v2i hair_dimensions = {64,64};
v2i eye_dimensions = {16,16};
v2i mouth_dimensions = {16,16};
v2i ear_dimensions = {16,16};
v2i body_dimensions = {48,48};
// full legs
v2i leg_dimensions = {64,48};
// full arms
v2i arm_dimensions = {128,64};

v2i first_person_pos = {73 + 160/2,140};


// Description
// He was definitely between 4 and 7 feet tall...
// He had a whimsical attitude and a clear thirst for knowledge
// Had large melons for hands

LineupPerson GeneratePerson(bool suspect=false) {
    LineupPerson person;
    
    Color eye_colors[6] = {Color(255,0,0,255),Color(255,210,0,255),Color(0,50,255,255),Color(45,20,0,255),Color(35,0,0,255),Color(50,255,80,255)};
    Color skin_colors[5] = {Color(240,190,190,255),Color(210,150,100,255),Color(200,140,90,255),Color(240,220,180,255),Color(50,10,0,255)};
    Color shirt_colors[13] = {Color(210,225,145,255),Color(165,235,120,255),Color(90,235,64,255),Color(44,84,30,255),Color(100,60,30,255),Color(110,60,120,255),Color(170,75,75,255),Color(140,30,0,255),Color(35,50,145,255),Color(70,150,35,255),Color(35,60,145,255),Color(90,200,190,255),Color(220,80,155,255)};
    Color pant_colors[14] = {Color(35,35,85,255),Color(60,80,140,255),Color(50,60,175,255),Color(15,80,165,255),Color(50,60,230,255),Color(0,85,185,255),Color(50,110,210,255),Color(40,65,40,255),Color(70,85,65,255),Color(60,100,180,255),Color(130,90,115,255),Color(130,70,120,255),Color(90,35,110,255),Color(100,20,45,255)};
    Color hair_colors[11] = {Color(75,30,10,255),Color(0,50,255,255),Color(45,20,0,255),Color(120,30,15,255),Color(250,230,130,255),Color(230,220,200,255),Color(60,35,10,255),Color(100,50,10,255),Color(125,70,0,255),Color(75,50,10,255),Color(170,180,210,255)};

    person.face.num = rand() % 10;
    person.body.num = rand() % 10;
    person.legs.num = rand() % 7;
    person.arms.num = suspect ? 8 : rand() % 8;
    person.hair.num = rand() % 10;
    person.eye = rand() % 8;
    person.mouth = rand() % 15;
    person.ear = rand() % 5;

    person.eye_color = eye_colors[rand()%6];
    person.skin_color = skin_colors[rand()%5];
    person.shirt_color = shirt_colors[rand() % 13];
    person.pant_color = pant_colors[rand() % 14];
    person.hair_color = hair_colors[rand() % 11];

    // generate face
    SDL_Surface *body_surface = IMG_Load("res/imgs/bodyparts.png");
    bool located_first_eye=false;
    bool located_first_ear=false;

    Color eye_color(255,0,0,255);
    Color ear_color(255,0,255,255);
    Color mouth_color(0,255,0,255);
    Color nose_color(0,0,255,255);
    Color body_color(150,0,255,255);
    Color skin_color(255,255,200,255);
    Color hair_color(255,255,0,255);
    
    for (i32 x=0; x<face_dimensions.x; x++) {
        for (i32 y=0; y<face_dimensions.y; y++) {
            Color col = {0,0,0,0};
            Uint32 data = getpixel(body_surface,person.face.num*face_dimensions.x + x,y);
            SDL_GetRGBA(data, body_surface->format, &col.r, &col.g, &col.b, &col.a);

            if (col == eye_color) {
                if (!located_first_eye) {
                    located_first_eye=true;
                    person.face.eyepos_1 = {x,y};
                } else {
                    if (x > person.face.eyepos_1.x) {
                        person.face.eyepos_2 = {x,y};
                    } else {
                        person.face.eyepos_2 = person.face.eyepos_1;
                        person.face.eyepos_1 = {x,y};
                    }
                }
            } else if (col == ear_color) {
                if (!located_first_ear) {
                    located_first_ear=true;
                    person.face.earpos_1 = {x,y};
                } else {
                    if (x > person.face.earpos_1.x) {
                        person.face.earpos_2 = {x,y};
                    } else {
                        person.face.earpos_2 = person.face.eyepos_1;
                        person.face.earpos_1 = {x,y};
                    }
                }
            } else if (col == mouth_color) {
                person.face.mouth_pos = {x,y};
            } else if (col == nose_color) {
                person.face.nose_pos = {x,y};
            } else if (col == body_color) {
                person.face.body_pos = {x,y};
            } else if (col == hair_color) {
                person.face.hair_pos = {x,y};
            } else if (col != skin_color && col != COLOR_BLACK && col != COLOR_TRANSPARENT) {
                printf("%u %u %u %u\n",col.r,col.g,col.b,col.a);
            }
        }
    }

    Color head_color = nose_color;
    Color arm_color = eye_color;
    Color leg_color = ear_color;

    bool located_first_arm = false;
    bool located_first_leg = false;

    for (i32 x=0; x<body_dimensions.x; x++) {
        for (i32 y=0; y<body_dimensions.y; y++) {
            Color col = {0,0,0,0};
            Uint32 data = getpixel(body_surface,person.body.num*body_dimensions.x + x,112+y);
            SDL_GetRGBA(data, body_surface->format, &col.r, &col.g, &col.b, &col.a);

            if (col == arm_color) {
                if (!located_first_arm) {
                    located_first_arm=true;
                    person.body.armpos_1 = {x,y};
                } else {
                    if (x > person.body.armpos_1.x) {
                        person.body.armpos_2 = {x,y};
                    } else {
                        person.body.armpos_2 = person.body.armpos_1;
                        person.body.armpos_1 = {x,y};
                    }
                }
            } else if (col == leg_color) {
                if (!located_first_leg) {
                    located_first_leg=true;
                    person.body.legpos_1 = {x,y};
                } else {
                    if (x > person.body.legpos_1.x) {
                        person.body.legpos_2 = {x,y};
                    } else {
                        person.body.legpos_2 = person.body.legpos_1;
                        person.body.legpos_1 = {x,y};
                    }
                }
            } else if (col == head_color) {
                person.body.head_pos = {x,y}; 
            } else if (col != skin_color && col != COLOR_BLACK && col != COLOR_TRANSPARENT) {
                printf("%u %u %u %u\n",col.r,col.g,col.b,col.a);
            }
        }
    }

    for (i32 x=0; x<leg_dimensions.x; x++) {
        for (i32 y=0; y<leg_dimensions.y; y++) {
            Color col = {0,0,0,0};
            Uint32 data = getpixel(body_surface,person.legs.num*leg_dimensions.x + x,160+y);
            SDL_GetRGBA(data, body_surface->format, &col.r, &col.g, &col.b, &col.a);

            if (col == leg_color) {
                if (x < leg_dimensions.x/2) {
                    person.legs.bodypos_1 = {x,y};
                } else {
                    person.legs.bodypos_2 = {x-(leg_dimensions.x/2),y};
                }
            } else if (col != skin_color && col != COLOR_BLACK && col != COLOR_TRANSPARENT) {
                printf("%u %u %u %u\n",col.r,col.g,col.b,col.a);
            }
        }
    }

    for (i32 x=0; x<arm_dimensions.x; x++) {
        for (i32 y=0; y<arm_dimensions.y; y++) {
            Color col = {0,0,0,0};
            Uint32 data = getpixel(body_surface,person.arms.num*128 + x,208+y);
            SDL_GetRGBA(data, body_surface->format, &col.r, &col.g, &col.b, &col.a);

            if (col == arm_color) {
                if (x < arm_dimensions.x/2) {
                    person.arms.bodypos_1 = {x,y};
                } else {
                    person.arms.bodypos_2 = {x-arm_dimensions.x/2,y};
                }
            } else if (col != skin_color && col != COLOR_BLACK && col != COLOR_TRANSPARENT) {
                printf("%u %u %u %u\n",col.r,col.g,col.b,col.a);
            }
        }
    }
    for (i32 x=0; x<hair_dimensions.x; x++) {
        for (i32 y=0; y<hair_dimensions.y; y++) {
            Color col = {0,0,0,0};
            Uint32 data = getpixel(body_surface,person.hair.num*hair_dimensions.x + x,272+y);
            SDL_GetRGBA(data, body_surface->format, &col.r, &col.g, &col.b, &col.a);

            if (col == hair_color) {
                person.hair.face_pos = {x,y};
            } else if (col != skin_color && col != COLOR_BLACK && col != COLOR_TRANSPARENT) {
                printf("%u %u %u %u\n",col.r,col.g,col.b,col.a);
            }
        }
    }

        
    SDL_FreeSurface(body_surface);

    person.suspect = suspect;
    
    return person;
}

void LineupLoadLevel(i32 n_level_num) {
    game_state->lineup_level = n_level_num;
    i32 suspect_num = 6;
    for (i32 i=0;i<12;i++) {
        if (i <= suspect_num+1) {
            game_state->people[i] = GeneratePerson(i == suspect_num);
        } else {
            // identical twins!!
            game_state->people[i] = game_state->people[i-1];
        }
    }
    game_state->lineup_camera.size = {NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT};
    game_state->lineup_camera.pos = {(float)first_person_pos.x,NATIVE_GAME_HEIGHT/2};
    game_state->current_target=0;
    game_state->lineup_choice_made=0;
}

void LineupIncorrectAudio() {
    i32 generic_choice = rand() % 4 == 0;
    Mix_Chunk *stream;
    if (generic_choice) {
        i32 choice = rand() % 3;
        stream = game_state->lineup_incorrect_generic_streams[choice];
    } else {
        if (game_state->lineup_level == GameState::LL_JAM) {
            stream = game_state->lineup_incorrect_jam_streams[rand() % 5];
        } else if (game_state->lineup_level == GameState::LL_MELONS) {
            stream = game_state->lineup_incorrect_melon_streams[rand() % 8];
        } else {
            return;
        }
    }
    
    Mix_PlayChannel(0,stream,0);
}


void InitializeGameMemory(GameMemory *memory) {
    *game_state = {};

    game_state->colorShader = CreateShader("color.vert","color.frag");
    game_state->textureShader = CreateShader("texture.vert","texture.frag");
    game_state->bodyShader = CreateShader("body.vert","body.frag");
    game_state->projection = glm::ortho(0.0f, static_cast<float>(NATIVE_GAME_WIDTH), static_cast<float>(NATIVE_GAME_HEIGHT), 0.0f, -1.0f, 1.0f);
    game_state->mode = GM_UNSELECTED;
    game_state->travelled_state = GameState::POEM_SCROLL;

    game_state->player = {};
    game_state->player_car.pos = {100,800};

    glGenTextures(1,&game_state->main_menu_buttons_texture);
    GL_load_texture(game_state->main_menu_buttons_texture,"res/imgs/main_menu_buttons.png");
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
    glGenTextures(1,&game_state->car_texture);
    GL_load_texture(game_state->car_texture,"res/imgs/car.png");
    glGenTextures(1,&game_state->course_texture);
    GL_load_texture(game_state->course_texture,"res/imgs/course.png");
    glGenTextures(1,&game_state->arrow_texture);
    GL_load_texture(game_state->arrow_texture,"res/imgs/arrow.png");
    glGenTextures(1,&game_state->police_lineup_texture);
    GL_load_texture(game_state->police_lineup_texture,"res/imgs/police_lineup.png");
    glGenTextures(1,&game_state->bodyparts_texture);
    GL_load_texture(game_state->bodyparts_texture,"res/imgs/bodyparts.png");
    glGenTextures(1,&game_state->notepad_texture);
    GL_load_texture(game_state->notepad_texture,"res/imgs/notepad.png");
    glGenTextures(1,&game_state->buttons_texture);
    GL_load_texture(game_state->buttons_texture,"res/imgs/buttons.png");
    glGenTextures(1,&game_state->lineup_arrow_texture);
    GL_load_texture(game_state->lineup_arrow_texture,"res/imgs/lineup_arrow.png");
    glGenTextures(1,&game_state->newspaper_texture);
    GL_load_texture(game_state->newspaper_texture,"res/imgs/newspaper.png");
    glGenTextures(1,&game_state->lineup_main_menu_bg_texture);
    GL_load_texture(game_state->lineup_main_menu_bg_texture,"res/imgs/police_backdrop.png");
    glGenTextures(1,&game_state->lineup_crime_bar_texture);
    GL_load_texture(game_state->lineup_crime_bar_texture,"res/imgs/crimebar.png");

    glGenTextures(1,&game_state->obese_menu_texture);
    GL_load_texture(game_state->obese_menu_texture,"res/imgs/obese_menu.png");

    glGenTextures(1,&game_state->obese_controls_texture);
    GL_load_texture(game_state->obese_controls_texture,"res/imgs/obese_controls.png");

    glGenTextures(1,&game_state->obese_credits_texture);
    GL_load_texture(game_state->obese_credits_texture,"res/imgs/obese_credits.png");
    

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
    game_state->obese_breathing = Mix_LoadWAV("res/sound/breathing.wav");

    for (i32 num=0;num<13;num++) {
        game_state->dialogue_streams[num] = Mix_LoadWAV(("res/sound/traveled_dialogue" + std::to_string(num+1) + ".mp3").c_str());
    }

    srand((u32)time(0));

    /*
    if (game_state->mode == GM_ROAD_LESS_TRAVELLED) {
        Mix_VolumeChunk(game_state->dialogue,128);
        Mix_PlayChannel(-1,game_state->dialogue,0);
        Mix_PlayMusic(game_state->travel_music,-1);
        Mix_VolumeMusic(18);
    } if (game_state->mode == GM_POLICE_LINEUP) {
        game_state->lineup_gotit = Mix_LoadWAV("res/sound/lineup_gotit.ogg");
        for (i32 num=0; num<6; num++) {
            game_state->lineup_dialogue_streams[num] = Mix_LoadWAV(("res/sound/suspect_" + std::to_string(num) + ".ogg").c_str());
        }

        for (i32 num=0; num<3; num++) {
            game_state->lineup_incorrect_generic_streams[num] = Mix_LoadWAV(("res/sound/generic_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }
        for (i32 num=0; num<5; num++) {
            game_state->lineup_incorrect_jam_streams[num] = Mix_LoadWAV(("res/sound/jam_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }
        for (i32 num=0; num<8; num++) {
            game_state->lineup_incorrect_melon_streams[num] = Mix_LoadWAV(("res/sound/melons_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }

        game_state->lineup_state = GameState::MAIN_MENU;
        game_state->lineup_level = -1;
    }
    */
    
    // load all textures
    memory->is_initialized = true;
    std::cout << "Initialized game memory\n";
}

void LoadGameMode(GameMode mode) {

    game_state->mode = mode;
    if (mode == GM_ROAD_LESS_TRAVELLED) {
        Mix_VolumeChunk(game_state->dialogue,128);
        Mix_PlayChannel(-1,game_state->dialogue,0);
        Mix_PlayMusic(game_state->travel_music,-1);
        Mix_VolumeMusic(18);
        
    } else if (mode == GM_POLICE_LINEUP) {
        game_state->lineup_gotit = Mix_LoadWAV("res/sound/lineup_gotit.ogg");
        for (i32 num=0; num<6; num++) {
            game_state->lineup_dialogue_streams[num] = Mix_LoadWAV(("res/sound/suspect_" + std::to_string(num) + ".ogg").c_str());
        }

        for (i32 num=0; num<3; num++) {
            game_state->lineup_incorrect_generic_streams[num] = Mix_LoadWAV(("res/sound/generic_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }
        for (i32 num=0; num<5; num++) {
            game_state->lineup_incorrect_jam_streams[num] = Mix_LoadWAV(("res/sound/jam_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }
        for (i32 num=0; num<8; num++) {
            game_state->lineup_incorrect_melon_streams[num] = Mix_LoadWAV(("res/sound/melons_hint_" + std::to_string(num+1) + ".ogg").c_str());
        }

        game_state->lineup_state = GameState::MAIN_MENU;
        game_state->lineup_level = -1;
    }
}


// Luck
// Strength
// Charisma
// Intelligence

global_variable iRect lineup_left_arrow_dest = {4,54,16,32};
global_variable iRect lineup_right_arrow_dest = {NATIVE_GAME_WIDTH-4-16,54,16,32};

global_variable iRect lineup_choice_button_rect = {NATIVE_GAME_WIDTH/2-64/2,144 + 4,64,20};
global_variable iRect lineup_main_menu_play_button_rect = {83,136,70,24};


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
    if (game_state->overall_game_state == GameState::GAMEPLAY){
        if (game_state->mode == GM_GOBLINS) {

            
        } else if (game_state->mode == GM_ROAD_LESS_TRAVELLED) {
            game_state->text_scroll_amount += timestep;

        } else if (game_state->mode == GM_POLICE_LINEUP) {
            if (game_state->lineup_state == GameState::MAIN_MENU) {
                v2i mpos = GetMousePositionIngame();
                if (rect_contains_point(lineup_main_menu_play_button_rect,mpos)) {
                    if (input->mouse_just_pressed) {
                        game_state->lineup_state = GameState::BRIEFING;
                        LineupLoadLevel(game_state->lineup_level+1);
                    }
                }
            } else if (game_state->lineup_state == GameState::BRIEFING) {
                if (input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed) {
                    game_state->lineup_dialogue_line++;
                    if (game_state->lineup_dialogue_line >= (game_state->lineup_level+1)*7) {
                        game_state->lineup_state = GameState::CHOOSING;
                    } else {
                        //Mix_PlayChannel(0,game_state->lineup_dialogue_streams[game_state->lineup_dialogue_line-1],0);
                    }
                }
            } else if (game_state->lineup_state == GameState::CHOOSING) {
                v2 &cam_pos = game_state->lineup_camera.pos;
                i32 absolute_pos = first_person_pos.x + game_state->current_target*160;
                if (game_state->lineup_choice_made) {
                    game_state->fade_timer+=timestep;
                    if (game_state->fade_timer >= 5.0f) {
                        game_state->lineup_state = GameState::NEWSPAPER;
                        game_state->lineup_newspaper_timer=0.f;
                        game_state->newspaper_fadeout=false;
                    }
                    goto render_begin;
                }

                if (cam_pos.x == absolute_pos) {
                    if (input->mouse_just_pressed) {
                        v2i mpos = GetMousePositionIngame();
                        if (rect_contains_point(lineup_left_arrow_dest,mpos)) {
                            game_state->current_target--;
                            if (game_state->current_target < 0) {
                                game_state->current_target = 0;
                            }
                        } else if (rect_contains_point(lineup_right_arrow_dest,mpos)) {
                            game_state->current_target++;
                            if (game_state->current_target >= 12) {
                                game_state->current_target = 11;
                            }
                        } else if (rect_contains_point(lineup_choice_button_rect,mpos)) {
                            if (game_state->people[game_state->current_target].suspect) {
                                game_state->lineup_choice_made = true;
                                game_state->fade_timer=0.f;
                                Mix_PlayChannel(0,game_state->lineup_gotit,0);
                            } else {
                                LineupIncorrectAudio();
                            }
                        }
                    }
                }

                if (cam_pos.x < absolute_pos) {
                    cam_pos.x += 3;
                } else if (cam_pos.x > absolute_pos) {
                    cam_pos.x -= 3;
                }
                if (abs(cam_pos.x - absolute_pos) < 3) {
                    cam_pos.x = (float)absolute_pos;
                }
            } else if (game_state->lineup_state == GameState::NEWSPAPER) {
                if (game_state->newspaper_fadeout) {
                    game_state->fade_timer += timestep;
                    if (game_state->fade_timer >= 3.f) {
                        game_state->lineup_state = GameState::BRIEFING;
                        LineupLoadLevel(game_state->lineup_level+1);
                    }
                } else {
                    if (input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed) {
                        game_state->fade_timer=0.f;
                        game_state->newspaper_fadeout=true;
                    }
                }
            }

        } else if (game_state->mode == GM_OBESE) {
            v2i mpos = GetMousePosition();
            mpos.x = (int)(((float)mpos.x) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
            mpos.y = (int)(((float)mpos.y) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));

            if (game_state->obese_state == GameState::MAIN_MENU) {
                iRect play_button = {97,109,50,21};
                iRect credits_button = {98,137,49,19};
                if (input->mouse_just_pressed) {
                    if (rect_contains_point(play_button,mpos)) {
                        game_state->obese_state = GameState::OB_CONTROLS;
                    } else if (rect_contains_point(credits_button,mpos)) {
                        game_state->obese_state = GameState::OB_CREDITS;
                    }
                }
            } else if (game_state->obese_state == GameState::OB_CONTROLS) {
                iRect play_button = {85,134,56,31};                
                if (input->mouse_just_pressed) {
                    if (rect_contains_point(play_button,mpos)) {
                        game_state->obese_state = GameState::OB_GAMEPLAY;
                    }
                }
            } else if (game_state->obese_state == GameState::OB_GAMEPLAY) {
                int is_playing = Mix_Playing(0);
                if (input->is_pressed[SDL_SCANCODE_W] || input->is_pressed[SDL_SCANCODE_S] ||
                    input->is_pressed[SDL_SCANCODE_D] || input->is_pressed[SDL_SCANCODE_A]) {
                    if (!is_playing) {
                        Mix_PlayChannel(0,game_state->obese_breathing,-1);
                    }
                } else {
                    if (is_playing) {
                        Mix_HaltChannel(0);
                    }
                }
            } else if (game_state->obese_state == GameState::OB_CREDITS) {
                iRect back_button = {93,142,54,29};
                if (input->mouse_just_pressed) {
                    if (rect_contains_point(back_button,mpos)) {
                        game_state->obese_state = GameState::OB_MAIN_MENU;
                    }
                }
            }

        } else if (game_state->mode == GM_DRIVING) {
            
            double turn_speed = PI * 2.0 * timestep;
            double turn_stop_speed = PI * 2.0 * timestep;
            double max_turn_angle = PI * (1.0/2.0);
            bool drifting = false;
            
            if (input->is_pressed[SDL_SCANCODE_D]) {
                game_state->player_car.turn += turn_speed;
            } else if (input->is_pressed[SDL_SCANCODE_A]) {
                game_state->player_car.turn -= turn_speed;
            } else {
                if (game_state->player_car.turn > 0.0) {
                    game_state->player_car.turn -= turn_stop_speed;
                    if (game_state->player_car.turn < 0.0) {
                        game_state->player_car.turn = 0.0;
                    }
                } else if (game_state->player_car.turn < 0.0) {
                    game_state->player_car.turn += turn_stop_speed;
                    if (game_state->player_car.turn > 0.0) {
                        game_state->player_car.turn = 0.0;
                    }
                }
            }

            game_state->player_car.turn = CLAMP(-max_turn_angle,max_turn_angle,game_state->player_car.turn);

            float acceleration = 110;
            float friction = 50;
            float max_speed = 230;

            if (input->is_pressed[SDL_SCANCODE_LSHIFT]) {
                drifting = true;
                friction = 0;
            }

            if (game_state->player_car.speed < max_speed/3) {
                //acceleration *= 1.65;
            }// if (abs(game_state->player_car.turn) >= max_turn_angle/3.0) {
            //friction *= 1+((abs(game_state->player_car.turn) - (max_turn_angle/3.0)) / (max_turn_angle-(max_turn_angle/3.0)));
            //acceleration *= 0.85;
            //}
            if (input->is_pressed[SDL_SCANCODE_W]) {
                game_state->player_car.speed += acceleration * timestep;
            }
            game_state->player_car.speed -= friction * timestep;

            game_state->player_car.speed = CLAMP(0,max_speed,game_state->player_car.speed);
            
            //printf("%f\n",game_state->player_car.turn);
            if (game_state->player_car.turn) {
                // get point along circle based on speed and turn
                double wheel_base = 10.0;
                
                double turning_radius = wheel_base / (sin(abs(game_state->player_car.turn/2.0)));
                double percentage_of_circumference = (game_state->player_car.speed*timestep) / (PI*turning_radius*turning_radius);
                v2 C;
                double angle_from_circle_to_car;
                if (game_state->player_car.turn > 0) {
                    angle_from_circle_to_car = game_state->player_car.angle_body-PI/2.0;
                    C = game_state->player_car.pos - (angle_to_vec((float)angle_from_circle_to_car)*(float)turning_radius);
                } else {
                    angle_from_circle_to_car = game_state->player_car.angle_body+PI/2.0;
                    C = game_state->player_car.pos - (angle_to_vec((float)angle_from_circle_to_car)*(float)turning_radius);
                }
                float angle_diff = (float)(percentage_of_circumference*PI*2.0);
                
                v2 new_point = C + (angle_to_vec(float(angle_from_circle_to_car+angle_diff))*(float)turning_radius);
                //game_state->player_car.pos = new_point;
                float new_angle = (float)angle_from_circle_to_car;
                if (game_state->player_car.turn > 0) {
                    new_angle += angle_diff;
                    new_angle += PIf/2.f;
                } else {
                    new_angle -= angle_diff;
                    new_angle -= PIf/2.f;
                }
                game_state->player_car.angle_body = new_angle;
            }
            
            game_state->player_car.pos += angle_to_vec((float)game_state->player_car.angle_body) * (game_state->player_car.speed * timestep);
            //printf("%f\n",game_state->player_car.angle_body);
            //game_state->player_car.pos -= delta;
        }
    }
render_begin:
    // <----------------------->
    //       RENDER BEGIN
    // <----------------------->
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (game_state->overall_game_state == GameState::MAIN_MENU) {
        UseShader(&game_state->colorShader);
        game_state->colorShader.UniformColor("color",{100,0,255,255});
        game_state->colorShader.UniformM4fv("projection",game_state->projection);
        game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));
        
        GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

        UseShader(&game_state->textureShader);
        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
        game_state->textureShader.UniformM4fv("projection",game_state->projection);
        game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));

        iRect less_travelled_src = {0,0,128,16};
        iRect obese_src = {0,24,144,24};
        iRect drive_src = {0,48,128,24};
        iRect lineup_src = {0,64,80,24};

        iRect less_travelled_rect = {24,16,128,16};
        iRect obese_rect = {24,48,128,24};
        iRect drive_rect = {24,80,128,24};
        iRect lineup_rect = {24,112,80,24};

        v2i mpos = GetMousePositionIngame();
        if (game_state->mode == GM_UNSELECTED) {
            if (rect_contains_point(less_travelled_rect,mpos)) {
                less_travelled_src.x += 144;
                if (input->mouse_just_pressed) {
                    game_state->mode = GM_ROAD_LESS_TRAVELLED;
                }
            } if (rect_contains_point(obese_rect,mpos)) {
                obese_src.x += 144;
                if (input->mouse_just_pressed) {
                    game_state->mode = GM_OBESE;
                }
            } if (rect_contains_point(drive_rect,mpos)) {
                drive_src.x += 144;
                if (input->mouse_just_pressed) {
                    game_state->mode = GM_DRIVING;
                }
            } if (rect_contains_point(lineup_rect,mpos)) {
                lineup_src.x += 144;
                if (input->mouse_just_pressed) {
                    game_state->mode = GM_POLICE_LINEUP;
                }
            }
            if (game_state->mode != GM_UNSELECTED) {
                game_state->fade_timer=0.f;
            }
        }
        
        game_state->textureShader.Uniform1i("_texture",game_state->main_menu_buttons_texture);
        GL_DrawTexture(less_travelled_src,less_travelled_rect);
        GL_DrawTexture(drive_src,drive_rect);
        GL_DrawTexture(obese_src,obese_rect);
        GL_DrawTexture(lineup_src,lineup_rect);

        if (game_state->mode != GM_UNSELECTED) {
            game_state->fade_timer += timestep;
            if (game_state->fade_timer > 4.0f) {
                game_state->overall_game_state = GameState::GAMEPLAY;
                LoadGameMode(game_state->mode);
            } else if (game_state->fade_timer > 1.f) {
                float fade_len = 2.f;
                float fade = MIN(1.f,(game_state->fade_timer-1.f)/fade_len);
                Color col = {0,0,0,(u8)(fade*255)};
                UseShader(&game_state->colorShader);
                game_state->colorShader.UniformColor("color",col);
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
            }
        }
        
        
        goto endof_frame;
    }
    
    if (game_state->mode == GM_GOBLINS) {
        UseShader(&game_state->textureShader);
        glBindTexture(GL_TEXTURE_2D,game_state->goblins_menu_texture);
        glUniform1i(game_state->textureShader.Uniform("_texture"),0);
        glUniform4f(game_state->textureShader.Uniform("colorMod"),1.0f,1.0f,1.0f,1.0f);
        glUniformMatrix4fv(game_state->textureShader.Uniform("projection"), 1, GL_FALSE, glm::value_ptr(game_state->projection));
        //glm::mat4 model = glm::mat4(1.0f);
        game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));
        //glUniformMatrix4fv(game_state->textureShader.Uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

        GL_DrawTexture({0,0,160,120},{0,0,160,120});
    } else if (game_state->mode == GM_OBESE) {
        UseShader(&game_state->colorShader);
        game_state->colorShader.UniformColor("color",COLOR_BLACK);
        game_state->colorShader.UniformM4fv("projection",game_state->projection);
        game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));

        UseShader(&game_state->textureShader);
        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
        game_state->textureShader.UniformM4fv("projection",game_state->projection);
        game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));
        
        if (game_state->obese_state == GameState::OB_MAIN_MENU) {
            game_state->bodyShader.Uniform1i("_texture",game_state->obese_menu_texture);
            GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        } else if (game_state->obese_state == GameState::OB_CONTROLS) {
            game_state->bodyShader.Uniform1i("_texture",game_state->obese_controls_texture);
            GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        } else if (game_state->obese_state == GameState::OB_CREDITS) {
            game_state->bodyShader.Uniform1i("_texture",game_state->obese_credits_texture);
            GL_DrawTexture({0,0,240,180},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        } else if (game_state->obese_state == GameState::OB_GAMEPLAY) {
            UseShader(&game_state->colorShader);
            GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        }
        
    } else if (game_state->mode == GM_POLICE_LINEUP) {
        UseShader(&game_state->textureShader);
        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
        game_state->textureShader.UniformM4fv("projection",game_state->projection);
        game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));
        v2i cam_pos = v2i(game_state->lineup_camera.get_offset());
        cam_pos.x *= -1;
        cam_pos.y *= -1;

        if (game_state->lineup_state == GameState::MAIN_MENU) {
            game_state->textureShader.Uniform1i("_texture",game_state->lineup_main_menu_bg_texture);
            GL_DrawTexture({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
            v2i mpos = GetMousePositionIngame();
            iRect button_rect = {70,24,70,24};
            if (rect_contains_point(lineup_main_menu_play_button_rect,mpos)) {
                button_rect.x = 70*2;
            }
            game_state->textureShader.Uniform1i("_texture",game_state->buttons_texture);
            GL_DrawTexture(button_rect,lineup_main_menu_play_button_rect);
            
        } else if (game_state->lineup_state == GameState::BRIEFING) {
            game_state->textureShader.Uniform1i("_texture",game_state->notepad_texture);
            GL_DrawTexture({game_state->lineup_dialogue_line*240,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT},{0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

        } else if (game_state->lineup_state == GameState::NEWSPAPER) {
            UseShader(&game_state->colorShader);
            game_state->colorShader.UniformColor("color",{37,37,37,255});
            game_state->colorShader.UniformM4fv("projection",game_state->projection);
            game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));
            GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
            
            UseShader(&game_state->textureShader);

            float &timer = game_state->lineup_newspaper_timer;
            timer += timestep;
            float timer_len = 1.f;
            timer = MIN(timer,timer_len);
            float rot_speed = PIf*4;
            float size_mult = timer/timer_len;
            v2i tex_size = {(i32)(NATIVE_GAME_WIDTH*size_mult),(i32)(NATIVE_GAME_HEIGHT*size_mult)};
            game_state->textureShader.Uniform1i("_texture",game_state->newspaper_texture);

            iRect draw_source = {(i32)game_state->lineup_level * NATIVE_GAME_WIDTH,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT};
            iRect draw_dest = {NATIVE_GAME_WIDTH/2 - tex_size.x/2,NATIVE_GAME_HEIGHT/2 - tex_size.y/2,tex_size.x,tex_size.y};
            game_state->textureShader.UniformM4fv("model",rotate_model_matrix((float)rot_speed*timer,draw_dest));
            GL_DrawTexture(draw_source,draw_dest);

            if (game_state->newspaper_fadeout) {
                float fade_len = 2.f;
                float fade = MIN(1.f,game_state->fade_timer / fade_len);
                Color col = {0,0,0,(u8)(fade*255)};

                UseShader(&game_state->colorShader);
                game_state->colorShader.UniformColor("color",col);
                GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
            }
        } else if (game_state->lineup_state == GameState::CHOOSING) {
            game_state->textureShader.Uniform1i("_texture",game_state->police_lineup_texture);
            {
                i32 dest = cam_pos.x;
                i32 target = -260;
                if (game_state->current_target == 11) {
                    dest += 8*160;
                } else {
                    while (dest < target) {
                        dest += 160;
                    }
                }
                GL_DrawTexture({0,0,774,180},{dest,0,774,180});
            }
            
            GL_PushOffset(cam_pos);
            UseShader(&game_state->bodyShader);
            game_state->bodyShader.UniformM4fv("projection",game_state->projection);
            game_state->bodyShader.UniformM4fv("model",glm::mat4(1.0f));
            game_state->bodyShader.Uniform1i("_texture",game_state->bodyparts_texture);

            i32 first_suspect = MAX(0,game_state->current_target-2);
            i32 last_suspect = MIN(11,game_state->current_target+2);
            i32 i=0;
            for (i32 target=first_suspect; target <= last_suspect; target++) {
                i++;
                LineupPerson &person = game_state->people[target];
                v2i pos = {first_person_pos.x + target*160,first_person_pos.y};

                if (person.suspect) {
                    //continue;
                    if (game_state->lineup_level == GameState::LL_JAM) {
                        UseShader(&game_state->textureShader);
                        GL_DrawTexture({0,336,64,80},{pos.x - 32, pos.y - 80,64,80});
                        UseShader(&game_state->bodyShader);
                        continue;
                    } else if (game_state->lineup_level == GameState::LL_BLOODY) {
                        UseShader(&game_state->textureShader);
                        GL_DrawTexture({64,336,128,112},{pos.x - 64, pos.y - 112,128,112});
                        UseShader(&game_state->bodyShader);
                        continue;
                        
                    }
                }
                game_state->bodyShader.UniformColor("eyeColor",person.eye_color);


                iRect body_rect = {person.body.num*48,112,48,48};
                v2i body_pos = {pos.x - body_dimensions.x/2,pos.y - body_dimensions.y + (body_dimensions.y-person.body.legpos_1.y) - (body_dimensions.y-person.legs.bodypos_1.y)};

                iRect left_leg_rect = {person.legs.num*64,160,leg_dimensions.x/2,leg_dimensions.y};
                v2i legpos_1_on_body = {body_pos.x + person.body.legpos_1.x, body_pos.y + person.body.legpos_1.y};
                v2i legpos_1 = {legpos_1_on_body.x - person.legs.bodypos_1.x,legpos_1_on_body.y - person.legs.bodypos_1.y};

                iRect right_leg_rect = {person.legs.num*leg_dimensions.x + leg_dimensions.x/2,160,leg_dimensions.x/2,leg_dimensions.y};
                v2i legpos_2_on_body = {body_pos.x + person.body.legpos_2.x, body_pos.y + person.body.legpos_2.y};
                v2i legpos_2 = {legpos_2_on_body.x - person.legs.bodypos_2.x,legpos_2_on_body.y - person.legs.bodypos_2.y};


                iRect left_arm_rect = {person.arms.num*arm_dimensions.x,208,64,64};
                v2i armpos_1_on_body = {body_pos.x + person.body.armpos_1.x, body_pos.y + person.body.armpos_1.y};
                v2i armpos_1 = {armpos_1_on_body.x - person.arms.bodypos_1.x,armpos_1_on_body.y - person.arms.bodypos_1.y};

                iRect right_arm_rect = {person.arms.num*arm_dimensions.x+arm_dimensions.x/2,208,64,64};
                v2i armpos_2_on_body = {body_pos.x + person.body.armpos_2.x, body_pos.y + person.body.armpos_2.y};
                v2i armpos_2 = {armpos_2_on_body.x - person.arms.bodypos_2.x,armpos_2_on_body.y - person.arms.bodypos_2.y};

                v2i head_pos_on_body = {body_pos.x+person.body.head_pos.x,body_pos.y+person.body.head_pos.y};
                // this head position is absolute in the game world, and originates in the top left of the head
                v2i head_pos = {head_pos_on_body.x - person.face.body_pos.x,head_pos_on_body.y-person.face.body_pos.y};
            
                iRect ear_rect = {person.ear*16,80,16,16};
                v2i earpos_1 = {head_pos.x+person.face.earpos_1.x,head_pos.y+person.face.earpos_1.y};
                v2i earpos_2 = {head_pos.x+person.face.earpos_2.x,head_pos.y+person.face.earpos_2.y};

                iRect face_rect = {person.face.num*face_dimensions.x,0,face_dimensions.x,face_dimensions.y};

                iRect eye_rect = {person.eye*16,48,16,16};
                v2i eyepos_1 = {head_pos.x+person.face.eyepos_1.x,head_pos.y+person.face.eyepos_1.y};
                v2i eyepos_2 = {head_pos.x+person.face.eyepos_2.x,head_pos.y+person.face.eyepos_2.y};

                iRect mouth_rect = {person.mouth*16,64,16,16};
                v2i mouth_pos = {head_pos.x+person.face.mouth_pos.x,head_pos.y+person.face.mouth_pos.y};

                iRect hair_rect = {person.hair.num * hair_dimensions.x,272,hair_dimensions.x,hair_dimensions.y};
                v2i hair_pos_on_face = {head_pos.x + person.face.hair_pos.x,head_pos.y + person.face.hair_pos.y};
                v2i hair_pos = {hair_pos_on_face.x-person.hair.face_pos.x,hair_pos_on_face.y - person.hair.face_pos.y};

                game_state->bodyShader.UniformColor("skinColor",person.hair_color);
                GL_DrawTexture(hair_rect,{hair_pos.x,hair_pos.y,hair_dimensions.x,hair_dimensions.y});
                game_state->bodyShader.UniformColor("skinColor",person.skin_color);
                GL_DrawTexture(left_arm_rect,{armpos_1.x,armpos_1.y,arm_dimensions.x/2,arm_dimensions.y});
                GL_DrawTexture(right_arm_rect,{armpos_2.x,armpos_2.y,arm_dimensions.x/2,arm_dimensions.y});

                game_state->bodyShader.UniformColor("skinColor",person.pant_color);
                GL_DrawTexture(left_leg_rect,{legpos_1.x,legpos_1.y,leg_dimensions.x/2,leg_dimensions.y});
                GL_DrawTexture(right_leg_rect,{legpos_2.x,legpos_2.y,leg_dimensions.x/2,leg_dimensions.y});

                game_state->bodyShader.UniformColor("skinColor",person.shirt_color);
                GL_DrawTexture(body_rect,{body_pos.x,body_pos.y,body_dimensions.x,body_dimensions.y});
                game_state->bodyShader.UniformColor("skinColor",person.skin_color);
                GL_DrawTexture(ear_rect,{earpos_1.x-8,earpos_1.y-8,16,16});

                GL_DrawTexture(ear_rect,{earpos_2.x-8,earpos_2.y-8,16,16},true);
                GL_DrawTexture(face_rect,{head_pos.x,head_pos.y,face_dimensions.x,face_dimensions.y});
                GL_DrawTexture(eye_rect,{eyepos_1.x-8,eyepos_1.y-8,16,16});
                GL_DrawTexture(eye_rect,{eyepos_2.x-8,eyepos_2.y-8,16,16},true);
                GL_DrawTexture(mouth_rect,{mouth_pos.x-8,mouth_pos.y-8,16,16},true);
            }
            GL_PopOffset();
            i32 absolute_pos = first_person_pos.x + game_state->current_target*160;
            iRect choice_rect = {0,0,70,24};

            iRect left_arrow_rect = {0,0,16,32};
            iRect right_arrow_rect = {0,0,16,32};

            if (game_state->lineup_camera.pos.x == absolute_pos) {
                v2i mpos = GetMousePosition();
                mpos.x = (int)(((float)mpos.x) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));
                mpos.y = (int)(((float)mpos.y) / ((float)WINDOW_WIDTH/(float)NATIVE_GAME_WIDTH));

                left_arrow_rect.x = 16*1;
                right_arrow_rect.x = 16*1;

                if (rect_contains_point(lineup_choice_button_rect,mpos)) {
                    choice_rect.x = 70*2;
                } else {
                    choice_rect.x = 70*1;
                } if (rect_contains_point(lineup_left_arrow_dest,mpos)) {
                    left_arrow_rect.x = 16*2;
                } else {
                    left_arrow_rect.x = 16*1;
                } if (rect_contains_point(lineup_right_arrow_dest,mpos)) {
                    right_arrow_rect.x = 16*2;
                } else {
                    right_arrow_rect.x = 16*1;
                }
            }
            UseShader(&game_state->textureShader);
            game_state->textureShader.Uniform1i("_texture",game_state->buttons_texture);
            GL_DrawTexture(choice_rect,lineup_choice_button_rect);
            game_state->textureShader.Uniform1i("_texture",game_state->lineup_arrow_texture);
            GL_DrawTexture(left_arrow_rect,lineup_left_arrow_dest);
            GL_DrawTexture(right_arrow_rect,lineup_right_arrow_dest,true);

            game_state->textureShader.Uniform1i("_texture",game_state->lineup_crime_bar_texture);
            GL_DrawTexture({game_state->lineup_level * 216,0,216,24},{12,0,216,24});
            

            if (game_state->lineup_choice_made) {
                float fade_timer = game_state->fade_timer;
                float fade_len = 2.0f;
                if (fade_timer > 1.0f) {
                    float fade = MIN(1.0f,(fade_timer - 1.0f) / fade_len);
                    UseShader(&game_state->colorShader);
                    u8 col = (u8)(fade * 255);
                    game_state->colorShader.UniformColor("color",Color(0,0,0,col));
                    game_state->colorShader.UniformM4fv("projection",game_state->projection);
                    game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));
                    GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
                }
            }
        }

    } else if (game_state->mode == GM_DRIVING) {
        game_state->camera.size = {NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT};
        game_state->camera.pos = game_state->player_car.pos;
        v2i cam_pos = v2i(game_state->camera.get_offset());
        cam_pos.x *= -1;
        cam_pos.y *= -1;
        
        UseShader(&game_state->textureShader);
        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
        game_state->textureShader.UniformM4fv("projection",game_state->projection);
        game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));

        game_state->textureShader.Uniform1i("_texture",game_state->course_texture);
        GL_DrawTexture({0,0,960,720},{cam_pos.x,cam_pos.y,960,720});
        game_state->textureShader.Uniform1i("_texture",game_state->car_texture);
        iRect p_draw_rect = {(int)game_state->player_car.pos.x,(int)game_state->player_car.pos.y,32,16};
        p_draw_rect.x += cam_pos.x;
        p_draw_rect.y += cam_pos.y;

        game_state->textureShader.UniformM4fv("model",rotate_model_matrix((float)game_state->player_car.angle_body,p_draw_rect,{10,8}));

        GL_DrawTexture({0,0,32,16},p_draw_rect);
        /*
          game_state->textureShader.Uniform1i("_texture",game_state->arrow_texture);
          iRect arrow_draw_rect={cam_pos.x+(i32)game_state->player_car.pos.x,cam_pos.y+(i32)game_state->player_car.pos.y,16,32};
          game_state->textureShader.UniformM4fv("model",rotate_model_matrix(game_state->player_car.angle_body+game_state->player_car.turn+PI/2,arrow_draw_rect,{8,32}));
          GL_DrawTexture({0,0,16,32},arrow_draw_rect);
        */
        
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
            int text_y_dest = (int)(NATIVE_GAME_HEIGHT + scroll_speed*2 - int(scroll_amount * scroll_speed));
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

            if (input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed || game_state->dialogue_line==-1) {
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
                if ((input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed || game_state->dialogue_line==-1)) {
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
                    if (input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed || game_state->dialogue_line==-1) {
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
                if ((input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed || game_state->dialogue_line==-1)) {
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
                    if (input->just_pressed[SDL_SCANCODE_RETURN] || input->mouse_just_pressed || game_state->dialogue_line==-1) {
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

endof_frame:

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
