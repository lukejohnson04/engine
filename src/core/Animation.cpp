#pragma once

internal_function
void AnimationPlayerPlay(AnimationPlayer *player, Sprite *sprite, u8 animation_id) {
    player->anim = &game_state->animations[animation_id];
    player->clock = 0;
    player->frame = player->anim->start_frame;
    player->playing = true;
    sprite->frame = player->anim->start_frame;
}

internal_function
void AnimationPlayerStop(AnimationPlayer *player) {
    player->playing = false;
    player->clock = 0;
    player->anim = nullptr;
    player->frame = 0;
}
