
#pragma once

struct Resource {
    union {
        GLuint texture;
        Mix_Chunk *chunk;
        Mix_Music *music;
    };

    struct {
        time_t last_write=0;
        std::string path;
        union {
            struct {
                i32 width,height;
            };
        };
    } meta;
};

struct game_assets {
    Resource textures[256];
    Resource chunks[256];
    Resource music[256];

    u32 texture_count=0;
    u32 chunk_count=0;
    u32 music_count=0;
};


internal game_assets *private_global_assets=nullptr;


internal GLuint GetTexture(std::string path);
internal Mix_Chunk *GetChunk(const std::string path);

internal void CheckForResourceUpdates(game_assets *assets);
