
v2 closest_point_on_line(v2 l1, v2 l2, v2 point) {
    float A1 = l2.y - l1.y;
    float B1 = l1.x - l2.x;

    double C1 = (l2.y - l1.y) * l1.x + (l1.x - l2.x) * l1.y;
    double C2 = -B1 * point.x + A1 * point.y;
    double det = A1 * A1 - ( - B1 * B1);
    
    v2 ans = point;
    if (det != 0) {
        ans.x = (float)((A1 * C1 - (B1 * C2)) / det);
        ans.y = (float)((A1 * C2 - ( - B1 * C1)) / det);
    }

    return ans;
}

iRect scrollbar_full = {NATIVE_GAME_WIDTH-8-8,20,8,NATIVE_GAME_HEIGHT-64};


void InitGmDinner() {
    GmDinnerData *data = &game_state->gm_dinner_data;
    
    SDL_Surface *house_layout_surface = IMG_Load("res/imgs/dinner_house.png");
    for (i32 x=0; x<DINNER_MAP_WIDTH; x++) {
        for (i32 y=0; y<DINNER_MAP_HEIGHT; y++) {
            game_state->gm_dinner_data.dinner_world_map[y][x] = 0;

            Color col = {0,0,0,0};
            Uint32 pixel = getpixel(house_layout_surface,x,y);
            SDL_GetRGBA(pixel, house_layout_surface->format, &col.r, &col.g, &col.b, &col.a);
            if (col == COLOR_BLACK) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 1;
            } else if (col == Color({255,0,0,255})) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 2;
            } else if (col == Color({255,100,0,255})) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 3;
            } else if (col == Color({0,0,255,255})) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 5;
                printf("123\n");
            } else if (col == 0x474747ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 6;
            } else if (col == 0x621a00ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 7;
            } else if (col == 0xd100ffff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 8;
            } else if (col == 0x69251aff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 9;
            } else if (col == 0xa83700ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 10;
            } else if (col == 0xffff00ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 11;
            } else if (col == 0xff00ffff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 12;
            } else if (col == 0x783200ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 13;
            } else {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 0;
            }
        }
    }

    data->world_objects[GmDinnerData::GO_TV].on = true;

    data->world_objects[GmDinnerData::GO_TV].pos = {30,25.5f};
    data->world_objects[GmDinnerData::GO_CHINA].pos = {29.5f,29};
    data->world_objects[GmDinnerData::GO_WRENCH].pos = {34.5f,32.5f};
    data->world_objects[GmDinnerData::GO_YAHTZEE].pos = {25.5f,30.5f};
    data->world_objects[GmDinnerData::GO_AQUARIUM].pos = {31.f,36.5f};
    
    //DOGSOUND
    Mix_PlayChannel(2,game_state->dinner_dog_whimpering,-1);
}

internal
void DinnerOnTrigger(v2i pos) {
    
}

internal
void DinnerOnInteract(i32 obj_type,Wobject *object) {
    
}

internal
bool DinnerChoice(iRect rect) {
    if (input->mouse_just_pressed) {
        v2i mpos = GetMousePositionIngame();
        if (rect_contains_point(rect,mpos)) {
            return true;
        }
    }
    return false;
}

internal
void DinnerDebugLoad() {
    InitGmDinner();
    GmDinnerData *data = &game_state->gm_dinner_data;
    data->player_x = 30;
    data->player_y = 28;
    data->host_x = 30;
    data->host_y = 27;
    data->host_task = GmDinnerData::WAITING_FOR_MILKSHAKE;
}


