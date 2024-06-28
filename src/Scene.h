#pragma once

struct SDL_Texture;
struct GameState;

struct Scene;

internal_function void SceneRender(Scene*);
internal_function void SceneUpdate(Scene*,float);
internal_function void SceneOnImguiRender(Scene*);
internal_function void SceneOnEvent(Scene*);
internal_function void SceneOnEvent(Scene *scene, Event &e);
internal_function void scene_on_anim_finished(animation* anim);

#define GET_PROPS(name) name &props = *(name*)scene->props

struct Scene {
    Scene();
    void FreeResources();
    void OnEntry(std::string name);

    void (*UpdateTemp)(Scene*,float)=&SceneUpdate;
    void (*RenderTemp)(Scene*)=&SceneRender;
    void (*OnEvent)(Scene*,Event&)=&SceneOnEvent;
    void (*OnImguiRender)(Scene *scene)=&SceneOnImguiRender;
    
    vec2i ConvertToSceneCoords(vec2i pos);

    void RunCustomEvent(std::string name);

    std::string current_room="";
    vec2i dimensions={320,180};

    bool update_entities=true;
    bool made_editor_changes=false;

    Transform camera;
    AnimationPlayer anim_player;

    // awful temp variables that used to be globals - remove eventually
    u32 entities_spawned_total=0;
    bool darkness=false;

    Sprite sprite;

    // 1KB slot for persistent scene state
    char props[1024] = {};
};
