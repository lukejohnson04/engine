#pragma once
/*
struct Entity;
internal_function void EntityUpdateStub(Entity *entity, float timestep);

#define EPROPS(entity, name) name &props = *(name *)entity->props

*/


internal_function void EntityUpdateStub(Entity *entity, float timestep)
{
    entity->transform.pos.x += entity->vel.x;
    entity->transform.pos.y += entity->vel.y;
    entity->lifetime += timestep;
}

internal_function void RenderEntity(Entity *e)
{
    if ((e->flags & Entity::VISIBLE) == 0)
    {
        return;
    }
    if (e->sprite.texture != nullptr)
    {
        PushTransform(e->transform);
        DrawSprite(&e->sprite);
        PopTransform();
        // IntRect frame = e->sprite.get_frame();
        // IntRect frame = {0,0,e->image_res->meta.width,e->image_res->meta.height};
        // if (e->anim_player.current != nullptr) {
        //     frame = e->anim_player.current->frames[e->anim_player.current->frame_num].rect;
        // } else if (e->anim_player.frame.w != 0 && e->anim_player.frame.h != 0) {
        //     frame = e->anim_player.frame;
        // }
        // DrawTexture(e->sprite.texture->texture, frame, e->transform);
    }

    if (e->collider.w != 0 && e->collider.h != 0)
    {
        SDL_SetRenderDrawColor(renderer->sdl_renderer, 0, 0, 255, 100);
        IntRect rect = {(int)e->collider.x, (int)e->collider.y, (int)e->collider.w, (int)e->collider.h};
        rect.x += e->transform.pos.x;
        rect.y += e->transform.pos.y;
        if (e->type == Entity::FIGHT_FIST_ATTACK)
        {
            vec2 hitbox_origin = {0, 40};
            vec2 h_pos = hitbox_origin.rotate(e->sprite.transform.rotation);
            rect.x += h_pos.x * e->sprite.transform.scale.x;
            rect.y += h_pos.y * e->sprite.transform.scale.y;
            // } else {
        }
        //DrawRect(rect, e->transform);
    }
    // SDL_SetRenderDrawColor(Renderer::Get(), 255,0,0,100);
    // IntRect point_rect = {transform.pos.x-8,transform.pos.y-8,16,16};
    // SDL_RenderDrawRect(Renderer::Get(), (SDL_Rect*)&point_rect);
}

void Entity::SetPosition(vec2 pos)
{
    int diff_x = pos.x - transform.pos.x;
    int diff_y = pos.y - transform.pos.y;
    transform.pos.x += diff_x;
    transform.pos.y += diff_y;
    // collider.x += diff_x;
    // collider.y += diff_y;
}


struct FightCharaProps {        
    enum {
        ATTACK_LOADING,
        ATTACK_LOADED,
        ATTACK_NONE
    } attack_state = ATTACK_NONE;
    float attack_load_timer=0.f;
    float damage_timer=0.f;
    u8 health=5;
    bool damaged=false;
    text_object health_text;
};


struct FistProps {
    bool hypnotized=false;
    bool hypnotize_activated=false;
    enum {
        ST_NONE,
        ST_HYPNOTIZED,
        ST_AIM_AT_BOSS,
        ST_SHOOT_AT_BOSS,
        ST_RETURN
    };
    u8 state = ST_NONE;
    float timer=0.f;
};


struct LeafProps {
    int dir = 1;
};


struct ArtTeacherProps {
    float timer=0;
    int walk_num=0;

    enum {
        None,
        Walk,
        Stop
    } state;
};

struct BoyProps {
    enum {
        None,
        LongWalk,
        Stop,
        ContinueWalk
    };
    u8 state;
    float timer;
};

struct MagProps {

};


internal_function
void UpdateHealthText(text_object *text, u32 num) {
    char char_text[] = "HP: ";
    char_text[3] = num+'0';
    GenerateTextResource(&text->texture,char_text,{255,255,60,255},game_state->dialogue_state.font);
}

internal_function
void FightCharaTakeDamage(Entity *fightChara) {
    EPROPS(fightChara,FightCharaProps);
    if (props.damaged) {
        // invulnerable
        return;
    }
    props.damaged = true;
    props.damage_timer = 0.0f;
    props.health--;
    UpdateHealthText(&props.health_text,props.health);
}


