#include "ParticleSystem.h"
#include "IngameEntityEditor.h"
#include "RoomDataLoader.h"


Scene::Scene() {
    anim_player.on_finished_callback = &scene_on_anim_finished;
}

void Scene::FreeResources() {
    // for (int i=0; i<8; i++) {
    //     // free resources here?
    //     if (sprites[i] == nullptr) {
    //         break;
    //     }
    // }
}

internal_function
void SceneOnEvent(Scene *scene, Event &e) {
    if (game_state->dialogue_state.isPlaying || game_state->script_state->curr_command != CMD_NONE) {
        return;
    }
    if (e.sdl_event.type == SDL_KEYDOWN) {
        if (e.sdl_event.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {
            // lshift
            for (entity_id id=0; id<game_state->entity_count; id++) {
                Entity *entity = &game_state->entity_pool[id];
                if (id == game_state->player || entity->collider.w == 0 || entity->collider.h == 0) {
                    continue;
                }
                if (entity->collider_layers == LAYER_NONE && entity->onInteract != "") {
                    FloatRect p_rect = game_state->entity_pool[game_state->player].collider;
                    p_rect.x += game_state->entity_pool[game_state->player].transform.pos.x;
                    p_rect.y += game_state->entity_pool[game_state->player].transform.pos.y;
                    if (AABBIsColliding(p_rect, entity->collider)) {
                        e.handled = true;
                        RunCommand(entity->onInteract);
                        break;
                    }
                }
            }
        }
    }
}

internal_function
void scene_on_anim_finished(animation *prev) {
    if (prev->repeat) {
        return;
    }
    if (prev != nullptr) {
        game_state->scene.update_entities = true;
        game_state->entity_pool[game_state->player].setFlag(Entity::VISIBLE);
        if (game_state->scene.current_room == "openingscene") {
            game_state->scene.sprite.frame = 19;
        } else if (game_state->scene.current_room == "parkinglot") {
            if (prev == &game_state->animations[ANIMATIONS::PARKING_LOT_FIRST_HALF]) {
                game_state->scene.sprite.frame = 2;
                QueueCommand(CMD_WAIT, 2.f);
                QueueCommand(CMD_SAY, "Any second now, she'll be here. Any second now.");
                QueueCommand(CMD_WAIT, 3.f);
                QueueCommand(CMD_SAY, "There she is!!! Thank the heavens!!");                
                QueueCommand(CMD_SAY, "I was starting to lose hope.");
                QueueCommand(CMD_SAY, "Have a great winter break Florence.");
                QueueCommand(CMD_SAY, "...");
                QueueCommand(CMD_SAY, "What's the matter?");
                QueueCommand(CMD_SAY, ".. Oh, don't worry Florence! I'm sure your mother will be here soon, and then everything will be okay!");
                QueueCommand(CMD_SCENE_ANIM, ANIMATIONS::PARKING_LOT_SECOND_HALF);
                ScriptRunQueue(game_state->script_state);
            } else if (prev == &game_state->animations[ANIMATIONS::PARKING_LOT_SECOND_HALF]) {
                game_state->scene.sprite.frame = 4;
            }
        }
    }
}

vec2i Scene::ConvertToSceneCoords(vec2i pos) {
    pos.x += camera.pos.x;
    pos.y += camera.pos.y;
    return pos;
}

struct CrashSiteProps {
    ParticleEmitter *bg_em=nullptr;
    vec2 em_vel={135,0};
};

internal_function void LoadFightScene(Scene *scene);

void Scene::OnEntry(std::string name) {
    if (current_room != "" && made_editor_changes) {
        // save room contents to file
        SaveRoomToFile(this, current_room);
    }
    made_editor_changes = false;
    
    FreeResources();
    
    current_room = name;
    std::string img_path = "../res/imgs/" + name + ".png";
    sprite = {};
    sprite.texture = GetImgResource(img_path);

    clear_entities();
    ParticleSystem::Clear();

    Entity *player = create_entity();
    player->Create(Entity::PLAYER);
    //CreatePlayer(player);
    game_state->player = player->id;

    AnimationPlayerStop(&anim_player);

    game_state->editor.entity = nullptr;
    game_state->editor.em = nullptr;
    game_state->editor.obj_type = SceneEditor::EDIT_NONE;
    
    player->setFlag(Entity::VISIBLE);
    player->setFlag(Entity::UPDATE);


    LoadRoomFromFile(this, current_room);

    if (name == "openingscene") {
        player->SetPosition({256, player->transform.pos.y});

        dimensions.x = 320;
        sprite.frame = 19;
        if (game_state->script_state->event_runcount.count(current_room)==0) {
            darkness = true;
            player->disableFlag(Entity::VISIBLE);

            sprite.h_frames = 20;

            #ifdef RELEASE
            Audio::PlayMusic("intro_audio.wav");
            QueueCommand(CMD_WAIT, 8.42);
            #endif
            QueueCommand(CMD_SCENE_ANIM, ANIMATIONS::OPENING_DOOR_BREAK);
            // QueueCommand(CMD_WAIT, 1.0);
            // QueueCommand(CMD_SCENE_ANIM, 1);
            ScriptRunQueue(game_state->script_state);
        }
        
    } else if (name == "testroom4") {
        player->SetPosition({200, player->transform.pos.y});

    } else if (name == "testroom5") {
        player->SetPosition({430*4, 120});
        
    } else if (name == "outside") {
        player->SetPosition({540*4, 120*4});
        
    } else if (name == "walk") {
        player->SetPosition({32, 535*4});

    } else if (name == "bus") {
        player->SetPosition({160, 90});
    
    } else if (name == "bathroom") {
        sprite.h_frames = 2;
    
    } else if (name == "parkinglot") {
        sprite.h_frames = 5;
        
    } else if (name == "basement") {
        player->SetPosition({65,100});

        // dimensions.x /= 19;
        sprite.h_frames = 19;
        // animation cageanim = Create(IntRect(1*480,0,480,180),18,0.1f,false);
        // cageanim.frames[0].len = 1.f;
        // cageanim.frames[1].len = 0.15f;
        // cageanim.frames[2].len = 0.15f;
        // cageanim.frames[3].len = 0.15f;
        // cageanim.frames[4].len = 0.15f;
        // cageanim.frames[5].len = 0.15f;
        // cageanim.frames[6].len = 0.15f;
        // anim_player.animations.push_back(cageanim);
        // anim_player.current = &anim_player.animations[0];
        // anim_player.frame = {0*480,0,480,180};
    } else if (name == "classroom") {
        player->disableFlag(Entity::VISIBLE);
    } else if (name == "cafeteria_full") {
        if (game_state->script_state->event_runcount.count(current_room)==0) {
            Audio::PlayMusic("bell.wav");
            QueueCommand(CMD_WAIT, 3.0);
            QueueCommand(CMD_EVENT, "bell_rings");
            ScriptRunQueue(game_state->script_state);
        }
    } else if (name == "artroom") {
        player->disableFlag(Entity::VISIBLE);
        sprite.h_frames = 2;
        if (game_state->script_state->event_runcount.count(current_room) == 0) {
            sprite.frame=0;
        } else {
            sprite.frame=1;
        }
    } else if (name == "cafeteria") {
        player->disableFlag(Entity::VISIBLE);
        sprite.h_frames = 5;
        if (game_state->script_state->event_runcount.count(current_room)==0) {
            sprite.frame=0;
        } else {
            AnimationPlayerPlay(&anim_player, &sprite, ANIMATIONS::CAFETERIA_RAIN);
        }
    } else if (name == "crashsite") {
        renderer->bg_color = {0,0,0,255};
        CrashSiteProps &casted_props = *(CrashSiteProps*)props;
        casted_props = {};
        casted_props.bg_em = &game_state->particle_state.emitters[1];
        casted_props.bg_em->pos.x = 150;
        casted_props.bg_em->pos.y = 600;
        casted_props.bg_em->render_layer = -1;
        game_state->editor.em = casted_props.bg_em;
        game_state->editor.obj_type = SceneEditor::EDIT_PARTICLE_EMITTER;
    } else if (name == "shadow_realm") {
        sprite.h_frames = 3;
    }
    if (game_state->script_state->curr_command ==CMD_NONE && game_state->script_state->file.event_lines.count(current_room) != 0) {
        QueueCommand(CMD_EVENT, current_room.c_str());
        ScriptRunQueue(game_state->script_state);
    } else {
        game_state->script_state->event_runcount[current_room]++;
    }
    dimensions.x = sprite.texture->meta.width / sprite.h_frames;
    dimensions.y = sprite.texture->meta.height / sprite.v_frames;

}

void Scene::RunCustomEvent(std::string name) {
    if (name == "boy_enters_bus") {
        game_state->entity_pool[game_state->player].disableFlag(Entity::UPDATE);
        Entity *boy = create_entity();
        boy->Create(Entity::BOY);
        boy->transform.pos = {355*4, 100*4};
    } else if (name == "boy_enters_cafeteria") {
        game_state->entity_pool[game_state->player].disableFlag(Entity::UPDATE);
        Entity *boy = create_entity();
        boy->Create(Entity::BOY);
        boy->transform.pos = {320*4, 128*4};
    } else if (name == "art_teacher_walks_in") {
        game_state->entity_pool[game_state->player].disableFlag(Entity::UPDATE);
        Entity *art_teacher = create_entity();
        art_teacher->Create(Entity::ART_TEACHER);
        art_teacher->transform.pos = {340*4,(150-20)*4};
    } else if (name == "stall_change_frame") {
        sprite.frame=1;
        ScriptRunEvent(game_state->script_state,"bathroom_stall");
    } else if (name == "shadow_realm_boy_interact") {
        QueueCommand(CMD_WAIT,1.0);
        INLINE_COMMAND_START();
        game_state->entity_pool[game_state->player].disableFlag(Entity::VISIBLE);
        INLINE_COMMAND_END();
        QueueCommand(CMD_WAIT,0.5);
        QueueCommand(CMD_FADEOUT,0.0);
        QueueCommand(CMD_WAIT,1.0);
        QueueCommand(CMD_FADEIN,0.0);
        INLINE_COMMAND_START();
        game_state->entity_pool[game_state->player].setFlag(Entity::VISIBLE);
        game_state->entity_pool[game_state->player].SetPosition({200, 550});
        INLINE_COMMAND_END();
        QueueCommand(CMD_SCENE_FRAME,1);
        QueueCommand(CMD_WAIT,3.0);
        QueueCommand(CMD_FADEOUT,0.0);
        QueueCommand(CMD_WAIT,0.5);
        QueueCommand(CMD_SCENE_FRAME,2);
        QueueCommand(CMD_FADEIN,0.0);
        QueueCommand(CMD_WAIT,4.0);
        QueueCommand(CMD_SAY,"...");
        QueueCommand(CMD_FADEOUT,0.5);
        QueueCommand(CMD_WAIT,2.0);
        QueueCommand(CMD_FADEIN,0.0);
        INLINE_COMMAND_START();
        LoadFightScene(&game_state->scene);
        INLINE_COMMAND_END();
        ScriptRunQueue(game_state->script_state);
    }
}


internal_function
void SceneRender(Scene *scene) {
    if (scene->darkness) {
// SDL_SetRenderDrawColor(renderer->sdl_renderer, 0, 0, 0, 255);
        // SDL_RenderClear(renderer->sdl_renderer);
        return;
    }
    if (scene->sprite.texture) {
        scene->sprite.transform.scale = {4,4};
        DrawSprite(&scene->sprite);
        // DrawTexture(scene->sprite.texture->texture, frame, bg_transform);
    }
    for (entity_id id=0; id<game_state->entity_count; id++) {
        RenderEntity(&game_state->entity_pool[id]);
    }

}

internal_function
void SceneUpdate(Scene *scene, float timestep) {
    if (scene->anim_player.anim && scene->anim_player.playing) {
        scene->darkness = false;
    }
    
    if (game_state->dialogue_state.isPlaying == false && !scene->darkness && scene->update_entities) {
        vec2i mpos = GetMousePosition();
        mpos = scene->ConvertToSceneCoords(mpos);

        for (entity_id id=0; id<game_state->entity_count; id++) {
            Entity *entity = &game_state->entity_pool[id];
            if (entity->checkFlags(Entity::UPDATE)) {// && entity->Update) {
                entity->MegaUpdate(timestep);
                //entity->Update(entity, timestep);
            }
            if (input->mouse_just_pressed) {
                if (entity->collider.contains(mpos.x, mpos.y)) {
                    // clicked on
                    game_state->editor.entity = entity;
                    break;
                }
            }
        }
    }

    if (scene->current_room == "crashsite") {
        CrashSiteProps &props = *(CrashSiteProps*)scene->props;
        props.bg_em->pos.x += props.em_vel.x;
        if (props.bg_em->pos.x >= 2800) {
            props.em_vel.x = -135;
            props.bg_em->pos.x = 2800;
        } if (props.bg_em->pos.x <= -250) {
            props.em_vel.x = 135;
            props.bg_em->pos.x = -250;
        }
    }

    AnimationPlayerUpdate(&scene->anim_player, &scene->sprite, int(timestep*1000));
}

internal_function
void SceneOnImguiRender(Scene *scene) {
    ImGui::SliderFloat("Camera x", &scene->camera.pos.x, -500, 2000);
    ImGui::Text("room size: %d %d", scene->dimensions.x, scene->dimensions.y);
    if (game_state->player != ID_DONT_EXIST) {
        Entity &player = game_state->entity_pool[game_state->player];
        IntRect p_frame = player.sprite.get_frame();
        ImGui::Text("Player rect: {%d, %d, %d, %d}", p_frame.x, p_frame.y, p_frame.w, p_frame.h);
        ImGui::Text("Player pos: {%f,%f}", player.transform.pos.x, player.transform.pos.y);
        // FloatRect global_collider = player.collider.ApplyTransform(player.transform).ApplyTransform(renderer->global_transform);
        FloatRect local_collider = player.collider;
        // ImGui::Text("Player collider: {%f,%f,%f,%f}", global_collider.x, global_collider.y, global_collider.w, global_collider.h);
        ImGui::Text("Local collider: {%f,%f,%f,%f}", local_collider.x, local_collider.y, local_collider.w, local_collider.h);
    }

    vec2i mpos = GetMousePosition();
    vec2i mpos_coords = scene->ConvertToSceneCoords(mpos);

    ImGui::Text("Mouse position: %d %d", mpos.x, mpos.y);
    ImGui::Text("Mouse coordinates: %d %d", mpos_coords.x, mpos_coords.y);

    Transform cam_transform = scene->camera.Apply(renderer->global_transform);
    ImGui::Text("Global Transform");
    ImGui::Text("Pos: %f, %f", cam_transform.pos.x, cam_transform.pos.y);
    ImGui::Text("Scale: %f, %f", cam_transform.scale.x, cam_transform.scale.y);
    ImGui::Text("Origin: %f, %f", cam_transform.origin.x, cam_transform.origin.y);
    // ImGui::SliderFloat("Edit scale x", &pic->transform.scale.x, 0, 10);
    // ImGui::SliderFloat("Edit scale y", &pic->transform.scale.y, 0, 10);
    // ImGui::SliderFloat("Position x", &pic->position.x, 0, 1280);
    // ImGui::SliderFloat("Position y", &pic->position.y, 0, 720);
    // ImGui::SliderFloat("Rotation", &pic->transform.rotation, -PI*6, PI*6);
}

// GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) {}

// GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
//     std::cout << 123;
// }
