#include "ParticleSystem.h"

namespace ParticleSystem {
    internal_function
    void Init() {
        *particle_state = {};
        // this isn't memory efficient. realistically we should store everything in the particle system on the heap
        particle_state->particle_pool = (Particle*)malloc(sizeof(Particle) * ParticleSystemState::particle_count_max);
        particle_state->particle_res = GetImgResource("../res/imgs/particle.png");
    }

    internal_function
    void Render(s16 render_layer) {
       for (u32 i=0; i<particle_state->emitter_count; i++) {
            if (particle_state->emitters[i].render_layer != render_layer) {
                continue;
            }
            particle_state->emitters[i].Update(particle_state, FRAME_TIME);
        }
        
        if (particle_state->particle_count == 0) {
            return;
        }

        // Update and render at same time
        for (u32 i=0; i<particle_state->particle_count_max; i++) {
            Particle &particle = particle_state->particle_pool[i];
            
            if (particle.life_remaining <= 0) {
                continue;
            }
            if (particle.render_layer != render_layer) {
                continue;
            }

            particle.life_remaining -= FRAME_TIME;
            particle.pos.x += particle.vel.x;
            particle.pos.y += particle.vel.y;
            particle.rotation += particle.rotation_speed * FRAME_TIME;

            if (particle.life_remaining <= 0) {
                particle_state->particle_count--;
                continue;
            }

            float life = particle.life_remaining / particle.lifetime;

            // note: don't interpolate over rgb color space! this is temporary
            Color color;
            color.r = lerp(particle.color_end.r, particle.color_start.r, life);
            color.b = lerp(particle.color_end.b, particle.color_start.b, life);
            color.g = lerp(particle.color_end.g, particle.color_start.g, life);
            color.a = lerp(particle.color_end.a, particle.color_start.a, life);
            // //color.r = (particle.color_end.r - particle.color_start.r) * life + particle.color_start.r;
            // color.b = (particle.color_end.b - particle.color_start.b) * life + particle.color_start.b;
            // color.g = (particle.color_end.g - particle.color_start.g) * life + particle.color_start.g;

            float render_rot = rad_2_deg(particle.rotation); // need to convert to degrees because SDL doesn't render radians for some fucking reason
            float radius = lerp(particle.radius_end, particle.radius_start, life);

            IntRect frame = {0, 0, 32, 32};
            SDL_Rect dest_rect = {(int)particle.pos.x, (int)particle.pos.y, (int)radius, (int)radius};

            // the fact i need a transform to do what I want to do is a massive flaw
            Transform draw_transform;
            draw_transform.pos = particle.pos;
            draw_transform.rotation = render_rot;
            draw_transform.scale.x = radius/32.f;
            draw_transform.scale.y = radius/32.f;
            draw_transform.origin = {particle_state->particle_res->meta.width/2.f,particle_state->particle_res->meta.width/2.f};
            
            
            SDL_SetTextureColorMod(particle_state->particle_res->texture, color.r, color.g, color.b);
            DrawTexture(particle_state->particle_res->texture, frame, draw_transform);
            // SDL_RenderCopyEx(renderer->sdl_renderer, state->particle_res->texture, &frame, &dest_rect, render_rot, NULL, SDL_FLIP_NONE);
        }
    }

    internal_function
    inline ParticleEmitter *CreateEmitter() {
        ParticleEmitter *emitter = &particle_state->emitters[particle_state->emitter_count];
        *emitter = {};
        emitter->id = particle_state->emitter_count;
        emitter->render_layer = -1;
        particle_state->emitter_count++;

        return emitter;
    }
    
    internal_function
    void Clear() {
        particle_state->particle_count = 0;
        particle_state->emitter_count = 0;
        particle_state->next_free_particle = 0;
        for (u32 i=0; i<1024; i++) {        
            particle_state->particle_pool[i].life_remaining=0;
        }
    }

    internal_function
    void FreeEmitter(u32 id) {
        if (id != particle_state->emitter_count && particle_state->emitter_count >= 2) {
            particle_state->emitters[id] = particle_state->emitters[particle_state->emitter_count-1];
            particle_state->emitters[id].id = id;
            if (game_state->editor.obj_type == SceneEditor::EDIT_PARTICLE_EMITTER) {
                if (game_state->editor.em == &particle_state->emitters[id]) {
                    game_state->editor.em = nullptr;
                    game_state->editor.obj_type = SceneEditor::EDIT_NONE;
                } else if (game_state->editor.em == &particle_state->emitters[particle_state->emitter_count-1]) {
                    game_state->editor.em = &particle_state->emitters[id];
                }
            }
        }
        particle_state->emitter_count--;
    }
}

void ParticleEmitter::Emit(ParticleSystemState *state) {
    for (int i=0;i<particles_per_burst; i++) {
        // foreach particle in the burst
        if (state->particle_pool[state->next_free_particle].life_remaining > 0) {
            state->particle_count--;
        }
        Particle particle;

        particle.lifetime = lifetime + (Random::Float() * particle.lifetime * 0.15f);
        particle.life_remaining = lifetime + (Random::Float() * particle.lifetime * 0.15f);
        particle.pos = pos;
        particle.render_layer = render_layer;

        vec2 vel = angle_to_vec(dir);
        float vel_angle_radians = dir;
        vel_angle_radians += Random::Float() * dir_variance - (dir_variance/2.f);
        vel.x = cos(vel_angle_radians);
        vel.y = sin(vel_angle_radians);
        float n_power = power;
        if (power_variance) {
            const float variance = power * power_variance;
            n_power += Random::Float() * variance - (variance/2.f);
        }
        vel = vel.mult(n_power);
        particle.vel = vel;

        particle.color_start = color_start;
        particle.color_end = color_end;

        particle.radius_start = radius_start;
        particle.radius_end = radius_end;
        particle.rotation = rotation;

        if (rotation_variance) {
            particle.rotation += Random::Float()*rotation_variance - (rotation_variance/2.f);
        }

        particle.rotation_speed = rotation_speed;
        if (rotation_speed_variance) {
            particle.rotation_speed += Random::Float()*rotation_speed_variance - (rotation_speed_variance/2.f);
        }

        // hard code this now
        const float n_radius_variance = radius_start * radius_variance;
        particle.radius_start += Random::Float()*n_radius_variance - (n_radius_variance/2.f);

        particle.color_start = color_start;
        particle.color_end = color_end;
        
        particle.color_start = color_start;
        particle.color_end = color_end;

        state->particle_pool[state->next_free_particle] = particle;

        state->particle_count++;
        state->next_free_particle = (state->next_free_particle+1) % state->particle_count_max;
    }
}


void ParticleEmitter::Update(ParticleSystemState *state, float timestep) {
    if (!emit) {
        return;
    }
    time_to_next -= timestep;

    while (time_to_next <= 0.0f) {
        time_to_next += rate;
        Emit(state);
        if (rate == 0) {
            break;
        }
    }
}
