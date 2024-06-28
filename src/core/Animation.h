#pragma once

struct animation {
    u16 frame_lens[64];
    u16 start_frame=0;
    u16 end_frame=0;
    bool repeat=false;
};

internal_function
animation CreateAnimation(int frame_count, int fill_len, bool repeat, int start_frame=0) {
    animation res;
    res.repeat = repeat;
    res.start_frame=start_frame;
    res.end_frame=start_frame+frame_count;
    for (int frame_num=0; frame_num<frame_count; frame_num++) {
        res.frame_lens[frame_num] = fill_len;
    }
    return res;
}

enum ANIMATIONS {
    PLAYER_WALK_LEFT,
    PLAYER_WALK_RIGHT,
    OPENING_DOOR_BREAK,
    BASEMENT_CAGE_ANIM,
    CAFETERIA_RAIN,
    PARKING_LOT_FIRST_HALF,
    PARKING_LOT_SECOND_HALF,
    BULLY_BOBBLE,
    FIGHT_SCENE_BOBBLE,
    XFATHER_FIGHT_MAG_SHUT,
    ANIMATION_COUNT
};

internal_function
void LoadAnimations(animation *animations) {
    animations[ANIMATIONS::PLAYER_WALK_LEFT]     = CreateAnimation(4,135,true);
    animations[ANIMATIONS::PLAYER_WALK_RIGHT]    = CreateAnimation(4,135,true,4);

    animation door_anim = CreateAnimation(19,50,false);
    door_anim.frame_lens[9] = 4000;
    door_anim.frame_lens[10] = 3500;
    door_anim.frame_lens[11] = 40;
    door_anim.frame_lens[12] = 40;
    door_anim.frame_lens[13] = 40;
    door_anim.frame_lens[14] = 40;
    door_anim.frame_lens[15] = 40;
    door_anim.frame_lens[16] = 40;
    door_anim.frame_lens[17] = 3000;
    animations[ANIMATIONS::OPENING_DOOR_BREAK] = door_anim;

    // animation second_intro_anim = Create(IntRect(9*320,0,320,180),10,0.04f,false);
    // second_intro_anim.frames[0].len = 2.f;
    // second_intro_anim.frames[1].len = 3.5f;
    // second_intro_anim.frames[8].len = 3.0f;
    // anim_player.frame = {9*320,0,320,180};

    animation cageanim = CreateAnimation(18,100,false,1);
    cageanim.frame_lens[0] = 1000;
    for (u32 i=1; i<7; i++) {
        cageanim.frame_lens[i] = 150;
    }
    animations[ANIMATIONS::BASEMENT_CAGE_ANIM] = cageanim;
    animations[ANIMATIONS::CAFETERIA_RAIN] = CreateAnimation(4, 100, true, 1);
    animations[ANIMATIONS::PARKING_LOT_FIRST_HALF] = CreateAnimation(3, 3000, false);
    animations[ANIMATIONS::PARKING_LOT_FIRST_HALF].frame_lens[0] = 5000;
    animations[ANIMATIONS::PARKING_LOT_SECOND_HALF] = CreateAnimation(3, 3000, false, 2);
    animations[ANIMATIONS::PARKING_LOT_SECOND_HALF].frame_lens[0] = 5000;

    animations[ANIMATIONS::BULLY_BOBBLE] = CreateAnimation(4, 75, true);
    animations[ANIMATIONS::FIGHT_SCENE_BOBBLE] = CreateAnimation(4, 100, true);
    animations[ANIMATIONS::FIGHT_SCENE_BOBBLE].frame_lens[0] = 1000;
    animations[ANIMATIONS::FIGHT_SCENE_BOBBLE].frame_lens[2] = 700;
    
    animations[ANIMATIONS::XFATHER_FIGHT_MAG_SHUT] = CreateAnimation(2, 50, false, 1);
    animations[ANIMATIONS::XFATHER_FIGHT_MAG_SHUT].frame_lens[1] = 600;
}


struct AnimationPlayer {
    animation *anim=nullptr;
    u16 frame=0;
    u16 clock=0;

    void (*on_finished_callback)(animation* anim);
    bool playing=false;
};

internal_function
void AnimationPlayerUpdate(AnimationPlayer *player, Sprite *sprite, int timestep_ms) {
    if (player->playing == false || sprite == nullptr || player->anim == nullptr) {
        return;
    }
    player->clock += timestep_ms;

    u16 frame_num = sprite->frame - player->anim->start_frame;
    if (player->clock >= player->anim->frame_lens[frame_num]) {
        player->clock -= player->anim->frame_lens[frame_num];
        sprite->frame++;
        if (sprite->frame >= player->anim->end_frame) {
            sprite->frame = player->anim->start_frame;
            if (!player->anim->repeat) {
                player->clock = 0;
                player->playing = false;

                player->clock=0;
                if (player->on_finished_callback) {
                    player->on_finished_callback(player->anim);
                }
                player->anim = nullptr;

                return;
            }
        }
    }
}

internal_function void AnimationPlayerPlay(AnimationPlayer *player, Sprite *sprite, u8 animation_id);
internal_function void AnimationPlayerStop(AnimationPlayer *player);
