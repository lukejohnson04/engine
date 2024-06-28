struct light {
    Transform transform;
    Color color={0,0,0,255};
    float energy=1.0f;
    enum {
        BLEND_ADD,
        BLEND_SUB,
        BLEND_MIX
    };
    u8 blend_mode = BLEND_ADD;
    bool enabled=true;
};

struct LightPool {
    light lights[256];
    u16 light_count=0;
    Resource *light_img=nullptr;
};

internal_function
light *create_light(LightPool* pool) {
    pool->lights[pool->light_count] = {};
    pool->light_count++;
    return &pool->lights[pool->light_count-1];
}