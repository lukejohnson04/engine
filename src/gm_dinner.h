
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

const double milkshake_give_length = 2.25;
const double milkshake_freeze_length = 0.5;
const double milkshake_spin_length = 1.0;
const double milkshake_spin_to_player_length = 0.5;
const double milkshake_give_length_total = milkshake_give_length + milkshake_freeze_length + milkshake_spin_length + milkshake_spin_to_player_length;

char milkshake_options[16][128] = {
    "The taste was excellent",
    "It was quite flavorful",
    "The aroma was enticing",
    "It was simply delicious",
    "It was candy for the eyes",
    "It was simply superb",
    "The flavors melted in my mouth",
    "The aftertaste was hypnotizing",
    "It tasted like glue, which I like",
    "It was quite special and excellent",
    "I wish I could drink it again",
    "It was indeed quite marvelous",
    "The flavors were to die for",
    "Consuming it was stupendous",
    "The texture was other worldly",
    "Time stood still while I consumed"
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

    double host_x=0;
    double host_y=0;

    i32 hover_object=-1;
    bool can_interact_with_host=false;
    i32 host_object = GO_NONE;

    double timer=0;

    bool can_move=true;
    bool no_clip=false;

    enum {
        GO_TV,
        GO_HOST,
        GO_CHINA,
        GO_WRENCH,
        GO_YAHTZEE,
        GO_AQUARIUM,
        GO_COUNT,

        GO_DOOR,
        GO_LOCKED_DOOR,
        GO_KEYPAD_LOCKED_DOOR,
        GO_NONE
    };
    Wobject world_objects[GO_COUNT];

    i32 instr=0;

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
        GETTING_DOOR,
        BLOCKING_DOORWAY,
        WAITING_FOR_MILKSHAKE
    } host_task = NONE;

    enum {
        MK_TRAVELLING_TO_WAIT,
        MK_WAIT,
        MK_INITIAL_SPEAK,
        MK_TRAVEL_TO_MILKSHAKE,
        MK_TRAVEL_BACK_FROM_MILKSHAKE,
        MK_GIVE_MILKSHAKE,
        MK_MILKSHAKE_DRANK_REACTION,
        MK_NONE
    } host_milkshake_state=MK_TRAVELLING_TO_WAIT;

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
        MILKSHAKE_SELECT,
        ENTERING_CODE,
    } gameplay_state=GAMEPLAY;

    generic_drawable choices[4];
    i32 choice_count=0;

    generic_drawable milkshake_text[16];

    i32 how_scrolled_on_milkshake_select=0;
    bool dragging_milkshake_scrollbar=false;
    i32 milkshake_selections[3] = {-1};

    generic_drawable keypad_codes[12];
    i32 code_values[12] = {0};
    i32 digit_click_direction=0;
    i32 digit_clicked=-1;
};

void InitGmDinner();

void UpdateGmDinner();
void DrawGmDinner();

