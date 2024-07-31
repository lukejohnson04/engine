
#include <sys/types.h>
#include <sys/stat.h>
// #ifndef WIN32
// #include <unistd.h>
// #endif

#ifdef WIN32
#define stat _stat
#endif

#include "game_assets.h"

// In release mode fetching a texture will be O(1), this is just
// ugly and brute force so it will always refresh updated textures!
internal
GLuint GetTexture(const std::string path) {
    for (u32 i=0; i<private_global_assets->texture_count; i++) {
        if (private_global_assets->textures[i].meta.path == path) {
            return private_global_assets->textures[i].texture;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    glGenTextures(1,&temp.texture);
    i32 loaded = GL_load_texture(temp.texture,path.c_str());
    
    if (!loaded) {
        printf("Texture %s does not exist!\n",path.c_str());
        return 0;
    }

    Resource *res = &private_global_assets->textures[private_global_assets->texture_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
    
    return res->texture;
}

internal
Mix_Chunk *GetChunk(const std::string path) {
    for (u32 i=0; i<private_global_assets->chunk_count; i++) {
        if (private_global_assets->chunks[i].meta.path == path) {
            return private_global_assets->chunks[i].chunk;
        }
    }
    
    // not found - check for it on file and load it
    Resource temp;
    temp.chunk = Mix_LoadWAV(path.c_str());
    
    if (temp.chunk == NULL) {
        printf("Chunk %s does not exist!\n",path.c_str());
        return NULL;
    }

    Resource *res = &private_global_assets->chunks[private_global_assets->chunk_count++];
    *res = temp;
    
    res->meta.path = path;
    struct stat file_data;
    if (stat(path.c_str(), &file_data)==0) {
        res->meta.last_write = file_data.st_mtime;
    }
    
    return res->chunk;
}



internal
bool ResourceWasUpdated(Resource *res) {
    struct stat file_data;
    if (stat(res->meta.path.c_str(), &file_data)==0) {
        time_t last_write = file_data.st_mtime;
        if (last_write == res->meta.last_write) {
            return false;
        }
        return true;
    }
    return false;
}


internal
void CheckForResourceUpdates(game_assets *assets) {
    for (u32 id=0; id<assets->texture_count; id++) {
        Resource *res = &assets->textures[id];
        if (ResourceWasUpdated(res)) {
            i32 loaded = GL_load_texture(res->texture,res->meta.path.c_str());
    
            if (!loaded) {
                continue;
            }

            struct stat file_data;
            if (stat(res->meta.path.c_str(), &file_data)==0) {
                res->meta.last_write = file_data.st_mtime;
            }
        }
    }

    for (u32 id=0; id<assets->chunk_count; id++) {
        Resource *res = &assets->chunks[id];
        if (ResourceWasUpdated(res)) {
            res->chunk = Mix_LoadWAV(res->meta.path.c_str());
            
            if (res->chunk == NULL) {
                continue;
            }

            struct stat file_data;
            if (stat(res->meta.path.c_str(), &file_data)==0) {
                res->meta.last_write = file_data.st_mtime;
            }
        }
    }
}
