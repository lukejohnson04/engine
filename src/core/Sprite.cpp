struct Resource;

// Performance note for future:
// Transforms could be stored elsewhere and stored as pointers in sprites (pointers are 8 bytes so unlikely to be worth it. could store u16 id or something)
// Regardless, scale and frame data are extremely unlikely to be unique.
struct Sprite {
    Resource *texture=nullptr;
    Transform transform={};
    u16 frame=0;
    u8 h_frames=1;
    u8 v_frames=1;
    Color color = {255,255,255,255};
    bool visible=true;

    inline IntRect get_frame() {
        // this is slow because we're accessing a pointer
        IntRect rect = {0,0,texture->meta.width/h_frames,texture->meta.height/v_frames};

        rect.x += rect.w*(frame % h_frames);
        rect.y += rect.h*(frame / h_frames);
        return rect;
    }
};