#pragma once

struct Resource {
    union {
        SDL_Texture *texture;
        Mix_Chunk *chunk;
        Mix_Music *music;
    };

    struct {
        time_t last_write=0;
        std::string path;
        union {
            // image meta
            struct {
                s32 width, height;
            };
        };
    } meta;
};

struct game_assets {
    Resource image_resource_pool[256];
    Resource chunk_resource_pool[256];
    Resource music_resource_pool[256];

    Resource temporary_image_pool[256];
    u32 next_free_image=0;
    
    u32 image_resource_count=0;
    u32 chunk_resource_count=0;
    u32 music_resource_count=0;
};


global_variable game_assets *global_assets=nullptr;

internal_function Resource *GetImgResource(std::string path);
internal_function Resource *GetChunkResource(std::string path);
internal_function Resource *GetMusicResource(game_assets *assets, std::string path);

internal_function void FreeImgResource(game_assets *assets, Resource *img);
internal_function void FreeChunkResource(game_assets *assets, Resource *chunk);
internal_function void FreeMusicResource(game_assets *assets, Resource *music);
internal_function void CheckForResourceUpdates(game_assets *assets);
