
const int DINNER_MAP_WIDTH = 64;
const int DINNER_MAP_HEIGHT = 64;


struct Wobject {
    v2 pos;
    union {
        bool on;
    };
};

enum TileType {
    BRICK,
    YELLOW,
    DOOR,
    TRIGGER,
    INACTIVE_TRIGGER,

    COUNT
};

struct GmDinnerData {
    int dinner_world_map[DINNER_MAP_WIDTH][DINNER_MAP_HEIGHT];

    double player_x=38;
    double player_y=30;
    double dir_x = 0;
    double dir_y = -1;
    double plane_x = -0.66;
    double plane_y = 0;

    double max_view_distance = 15.0;

    double host_x=5;
    double host_y=6;

    i32 hover_object=-1;

    double timer=0;

    bool can_move=true;
    bool no_clip=false;

    enum {
        GO_TV,
        GO_HOST,
        GO_CHINA,
        GO_COUNT,
        GO_DOOR,
        GO_WRENCH,
        GO_NONE
    };
    Wobject world_objects[GO_COUNT];

    enum {
        DOORWAY_BLOCK_SLIDE,
        DOORWAY_BLOCK_SPEAK,
        DOORWAY_BLOCK_CHOICE,
        DOORWAY_BLOCK_LEMME_CHECK_SPEAK,
        DOORWAY_BLOCK_CHECKING_DOG_TRAVELTO,
        DOORWAY_BLOCK_CHECKING_DOG,
        DOORWAY_BLOCK_CHECKING_DOG_TRAVELBACK,
        DOORWAY_BLOCK_CHECKED_DOG_SPEAK,
        DOORWAY_BLOCK_NONE
    } doorway_blocking_state=DOORWAY_BLOCK_SLIDE;

    enum {
        NONE,
        BLOCKING_DOORWAY
    } host_task = NONE;

    enum {
        DOOR_CLOSED,
        DOOR_KNOCKED,
        DOOR_OPENING,
        SLIDE_IN_FRONT,
        SPEAK
    } door_state = DOOR_CLOSED;

    enum {
        SILENT,
        TRAVELLING,
        SPEAKING,
        HOST_STATE_COUNT
    } host_state = SILENT;

    enum {
        GAMEPLAY,
        CHOICE,
    } gameplay_state=GAMEPLAY;

    generic_drawable text_yes;
    generic_drawable text_no;
};

void InitGmDinner();

void UpdateGmDinner();
void DrawGmDinner();