void UpdateGmDinner(float timestep) {
    GmDinnerData *data = &game_state->gm_dinner_data;
    auto &dinner_world_map = data->dinner_world_map;
    double &dir_x = data->dir_x;
    double &dir_y = data->dir_y;
    double &player_x = data->player_x;
    double &player_y = data->player_y;
    double &plane_x = data->plane_x;
    double &plane_y = data->plane_y;

    double delta = timestep;
    double move_x = 0;
    double move_y = 0;

    double move_speed = 1.5;
    double rot_speed = 1.5;

    auto &door_state = data->door_state;
    static float door_opened_timer=0.f;
    static v2i door_pos = {0,0};

    auto &world_objects = data->world_objects;
    world_objects[GmDinnerData::GO_HOST].pos = {(float)data->host_x,(float)data->host_y};
    //world_objects[GmDinnerData::GO_WRENCH].ypos = 0.5f;
    

    data->hover_object = GmDinnerData::GO_NONE;

#ifdef DEBUG
    if (input->just_pressed[SDL_SCANCODE_V]) {
        InitGmDinner();
    }
#endif

    if (input->is_pressed[SDL_SCANCODE_LSHIFT]) {
        move_speed *= 2.0;
        rot_speed *= 2.0;
    }

    if (input->just_pressed[SDL_SCANCODE_R]) {
        data->no_clip = !data->no_clip;
    }

    if (data->can_move) {
        if (input->is_pressed[SDL_SCANCODE_W]) {
            move_x = dir_x * delta;
            move_y = dir_y * delta;
        } if (input->is_pressed[SDL_SCANCODE_S]) {
            move_x = -dir_x * delta;
            move_y = -dir_y * delta;
        } if (input->is_pressed[SDL_SCANCODE_A]) {
            move_x += -dir_y * delta;
            move_y += dir_x * delta;
        } if (input->is_pressed[SDL_SCANCODE_D]) {
            // move perpendicular
            move_x += dir_y * delta;
            move_y += -dir_x * delta;
        }
    }
    struct {
        union {
            v2i pos={0,0};
        };
    } hover_obj;

    const double PLAYER_RADIUS = 0.05;
    v2 player = {(float)player_x,(float)player_y};
    {
        // loop through and see what was interacted with
        for (i32 x=0; x<DINNER_MAP_WIDTH; x++) {
            for (i32 y=0; y<DINNER_MAP_HEIGHT; y++) {
                if (dinner_world_map[x][y] != 3 && dinner_world_map[x][y] != 12 && dinner_world_map[x][y] != 13) {
                    continue;
                }

                v2 center = v2(x+0.5f,y+0.5f);
                if (distance_between(player,center) > 2) {
                    continue;
                }

                v2 ray = {player.x+(float)dir_x,player.y+(float)dir_y};
                v2 ray_col = closest_point_on_line(player,ray,center);
                float dist_to_ray = distance_between(ray_col,center);
                
                if (dist_to_ray < 0.5f) {
                    if (dinner_world_map[x][y] == 3) {
                        data->hover_object = GmDinnerData::GO_DOOR;
                    } else if (dinner_world_map[x][y] == 13) {
                        data->hover_object = GmDinnerData::GO_LOCKED_DOOR;
                    } else {
                        data->hover_object = GmDinnerData::GO_KEYPAD_LOCKED_DOOR;                        
                    }
                    hover_obj.pos = {x,y};
                    break;
                }
            }
        }
        
        for (i32 ind=0;ind<GmDinnerData::GO_COUNT;ind++) {
            v2 object = data->world_objects[ind].pos;
            if (distance_between(player,object) > 2) {
                continue;
            }

            v2 ray = {player.x+(float)dir_x,player.y+(float)dir_y};
            v2 ray_col = closest_point_on_line(player,ray,object);
            float dist_to_ray = distance_between(ray_col,object);
                
            if (dist_to_ray < 0.5f) {
                data->hover_object = ind;
                break;
            }                
        }
    }

    // on interact
    if (data->hover_object != GmDinnerData::GO_NONE && input->just_pressed[SDL_SCANCODE_E]) {
        if (data->hover_object == GmDinnerData::GO_DOOR) {
            if (hover_obj.pos.x==36 && hover_obj.pos.y==29) {
                Mix_PlayChannel(0,game_state->dinner_knock,0);
                door_pos=hover_obj.pos;
                data->host_task = GmDinnerData::GETTING_DOOR;
                door_state = GmDinnerData::DOOR_KNOCKED;
            } else {
                dinner_world_map[hover_obj.pos.x][hover_obj.pos.y] = 0;
                Mix_PlayChannel(3,GetChunk("res/sound/dinner_door_open.ogg"),0);
            }
        } else if (data->hover_object == GmDinnerData::GO_LOCKED_DOOR) {
            Mix_PlayChannel(1,GetChunk("res/sound/dinner_door_lock.ogg"),0);
        } else if (data->hover_object == GmDinnerData::GO_KEYPAD_LOCKED_DOOR) {
            data->gameplay_state = GmDinnerData::ENTERING_CODE;
            data->can_move = false;

            for (i32 i=0; i<12; i++) {
                i32 dest = (NATIVE_GAME_WIDTH - 12*16)/2 + i*16;
                data->keypad_codes[i] = generate_text_obj(game_state->font,std::to_string(data->code_values[i]),COLOR_BLACK,0);
                data->keypad_codes[i].position.x = dest+8-(data->keypad_codes[i].get_draw_rect().w/2);
                data->keypad_codes[i].position.y = 48;
            }
        } else if (data->hover_object == GmDinnerData::GO_TV) {
            data->world_objects[GmDinnerData::GO_TV].on ^= 1;
        } else if (data->hover_object == GmDinnerData::GO_CHINA) {
            Mix_PlayChannel(0,game_state->dinner_china_interact,0);
        } else if (data->hover_object == GmDinnerData::GO_YAHTZEE) {
            
        } else if (data->hover_object == GmDinnerData::GO_HOST && data->can_interact_with_host) {
            if (data->host_task == GmDinnerData::WAITING_FOR_MILKSHAKE) {
                data->host_milkshake_state = GmDinnerData::MK_INITIAL_SPEAK;
                data->can_interact_with_host = false;
                Mix_PlayChannel(0,game_state->dinner_give_milkshake,0);
            }
        }
    }

    if (input->just_pressed[SDL_SCANCODE_GRAVE]) {
        DinnerDebugLoad();
    }
    
    move_x *= move_speed;
    move_y *= move_speed;
    
    player_y += move_y;
    if (player_y > DINNER_MAP_HEIGHT-1) player_y = DINNER_MAP_HEIGHT-1;
    if (player_y < 0) player_y = 0;
    if (!data->no_clip) {
        double player_top = player_y - PLAYER_RADIUS;
        i32 top_tile = dinner_world_map[(i32)player_x][(i32)player_top];
        if (top_tile != 0 && top_tile != 5) {
            player_y = (i32)player_top + 1 + PLAYER_RADIUS;
        }
        double player_bottom = player_y + PLAYER_RADIUS;
        i32 bottom_tile = dinner_world_map[(i32)player_x][(i32)player_bottom];
        if (bottom_tile != 0 && bottom_tile != 5) {
            player_y = (i32)player_bottom - PLAYER_RADIUS;
        }
    }

    player_x += move_x;
    if (player_x > DINNER_MAP_WIDTH-1) player_x = DINNER_MAP_WIDTH-1;
    if (player_x < 0) player_x = 0;
    if (!data->no_clip) {
        double player_left = player_x - PLAYER_RADIUS;
        i32 left_tile = dinner_world_map[(i32)player_left][(i32)player_y];
        if (left_tile != 0 && left_tile != 5) {
            player_x = (i32)player_left + 1 + PLAYER_RADIUS;
        }
        double player_right = player_x + PLAYER_RADIUS;
        i32 right_tile = dinner_world_map[(i32)player_right][(i32)player_y];
        if (right_tile != 0 && right_tile != 5) {
            player_x = (i32)player_right - PLAYER_RADIUS;
        }
    }

    if (input->is_pressed[SDL_SCANCODE_LEFT] || input->is_pressed[SDL_SCANCODE_RIGHT]) {
        double ROT_SPEED = rot_speed * (input->is_pressed[SDL_SCANCODE_LEFT] ? 1 : -1);
        ROT_SPEED *= delta;
        {
            double old_plane_x = plane_x;
            plane_x = plane_x * cos(ROT_SPEED) - plane_y * sin(ROT_SPEED);
            plane_y = old_plane_x * sin(ROT_SPEED) + plane_y * cos(ROT_SPEED);
        }
        {
            double old_dir_x = dir_x;
            dir_x = dir_x * cos(ROT_SPEED) - dir_y * sin(ROT_SPEED);
            dir_y = old_dir_x * sin(ROT_SPEED) + dir_y * cos(ROT_SPEED);
        }
    }

    // look for triggers
    // see if player just entered a trigger
    v2i ppos_rounded = {(i32)player_x,(i32)player_y};
    {
        double block_doorway_startx = 27.5;
        double block_doorway_starty = 34.5;
        double block_doorway_endx = 27.5;
        double block_doorway_endy = 33.5;
        static i32 travel_point=0;
        
        if (dinner_world_map[(i32)player_x][(i32)player_y] == 5) {
            // disabled trigger
            dinner_world_map[(i32)player_x][(i32)player_y] = 0;
            data->host_x = block_doorway_startx;
            data->host_y = block_doorway_starty;
            data->host_task = GmDinnerData::BLOCKING_DOORWAY;
            data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_SLIDE;
            data->timer = 0;
        }

        // update host
        if (data->host_task == GmDinnerData::BLOCKING_DOORWAY) {
            data->can_move = false;
            data->timer += timestep;
            if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_SLIDE) {
                double timer_len = 0.5;
                v2 dir = {(float)(block_doorway_endx - block_doorway_startx),(float)(block_doorway_endy-block_doorway_starty)};
                dir = dir.normalize();
                dir *= (float)(data->timer / timer_len);
                data->host_x = block_doorway_startx+dir.x;
                data->host_y = block_doorway_starty+dir.y;
                if (data->timer > 0.5) {
                    data->timer = 0.0;
                    data->host_x = block_doorway_endx;
                    data->host_y = block_doorway_endy;
                    data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_SPEAK;
                    Mix_PlayChannel(0,game_state->dinner_block_doorway,0);
                }
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_SPEAK) {
                if (Mix_Playing(0) == false) {
                    data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_CHOICE;
                }
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHOICE) {
                data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_LEMME_CHECK_SPEAK;
                Mix_PlayChannel(0,game_state->dinner_dog_lemme_check,0);
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_LEMME_CHECK_SPEAK) {
                if (Mix_Playing(0) == false) {
                    data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG_TRAVELTO;
                    travel_point=0;
                }
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG_TRAVELTO || data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG_TRAVELBACK) {
                v2 points[] = {{(float)block_doorway_endx,(float)block_doorway_endy},{25.5,33.5},{25.5,37.5}};
                v2 host = {(float)data->host_x,(float)data->host_y};
                if (distance_between(host,v2((float)points[travel_point].x,(float)points[travel_point].y)) < 0.1) {
                    bool travelto = data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG_TRAVELTO;
                    if (travelto) {
                        travel_point++;
                    } else {
                        travel_point--;
                    }
                    if (travel_point >= 3) {
                        data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG;
                        data->timer = 0.0;
                    } else if (travel_point < 0) {
                        data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_CHECKED_DOG_SPEAK;
                        Mix_PlayChannel(0,game_state->dinner_after_checking_dog,0);
                    }
                } else {
                    v2 vec = vec_to(host,v2((float)points[travel_point].x,(float)points[travel_point].y)) * timestep * 2.5;
                    data->host_x += (double)vec.x;
                    data->host_y += (double)vec.y;
                }
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG) {
                local_persist bool played_gunshot=false;
                if (data->timer > 5) {
                    data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_CHECKING_DOG_TRAVELBACK;
                    travel_point = 2;
                } else {
                    if (Mix_Playing(0) == false && !played_gunshot) {
                        if (data->timer >= 2.0) {
                            played_gunshot=true;
                            Mix_PlayChannel(0,game_state->dinner_gunshot,0);
                            Mix_HaltChannel(2);
                        }
                    }
                }
            } else if (data->doorway_blocking_state == GmDinnerData::DOORWAY_BLOCK_CHECKED_DOG_SPEAK) {
                if (Mix_Playing(0) == false) {
                    data->host_task = GmDinnerData::WAITING_FOR_MILKSHAKE;
                    data->host_milkshake_state = GmDinnerData::MK_TRAVELLING_TO_WAIT;
                    data->can_move = true;
                }
            }
        } else if (data->host_task == GmDinnerData::WAITING_FOR_MILKSHAKE) {
            if (data->host_milkshake_state == GmDinnerData::MK_TRAVELLING_TO_WAIT) {
                v2i points[] = {{31,33},{31,29},{30,27}};
                local_persist i32 point=0;
                v2 host = {(float)data->host_x,(float)data->host_y};
                if (distance_between(host,v2((float)points[point].x,(float)points[point].y)) < 0.1) {
                    point++;
                    if (point >= 3) {
                        data->host_milkshake_state = GmDinnerData::MK_WAIT;
                        data->can_interact_with_host=true;
                    }
                } else {
                    v2 vec = vec_to(host,v2((float)points[point].x,(float)points[point].y)) * timestep * 5.0;
                    data->host_x += (double)vec.x;
                    data->host_y += (double)vec.y;
                }
            } else if (data->host_milkshake_state == GmDinnerData::MK_INITIAL_SPEAK) {
                if (Mix_Playing(0) == false) {
                    data->host_milkshake_state = GmDinnerData::MK_GIVE_MILKSHAKE;
                    data->timer = 0.0;
                }
            } else if (data->host_milkshake_state == GmDinnerData::MK_GIVE_MILKSHAKE) {
                data->timer += timestep;
                if (data->timer > milkshake_give_length_total) {
                    data->timer = 0;
                    data->host_milkshake_state = GmDinnerData::MK_MILKSHAKE_DRANK_REACTION;
                    Mix_PlayChannel(1,game_state->dinner_drink,0);
                    data->instr=0;
                }
            } else if (data->host_milkshake_state == GmDinnerData::MK_MILKSHAKE_DRANK_REACTION) {
                if (data->instr==0) {
                    if (Mix_Playing(1) == false) {
                        data->instr++;
                        Mix_PlayChannel(0,game_state->dinner_drink_milkshake_reaction,0);
                    }
                } else if (data->instr == 1) {
                    if (Mix_Playing(0) == false) {
                        data->gameplay_state = GmDinnerData::CHOICE;
                        data->choices[0] = generate_text_obj(game_state->font,"yes",COLOR_WHITE,0);
                        data->choices[1] = generate_text_obj(game_state->font,"no",COLOR_WHITE,0);
                        data->choices[0].position = {NATIVE_GAME_WIDTH/2 - NATIVE_GAME_WIDTH/5 - data->choices[0].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                        data->choices[1].position = {NATIVE_GAME_WIDTH/2 + NATIVE_GAME_WIDTH/5 - data->choices[1].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                        data->choice_count=2;

                        data->instr++;
                    }
                } else if (data->instr == 2) {
                    if (data->gameplay_state == GmDinnerData::CHOICE) {
                        if (DinnerChoice(data->choices[0].get_draw_rect())) {
                            data->gameplay_state = GmDinnerData::GAMEPLAY;
                            Mix_PlayChannel(0,game_state->dinner_milkshake_give_reasons,0);
                            data->can_move = false;
                            data->instr++;
                        } else if (DinnerChoice(data->choices[1].get_draw_rect())) {
                            data->gameplay_state = GmDinnerData::GAMEPLAY;
                            Mix_PlayChannel(0,game_state->dinner_milkshake_please_leave,0);
                            Mix_HaltMusic();
                            data->instr++;
                        }
                    }
                } else if (data->instr == 3) {
                    if (Mix_Playing(0) == false) {
                        data->gameplay_state = GmDinnerData::MILKSHAKE_SELECT;
                        for (i32 ind=0; ind<16; ind++) {
                            auto &text = data->milkshake_text[ind];
                            text = generate_text_obj(game_state->font,milkshake_options[ind],COLOR_WHITE,0);
                            text.position = {12, 20 + ind*24};
                        }
                        
                        data->instr++;
                    }
                } else if (data->instr == 4) {
                    if (data->gameplay_state == GmDinnerData::MILKSHAKE_SELECT) {
                        if (data->dragging_milkshake_scrollbar == false) {
                            v2i mpos = GetMousePositionIngame();
                            if (input->mouse_just_pressed) {
                                i32 scroll_render_value = data->how_scrolled_on_milkshake_select/3;
                                scroll_render_value = MIN(scrollbar_full.h-16,scroll_render_value);
                                scroll_render_value = MAX(scroll_render_value,0);

                                
                                iRect scrollbar_rect = {scrollbar_full.x,scrollbar_full.y+scroll_render_value,scrollbar_full.w,16};
                                if (rect_contains_point(scrollbar_rect,mpos)) {
                                    data->dragging_milkshake_scrollbar=true;
                                }

                                i32 text_render_range = data->milkshake_text[15].get_draw_rect().y+24 - data->milkshake_text[0].get_draw_rect().y - NATIVE_GAME_HEIGHT + 52;
                                i32 text_scroll_ratio = (i32)((double)text_render_range * ((double)scroll_render_value/(double)(scrollbar_full.h-16)));

                                iRect submit_rect = {NATIVE_GAME_WIDTH/2 - 32, NATIVE_GAME_HEIGHT - 32, 64, 16};
                                submit_rect.y += text_render_range;
                                submit_rect.y -= text_scroll_ratio;
                                iRect submit_src = {16,96,48,16};

                                if (DinnerChoice(submit_rect)) {
                                    bool all_selected=true;
                                    for (i32 selection_ind=0; selection_ind<3; selection_ind++) {
                                        if (data->milkshake_selections[selection_ind] == -1) {
                                            all_selected = false;
                                            break;
                                        }
                                    }
                                    
                                    if (all_selected) {
                                        data->gameplay_state = GmDinnerData::GAMEPLAY;
                                        data->instr++;
                                        Mix_PlayChannel(0,GetChunk("res/sound/dinner_milkshake_reason_reaction.ogg"),0);
                                    } else {
                                        Mix_PlayChannel(3,GetChunk("res/sound/dinner_wrong_buzzer.ogg"),0);
                                        Mix_VolumeChunk(GetChunk("res/sound/dinner_wrong_buzzer.ogg"),15);
                                    }
                                }
                            }
                        } else {
                            if (input->mouse_just_released) {
                                data->dragging_milkshake_scrollbar=false;
                                data->how_scrolled_on_milkshake_select=MIN((scrollbar_full.h-16)*3,data->how_scrolled_on_milkshake_select);
                                data->how_scrolled_on_milkshake_select=MAX(0,data->how_scrolled_on_milkshake_select);
                            } else {
                                data->how_scrolled_on_milkshake_select += input->mouseYMotion;
                            }
                        }
                    }
                } else if (data->instr == 5) {
                    if (Mix_Playing(0) == false) {
                        data->instr++;
                        data->choices[0] = generate_text_obj(game_state->font,"Book",COLOR_WHITE,0);
                        data->choices[1] = generate_text_obj(game_state->font,"Can't wait",COLOR_WHITE,0);
                        data->choices[0].position = {NATIVE_GAME_WIDTH/2 - NATIVE_GAME_WIDTH/5 - data->choices[0].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                        data->choices[1].position = {NATIVE_GAME_WIDTH/2 + NATIVE_GAME_WIDTH/5 - data->choices[1].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                        data->choice_count = 2;
                        data->gameplay_state = GmDinnerData::CHOICE;
                    }
                } else if (data->instr == 6) {
                    if (DinnerChoice(data->choices[0].get_draw_rect())) {
                        data->instr++;
                        Mix_PlayChannel(0,GetChunk("res/sound/dinner_after_milkshake_book_reaction.ogg"),0);
                        data->gameplay_state = GmDinnerData::GAMEPLAY;
                    } else if (DinnerChoice(data->choices[1].get_draw_rect())) {
                        // we getting silly w it with the branching instruction pointer!!!
                        data->instr += 2;
                        Mix_PlayChannel(0,GetChunk("res/sound/dinner_yahtzee_wait_for_it.ogg"),0);
                        data->gameplay_state = GmDinnerData::GAMEPLAY;
                    }
                } else if (data->instr == 7) {
                    if (Mix_Playing(0) == false) {
                        data->instr++;
                        Mix_PlayChannel(0,GetChunk("res/sound/dinner_yahtzee_wait_for_it.ogg"),0);
                    }
                } else if (data->instr == 8) {
                    if (Mix_Playing(0) == false) {
                        data->instr++;
                    }
                } else if (data->instr == 9) {
                    v2i points[] = {{30,27},{31,29},{31,33},{22,33},{22,28},{26,28}};
                    local_persist i32 point=0;
                    local_persist bool backwards=false;
                    v2 host = {(float)data->host_x,(float)data->host_y};
                    if (distance_between(host,v2((float)points[point].x,(float)points[point].y)) < 0.1) {
                        if (backwards == false) {
                            if (point >= 5) {
                                backwards = true;
                                data->host_object = GmDinnerData::GO_YAHTZEE;
                            } else {
                                point++;
                            }
                        } else {
                            point--;
                            if (point < 0) {
                                data->instr++;
                            }
                        }
                    } else {
                        v2 vec = vec_to(host,v2((float)points[point].x,(float)points[point].y)) * timestep * 5.0;
                        data->host_x += (double)vec.x;
                        data->host_y += (double)vec.y;
                    }
                } else if (data->instr == 10) {
                    data->instr++;
                    Mix_PlayChannel(0,GetChunk("res/sound/dinner_yahtzee.ogg"),0);
                }
            }
        }
    }

    // spatial audio for dog whimpering
    v2 dogpos = {25,37};
    float angle_rad = angle_to(player,dogpos) - PIf/2;
    i16 angle = (i16)rad_2_deg(angle_rad);

    float dist = distance_between(player, dogpos);

    v2 dir = {(float)dir_x,(float)dir_y};
    angle = (i16)rad_2_deg(vec_to_angle(dir));

    float min_volume=0.0f;
    float max_volume=0.85f;
    float min_volume_radius=10.0f;
    float max_volume_radius=0.f;

    if (dist < min_volume_radius) {
        dist /= min_volume_radius;

        Mix_SetPosition(2, angle, u8(dist*255.f));
        Mix_VolumeChunk(game_state->dinner_dog_whimpering,128);
    } else {
        Mix_VolumeChunk(game_state->dinner_dog_whimpering,0);
    }
    
    // update world
    if (data->host_task == GmDinnerData::GETTING_DOOR) {
        door_opened_timer += timestep;
        if (door_state == GmDinnerData::DOOR_KNOCKED) {
            if (door_opened_timer >= 2.f) {
                dinner_world_map[door_pos.x][door_pos.y] = 0;
                data->host_x = 35;
                data->host_y = 28;
                door_state = GmDinnerData::DOOR_OPENING;
                door_opened_timer=0;
                Mix_PlayChannel(1,GetChunk("res/sound/dinner_front_door_open.ogg"),0);
            }
            
        } else if (door_state == GmDinnerData::DOOR_OPENING) {
            if (door_opened_timer >= 0.75f) {
                door_opened_timer=0;
                door_state = GmDinnerData::SLIDE_IN_FRONT;
            }
        } else if (door_state == GmDinnerData::SLIDE_IN_FRONT) {
            double timer_len = 0.5;
            double timer = MIN(timer_len,door_opened_timer/timer_len);
            data->host_x = 35 + timer*2.0;
            data->host_y = 28.5 + timer*2.0;
            if (door_opened_timer >= timer_len) {
                door_state = GmDinnerData::SPEAK;
                Mix_PlayChannel(0,game_state->dinner_greeting,0);
                Mix_PlayMusic(game_state->dinner_jazz_music,-1);
                Mix_VolumeMusic(40);
                door_opened_timer=0;
            }
        } else if (door_state == GmDinnerData::SPEAK) {
            if (Mix_Playing(0) == false) {
                door_state = GmDinnerData::DOOR_CLOSED;
                data->gameplay_state = GmDinnerData::CHOICE;
                data->choices[0] = generate_text_obj(game_state->font,"yes",COLOR_WHITE,0);
                data->choices[1] = generate_text_obj(game_state->font,"no",COLOR_WHITE,0);
                data->choices[0].position = {NATIVE_GAME_WIDTH/2 - NATIVE_GAME_WIDTH/5 - data->choices[0].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                data->choices[1].position = {NATIVE_GAME_WIDTH/2 + NATIVE_GAME_WIDTH/5 - data->choices[1].get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                data->choice_count=2;

            }
        }
        
        if (data->gameplay_state == GmDinnerData::CHOICE) {
            if (DinnerChoice(data->choices[0].get_draw_rect())) {
                data->gameplay_state = GmDinnerData::GAMEPLAY;
                Mix_PlayChannel(0,game_state->dinner_yes_to_drink,0);
                data->host_state = GmDinnerData::SPEAKING;
                data->host_task = GmDinnerData::NONE;
            } else if (DinnerChoice(data->choices[1].get_draw_rect())) {
                data->gameplay_state = GmDinnerData::GAMEPLAY;
                Mix_PlayChannel(0,game_state->dinner_no_to_drink,0);
                data->host_state = GmDinnerData::SPEAKING;
                data->host_task = GmDinnerData::NONE;
            }
        } 
    }

    if (data->host_state == GmDinnerData::SPEAKING) {
        if (Mix_Playing(0) == false) {
            data->host_state = GmDinnerData::TRAVELLING;
        }
    } if (data->host_state == GmDinnerData::TRAVELLING) {
        v2i points[] = {{30,29},{30,25},{34,25}};
        local_persist i32 point=0;
        v2 host = {(float)data->host_x,(float)data->host_y};
        if (distance_between(host,v2((float)points[point].x,(float)points[point].y)) < 0.1) {
            point++;
            if (point >= 3) {
                data->host_state = GmDinnerData::SILENT;
            }
        } else {
            v2 vec = vec_to(host,v2((float)points[point].x,(float)points[point].y)) * timestep * 5.0;
            data->host_x += (double)vec.x;
            data->host_y += (double)vec.y;
        }
    }

    // align host object to host
    if (data->host_object != GmDinnerData::GO_NONE) {
        auto &host_object = data->world_objects[data->host_object];
        host_object.pos = {(float)data->host_x,(float)data->host_y};
        host_object.pos += {(float)-dir_x/10.0f,(float)-dir_y/10.0f};
    }
}


void DrawGmDinner() {
    GmDinnerData *data = &game_state->gm_dinner_data;
    
    glBindFramebuffer(GL_FRAMEBUFFER,game_state->game_framebuffer);
    glViewport(0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    double dir_x = data->dir_x;
    double dir_y = data->dir_y;
    double player_x = data->player_x;
    double player_y = data->player_y;
    double plane_x = data->plane_x;
    double plane_y = data->plane_y;
    auto &world_objects = data->world_objects;

    glBindFramebuffer(GL_FRAMEBUFFER,game_state->game_framebuffer);
    
    UseShader(&game_state->colorShader);
    game_state->colorShader.UniformColor("color",COLOR_BLACK);
    game_state->colorShader.UniformM4fv("projection",game_state->projection);//glm::ortho(0.0f, static_cast<float>(NATIVE_GAME_WIDTH), static_cast<float>(NATIVE_GAME_HEIGHT), 0.0f, -1.0f, 1.0f));//game_state->projection);
    game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));
    GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

    UseShader(&game_state->textureShader);
    game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
    game_state->textureShader.UniformM4fv("projection",game_state->projection);
    game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));

    // crude and disgusting
    i32 object_order[GmDinnerData::GO_COUNT];
    for (i32 ind=0; ind<GmDinnerData::GO_COUNT; ind++) {
        object_order[ind] = ind;
    }
    v2 player = {(float)player_x,(float)player_y};
    // this code works but is extremely slow and terrible
    for (i32 pos=0; pos<GmDinnerData::GO_COUNT; pos++) {
        v2 object = data->world_objects[object_order[pos]].pos;
        double closest_dist = distance_between(player,object);
        i32 closest_ind = pos;
        for (i32 ind=pos+1; ind<GmDinnerData::GO_COUNT; ind++) {
            double dist = distance_between(player,world_objects[object_order[ind]].pos);
            if (dist > closest_dist) {
                closest_dist = dist;
                closest_ind = ind;
            }
        }
        i32 temp = object_order[closest_ind];
        object_order[closest_ind] = object_order[pos];
        object_order[pos] = temp;
    }

    // go through every verticle stripe and cast a ray in that direction, outwards from the player
    for (int x=0; x<NATIVE_GAME_WIDTH; x++) {
        double normalized_coord = (2 * x) / ((double)(NATIVE_GAME_WIDTH)) - 1.0;
        double point_on_plane_x = player_x+plane_x*normalized_coord;
        double point_on_plane_y = player_y+plane_y*normalized_coord;

        double target_x = dir_x + plane_x * normalized_coord;
        double target_y = dir_y + plane_y * normalized_coord;

        int collision=0;

        int map_x = (int)player_x;
        int map_y = (int)player_y;

        // The distance the ray must travel to take 1 x or 1 y step
        // For example, if the angle is pointing almost totally upwards,
        // the ray would need to travel a great distance to move 1 full x step
        double delta_dist_x = target_x == 0 ? 1e30 : abs(1.0 / target_x);
        double delta_dist_y = target_y == 0 ? 1e30 : abs(1.0 / target_y);

        // how far we are from the grid based on the start point
        double side_dist_x = 0;
        double side_dist_y = 0;

        int step_x, step_y;
        int side=0;

        if (target_x >= 0) {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - player_x) * delta_dist_x;
        } else {
            step_x = -1;
            side_dist_x = (player_x - map_x) * delta_dist_x;
        }

        if (target_y >= 0) {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - player_y) * delta_dist_y;
        } else {
            step_y = -1;
            side_dist_y = (player_y - map_y) * delta_dist_y;
        }

        double perp_distance = 0.0;
        double prev_wall_height=0;
        i32 prev_wall_top=0;
        i32 collision_count=0;

        bool just_collided=false;

        while (true) {
            // the side_dist_x/y variables are incremented throughout the DDA
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                if (just_collided == false)
                    map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                if (just_collided == false)
                    map_y += step_y;
                side = 1;
            }
            if (map_x < 0 || map_x >= DINNER_MAP_WIDTH || map_y < 0 || map_y >= DINNER_MAP_HEIGHT)
                break;
            if (just_collided || (data->dinner_world_map[map_x][map_y] != 0 && data->dinner_world_map[map_x][map_y] != 4 && data->dinner_world_map[map_x][map_y] != 5)) {
                // collision
                collision = data->dinner_world_map[map_x][map_y];

                double col_x;
                int texture_x_coord;

                if (side == 0) {
                    perp_distance = side_dist_x - delta_dist_x;
                    col_x = player_y + perp_distance * target_y;
                } else {
                    perp_distance = side_dist_y - delta_dist_y;
                    col_x = player_x + perp_distance * target_x;
                }


                col_x -= floor(col_x);

                // flip the texture if its the right or back side of it
                if (player_y > map_y+1 && side == 1 || player_x < map_x && side == 0) {
                    col_x = -(col_x - 0.5) + 0.5;
                }
                
                double wall_height=0.0;
                iRect src_rect = {0,0,32,32};
                if (collision == 1) {
                    src_rect = {96,0,32,32};
                } else if (collision == 2) {
                    src_rect = {0,0,32,32};
                } else if (collision == 3 || collision == 13) {
                    src_rect = {64,0,32,32};
                    
                } else if (collision == 6) {
                    src_rect = {160,0,32,32};
                } else if (collision == 7) {
                    src_rect = {192,20,32,12};
                    //wall_height = 12.0 / 32.0;
                } else if (collision == 8) {
                    src_rect = {224,0,32,32};
                    // scaffolding
                } else if (collision == 9) {
                    src_rect = {192,34,32,30};
                } else if (collision == 10) {
                    src_rect = {128,52,32,12};
                    //wall_height = 30.0 / 32.0;
                } else if (collision == 11) {
                    src_rect = {0,54,32,10};
                } else if (collision == 12) {
                    src_rect = {32,32,32,32};
                }

                if (wall_height == 0.0) {
                    wall_height = src_rect.h / 32.0;
                }

                texture_x_coord = int(col_x * (double)src_rect.w);

                double full_wall_height = (NATIVE_GAME_HEIGHT/perp_distance);
                i32 projected_wall_height = (i32)(full_wall_height * wall_height);

                i32 wall_bottom, wall_top;

                wall_bottom = (i32)(NATIVE_GAME_HEIGHT/2.0+full_wall_height/2.0);
                wall_top = (i32)(wall_bottom - projected_wall_height);

                bool continuation = false;
                if (collision_count) {
                    if (prev_wall_top < wall_bottom) {
                        wall_bottom = prev_wall_top;
                        continuation = true;
                    }
                }

                if (prev_wall_top != 0 && wall_top > prev_wall_top) {
                    goto ray_draw_finish;
                }

                // render top of wall
                if (just_collided) {
                    // only draw ceiling if the second point is above the first point
                    iRect rect = {x,(i32)(wall_top),1,(i32)(prev_wall_top-wall_top)};
                    if (rect.y < prev_wall_top) {
                        UseShader(&game_state->colorShader);
                    
                        double min_dist = 6.0;
                        double max_dist = data->max_view_distance;
                        double dist_val = (max_dist - MAX(0,perp_distance - min_dist)) / max_dist;
                        dist_val = MAX(0,dist_val);
                        u8 dist_col = (u8)(dist_val * 255);
                        dist_col = MAX(dist_col,35);

                        Color col = Color::hexToColor(0x411606ff);

                        game_state->colorShader.UniformColor("color",col);
                    
                        GL_DrawRect(rect);
                        UseShader(&game_state->textureShader);
                    }
                    
                } else {
                    i32 absolute_wall_height = wall_bottom-wall_top;

                    game_state->textureShader.Uniform1i("_texture",GetTexture("res/imgs/dinner_tiles.png"));

                    double min_dist = 6.0;
                    double max_dist = data->max_view_distance;
                    double dist_val = (max_dist - MAX(0,perp_distance - min_dist)) / max_dist;
                    dist_val = MAX(0,dist_val);
                    u8 dist_col = (u8)(dist_val * 255);
                    dist_col = MAX(dist_col,35);
                    game_state->textureShader.UniformColor("colorMod",Color(dist_col,dist_col,dist_col,255));

                    if (absolute_wall_height < projected_wall_height && prev_wall_top >= wall_bottom) {
                        src_rect = {src_rect.x+texture_x_coord,src_rect.y,1,src_rect.h};
                        iRect rect = {x,wall_top,1,projected_wall_height};

                        glBindFramebuffer(GL_FRAMEBUFFER,game_state->dinner_line_framebuffer);
                        glViewport(0,0,1,NATIVE_GAME_HEIGHT*2);
                        glm::mat4 line_projection = glm::ortho(0.f,1.f,0.f,(float)NATIVE_GAME_HEIGHT*2);
                        // god only knows WHY the 
                        game_state->textureShader.UniformM4fv("projection",line_projection);
                        GL_DrawTexture(src_rect,{0,0,1,projected_wall_height});

                        glBindFramebuffer(GL_FRAMEBUFFER,game_state->game_framebuffer);
                        glViewport(0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT);
                        
                        game_state->textureShader.UniformM4fv("projection",game_state->projection);
                        game_state->textureShader.Uniform1i("_texture",game_state->dinner_line_framebuffer_texture);
                        GL_DrawTexture({0,0,1,absolute_wall_height},{x,wall_top,1,absolute_wall_height},false,false);
                    } else {
                        src_rect = {src_rect.x+texture_x_coord,src_rect.y,1,src_rect.h};
                        iRect rect = {x,wall_top,1,absolute_wall_height};
                        GL_DrawTexture(src_rect,rect);
                    }
                }

        ray_draw_finish:
                prev_wall_height = wall_height;
                if (collision_count) {
                    prev_wall_top = MIN(prev_wall_top,wall_top);
                } else {
                    prev_wall_top = wall_top;
                }
                collision_count++;

                if (wall_height == 1.0 || prev_wall_top <= 0) {
                    break;
                }
                if (just_collided) {
                    just_collided = false;
                    if (side == 0) {
                        side_dist_x -= delta_dist_x;
                    } else {
                        side_dist_y -= delta_dist_y;
                    }
                } else {
                    just_collided = true;
                }
            } else {
                just_collided = false;
            }
        }

        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);

        for (i32 ind=0;ind<GmDinnerData::GO_COUNT;ind++) {
            i32 obj=object_order[ind];
            v2 object=world_objects[obj].pos;
            // see if the ray collides with the world object, and if there is a wall collision, check if
            // the collision to the object is closer than the collision to the wall
            bool collides = false;
            double tex_size=0.0;

            v2 object_size = {1.f,1.f};
            v2i tex_begin={0,0};
            
            if (obj == GmDinnerData::GO_TV) {
                tex_size = 128;
                tex_begin.x = data->world_objects[GmDinnerData::GO_TV].on * (i32)tex_size;
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                object_size *= 1.5f;
            } else if (obj == GmDinnerData::GO_CHINA) {
                tex_size = 128;
                tex_begin = {0,128};
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                object_size *= 0.5f;
            } else if (obj == GmDinnerData::GO_HOST) {
                tex_size = 640;
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_host_texture);
                object_size.x = 2.0f;
                //width = object_size*2.0;
            } else if (obj == GmDinnerData::GO_WRENCH) {
                tex_size = 128;
                tex_begin = {0,384};
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                object_size *= 0.25f;
            } else if (obj == GmDinnerData::GO_YAHTZEE) {
                tex_size = 128;
                tex_begin = {0,512};
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                object_size *= 0.25f;
            } else if (obj == GmDinnerData::GO_AQUARIUM) {
                tex_size = 128;
                tex_begin = {0,640};
                game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                object_size *= 0.5f;
            }

            
            v2 line_end = player + v2({(float)target_x,(float)target_y});
            v2 col = closest_point_on_line(player,line_end,object);
            float dist_from_ray_to_obj = distance_between(col,object);
            if (dist_from_ray_to_obj < object_size.x/2.0f) {
                // scuffed way of making sure the ray doesn't intersect out the wrong side
                bool correct_direction = distance_between(col,line_end) < distance_between(col,player);
                if (correct_direction) {
                    collides = true;
                }
            }

            // TODO: fix fisheye effect when looking at object
            // This originates from the fact that the sprite is perpendicular to every ray,
            // when it should be fixed as perpendicular to the player's true look angle
            double obj_distance = distance_between(player,object);
            if (collides && (collision == false || obj_distance < perp_distance)) {
                float angle_to_obj = angle_to(player,object);
                float angle_to_target = angle_to(player,line_end);
                double col_x;
                if (angle_to_obj-angle_to_target > PIf) {
                    angle_to_obj += PIf*2;
                } if (angle_to_target-angle_to_obj > PIf) {
                    angle_to_target += PIf*2;
                }
                if (angle_to_obj > angle_to_target) {
                    col_x = 0.5 - (dist_from_ray_to_obj/(object_size.x));
                } else {
                    col_x = 0.5 + (dist_from_ray_to_obj/(object_size.x));
                }

                int texture_x_coord = int(tex_begin.x + col_x * tex_size);

                double dist_to_obj = (double)distance_between(player,object);

                // replace this with the height of the distance from the player to the center of the object
                // that way the height is constant for all angles

                double full_tile_height = (i32)(NATIVE_GAME_HEIGHT/dist_to_obj);

                // all this should only be calculated once, its all the same
                // outputs regardless of which ray is cast
                
                // get the tile its resting on
                v2i map_pos = {(i32)(object.x),(i32)(object.y)};
                
                i32 tile_height = 0;

                if (obj == data->host_object) {
                    tile_height = (i32)(0.3 * full_tile_height);
                } else if (obj == GmDinnerData::GO_HOST) {
                    tile_height = 0;
                } else {
                    i32 tile = data->dinner_world_map[map_pos.x][map_pos.y];
                    if (tile != 0 && tile != 3) {
                        double height_in_tiles=1.0;
                        if (tile == 7) {
                            height_in_tiles = 12.0/32.0;
                        } else if (tile == 10) {
                            height_in_tiles = 12.0/32.0;                            
                        } else if (tile == 11) {
                            height_in_tiles = 10.0/32.0;                            
                        }
                        tile_height = (i32)(full_tile_height * height_in_tiles);
                    }
                }
                i32 tile_bottom = (i32)(NATIVE_GAME_HEIGHT/2.0+full_tile_height/2.0);
                i32 tile_top = (i32)(tile_bottom - tile_height);

                int object_height = (int)(full_tile_height * object_size.y);
                iRect rect = {x,tile_top-object_height,1,object_height};
                iRect src_rect = {(int)(texture_x_coord),tex_begin.y,1,(i32)tex_size};
                GL_DrawTexture(src_rect,rect);

                /*
                i32 object_height = (i32)(wall_height * object_size);
                object_height = MIN(object_height,wall_height);
                iRect rect = {x,tile_bottom-object_height,1,object_height};
                iRect src_rect = {(int)(texture_x_coord),tex_begin.y,1,(i32)tex_size};
                GL_DrawTexture(src_rect,rect);
                */
            }
        }
    }

    if (data->host_task == GmDinnerData::WAITING_FOR_MILKSHAKE) {
        if (data->host_milkshake_state == GmDinnerData::MK_GIVE_MILKSHAKE) {
            game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
            iRect final_milkshake_position;
            final_milkshake_position.w = 128;
            final_milkshake_position.h = 128;
            final_milkshake_position.x = NATIVE_GAME_WIDTH/2 - final_milkshake_position.w/2;
            final_milkshake_position.y = NATIVE_GAME_HEIGHT/2 - final_milkshake_position.h/2;

            iRect src_rect = {0,256,128,128};
            iRect rect;
            double milkshake_spin_start_time = milkshake_give_length + milkshake_freeze_length;

            if (data->timer < milkshake_give_length + milkshake_freeze_length) {
                float lerp_val = MIN(1.0f,(float)(data->timer / milkshake_give_length));
                i32 scale = (i32)lerp(0.f,(float)final_milkshake_position.w,lerp_val);
                i32 curr_x = NATIVE_GAME_WIDTH/2-scale/2;
                i32 curr_y = NATIVE_GAME_HEIGHT/2-scale/2 - 10;
                rect = {curr_x,curr_y,scale,scale};
            } else if (data->timer < milkshake_spin_start_time+milkshake_spin_length) {
                rect = {final_milkshake_position.x,final_milkshake_position.y,final_milkshake_position.w,final_milkshake_position.h};
                
                // spin uncontrollably!!!
                double timer = data->timer - milkshake_spin_start_time;
                double rot_speed = PI*4;
                game_state->textureShader.UniformM4fv("model",rotate_model_matrix((float)(rot_speed*timer),rect));
            } else if (data->timer < milkshake_give_length_total) {
                double timer = data->timer - (milkshake_spin_start_time+milkshake_spin_length);
                float lerp_val = MIN(1.0f,(float)(timer / milkshake_spin_to_player_length));
                i32 scale = (i32)lerp((float)final_milkshake_position.w,(float)final_milkshake_position.w*2,lerp_val);
                i32 curr_x = NATIVE_GAME_WIDTH/2-scale/2;
                i32 curr_y = (i32)lerp(final_milkshake_position.y,NATIVE_GAME_HEIGHT+16,lerp_val);
                rect = {curr_x,curr_y,scale,scale};
            }
            
            GL_DrawTexture(src_rect,rect);
            game_state->textureShader.UniformM4fv("model",glm::mat4(1.0));
            
            //GL_DrawTexture
        }
    }

    // choice text
    if (data->gameplay_state == GmDinnerData::CHOICE) {
        if (data->choice_count) {
            for (i32 i=0;i<data->choice_count;i++) {
                generic_drawable *choice = &data->choices[i];
                game_state->textureShader.Uniform1i("_texture",choice->gl_texture);
                GL_DrawTexture({0,0,0,0},choice->get_draw_rect());
            }
        }
    } else if (data->gameplay_state == GmDinnerData::ENTERING_CODE) {
        UseShader(&game_state->colorShader);
        game_state->colorShader.UniformColor("color",Color(8,3,0,200));
        GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

        UseShader(&game_state->textureShader);

        if (input->mouse_just_released) {
            data->digit_clicked=-1;
        }
        
        for (i32 i=0; i<12; i++) {
            auto &code=data->keypad_codes[i];
            game_state->textureShader.Uniform1i("_texture",GetTexture("res/imgs/dinner_tiles.png"));
            i32 x_dest = (NATIVE_GAME_WIDTH - 12*16)/2 + i*16;
            iRect dest = {x_dest,48,16,16};
            GL_DrawTexture({0,64,16,16},dest);

            // up and down arrows
            iRect up_arrow_src = {112,96,16,8};
            iRect down_arrow_src = {112,96,16,8};
            iRect up_arrow_dest = {dest.x,dest.y-10,16,8};
            iRect down_arrow_dest = {dest.x,dest.y+dest.h+2,16,8};

            bool generate_new_texture=false;
            
            v2i mpos = GetMousePositionIngame();
            if (rect_contains_point(up_arrow_dest,mpos)) {
                up_arrow_src.y += 8;
                if (input->mouse_just_pressed) {
                    data->digit_clicked = i;
                    data->digit_click_direction = 1;
                    generate_new_texture=true;
                }
            }
            if (rect_contains_point(down_arrow_dest,mpos)) {
                down_arrow_src.y += 8;
                if (input->mouse_just_pressed) {
                    data->digit_clicked = i;
                    data->digit_click_direction = -1;
                    generate_new_texture=true;
                }
            }

            if (data->digit_clicked == i) {
                if (data->digit_click_direction == 1) {
                    up_arrow_src.y = 112;
                } else {
                    down_arrow_src.y = 112;
                }
            }

            GL_DrawTexture(up_arrow_src,up_arrow_dest);
            GL_DrawTexture(down_arrow_src,down_arrow_dest,false,true);

            if (generate_new_texture) {
                i32 new_num = data->code_values[i] + data->digit_click_direction;
                if (new_num >= 10) {
                    new_num = 0;
                } else if (new_num < 0) {
                    new_num = 9;
                }
                data->code_values[i] = new_num;
                code = generate_text_obj(game_state->font,std::to_string(new_num),COLOR_BLACK,code.gl_texture);
                code.position.x = dest.x+8-(code.get_draw_rect().w/2);
                code.position.y = 48;
            }


            game_state->textureShader.Uniform1i("_texture",code.gl_texture);
            GL_DrawTexture({0,0,0,0},code.get_draw_rect());
        }
        
    } else if (data->gameplay_state == GmDinnerData::MILKSHAKE_SELECT) {
        
        UseShader(&game_state->colorShader);
        game_state->colorShader.UniformColor("color",Color(8,3,0,200));
        GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});
        GL_DrawRect(scrollbar_full);
        UseShader(&game_state->textureShader);

        i32 scroll_render_value = data->how_scrolled_on_milkshake_select / 3;
        scroll_render_value = MIN(scrollbar_full.h-16,scroll_render_value);
        scroll_render_value = MAX(scroll_render_value,0);

        v2i mpos = GetMousePositionIngame();
        // text render range
        i32 text_render_range = data->milkshake_text[15].get_draw_rect().y+24 - data->milkshake_text[0].get_draw_rect().y - NATIVE_GAME_HEIGHT + 52;
        i32 text_scroll_ratio = (i32)((double)text_render_range * ((double)scroll_render_value/(double)(scrollbar_full.h-16)));

        auto &milkshake_selections = data->milkshake_selections;

        bool all_selected=true;
        for (i32 selection_ind=0; selection_ind<3; selection_ind++) {
            if (milkshake_selections[selection_ind] == -1) {
                all_selected = false;
                break;
            }
        }

        for (i32 ind=0; ind<16; ind++) {
            auto text = data->milkshake_text[ind];
            iRect dest = text.get_draw_rect();
            dest.y -= text_scroll_ratio;

            iRect mRect = dest;
            mRect.x -= 4;
            mRect.y -= 2;
            mRect.h += 4;
            mRect.w = (scrollbar_full.x-4 - mRect.x);

            bool selected = false;
            for (i32 selection_ind=0; selection_ind<3; selection_ind++) {
                if (milkshake_selections[selection_ind] == ind) {
                    selected = true;
                    break;
                }
            }

            bool hover = rect_contains_point(mRect,mpos);
            if (hover) {
                if (input->mouse_just_pressed) {
                    bool placed=false;
                    for (i32 selection_ind=0; selection_ind<3; selection_ind++) {
                        if (milkshake_selections[selection_ind] == ind) {
                            milkshake_selections[selection_ind] = -1;
                            placed = true;
                            selected = false;
                            break;
                        }
                    } if (!placed) {
                        for (i32 selection_ind=0; selection_ind<3; selection_ind++) {
                            if (milkshake_selections[selection_ind] == -1) {
                                milkshake_selections[selection_ind] = ind;
                                placed = true;
                                selected = true;
                                break;
                            }
                        }
                    }
                    if (!placed) {
                        milkshake_selections[2] = ind;
                        selected = true;
                    }
                }
            } if (hover || selected) {
                UseShader(&game_state->colorShader);
                Color box_col = Color(0,0,0,220);
                if (selected) {
                    // draw outline as well
                    Color outline_col = Color(255,150,0,255);
                    if (all_selected) {
                        outline_col = Color(25,150,40,255);
                    }
                    game_state->colorShader.UniformColor("color",outline_col);
                    GL_DrawRect({mRect.x,mRect.y,mRect.w,1});
                    GL_DrawRect({mRect.x,mRect.y+mRect.h,mRect.w,1});
                    GL_DrawRect({mRect.x,mRect.y,1,mRect.h});
                    GL_DrawRect({mRect.x+mRect.w,mRect.y,1,mRect.h+1});
                    
                    box_col = Color(255,255,255,100);
                }
                game_state->colorShader.UniformColor("color",box_col);
                GL_DrawRect(mRect);
                UseShader(&game_state->textureShader);
            }
            
            game_state->textureShader.Uniform1i("_texture",text.gl_texture);
            GL_DrawTexture({0,0,0,0},dest);
        }

        iRect scrollbar_rect = {scrollbar_full.x,scrollbar_full.y+scroll_render_value,scrollbar_full.w,16};
        game_state->textureShader.Uniform1i("_texture",game_state->dinner_tiles_texture);
        GL_DrawTexture({0,96,8,16},scrollbar_rect);

        iRect submit_rect = {NATIVE_GAME_WIDTH/2 - 32, NATIVE_GAME_HEIGHT - 32, 64, 16};
        submit_rect.y += text_render_range;
        submit_rect.y -= text_scroll_ratio;
        iRect submit_src = {16,96,48,16};

        if (rect_contains_point(submit_rect,mpos)) {
            submit_src.x = 64;
        } if (all_selected) {
            submit_src.y = 112;
        }
        
        GL_DrawTexture(submit_src,submit_rect);

    } else if (data->gameplay_state == GmDinnerData::GAMEPLAY) {
        if (data->hover_object != GmDinnerData::GO_NONE && (data->hover_object != GmDinnerData::GO_HOST || data->can_interact_with_host)) {
            game_state->textureShader.Uniform1i("_texture",game_state->dinner_mouse_icons_texture);
            i32 size = 32;
            iRect src_rect = {0,0,16,16};
            if (data->hover_object == GmDinnerData::GO_DOOR) {
                src_rect.x = 16;
            }
            GL_DrawTexture(src_rect,{NATIVE_GAME_WIDTH/2-size/2,NATIVE_GAME_HEIGHT/2-size/2,size,size});
        }
    }
}

