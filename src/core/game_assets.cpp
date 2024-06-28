#include <sys/types.h>
#include <sys/stat.h>
// #ifndef WIN32
// #include <unistd.h>
// #endif

#ifdef WIN32
#define stat _stat
#endif

internal_function
void StoreImgResource(Resource &res) {
    global_assets->image_resource_pool[global_assets->image_resource_count] = res;
    global_assets->image_resource_count++;
}

internal_function
void StoreChunkResource(Resource &res) {
    global_assets->chunk_resource_pool[global_assets->chunk_resource_count] = res;
    global_assets->chunk_resource_count++;
}

internal_function
void StoreMusicResource(Resource &res) {
    global_assets->music_resource_pool[global_assets->music_resource_count] = res;
    global_assets->music_resource_count++;
}

internal_function
void LoadImgResource(std::string path, Resource *res) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Jeez loueise! %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return;
    }

    newTexture = SDL_CreateTextureFromSurface(sdl_renderer, loadedSurface);
    if (newTexture == NULL) {
        printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);


    res->texture = newTexture;
    SDL_QueryTexture(newTexture, NULL, NULL, &res->meta.width, &res->meta.height);

    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
}

internal_function
Resource *GetImgResource(std::string path) {
    for (u32 i=0; i<game_state->assets.image_resource_count; i++) {
        if (game_state->assets.image_resource_pool[i].meta.path == path) {
            return &game_state->assets.image_resource_pool[i];
        }
    }
    // not found
    Resource temp;
    LoadImgResource(path, &temp);
    StoreImgResource(temp);

    Resource *res = &game_state->assets.image_resource_pool[game_state->assets.image_resource_count-1];
    if (res == nullptr || res->meta.width == 0 || res->meta.height == 0) {
        std::cout << "Failed to retrieve image!!!\n";
        return nullptr;
    }
    return res;
}

internal_function
void LoadChunkResource(std::string path, Resource &res) {
    res.chunk = Mix_LoadWAV(path.c_str());
    res.meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res.meta.last_write = file_data.st_mtime;
    }
}

internal_function
Resource *GetChunkResource(std::string path) {
    for (u32 i=0; i<global_assets->chunk_resource_count; i++) {
        if (global_assets->chunk_resource_pool[i].meta.path == path) {
            return &global_assets->chunk_resource_pool[i];
        }
    }
    
    Resource res;
    LoadChunkResource(path, res);
    StoreChunkResource(res);
    return &global_assets->chunk_resource_pool[global_assets->chunk_resource_count-1];
}

internal_function
void FreeChunkResource(game_assets *assets, Resource *chunk) {
    Mix_FreeChunk(chunk->chunk);
}


internal_function
void LoadMusicResource(std::string path, Resource &res) {
    res.music = Mix_LoadMUS(path.c_str());
    res.meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res.meta.last_write = file_data.st_mtime;
    }
}

internal_function
Resource *GetMusicResource(std::string path) {
    for (u32 i=0; i<global_assets->chunk_resource_count; i++) {
        if (global_assets->music_resource_pool[i].meta.path == path) {
            return &global_assets->music_resource_pool[i];
        }
    }
    
    Resource res;
    LoadMusicResource(path, res);
    StoreMusicResource(res);
    return &global_assets->music_resource_pool[global_assets->music_resource_count-1];
}

internal_function
void FreeMusicResource(game_assets *assets, Resource *music) {
    Mix_FreeMusic(music->music);
}


internal_function
bool ResourceWasUpdated(Resource &res) {
    struct stat file_data;
    if (stat(res.meta.path.c_str(), &file_data)==0) {
        time_t last_write = file_data.st_mtime;
        if (last_write == res.meta.last_write) {
            return false;
        }
        return true;
    }
    return false;
}

internal_function
void CheckForResourceUpdates(game_assets *assets) {
    for (u32 id=0; id<assets->image_resource_count; id++) {
        Resource &res = assets->image_resource_pool[id];
        if (ResourceWasUpdated(res)) {
            SDL_DestroyTexture(res.texture);
            LoadImgResource(res.meta.path, &res);
        }
    }
    for (u32 id=0; id<assets->chunk_resource_count; id++) {
        Resource &res = assets->chunk_resource_pool[id];
        if (ResourceWasUpdated(res)) {
            Mix_FreeChunk(res.chunk);
            LoadChunkResource(res.meta.path, res);
        }
    }
    for (u32 id=0; id<assets->music_resource_count; id++) {
        Resource &res = assets->music_resource_pool[id];
        if (ResourceWasUpdated(res)) {
            Mix_FreeMusic(res.music);
            LoadMusicResource(res.meta.path, res);
        }
    }
    // for (u32 id=0; id<assets->image_resource_count; id++) {
    //     Resource &res = assets->image_resource_pool[id];
    //     if (ResourceWasUpdated(res)) {
    //         SDL_DestroyTexture(res.texture);
    //         LoadImgResource(res.meta.path, &res);
    //     }
    // }

}

internal_function
Resource *CreateTemporaryImageResource() {
    if (global_assets->temporary_image_pool[global_assets->next_free_image].texture) {
        // free resource before overwriting here
    }
    Resource *res = &global_assets->temporary_image_pool[global_assets->next_free_image++];
    if (global_assets->next_free_image >= 256) {
        global_assets->next_free_image = 0;
    }
    return res;
}
