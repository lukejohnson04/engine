
const int DINNER_MAP_WIDTH = 32;
const int DINNER_MAP_HEIGHT = 32;


struct GmDinnerData {
    int dinner_world_map[DINNER_MAP_WIDTH][DINNER_MAP_HEIGHT];

    double player_x=20;
    double player_y=11;
    double dir_x = -1;
    double dir_y = 0;
    double plane_x = 0;
    double plane_y = 0.66;

    double max_view_distance = 8.0;

    double host_x=14;
    double host_y=11;
};

void UpdateGmDinner();
void DrawGmDinner();
