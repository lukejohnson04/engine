#pragma once
#include <vector>

struct Particle {
    vec2 pos;
    vec2 vel;

    // alot of this could be factored out of the particle struct by storing a reference to the emitter
    Color color_start;
    Color color_end;

    float radius_start=4.0f;
    float radius_end=0.0f;
    
    float rotation=0.0f;
    float rotation_speed=0.0f;

    float lifetime=1.0f;
    float life_remaining=0.0f;
    s16 render_layer=0;
};

struct ParticleSystemState;
struct ParticleEmitter {
    vec2 pos={0,0};
    float dir=0.0f; // radians
    float dir_variance=0.5f; // radians
    // random degrees to rotate by
    float power=5.0f;
    float power_variance=0.20f; // percentage based

    Color color_start={255,255,255,255};
    Color color_end={0,0,0,255};
    float color_variance=0.1f; // percentage based

    float radius_start=5.f;
    float radius_end=0.5f;
    float radius_variance=0.1f; // percentage based

    float rotation=0.0f;
    float rotation_variance=0.5f; // radians
    float rotation_speed=0.0f;
    float rotation_speed_variance=1.f; // radians

    float lifetime=1.0f;
    float rate=0.1f;
    float time_to_next=0.0f;
    int particles_per_burst=5;

    float explosiveness=0.0f;

    bool emit=true;
    u32 id=ID_DONT_EXIST;
    s16 render_layer=0;
    void Emit(ParticleSystemState *state);
    void Update(ParticleSystemState *state, float timestep);
};

struct ParticleSystemState {
    ParticleEmitter emitters[32];
    u32 emitter_count=0;

    static const u32 particle_count_max=20480;
    // Particle particle_pool[particle_count_max];//=nullptr;//[particle_count_max];
    Particle *particle_pool=nullptr;//[particle_count_max];
    u32 next_free_particle=0;
    u32 particle_count=0;
    Resource *particle_res=nullptr;
};

global_variable ParticleSystemState *particle_state;

namespace ParticleSystem {
    internal_function void Init();
    internal_function void Render(s16 render_layer);
    internal_function void OnImguiRender();
    internal_function void Clear();
    internal_function inline ParticleEmitter *CreateEmitter();
};