void Entity::Create(Entity::Type n_type) {
    type = n_type;
    memset(props,0,256*sizeof(u8));
    switch(type) {
        case Entity::BOY:
        {
            sprite.texture = GetImgResource("../res/imgs/boy.png");
            transform.scale = {4,4};
            transform.origin = {16,16};

            EPROPS(this,BoyProps);
            props = {};
            props.timer = 0;
            props.state = BoyProps::LongWalk;

        }
            break;
        case Entity::PLAYER:
            sprite.texture = GetImgResource("../res/imgs/charatest.png");
    
            transform.scale = {4,4};
            sprite.transform.origin = {8,16};
            collider = {0,-4*4,16*4,20*4};
    
            // collider.x *= 4;
            // collider.y *= 4;
            // collider.w *= player->transform.scale.x;
            // collider.h *= player->transform.scale.y;
            // collider = collider;

            // player->anim_player.frame = {0,0,32,32};

            sprite.h_frames = 4;
            sprite.v_frames = 2;
            sprite.frame = 0;

            // Animation right = Create(IntRect(0,32,32,32),4,0.135f,true);
            // Animation left = Create(IntRect(0,0,32,32),4,0.135f,true);

            // player->anim_player.animations.push_back(right);
            // player->anim_player.animations.push_back(left);
            // Update = &UpdatePlayer;

            break;
        case Entity::FIGHT_LOVE_WAVE_ATTACK:
        {
            sprite.texture = GetImgResource("../res/imgs/wave.png");
            sprite.transform.origin = {32, 32};
            collider = {-48,-48,96,96};

            // Update = &UpdateWaveAttack;
        }
            break;
        case Entity::FIGHT_FIST_ATTACK:
        {
            sprite.texture = GetImgResource("../res/imgs/fist.png");
            sprite.transform.scale = {0,0};
            sprite.transform.origin = {24, 16};
            collider = {-32,-32,64,64};
            //Update = &UpdateFist;
            EPROPS(this,FistProps);
            props = {};
        }
            break;

        case Entity::FIGHT_CHARA:
        {
            sprite.texture = GetImgResource("../res/imgs/fightchara.png");
            sprite.transform.scale = {2,2};
            sprite.transform.origin = {16,16};
            collider = {-16,-16,32,32};
            sprite.h_frames = 2;

            EPROPS(this,FightCharaProps);
            props = {};

            UpdateHealthText(&props.health_text,props.health);
         
    
            // Update = &UpdateFightChara;
        }
            break;
        case FIGHT_SCENE_EYEBALL:
            break;
        case Entity::XFATHER_FIGHT_SCENE_LEAF:
        {
            sprite.texture = GetImgResource("../res/imgs/leaf.png");
            sprite.transform.scale = {1,1};
            sprite.transform.origin = {32, 16};
            //Update = &UpdateLeaf;
            EPROPS(this,LeafProps);
            props = {};
            vel.y = 2.5f;
            collider = {-24,-8,48,16};
        }
            break;

        case Entity::XFATHER_FIGHT_SCENE_MAG:
        {
            sprite.texture = GetImgResource("../res/imgs/mag.png");
            sprite.transform.scale = {3,3};
            sprite.transform.origin = {24,24};
            sprite.h_frames = 3;
            
            collider = {-32,-32,64,64};
        }
            break;

        case Entity::ART_TEACHER:
        {
            sprite.texture = GetImgResource("../res/imgs/artteacher.png");
            transform.scale = {4,4};
            transform.origin = {32,32};
    
            sprite.h_frames = 2;
            transform.flip = true;
    
            //Update = &UpdateArtTeacher;
            // this is
            EPROPS(this,ArtTeacherProps);
            //ArtTeacherProps &props = *(ArtTeacherProps*)props;
            props.timer=0;
            props.state = ArtTeacherProps::Walk;

        }
            break;

    }
}

