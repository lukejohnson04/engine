#include "IngameEntityEditor.h"
#include "RoomDataLoader.h"
#include <iostream>

void SceneEditor::OnImguiRender() {
    Scene *current_scene = &game_state->scene;
    if (obj_type != EDIT_ENTITY) {
        ImGui::Text("No entity selected");
    } else {
        ImGui::InputFloat("Collider X", &entity->collider.x, 0, 10000);
        ImGui::InputFloat("Collider Y", &entity->collider.y, 0, 10000);
        ImGui::InputFloat("Collider W", &entity->collider.w, 0, 10000);
        ImGui::InputFloat("Collider H", &entity->collider.h, 0, 10000);
        if (entity) {
            ImGui::InputInt("Physics layer", (int*)&entity->collider_layers);
            //entity->collider_layers = u32(physics_layer);
            if (entity->collider_layers == LAYER_NONE) {
                ImGui::InputText("Input here", &entity->onInteract);
            }
            ImGui::Checkbox("On Contact", &entity->collider_on_contact);
            if (entity->collider_on_contact) {
                ImGui::InputInt("Trigger Count", &entity->collider_trigger_count);
            }
        }
    } if (obj_type != EDIT_PARTICLE_EMITTER) {
        ImGui::Text("No particle emitter selected");
    } else {
        local_persist ImVec4 color_start = {em->color_start.r/255.f,em->color_start.g/255.f,em->color_start.b/255.f,em->color_start.a/255.f};
        local_persist ImVec4 color_end = {em->color_end.r/255.f,em->color_end.g/255.f,em->color_end.b/255.f,em->color_end.a/255.f};

        ImGui::Text("Next particle: %d", game_state->particle_state.next_free_particle);

        ImGui::SliderFloat("Pos x", &em->pos.x, 0.000f, 3000.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Pos y", &em->pos.y, 0.000f, 2000.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Dir", &em->dir, -PI, PI);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Dir Variance", &em->dir_variance, -PI*2, PI*2);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Power", &em->power, 0.01f, 20.f);
        ImGui::SliderFloat("Power Variance", &em->power_variance, 0.f, 2.f);

        ImGui::ColorEdit3("Color start", (float*)&color_start);
        ImGui::ColorEdit3("Color end", (float*)&color_end);
        ImGui::SliderFloat("Color variance", &em->color_variance, 0.0f, 2.0f);

        ImGui::SliderFloat("Radius start", &em->radius_start, 1.0f, 100.0f);
        ImGui::SliderFloat("Radius end", &em->radius_end, 0.0000f, 100.0f);
        ImGui::SliderFloat("Radius variance", &em->radius_variance, 0.0000f, 2.0f);
        
        ImGui::SliderFloat("Rotation", &em->rotation, -PI, PI);
        ImGui::SliderFloat("Rotation variance", &em->rotation_variance, 0.0f, 2.0f);
        ImGui::SliderFloat("Rotation speed", &em->rotation_speed, -PI*3, PI*3);
        ImGui::SliderFloat("Rotation speed variance", &em->rotation_speed_variance, 0.0f, 2.0f);

        ImGui::SliderFloat("Lifetime", &em->lifetime, 0.00001f, 20.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Rate", &em->rate, 0.0001f, 0.5f);
        ImGui::SliderInt("Burst", &em->particles_per_burst, 0, 10);
        ImGui::Checkbox("Emit", (bool*)&em->emit);

        // vec2 pos={0,0};
        // vec2 dir={1,0};
        // float dir_variance=0.5f; // radians
        // // random degrees to rotate by
        // float power=5.0f;
        // float power_variance=0.20f; // percentage based

        // Color color_start={255,255,255,255};
        // Color color_end={0,0,0,255};
        // float color_variance=0.1f; // percentage based

        // float radius_start=5.f;
        // float radius_end=0.5f;
        // float radius_variance=0.1f; // percentage based

        em->color_start.r = u8(color_start.x*255);
        em->color_start.g = u8(color_start.y*255);
        em->color_start.b = u8(color_start.z*255);
        em->color_start.a = u8(color_start.w*255);

        em->color_end.r = u8(color_end.x*255);
        em->color_end.g = u8(color_end.y*255);
        em->color_end.b = u8(color_end.z*255);
        em->color_end.a = u8(color_end.w*255);
    }

    if (obj_type != EDIT_NONE && ImGui::Button("Remove")) {
        if (obj_type == EDIT_ENTITY) {
            queue_free_entity(entity->id);
            entity = nullptr;
        } else if (obj_type == EDIT_PARTICLE_EMITTER) {
            ParticleSystem::FreeEmitter(em->id);
            em = nullptr;
        }
        obj_type = EDIT_NONE;
    }

    if (ImGui::Button("Save Changes")) {
        SaveRoomToFile(current_scene, current_scene->current_room);
    } if (ImGui::Button("Create Particle Emitter")) {
        em = ParticleSystem::CreateEmitter();
        em->pos.x = game_state->scene.camera.pos.x;
        em->pos.y = game_state->scene.camera.pos.x;
        obj_type = EDIT_PARTICLE_EMITTER;
    }

    current_scene->made_editor_changes = true;
}

void SceneEditor::Update() {
    if (input->just_pressed[SDL_SCANCODE_GRAVE]) {
        enabled = true;
    }
    if (!enabled) {
        return;
    }
    Scene *scene = &game_state->scene;
    if (input->just_pressed[SDL_SCANCODE_LCTRL]) {
        // spawn collider entity where mouse is
        entity = create_entity();
        vec2i pos = GetMousePosition();
        pos = scene->ConvertToSceneCoords(pos);
        entity->collider.x = pos.x-16;
        entity->collider.y = pos.y-16;
        entity->collider.w = 64;
        entity->collider.h = 64;
        scene->made_editor_changes = true;
    }
    // if (scene->made_editor_changes == false) {
    //     entity = nullptr;
    // }
}

void SceneEditor::OnEvent(Event &e) {
    if (e.sdl_event.type == SDL_MOUSEBUTTONDOWN) {
        if (e.sdl_event.button.button == SDL_BUTTON_LEFT) {
            // check if clicked on an entity
            vec2i mpos = GetMousePosition();
            mpos = game_state->scene.ConvertToSceneCoords(mpos);
            // check for click on entity
            for (entity_id id=0; id<game_state->entity_count; id++) {
                Entity *t_entity = &game_state->entity_pool[id];
                if (id == game_state->player || id == game_state->fightChara || t_entity->collider.w == 0 || t_entity->collider.h == 0) {
                    continue;
                }
                if (t_entity->collider.contains(mpos.x,mpos.y)) {
                    entity = t_entity;
                    e.handled = true;
                    obj_type = EDIT_ENTITY;
                    return;
                }
            }
            // check for click on emitter
            for (u32 id=0; id<game_state->particle_state.emitter_count; id++) {
                ParticleEmitter *emitter = &game_state->particle_state.emitters[id];
                int len = 24;
                FloatRect click_rect = {(float)emitter->pos.x-len,(float)emitter->pos.y-len,len*2.f,len*2.f};
                if (click_rect.contains(mpos.x,mpos.y)) {
                    em = emitter;
                    e.handled = true;
                    obj_type = EDIT_PARTICLE_EMITTER;
                    return;
                }
            }
        }
    }
}