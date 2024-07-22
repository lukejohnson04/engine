
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

void InitGmDinner() {
    GmDinnerData *data = &game_state->gm_dinner_data;
    SDL_Surface *house_layout_surface = IMG_Load("res/imgs/dinner_house.png");
    for (i32 x=0; x<DINNER_MAP_WIDTH; x++) {
        for (i32 y=0; y<DINNER_MAP_HEIGHT; y++) {
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
                game_state->gm_dinner_data.dinner_world_map[y][x] = 4;
            } else if (col == 0x474747ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 6;
            } else if (col == 0x621a00ff) {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 7;
            } else {
                game_state->gm_dinner_data.dinner_world_map[y][x] = 0;
            }
        }
    }

    data->world_objects[GmDinnerData::GO_TV].on = true;
    //DOGSOUND
    Mix_PlayChannel(2,game_state->dinner_dog_whimpering,-1);
}

internal
void DinnerOnTrigger(v2i pos) {
    
}

internal
void DinnerOnInteract(i32 obj_type,Wobject *object) {
    
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

    world_objects[GmDinnerData::GO_TV].pos = {30,25.5f};
    world_objects[GmDinnerData::GO_CHINA].pos = {29.5f,29};
    world_objects[GmDinnerData::GO_HOST].pos = {(float)data->host_x,(float)data->host_y};
    world_objects[GmDinnerData::GO_WRENCH].pos = {34.f,32.f};
    //world_objects[GmDinnerData::GO_WRENCH].ypos = 0.5f;
    

    data->hover_object = GmDinnerData::GO_NONE;        

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
    
    v2 player = {(float)player_x,(float)player_y};
    {
        // loop through and see what was interacted with
        for (i32 x=0; x<DINNER_MAP_WIDTH; x++) {
            for (i32 y=0; y<DINNER_MAP_HEIGHT; y++) {
                if (dinner_world_map[x][y] != 3) {
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
                    data->hover_object = GmDinnerData::GO_DOOR;
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

    if (data->hover_object != GmDinnerData::GO_NONE && input->just_pressed[SDL_SCANCODE_E]) {
        if (data->hover_object == GmDinnerData::GO_DOOR) {
            if (hover_obj.pos.x!=36 || hover_obj.pos.y!=29) {
                Mix_PlayChannel(1,game_state->dinner_door_lock,0);
            } else {
                Mix_PlayChannel(0,game_state->dinner_knock,0);
                door_pos=hover_obj.pos;
                door_state = GmDinnerData::DOOR_KNOCKED;
            }
        } else if (data->hover_object == GmDinnerData::GO_TV) {
            data->world_objects[GmDinnerData::GO_TV].on ^= 1;
        } else if (data->hover_object == GmDinnerData::GO_CHINA) {
            Mix_PlayChannel(0,game_state->dinner_china_interact,0);
        }
    }
    
    move_x *= move_speed;
    move_y *= move_speed;

    
    player_y += move_y;
    if (player_y > DINNER_MAP_HEIGHT-1) player_y = DINNER_MAP_HEIGHT-1;
    if (player_y < 0) player_y = 0;
    if (!data->no_clip) {
        if (move_y < 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 4 && dinner_world_map[(i32)player_x][(i32)player_y] != 5) {
            player_y = ceil(player_y);
        } else if (move_y > 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 4 && dinner_world_map[(i32)player_x][(i32)player_y] != 5) {
            player_y = floor(player_y) - 0.00001;
        }
    }

    player_x += move_x;
    if (player_x > DINNER_MAP_WIDTH-1) player_x = DINNER_MAP_WIDTH-1;
    if (player_x < 0) player_x = 0;
    if (!data->no_clip) {
        if (move_x < 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 0 && data->dinner_world_map[(i32)player_x][(i32)player_y] != 4 && data->dinner_world_map[(i32)player_x][(i32)player_y] != 5) {
            player_x = ceil(player_x);
        } else if (move_x > 0 && dinner_world_map[(i32)player_x][(i32)player_y] != 0 && data->dinner_world_map[(i32)player_x][(i32)player_y] != 4 && data->dinner_world_map[(i32)player_x][(i32)player_y] != 5) {
            player_x = floor(player_x) - 0.00001;
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
        
        if (dinner_world_map[(i32)player_x][(i32)player_y] == 4) {
            // disabled trigger
            dinner_world_map[(i32)player_x][(i32)player_y] = 0;
            data->host_x = block_doorway_startx;
            data->host_y = block_doorway_starty;
            data->host_task = GmDinnerData::BLOCKING_DOORWAY;
            data->doorway_blocking_state = GmDinnerData::DOORWAY_BLOCK_SLIDE;
            data->timer = 0;
        }

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
                static bool played_gunshot=false;
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
    //printf("%f\n",dist);
    
    // update world
    if (door_state != GmDinnerData::DOOR_CLOSED) {
        door_opened_timer += timestep;
        if (door_state == GmDinnerData::DOOR_KNOCKED) {
            if (door_opened_timer >= 2.f) {
                dinner_world_map[door_pos.x][door_pos.y] = 0;
                data->host_x = 35;
                data->host_y = 28;
                door_state = GmDinnerData::DOOR_OPENING;
                door_opened_timer=0;
                Mix_PlayChannel(1,game_state->dinner_door_open,0);
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
                data->text_yes = generate_text_obj(game_state->font,"yes",COLOR_WHITE,0);
                data->text_no = generate_text_obj(game_state->font,"no",COLOR_WHITE,0);
                data->text_yes.position = {NATIVE_GAME_WIDTH/2 - NATIVE_GAME_WIDTH/5 - data->text_yes.get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
                data->text_no.position = {NATIVE_GAME_WIDTH/2 + NATIVE_GAME_WIDTH/5 - data->text_yes.get_draw_rect().w/2,NATIVE_GAME_HEIGHT/2 + NATIVE_GAME_HEIGHT/5};
            }
        }
    } if (data->gameplay_state == GmDinnerData::CHOICE) {
        if (input->mouse_just_pressed) {
            v2i mpos = GetMousePositionIngame();
            if (rect_contains_point(data->text_yes.get_draw_rect(),mpos)) {
                data->gameplay_state = GmDinnerData::GAMEPLAY;
                Mix_PlayChannel(0,game_state->dinner_yes_to_drink,0);
                data->host_state = GmDinnerData::SPEAKING;
            } else if (rect_contains_point(data->text_no.get_draw_rect(),mpos)) {
                data->gameplay_state = GmDinnerData::GAMEPLAY;
                Mix_PlayChannel(0,game_state->dinner_no_to_drink,0);
                data->host_state = GmDinnerData::SPEAKING;
            }
        }
    } if (data->host_state == GmDinnerData::SPEAKING) {
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
}


void DrawGmDinner() {
    GmDinnerData *data = &game_state->gm_dinner_data;
    double dir_x = data->dir_x;
    double dir_y = data->dir_y;
    double player_x = data->player_x;
    double player_y = data->player_y;
    double plane_x = data->plane_x;
    double plane_y = data->plane_y;
    auto &world_objects = data->world_objects;
    
    UseShader(&game_state->colorShader);
    game_state->colorShader.UniformColor("color",COLOR_BLACK);
    game_state->colorShader.UniformM4fv("projection",game_state->projection);
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

                GLuint texture = game_state->dinner_tiles_texture;

                double wall_height=1.0;
                iRect src_rect = {0,0,32,32};
                if (collision == 1) {
                    src_rect = {96,0,32,32};
                } else if (collision == 2) {
                    src_rect = {0,0,32,32};
                } else if (collision == 3) {
                    src_rect = {64,0,32,32};
                } else if (collision == 6) {
                    src_rect = {160,0,32,32};
                } else if (collision == 7) {
                    src_rect = {192,20,32,12};
                    wall_height = 12.0 / 32.0;
                }

                texture_x_coord = int(col_x * (double)src_rect.w);

                double full_wall_height = (NATIVE_GAME_HEIGHT/perp_distance);
                double projected_wall_height = (full_wall_height * wall_height);

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

                // render top of wall
                if (just_collided) {
                    iRect rect = {x,(i32)(wall_top),1,(i32)(prev_wall_top-wall_top)};

                    UseShader(&game_state->colorShader);
                    
                    double min_dist = 6.0;
                    double max_dist = data->max_view_distance;
                    double dist_val = (max_dist - MAX(0,perp_distance - min_dist)) / max_dist;
                    dist_val = MAX(0,dist_val);
                    u8 dist_col = (u8)(dist_val * 255);
                    dist_col = MAX(dist_col,35);

                    Color col = Color(0,0,(u8)((dist_col/255.0) * 255),255);

                    game_state->colorShader.UniformColor("color",col);
                    
                    GL_DrawRect(rect);
                    UseShader(&game_state->textureShader);
                    
                } else {
                    i32 absolute_wall_height = wall_bottom-wall_top;
                    double percentage_visible = (double)absolute_wall_height / (double)projected_wall_height;
                    if (percentage_visible > 1) {
                        percentage_visible = MIN(percentage_visible,1);
                    }
                    if (!continuation) {
                        percentage_visible = 1.0;
                    }
                    i32 src_height = (i32)(src_rect.h * percentage_visible);
                    iRect rect = {x,wall_top,1,absolute_wall_height};
                    if (src_height > src_rect.h) {
                        //printf("%d\n",src_height);
                    }
                    src_rect = {src_rect.x+texture_x_coord,src_rect.y,1,src_height};

                    game_state->textureShader.Uniform1i("_texture",texture);

                    double min_dist = 6.0;
                    double max_dist = data->max_view_distance;
                    double dist_val = (max_dist - MAX(0,perp_distance - min_dist)) / max_dist;
                    dist_val = MAX(0,dist_val);
                    u8 dist_col = (u8)(dist_val * 255);
                    dist_col = MAX(dist_col,35);
                    game_state->textureShader.UniformColor("colorMod",Color(dist_col,dist_col,dist_col,255));
                    GL_DrawTexture(src_rect,rect);                
                }

                prev_wall_height = wall_height;
                prev_wall_top = wall_top;
                collision_count++;

                if (wall_height == 1.0) {
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

        /*
        if (collision) {
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


            GLuint texture = game_state->dinner_tiles_texture;

            double wall_height=1.0;
            iRect src_rect = {0,0,32,32};
            if (collision == 1) {
                src_rect = {96,0,32,32};
            } else if (collision == 2) {
                src_rect = {0,0,32,32};
            } else if (collision == 3) {
                src_rect = {64,0,32,32};
            } else if (collision == 6) {
                src_rect = {160,0,32,32};
                wall_height = 0.2;
                printf("WTF\n");
            }
            texture_x_coord = int(col_x * (double)src_rect.w);
            //src_rect.y = (i32)((1.0 - last_rendered_height) * src_rect.h);
            src_rect.h = (i32)((1.0 - last_rendered_height) * src_rect.h);

            int full_wall_height = (int)(NATIVE_GAME_HEIGHT/perp_distance);
            int projected_wall_height = (int)((NATIVE_GAME_HEIGHT/perp_distance) * wall_height);
            i32 wall_bottom = (NATIVE_GAME_HEIGHT/2+full_wall_height/2);
            i32 wall_top = wall_bottom - projected_wall_height;
            iRect rect = {x,wall_top,1,wall_bottom-wall_top};
            src_rect = {src_rect.x + (int)(texture_x_coord),0,1,src_rect.h};
            game_state->textureShader.Uniform1i("_texture",texture);

            double min_dist = 6.0;
            double max_dist = data->max_view_distance;
            double dist_val = (max_dist - MAX(0,perp_distance - min_dist)) / max_dist;
            dist_val = MAX(0,dist_val);
            u8 dist_col = (u8)(dist_val * 255);
            dist_col = MAX(dist_col,35);
            game_state->textureShader.UniformColor("colorMod",Color(dist_col,dist_col,dist_col,255));
            GL_DrawTexture(src_rect,rect);
            
            last_rendered_height = wall_height;
        }
        */

        game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);

        for (i32 ind=0;ind<GmDinnerData::GO_COUNT;ind++) {
            i32 obj=object_order[ind];
            v2 object=world_objects[obj].pos;
            // see if the ray collides with the world object, and if there is a wall collision, check if
            // the collision to the object is closer than the collision to the wall
            bool collides = false;
            float width = 2.0f;
            v2 line_end = player + v2({(float)target_x,(float)target_y});
            v2 col = closest_point_on_line(player,line_end,object);
            float dist_from_ray_to_obj = distance_between(col,object);
            if (dist_from_ray_to_obj < width/2.f) {
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
                    col_x = 0.5 - (dist_from_ray_to_obj/width);
                } else {
                    col_x = 0.5 + (dist_from_ray_to_obj/width);
                }

                double tex_size=0.0;
                v2i tex_begin={0,0};
                if (obj == GmDinnerData::GO_TV) {
                    tex_size = 128;
                    tex_begin.x = data->world_objects[GmDinnerData::GO_TV].on * (i32)tex_size;
                    game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                } else if (obj == GmDinnerData::GO_CHINA) {
                    tex_size = 128;
                    tex_begin = {0,128};
                    game_state->textureShader.Uniform1i("_texture",game_state->dinner_sprites_texture);
                } else if (obj == GmDinnerData::GO_HOST) {
                    tex_size = 640;
                    game_state->textureShader.Uniform1i("_texture",game_state->dinner_host_texture);
                } else if (obj == GmDinnerData::GO_WRENCH) {
                    tex_size = 128;
                    tex_begin = {0,384};
                    game_state->textureShader.Uniform1i("_texture",game_state->dinner_host_texture);
                }
                int texture_x_coord = int(tex_begin.x + col_x * tex_size);

                double dist_to_obj = (double)distance_between(player,object);

                // replace this with the height of the distance from the player to the center of the object
                // that way the height is constant for all angles
                int wall_height = (int)(NATIVE_GAME_WIDTH/dist_to_obj);
                double tile_height = 12.0 / 32.0;

                double full_tile_height = (NATIVE_GAME_HEIGHT/dist_to_obj);
                double projected_tile_height = (full_tile_height * tile_height);
                i32 tile_bottom = (i32)(NATIVE_GAME_HEIGHT/2.0+full_tile_height/2.0);
                i32 tile_top = (i32)(tile_bottom - projected_tile_height);

                iRect rect = {x,tile_top-wall_height,1,wall_height};
                iRect src_rect = {(int)(texture_x_coord),tex_begin.y,1,(i32)tex_size};
                GL_DrawTexture(src_rect,rect);
            }
        }
    }

    // choice text
    if (data->gameplay_state == GmDinnerData::CHOICE) {
        game_state->textureShader.Uniform1i("_texture",data->text_yes.gl_texture);
        GL_DrawTexture({0,0,0,0},data->text_yes.get_draw_rect());
        game_state->textureShader.Uniform1i("_texture",data->text_no.gl_texture);
        GL_DrawTexture({0,0,0,0},data->text_no.get_draw_rect());
    }
    
    if (data->hover_object != GmDinnerData::GO_NONE && data->hover_object != GmDinnerData::GO_HOST) {
        game_state->textureShader.Uniform1i("_texture",game_state->dinner_mouse_icons_texture);
        i32 size = 32;
        iRect src_rect = {0,0,16,16};
        if (data->hover_object == GmDinnerData::GO_DOOR) {
            src_rect.x = 16;
        }
        GL_DrawTexture(src_rect,{NATIVE_GAME_WIDTH/2-size/2,NATIVE_GAME_HEIGHT/2-size/2,size,size});
    }
}

