
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

    if (input->is_pressed[SDL_SCANCODE_LSHIFT]) {
        move_speed *= 2.0;
        rot_speed *= 2.0;
    }
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

    v2 player = {(float)player_x,(float)player_y};
    if (input->just_pressed[SDL_SCANCODE_E]) {
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
                    dinner_world_map[x][y] = 0;
                    break;
                }
            }
        }
    }
    
    move_x *= move_speed;
    move_y *= move_speed;
    
    player_y += move_y;
    if (player_y > DINNER_MAP_HEIGHT-1) player_y = DINNER_MAP_HEIGHT-1;
    if (player_y < 0) player_y = 0;
    if (move_y < 0 && dinner_world_map[(int)player_x][(int)player_y] != 0) {
        player_y = ceil(player_y);
    } else if (move_y > 0 && dinner_world_map[(int)player_x][(int)player_y]) {
        player_y = floor(player_y) - 0.00001;
    }
        
    player_x += move_x;
    if (move_x < 0 && dinner_world_map[(int)player_x][(int)player_y] != 0) {
        player_x = ceil(player_x);
    } else if (move_x > 0 && dinner_world_map[(int)player_x][(int)player_y]) {
        player_x = floor(player_x) - 0.00001;
    }
    if (player_x > DINNER_MAP_WIDTH-1) player_x = DINNER_MAP_WIDTH-1;
    if (player_x < 0) player_x = 0;

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

}


void DrawGmDinner() {
    GmDinnerData *data = &game_state->gm_dinner_data;
    double dir_x = data->dir_x;
    double dir_y = data->dir_y;
    double player_x = data->player_x;
    double player_y = data->player_y;
    double plane_x = data->plane_x;
    double plane_y = data->plane_y;
    
    UseShader(&game_state->colorShader);
    game_state->colorShader.UniformColor("color",COLOR_BLACK);
    game_state->colorShader.UniformM4fv("projection",game_state->projection);
    game_state->colorShader.UniformM4fv("model",glm::mat4(1.0f));
    GL_DrawRect({0,0,NATIVE_GAME_WIDTH,NATIVE_GAME_HEIGHT});

    UseShader(&game_state->textureShader);
    game_state->textureShader.UniformColor("colorMod",COLOR_WHITE);
    game_state->textureShader.UniformM4fv("projection",game_state->projection);
    game_state->textureShader.UniformM4fv("model",glm::mat4(1.0f));

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

        while (!collision) {
            // the side_dist_x/y variables are incremented throughout the DDA
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }
            if (map_x < 0 || map_x >= DINNER_MAP_WIDTH || map_y < 0 || map_y >= DINNER_MAP_HEIGHT)
                break;
            if (data->dinner_world_map[map_x][map_y] != 0) {
                collision = data->dinner_world_map[map_x][map_y];
                break;
            }
        }


        double perp_distance = 0.0;

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

            iRect src_rect = {0,0,32,32};
            if (collision == 2) {
                src_rect = {32,0,32,32};
            } else if (collision == 3) {
                src_rect = {64,0,32,32};                
            }
            texture_x_coord = int(col_x * (double)src_rect.w);

            int wall_height = (int)(NATIVE_GAME_WIDTH/perp_distance);
            iRect rect = {x,(NATIVE_GAME_HEIGHT-wall_height)/2,1,wall_height};
            src_rect = {src_rect.x + (int)(texture_x_coord),0,1,src_rect.h};
            game_state->textureShader.Uniform1i("_texture",texture);
            GL_DrawTexture(src_rect,rect);
        }

        // see if the ray collides with the host, and if there is a wall collision, check if
        // the collision to the host is closer than the collision to the wall
        bool host_collides = false;
        float host_width = 2.0f;
        v2 player = {(float)player_x,(float)player_y};
        v2 line_end = player + v2({(float)target_x,(float)target_y});
        v2 host = {(float)data->host_x,(float)data->host_y};
        v2 host_col = closest_point_on_line(player,line_end,host);
        float dist_to_host = distance_between(host_col,{(float)data->host_x,(float)data->host_y});
        if (dist_to_host < host_width/2.f) {
            // scuffed way of making sure the ray doesn't intersect out the wrong side
            bool correct_direction = distance_between(host_col,line_end) < distance_between(host_col,player);
            if (correct_direction) {
                host_collides = true;
            }
        }

        // TODO: fix fisheye effect when looking at host
        // This originates from the fact that the sprite is perpendicular to every ray,
        // when it should be fixed as perpendicular to the player's true look angle
        double host_perp_distance = distance_between(player,host_col);
        if (host_collides && (collision == false || host_perp_distance < perp_distance)) {
            float angle_to_host = angle_to(player,host);
            float angle_to_target = angle_to(player,line_end);
            double col_x;
            if (angle_to_host-angle_to_target > PIf) {
                angle_to_host += PIf*2;
                printf("%f %f\n",angle_to_target,angle_to_host);
            } if (angle_to_target-angle_to_host > PIf) {
                angle_to_target += PIf*2;
                printf("%f %f\n",angle_to_target,angle_to_host);
            }
            if (angle_to_host > angle_to_target) {
                col_x = 0.5 - (dist_to_host/host_width);
            } else {
                col_x = 0.5 + (dist_to_host/host_width);
            }
            //col_x = player_x + perp_distance * target_x;
            //col_x -= floor(col_x);

            int texture_x_coord = int(col_x * 128.0);

            int wall_height = (int)(NATIVE_GAME_WIDTH/host_perp_distance);
            iRect rect = {x,(NATIVE_GAME_HEIGHT-wall_height)/2,1,wall_height};
            iRect src_rect = {(int)(texture_x_coord),0,1,128};
            game_state->textureShader.Uniform1i("_texture",game_state->dinner_host_texture);
            GL_DrawTexture(src_rect,rect);
        }

    }

}
