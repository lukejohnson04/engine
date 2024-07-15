
#pragma once

struct world_sound {
    Resource* res;
    vec2 position;
    float min_volume=0.f;
    float max_volume=1.f;
    float max_volume_radius=0.f;
    float min_volume_radius=100.f;
    u8 channel=0;
    bool playing=false;
};

struct music_object {
    Resource *res;
    bool playing = false;
};

namespace Audio {
    enum {
        MUSIC_NONE
    };

    enum {
        CHUNK_NONE
    };

    internal_function
    void PlayChunk(std::string path, s32 channel) {
        Resource *res = GetChunkResource(path);
        Mix_PlayChannel(channel, res->chunk, -1);
    }
}

struct world_audio {
    world_sound channels[8];
    u8 next_free=0;
    vec2 global_listener_pos={0,0};
};


void stop_world_sound(world_sound *sound) {
    Mix_HaltChannel(sound->channel);
    sound->playing = false;
}

void play_world_sound(world_sound *sound) {
    Mix_PlayChannel(sound->channel, sound->res->chunk, -1);
    sound->playing=true;
}

void push_global_listener_pos(vec2 pos) {
    global_world_audio->global_listener_pos = pos;
}

void play_music(music_object *music) {
    Mix_PlayMusic(music->res->music,-1);
    music->playing = true;
}

void stop_music(music_object *music) {
    Mix_HaltMusic();
    music->playing = false;
}


void position_world_sound(vec2 listener_pos, world_sound *sound) {
    float angle_rad = angle_to(listener_pos, sound->position) - PI/2;
    s16 angle = rad_2_deg(angle_rad);

    float dist = distance_between(listener_pos, sound->position);
    float dist_scale_size = sound->min_volume_radius - sound->max_volume_radius;

    dist /= sound->min_volume_radius;
    dist = std::max((1.0f-sound->max_volume), 1.f-dist);
    dist = std::min((1.0f-sound->min_volume), 1.f-dist);

    Mix_SetPosition(sound->channel, angle, u8(dist*255.f));
}


void update_world_sound() {
    for (u8 channel=0; channel<8; channel++) {
        if (global_world_audio->channels[channel].playing) {
            position_world_sound(global_world_audio->global_listener_pos,&global_world_audio->channels[channel]);
        }
    }
}
