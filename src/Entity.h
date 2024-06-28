#pragma once

struct SDL_Texture;
struct Scene;

struct Entity;
internal_function void EntityUpdateStub(Entity *entity, float timestep);

#define EPROPS(entity,name) name &props = *(name*)entity->props

struct Entity
{
    enum Type
    {
        NONE,
        GENERIC,
        PLAYER,
        FIGHT_LOVE_WAVE_ATTACK,
        FIGHT_FIST_ATTACK,
        FIGHT_SCENE_EYEBALL,
        FIGHT_CHARA,
        XFATHER_FIGHT_SCENE_LEAF,
        XFATHER_FIGHT_SCENE_MAG,
        ART_TEACHER,
        BOY
    } type = GENERIC;

    enum
    {
        TRANSFORM = 1,
        TEXTURE = 2,
        COLLIDER = 4,
        UPDATE = 8,
        VISIBLE = 16
    };

    u32 id = ID_DONT_EXIST;
    u32 flags = TRANSFORM | UPDATE | VISIBLE;

    vec2 vel = {0, 0};
    Transform transform;
    // these are both affected by transform
    Sprite sprite;
    FloatRect collider;

    AnimationPlayer anim_player;

    bool collider_on_contact = false;
    s32 collider_trigger_count = 1;
    u32 collider_layers = LAYER_PHYSICS;

    // convert this to id
    std::string onInteract = "";

    // pointer to additional state info that can be cast to entity-specific prop structs
    // 256 bytes max for now
    float lifetime = 0;
    u8 props[256] = {0};

    void Create(Entity::Type);
    void MegaUpdate(float);
    //void (*Update)(Entity *, float) = &EntityUpdateStub;
    void SetPosition(vec2 pos);
    inline bool checkFlags(u32 comp_flags)
    {
        return (flags & comp_flags) == comp_flags;
    }

    inline void setFlag(u32 flag)
    {
        flags |= flag;
    }

    inline void disableFlag(u32 flag)
    {
        flags &= ~flag;
    }
};

internal_function void RenderEntity(Entity *entity);
