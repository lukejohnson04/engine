#include "RoomDataLoader.h"

#include <fstream>
#include <iostream>


void LoadRoomFromFile(Scene *scene, std::string room) {
    std::ifstream file("../res/roomdata");
	if (!file.is_open()) {
		printf("Failed to open entity data file!\n");
        return;
	}
    //scene->dimensions = {320,180};
	std::string line;
    bool start=false;
    int lncount=0;

	while (getline(file, line)) {
        if (!start) {
            if (line == room) {
                start = true;
            }
            continue;
        }
        if (line.empty()) {
            // end of data to read
            break;
        }

        lncount++;
        std::istringstream ss(line);
        std::string data_type;
        ss >> data_type;
        if (data_type == "pentity" || data_type == "centity") {
            Entity *e = create_entity();

            std::string sx, sy, sw, sh;
            ss >> sx >> sy >> sw >> sh;
            e->collider = {(float)stoi(sx), (float)stoi(sy), (float)stoi(sw), (float)stoi(sh)};
            if (data_type == "pentity") {
                e->collider_layers = LAYER_PHYSICS;
                e->disableFlag(Entity::UPDATE);
            } else if (data_type == "centity") {
                e->collider_layers = LAYER_NONE;
                std::string on_interact;
                u32 len_so_far = data_type.size()+sx.size()+sy.size()+sw.size()+sh.size()+4;
                if (len_so_far == line.size()) {
                    on_interact = "";
                } else {
                    on_interact = line.substr(len_so_far+1, line.size() - (len_so_far+1));
                }
                e->onInteract = on_interact;
            }
        } else if (data_type == "emitter") {
            ParticleEmitter *em = ParticleSystem::CreateEmitter();
            std::string px, py;
            ss >> px >> py;
            em->pos.x = stof(px);
            em->pos.y = stof(py);
            ss >> px;
            em->dir = stof(px);
            std::string s_dir_variance, s_power, s_power_variance;
            ss >>  s_dir_variance >> s_power >> s_power_variance;
            em->dir_variance = stof(s_dir_variance);
            em->power = stof(s_power);
            em->power_variance = stof(s_power_variance);

            std::string r,g,b,a;
            ss >> r >> g >> b >> a;
            em->color_start = {(u8)stoi(r),(u8)stoi(g),(u8)stoi(b),(u8)stoi(a)};
            ss >> r >> g >> b >> a;
            em->color_end = {(u8)stoi(r),(u8)stoi(g),(u8)stoi(b),(u8)stoi(a)};

            std::string color_var;
            ss >> color_var;
            em->color_variance = stof(color_var);
            std::string rad_start, rad_end, rad_var;
            ss >> rad_start >> rad_end >> rad_var;
            em->radius_start = stof(rad_start);
            em->radius_end = stof(rad_end);
            em->radius_variance = stof(rad_var);

            std::string s_rot, s_rot_var, s_rot_speed, s_rot_speed_var;
            ss >> s_rot >> s_rot_var >> s_rot_speed >> s_rot_speed_var;
            em->rotation = stof(s_rot);
            em->rotation_variance = stof(s_rot_var);
            em->rotation_speed = stof(s_rot_speed);
            em->rotation_speed_variance = stof(s_rot_speed_var);

            std::string s_life, s_rate, s_burst, s_emit;
            ss >> s_life >> s_rate >> s_burst >> s_emit;
            em->lifetime = stof(s_life);
            em->rate = stof(s_rate);
            em->particles_per_burst = stoi(s_burst);
            em->emit = true;//stoi(s_emit);

            // vec2 pos;
            // vec2 dir;
            // float dir_variance=0.5f; // radians
            // // random degrees to rotate by
            // float power=5.0f;
            // float power_variance=0.20f; // percentage based

            // Color color_start={255,255,255,255};
            // Color color_end={0,0,0,255};
            // float color_variance=0.1f; // percentage based

            // float radius_start;
            // float radius_end;
            // float radius_variance=0.1f; // percentage based

            // float rotation=0.0f;
            // float rotation_variance=0.5f; // radians
            // float rotation_speed=0.0f;
            // float rotation_speed_variance=1.f; // radians

            // float lifetime=1.0f;
            // float rate=0.1f;
            // float time_to_next;
            // int particles_per_burst=5;
            // bool emit=true;
        } else if (data_type == "dimensions") {
            int width, height;
            std::string sw, sh;
            ss >> sw >> sh;
            width = stoi(sw);
            height = stoi(sh);
            scene->dimensions = {width,height};   
        } else {
        }
	}
    file.close();
}

