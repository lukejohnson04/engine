#pragma once

struct Entity;
struct ParticleEmitter;

struct SceneEditor {
    void OnImguiRender();
    void Update();
    void OnEvent(Event &e);

    enum {
        EDIT_NONE,
        EDIT_ENTITY,
        EDIT_PARTICLE_EMITTER,
    } obj_type;

    union {
        Entity *entity;
        ParticleEmitter *em;
    };

    bool enabled=false;
};