void Entity::MegaUpdate(float timestep) {
    Entity &fightChara = game_state->entity_pool[game_state->fightChara];
    switch(type) {
        case Entity::GENERIC:
            break;
        case Entity::PLAYER:
        {
            int speed_normal = 8;
            int speed_debug = 45;
            local_persist int speed = speed_normal;
            if (input->just_pressed[SDL_SCANCODE_V]) {
                if (speed == speed_debug) {
                    speed = speed_normal;
                } else {
                    speed = speed_debug;
                }
            }

            if (input->is_pressed[SDL_SCANCODE_D]) {
                vel.x = speed;
            } else if (input->is_pressed[SDL_SCANCODE_A]) {
                vel.x = -speed;
            } else {
                if (vel.x < 0) {
                    anim_player.frame = 0;
                } else if (vel.x > 0) {
                    anim_player.frame = 4;
                }
                vel.x = 0;
            }
    
            // const int gravity = 50;
            if (input->is_pressed[SDL_SCANCODE_W]) {
                vel.y = -speed;
            } else if (input->is_pressed[SDL_SCANCODE_S]) {
                vel.y = speed;
            } else {
                vel.y = 0;
            }

            transform.pos.x += vel.x;
            // player->collider.x += player->vel.x;
            FloatRect test_col = collider;
            test_col.x += transform.pos.x;
            test_col.y += transform.pos.y;

            for (entity_id e_id=0; e_id<game_state->entity_count; e_id++) {
                if (e_id == id) {
                    continue;
                }

                Entity *e = &game_state->entity_pool[e_id];
                if (e->collider.w == 0 || e->collider.h == 0) {
                    continue;
                }
                if (AABBIsColliding(test_col, e->collider)) {
                    if (!(e->collider_layers & LAYER_PHYSICS)) {
                        if (e->collider_on_contact && e->collider_trigger_count) {
                            e->collider_trigger_count--;
                            RunCommand(e->onInteract);
                            break;
                        }
                        continue;
                    }
                    // collision
                    const float prev_pos = test_col.x;
                    if (vel.x > 0) {
                        test_col.x = e->collider.x - test_col.w;
                    } else {
                        test_col.x = e->collider.x + e->collider.w;
                    }
                    transform.pos.x += (test_col.x - prev_pos);
                    break;
                }
            }

            transform.pos.y += vel.y;
            test_col = collider;
            test_col.x += transform.pos.x;
            test_col.y += transform.pos.y;

            for (entity_id e_id=0; e_id<game_state->entity_count; e_id++) {

                if (e_id == id) {
                    continue;
                }

                Entity *e = &game_state->entity_pool[e_id];
                if (e->collider.w == 0 || e->collider.h == 0) {
                    continue;
                }

                if (AABBIsColliding(test_col, e->collider)) {
                    if (!(e->collider_layers & LAYER_PHYSICS)) {
                        if (e->collider_on_contact && e->collider_trigger_count) {
                            e->collider_trigger_count--;
                            RunCommand(e->onInteract);
                            break;
                        }

                        continue;
                    }
                    // collision
                    const float prev_pos = test_col.y;
                    if (vel.y > 0) {
                        test_col.y = e->collider.y - test_col.h;
                    } else {
                        test_col.y = e->collider.y + e->collider.h;
                    }
                    transform.pos.y += (test_col.y - prev_pos);
                    vel.y = 0;
                    break;
                }
            }

            if (!anim_player.playing) {
                if (vel.x > 0) {
                    AnimationPlayerPlay(&anim_player, &sprite, ANIMATIONS::PLAYER_WALK_RIGHT);
                } else if (vel.x < 0) {
                    AnimationPlayerPlay(&anim_player, &sprite, ANIMATIONS::PLAYER_WALK_LEFT);
                }
            } else if (vel.x == 0) {
                AnimationPlayerStop(&anim_player);
            }

            AnimationPlayerUpdate(&anim_player, &sprite, int(timestep * 1000));

            Transform &camera = game_state->scene.camera;
            camera.pos.x = transform.pos.x - 1280/2;
            camera.pos.y = transform.pos.y - 720/2;
            // clamp camera bounds
            if (camera.pos.x < 0) {
                camera.pos.x = 0;
            } else if (camera.pos.x > game_state->scene.dimensions.x*4 - 1280) {
                camera.pos.x = game_state->scene.dimensions.x*4 - 1280;
            } if (camera.pos.y < 0) {
                camera.pos.y = 0;
            } else if (camera.pos.y > game_state->scene.dimensions.y*4 - 720) {
                camera.pos.y = game_state->scene.dimensions.y*4 - 720;
            }
        }
    
            break;
        case Entity::FIGHT_CHARA:
        {
            EPROPS(this,FightCharaProps);

            int speed = 5;
            if (input->is_pressed[SDL_SCANCODE_D]) {
                vel.x = speed;
            } else if (input->is_pressed[SDL_SCANCODE_A]) {
                vel.x = -speed;
            } else {
                vel.x = 0;
            }
    
            if (input->is_pressed[SDL_SCANCODE_W]) {
                vel.y = -speed;
            } else if (input->is_pressed[SDL_SCANCODE_S]) {
                vel.y = speed;
            } else {
                vel.y = 0;
            }

            transform.pos.x += vel.x;
            transform.pos.y += vel.y;

            if (props.attack_state == FightCharaProps::ATTACK_LOADING) {
                props.attack_load_timer += timestep;
                if (props.attack_load_timer >= 1.5f) {
                    props.attack_state = FightCharaProps::ATTACK_LOADED;
                }
            } if (props.attack_state == FightCharaProps::ATTACK_LOADED) {
                sprite.frame = 1;
            } else {
                sprite.frame = 0;
            }

            if (input->just_pressed[SDL_SCANCODE_LCTRL]) {
                if (props.attack_state == FightCharaProps::ATTACK_NONE) {
                    props.attack_state = FightCharaProps::ATTACK_LOADING;
                    props.attack_load_timer = 0.f;
                } else if (props.attack_state == FightCharaProps::ATTACK_LOADED) {
                    Entity *wave = create_entity();
                    float rot = sprite.transform.rotation;
                    wave->Create(Entity::FIGHT_LOVE_WAVE_ATTACK);
                    wave->vel = angle_to_vec(rot-PI/2).mult(-6.f);
                    wave->sprite.transform.rotation = rot - PI*2;
                    wave->SetPosition(transform.pos);

                    // LoadWaveAttack(wave,transform.pos,rot-PI/2);
                    props.attack_state = FightCharaProps::ATTACK_NONE;
                    props.attack_load_timer = 0.f;
                }
            }

            const float rot_speed = PI*1.25f;
            if (input->is_pressed[SDL_SCANCODE_LEFT]) {
                sprite.transform.rotation -= rot_speed*timestep;
            } if (input->is_pressed[SDL_SCANCODE_RIGHT]) {
                sprite.transform.rotation += rot_speed*timestep;
            }

            if (sprite.transform.rotation >= PI*2) {
                sprite.transform.rotation -= PI*2;
            } else if (sprite.transform.rotation < 0) {
                sprite.transform.rotation += PI*2;
            }

            if (props.damaged) {
                props.damage_timer += timestep;
                // times you complete a full wave per second
                float freq=2.5f;
                if (props.damage_timer >= 5.f/freq) {
                    props.damage_timer = 0.f;
                    props.damaged = false;
                    sprite.color = {255,255,255,255};
                } else {

                    double wave = (sin((props.damage_timer*freq*PI*2)-(PI/2)) + 1.0) / 2.0;
                    double amplitude=255.0;
                    u8 val = 255-(wave*amplitude);

                    double test = sin(-PI+props.damage_timer);

                    sprite.color.g = val;
                    sprite.color.b = 170;
                }
            } else {
                sprite.color = {255,255,255,255};
            }


            AnimationPlayerUpdate(&anim_player,&sprite,int(timestep*1000));
        }
            break;
        case Entity::ART_TEACHER:
        {
            EPROPS(this,ArtTeacherProps);
            props.timer += timestep;

            if (props.state == ArtTeacherProps::Walk && !game_state->dialogue_state.isPlaying) {
                vel.x = -4;
                if (props.walk_num == 0) {
                    if (transform.pos.x <= 300*4) {
                        props.state = ArtTeacherProps::Stop;
                        props.timer = 0;
                    }
                } else if (props.walk_num == 1) {
                    if (transform.pos.x <= 225*4) {
                        props.state = ArtTeacherProps::Stop;
                        props.timer = 0;
                    }
                } else if (props.walk_num == 2) {
                    if (transform.pos.x <= 150*4) {
                        props.state = ArtTeacherProps::Stop;
                        props.timer = 0;
                    }
                } else if (props.walk_num == 3) {
                    if (transform.pos.x <= 75*4) {
                        props.state = ArtTeacherProps::Stop;
                        props.timer = 0;
                    }
                }
            } if (props.state == ArtTeacherProps::Stop) {
                vel.x = 0;
                if (props.timer > 1.5) {
                    props.timer = 0;
                    props.state = ArtTeacherProps::Walk;
                    QueueCommand(CMD_EVENT, "artteacher_comment");
                    // if (walk_num == 0) {
                    //     Script::QueueCommand(CMD_SAY, "Oh my! Well done!");
                    //     Script::QueueCommand(CMD_SAY, "I love the use of red in this one. It really pops!");
                    // } else if (walk_num == 1) {
                    //     Script::QueueCommand(CMD_SAY, "Great imagery! Feels like my happy place. Man, I wish I could be there!");
                    // } else if (walk_num == 2) {
                    //     Script::QueueCommand(CMD_SAY, "A happy smile for a happy student!");
                    //     Script::QueueCommand(CMD_SAY, "A painting a day makes you feel extra great! Good work!");
                    // } else if (walk_num == 3) {
                    //     Script::QueueCommand(CMD_SAY, "...");
                    //     Script::QueueCommand(CMD_SAY, "Oh my. Oh me oh my!");
                    //     Script::QueueCommand(CMD_SAY, "This is the worst painting I've ever seen.");
                    //     Script::QueueCommand(CMD_SAY, "What the fuck is this supposed to be? This is embarrassing.");
                    //     Script::QueueCommand(CMD_EVENT, "leave_artroom");
                    // }
                    props.walk_num++;
                    ScriptRunQueue(game_state->script_state);
                }
            }

            transform.pos.x += vel.x;
            transform.pos.y += vel.y;

            AnimationPlayerUpdate(&anim_player, &sprite, int(timestep * 1000));

        }
            break;
        case Entity::FIGHT_LOVE_WAVE_ATTACK:
            lifetime += timestep;
    
            transform.pos.x += vel.x;
            transform.pos.y += vel.y;
            transform.scale.x = 1 + lifetime*1.5f;
            transform.scale.y = 1 + lifetime*1.5f;
    
            if (lifetime >= 3.5f) {
                queue_free_entity(id);
            }
            break;
        case Entity::FIGHT_FIST_ATTACK:
        {
            Entity *fightChara = &game_state->entity_pool[game_state->fightChara];
            lifetime += timestep;
            float first_phase_len=1.0;
    
            EPROPS(this,FistProps);
            if (props.state == FistProps::ST_AIM_AT_BOSS) {
                props.timer += timestep;
                float target_rot = angle_to(sprite.transform.pos, {640,100});
                float rot = lerp(sprite.transform.rotation, target_rot, 0.0275f);
                sprite.transform.rotation = rot;
                vel = {0,0};
            } else if (props.state == FistProps::ST_SHOOT_AT_BOSS) {
                vel = angle_to_vec(sprite.transform.rotation - PI/2.f).mult(12);
                if (distance_between(transform.pos,{1280/2,720/2}) >= 1000.f) {
                    queue_free_entity(id);
                }
            } else {
                if (props.hypnotized) {
                    vel.x = lerp(vel.x, 0.f, 0.125f);
                    vel.y = lerp(vel.y, 0.f, 0.125f);
                } else {
                    if (lifetime <= 0.5+first_phase_len) {
                        if (lifetime <= first_phase_len) {
                            float scale_mult = sin(((lifetime/first_phase_len) * PI) / 2.f)*3.f;
                            sprite.transform.scale = {scale_mult,scale_mult};
                            sprite.color.a = u8(255.f*(lifetime/first_phase_len));
                        } else {
                            sprite.transform.scale = {3,3};
                            sprite.color.a = 255;
                        }

                        float rad_to_player = angle_to(transform.pos, fightChara->transform.pos);
                        sprite.transform.rotation = rad_to_player + PI/2.f;
                        vel = angle_to_vec(rad_to_player).mult(-2);
                    } else if (lifetime <= 0.75+first_phase_len) {
                        vel = angle_to_vec(sprite.transform.rotation - PI/2.f).mult(12);
                    } else if (lifetime >= 1.30+first_phase_len) {
                        float speed = vel.length();
                        if (speed) {
                            // slow down
                            speed -= 0.125;
                            if (speed <= 0) {
                                vel = {0,0};
                            } else {
                                vel = vel.normalize().mult(speed);
                            }
                        }
            
                        if (lifetime >= 2.0+first_phase_len) {
                            queue_free_entity(id);
                        }
                    }
                    if (lifetime >= first_phase_len) {
                        Entity *fightChara = get_entity(game_state->fightChara);
                        
                        
                        if (EntitiesAreColliding(fightChara,this)) {
                            FightCharaTakeDamage(fightChara);
                        }
                    }
        
                    vec2 hitbox_origin = {0, 40};
                    vec2 h_pos = hitbox_origin.rotate(sprite.transform.rotation);

                    FloatRect rect = collider;
                    rect.x += transform.pos.x;
                    rect.y += transform.pos.y;
                    rect.x += h_pos.x * sprite.transform.scale.x;
                    rect.y += h_pos.y * sprite.transform.scale.y;

                    if (lifetime > first_phase_len) {
                        for (entity_id id=0; id<game_state->entity_count; id++) {
                            Entity *e = &game_state->entity_pool[id];
                            if (e->type != Entity::FIGHT_LOVE_WAVE_ATTACK) {
                                continue;
                            }
                            FloatRect e_col = e->collider;
                            e_col.x += e->transform.pos.x;
                            e_col.y += e->transform.pos.y;

                            // If you're hit by a wave
                            if (AABBIsColliding(rect, e_col)) {
                                // queue_free_entity(id);
                                sprite.color.r = 150;
                                sprite.color.b = 150;
                                props.hypnotized = true;
                                break;
                            }
                        }
                    }

                }
            }
            transform.pos.x += vel.x;
            transform.pos.y += vel.y;

        }
            break;
        case Entity::FIGHT_SCENE_EYEBALL:
        {
            float dir_to_eye = angle_to(fightChara.transform.pos, transform.pos)-PI/2;
        
            if (dir_to_eye >= PI*2) {
                dir_to_eye -= PI*2;
            } else if (dir_to_eye < 0) {
                dir_to_eye += PI*2;
            }
            
            const float look_range = PI*0.5f;
            const float diff = angle_diff(fightChara.sprite.transform.rotation, dir_to_eye);
            if (diff < look_range/2.f) {
                sprite.frame = 1;
            } else {
                sprite.frame = 0;
            }
        }
            break;
        case Entity::XFATHER_FIGHT_SCENE_LEAF:
        {
            EPROPS(this,LeafProps);

            lifetime += timestep;
            if (lifetime > 1.f && false) {
                // props.dir *= -1;
                // lifetime -= 1.f;
            } else {
                float freq = 1.f;
                float x = lifetime/freq;
                x = sin(x*PI);
                vel.x = x * 4.5f;// * props.dir;
                // vel.x = (lifetime * lifetime * lifetime) / wave_len * 10.f * props.dir;
            }

            if (transform.pos.y >= 800) {
                queue_free_entity(id);
            }
    
            {
                Entity *fightChara = &game_state->entity_pool[game_state->fightChara];
                if (EntitiesAreColliding(fightChara, this)) {
                    FightCharaTakeDamage(fightChara);
                }
            }
            
            transform.pos.x += vel.x;
            transform.pos.y += vel.y;
        }
            break;
        case Entity::BOY:
        {
            EPROPS(this,BoyProps);
            props.timer+=timestep;
            if (game_state->scene.current_room == "bus") {
                if (props.state == BoyProps::LongWalk) {
                    vel.x = 2;
                    if (transform.pos.x >= 525*4) {
                        vel.x = 0;
                        props.state = BoyProps::Stop;
                        props.timer = 0;
                    }
                } else if (props.state == BoyProps::Stop) {
                    if (props.timer >= 3.f) {
                        props.timer = 0.f;
                        props.state = BoyProps::ContinueWalk;
                        vel.x = 2;
                    } else if (props.timer >= 2.f) {
                        transform.flip = false;
                    } else if (props.timer >= 1.f) {
                        transform.flip = true;
                    }
                } else if (props.state == BoyProps::ContinueWalk) {
                    if (transform.pos.x >= 650*4) {
                        if (vel.x) {
                            props.timer = 0;
                        }
                        vel.x = 0;
                        if (props.timer > 1.f) {
                            props.state = BoyProps::None;
                            transform.pos = {655*4, 75*4};
                            QueueCommand(CMD_EVENT, "bus_boy_sits_down");
                            ScriptRunQueue(game_state->script_state);
                        }
                    }
                }
            } else if (game_state->scene.current_room == "cafeteria") {
                if (props.state == BoyProps::LongWalk) {
                    vel.x = -2;
                    transform.flip = true;
                    if (transform.pos.x <= 245*4) {
                        vel.x = 0;
                        props.state = BoyProps::Stop;
                        props.timer = 0;
                    }
                } else if (props.state == BoyProps::Stop) {
                    if (props.timer >= 3.f) {
                        props.timer = 0.f;
                        props.state = BoyProps::ContinueWalk;
                        vel.x = -3;
                    } else if (props.timer >= 2.f) {
                        transform.flip = true;
                    } else if (props.timer >= 1.f) {
                        transform.flip = false;
                    }
                } else if (props.state == BoyProps::ContinueWalk) {
                    if (transform.pos.x <= 90*4) {
                        if (vel.x) {
                            props.timer = 0;
                        }
                        vel.x = 0;
                        if (props.timer > 1.f) {
                            props.state = BoyProps::None;
                            // position = {655*4, 75*4};
                            QueueCommand(CMD_EVENT, "boy_arrives_cafeteria");
                            ScriptRunQueue(game_state->script_state);
                        }
                    }
                }
            }
            transform.pos.x += vel.x;
            transform.pos.y += vel.y;

            AnimationPlayerUpdate(&anim_player,&sprite,int(timestep*1000));

        }
            break;
        case Entity::XFATHER_FIGHT_SCENE_MAG:
        {
            //vel = angle_to_vec(sprite.transform.rotation) * 5.5f;
            AnimationPlayerUpdate(&anim_player,&sprite,int(timestep*1000));
            if (anim_player.playing) {
                return;
            } else if (vel.x==0.f && vel.y == 0.f) {
                queue_free_entity(id);
                return;
            }
            
            FloatRect test_col = collider;
            test_col.x += transform.pos.x;
            test_col.y += transform.pos.y;
            FloatRect f_col = fightChara.collider;
            f_col.x += fightChara.transform.pos.x;
            f_col.y += fightChara.transform.pos.y;
            
            if (AABBIsColliding(test_col, f_col)) {
                vel = {0,0};
                AnimationPlayerPlay(&anim_player, &sprite, ANIMATIONS::XFATHER_FIGHT_MAG_SHUT);
                FightCharaTakeDamage(&fightChara);
                break;
            }
            float look_range = (PI*2)/6.f;
            float dir_to_mag = angle_to(fightChara.transform.pos, transform.pos)-PI/2;
        
            if (dir_to_mag >= PI*2) {
                dir_to_mag -= PI*2;
            } else if (dir_to_mag < 0) {
                dir_to_mag += PI*2;
            }
            
            const float diff = angle_diff(fightChara.sprite.transform.rotation, dir_to_mag);
            float speed_mult;
            if (diff < look_range/2.f) {
                sprite.color = {100,100,100,255};
                // smoothly rotate to player
                float target_rot = angle_to(transform.pos, fightChara.transform.pos)-PI;
                target_rot = wrap_rotation(target_rot);
                sprite.transform.rotation = lerp_rotation(sprite.transform.rotation, target_rot, 0.0625f);
                speed_mult = 5.5f;
            } else {
                sprite.color = {255,255,255,255};
                speed_mult = 4.0f;
            }
            vel = angle_to_vec(sprite.transform.rotation) * -speed_mult;
            
            transform.pos.x += vel.x;
            transform.pos.y += vel.y;
        }
            break;
    }
}