void SaveRoomToFile(Scene *scene, std::string room) {
    std::ifstream file("../res/roomdata");
	if (!file.is_open()) {
		printf("Failed to open entity data file!\n");
        return;
	}
    std::string line;
    bool reading_old_segment=false;
    std::vector<std::string> contents;
	while (getline(file, line)) {
        if (line == room) {
            reading_old_segment = true;
        } if (reading_old_segment) {
            if (line == "") {
                reading_old_segment = false;
            }
        } else if (!reading_old_segment) {
            contents.push_back(line);
        }
    }
    file.close();

    if (!contents.empty() && contents.back() != "") {
        contents.push_back("");
    }
    contents.push_back(room);
    // here's where we actually convert the room data to text in the file
    for (entity_id id=0; id<game_state->entity_count; id++) {
        if (id == game_state->player || id == game_state->fightChara) {
            continue;
        }
        Entity *entity = &game_state->entity_pool[id];
        if (entity->sprite.texture) {
            continue;
        }
        int x = entity->collider.x;
        int y = entity->collider.y;
        int w = entity->collider.w;
        int h = entity->collider.h;
        if (!w || !h) {
            continue;
        }
        std::string data_type;
        if (entity->collider_layers == LAYER_NONE) {
            data_type = "centity";
        } else {
            data_type = "pentity";
        }
        std::string line = data_type + " " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(w) + " " + std::to_string(h);
        if (data_type == "centity" && entity->onInteract != "") {
            line += " " + entity->onInteract;
        }
        contents.push_back(line);
    } for (u32 emitter_id=0; emitter_id<game_state->particle_state.emitter_count; emitter_id++) {
        ParticleEmitter &em=game_state->particle_state.emitters[emitter_id];
        std::string line="emitter";
        line += " " + std::to_string(em.pos.x);
        line += " " + std::to_string(em.pos.y);
        line += " " + std::to_string(em.dir);
        line += " " + std::to_string(em.dir_variance);
        line += " " + std::to_string(em.power);
        line += " " + std::to_string(em.power_variance);

        line += " " + std::to_string(em.color_start.r);
        line += " " + std::to_string(em.color_start.g);
        line += " " + std::to_string(em.color_start.b);
        line += " " + std::to_string(em.color_start.a);

        line += " " + std::to_string(em.color_end.r);
        line += " " + std::to_string(em.color_end.g);
        line += " " + std::to_string(em.color_end.b);
        line += " " + std::to_string(em.color_end.a);

        line += " " + std::to_string(em.color_variance);

        line += " " + std::to_string(em.radius_start);
        line += " " + std::to_string(em.radius_end);
        line += " " + std::to_string(em.radius_variance);
        line += " " + std::to_string(em.rotation);
        line += " " + std::to_string(em.rotation_variance);
        line += " " + std::to_string(em.rotation_speed);
        line += " " + std::to_string(em.rotation_speed_variance);
        line += " " + std::to_string(em.lifetime);
        line += " " + std::to_string(em.rate);
        line += " " + std::to_string(em.particles_per_burst);
        line += " ";
        em.emit ? line += '1' : line += '0';
        contents.push_back(line);

        // vec2 pos={0,0};
        // vec2 dir={1,0};
        // float dir_variance=0.5f; // radians
        // // random degrees to rotate by
        // float power=5.0f;
        // float power_variance=0.20f; // percentage based

        // Color color_start={255,255,255,255};
        // Color color_end={0,0,0,255};
        // float color_variance=0.1f; // percentage based

        // float radius_start=5.f;
        // float radius_end=0.5f;
        // float radius_variance=0.1f; // percentage based

        // float rotation=0.0f;
        // float rotation_variance=0.5f; // radians
        // float rotation_speed=0.0f;
        // float rotation_speed_variance=1.f; // radians

        // float lifetime=1.0f;
        // float rate=0.1f;
        // float time_to_next=0.0f;
        // int particles_per_burst=5;
        // bool emit=true;
    }
    std::string str_dimensions = "dimensions " + std::to_string(scene->dimensions.x) + " " + std::to_string(scene->dimensions.y);
    contents.push_back(str_dimensions);

    std::ofstream wfile("../res/roomdata");
    for (std::string &line: contents) {
        wfile << line << '\n';
    }
    wfile.close();
}