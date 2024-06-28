#include "EventCommandInterpreter.h"

// be more sophisticated later on
// not supported!!!
Command::Command(CommandType n_type, void* args) {
    type = n_type;
    if (type == CMD_SAY) {
        strcpy_s(text, (const char*)args);
        // memcpy(&text, args, strlen((*(char*)args)));
    } else if (type == CMD_FADEIN || type == CMD_FADEOUT || type == CMD_WAIT) {
        len = *(double*)args;
    } else if (type == CMD_ENTER_ROOM) {
        strcpy_s(target_scene, (const char*)args);
    } else if (type == CMD_EVENT) {
        strcpy_s(event_name, (const char*)args);
    } else if (type == CMD_CHANGESCENE) {
        load_func = (void(*)(Scene*))(args);
    } else if (type == CMD_SCENE_ANIM || type == CMD_ANIM) {
        anim_num = *(int*)args;
    } else if (type == CMD_SCENE_FRAME) {
        frame_num = *(int*)args;
    }
}


internal_function void ScriptRunNextLine(ScriptState*);
internal_function void RunCommand(Command&);
internal_function void RunCommand(std::string);
internal_function void ScriptRunQueue(ScriptState*);

internal_function
void ScriptRunEvent(ScriptState *state, const char* name) {
    if (state->event_runcount.count(name) != 0) {
        state->event_runcount[name]++;
    } else {
        state->event_runcount[name]=0;
    }
    int line_num = EventFileGetEventLineNumber(state->file, name) + 1;
    
    std::string& str = state->file.commands[line_num];
    if (str == "") {
        std::cout << "Event " << name << " not found\n";
        return;
    }
    state->current_line_num = line_num;
    state->current_event = name;
    RunCommand(state->file.commands[line_num]);
}

internal_function
void ScriptUpdate(ScriptState *state, float timestep) {
    if (state->isWaiting) {
        state->waitLen -= timestep;
        if (state->waitLen <= 0) {
            state->isWaiting = false;
            game_state->scene.update_entities=true;
            ScriptRunNextLine(state);
        }
    }
}

internal_function
void FadeCallback(void) {
    ScriptRunQueue(game_state->script_state);
}

internal_function
void RunCommand(Command &cmd) {
    ScriptState *state = game_state->script_state;
    state->curr_command = cmd.type;
    if (cmd.type == CMD_EVENT) {
        ScriptRunEvent(state, cmd.event_name);
    } else if (cmd.type == CMD_SAY) {
        DialoguePlayer::SetText(game_state->dialogue_state, cmd.text);
    } else if (cmd.type == CMD_WAIT) {
        state->waitLen = cmd.len;
        state->isWaiting = true;
        game_state->scene.update_entities=false;
    } else if (cmd.type == CMD_FADEIN) {
        FadeIn(cmd.len);
        // Command cmd(CMD_WAIT, (void*)&cmd.len);
        // RunCommand(cmd);
        if (cmd.len == 0) {
            renderer->fade.callback = nullptr;
            ScriptRunNextLine(game_state->script_state);
        } else {
            renderer->fade.callback = &FadeCallback;
        }
    } else if (cmd.type == CMD_FADEOUT) {
        if (cmd.len == 0) {
            renderer->fade.callback = nullptr;
            FadeOut(cmd.len);
            ScriptRunNextLine(game_state->script_state);
        } else {
            renderer->fade.callback = &FadeCallback;
            FadeOut(cmd.len);
        }
        // Command cmd(CMD_WAIT, (void*)&cmd.len);
        // RunCommand(cmd);
    } else if (cmd.type == CMD_CHANGESCENE) {
        state->curr_command = CMD_NONE;
        game_state->scene.FreeResources();
        if (cmd.load_func==nullptr) {
            game_state->scene = Scene();
        } else {
            while (!state->choice_stack.empty()) state->choice_stack.pop();
            cmd.load_func(&game_state->scene);
        }
        ScriptRunNextLine(state);
    } else if (cmd.type == CMD_ENTER_ROOM) {
        while (!state->choice_stack.empty()) state->choice_stack.pop();
        state->curr_command = CMD_NONE;
        game_state->scene.OnEntry(cmd.target_scene);
    } else if (cmd.type == CMD_SCENE_ANIM) {
        AnimationPlayerPlay(&game_state->scene.anim_player,&game_state->scene.sprite,cmd.anim_num);
        // need to divide cause this is just for the wait command
        double len=0;
        for (u32 i=0; i<game_state->scene.anim_player.anim->end_frame-game_state->scene.anim_player.anim->start_frame; i++) {
            len += game_state->scene.anim_player.anim->frame_lens[i];
        }
        len /= 1000.0;
        Command n_cmd(CMD_WAIT, (void*)&len);
        RunCommand(n_cmd);
    } else if (cmd.type == CMD_ANIM) {
        // global_app->scene->entities[0]->anim_player.play(cmd.anim_num);
        // double len = global_app->scene->entities[0]->anim_player.current->GetLength();
        ScriptRunNextLine(state);
    } else if (cmd.type == CMD_SCENE_FRAME) {
        game_state->scene.sprite.frame = cmd.frame_num;
        ScriptRunNextLine(state);
    } else if (cmd.type == CMD_CALLBACK) {
        cmd.callback();
        ScriptRunNextLine(state);
    } else {
        std::cout << "Command " << cmd.type << " is not a valid command!\n";
    }
}

internal_function
void RunCommand(std::string str) {
    Command cmd;
    cmd.type = CMD_NONE;
    int indent=0;
    while (str[indent] == '\t') {
        indent++;
    }
    str = str.substr(indent,str.size()-indent);
    std::istringstream ss(str);
    std::string root;
    ss >> root;

    ScriptState *state = game_state->script_state;
    if (root == "event") {
        std::string text = str.substr(6, str.size() - 6);
        cmd = Command(CMD_EVENT, (void*)text.c_str());
    } else if (root == "say") {
        std::string text = str.substr(4, str.size() - 4);
        cmd = Command(CMD_SAY, (void*)text.c_str());
    } else if (root == "wait") {
        cmd.type = CMD_WAIT;
        std::string str_len = str.substr(5, str.size() - 5);
        double wait_len = std::stod(str_len);
        cmd = Command(CMD_WAIT, (void*)&wait_len);
    } else if (root == "changescene") {
        std::string to_scene = str.substr(12, str.size() - 12);
        cmd = Command(CMD_CHANGESCENE, (void*)to_scene.c_str());
    } else if (root == "enter") {
        std::string to_room = str.substr(6, str.size() - 6);
        cmd = Command(CMD_ENTER_ROOM, (void*)to_room.c_str());
    } else if (root == "sceneanim") {
        std::string str_num = str.substr(10, str.size()-10);
        int anim_num = std::stoi(str_num);
        cmd = Command(CMD_SCENE_ANIM, (void*)&anim_num);
    } else if (root == "anim") {
        std::string str_num = str.substr(5, str.size()-5);
        int anim_num = std::stoi(str_num);
        cmd = Command(CMD_ANIM, (void*)&anim_num);
    } else if (root == "func") {
        std::string func_name;
        ss >> func_name;
        game_state->scene.RunCustomEvent(func_name);
        return;
    } else if (root == "fadein") {
        std::string str_len = str.substr(7, str.size()-7);
        double len = std::stod(str_len);
        cmd = Command(CMD_FADEIN, (void*)&len);
    } else if (root == "fadeout") {
        std::string str_len = str.substr(7, str.size()-7);
        double len = std::stod(str_len);
        cmd = Command(CMD_FADEOUT, (void*)&len);
    } else if (root == "if") {
        // hard coded in the string system for now - can't queue if statements
        std::string var_name;
        ss >> var_name;
        size_t expr_len = 3 + var_name.size() + 1;
        std::string str_val = str.substr(expr_len, str.size() - expr_len);
        int lvalue;
        if (var_name == "count") {
            lvalue = state->event_runcount[state->current_event];
        } else if (var_name == "choice") {
            lvalue = state->choice_stack.top();
        }
        
        int rvalue = std::stoi(str_val);
        if (lvalue == rvalue) {
            ScriptRunNextLine(state);
            return;
        } else {
            // skip to next equal indent level
            while (state->current_line_num < state->file.commands.size() && state->file.commands[state->current_line_num] != "") {
                state->current_line_num++;
                int iter_indent=0;
                while (state->file.commands[state->current_line_num][iter_indent] == '\t') {
                    iter_indent++;
                }
                if (iter_indent <= indent) {
                    break;
                }
            }
            if (state->current_line_num < state->file.commands.size() && state->file.commands[state->current_line_num] != "") {
                state->current_line_num--;
                ScriptRunNextLine(state);
            }
            return;
        }
    } else if (root == "return") {
        state->curr_command = CMD_NONE;
        state->current_line_num = 0;
        return;
    } else {
        state->curr_command = CMD_NONE;
        std::cout << "Command " << root << " is not a valid command!\n";
        return;
    }
    RunCommand(cmd);
}

internal_function
void QueueCommand(Command &cmd) {
    game_state->script_state->event_queue.push(cmd);
}

internal_function
void QueueCommand(CommandType type, double dbl) {
    Command cmd(type, (void*)&dbl);
    QueueCommand(cmd);
}

internal_function
void QueueCommand(CommandType type, int val) {
    Command cmd(type, (void*)&val);
    QueueCommand(cmd);
}

internal_function
void QueueCommand(CommandType type, std::string str) {
    Command cmd(type, (void*)str.c_str());
    QueueCommand(cmd);
}

internal_function
void QueueCommand(CommandType type, void* args) {
    if (type == CMD_CALLBACK) {
        Command cmd = {};
        cmd.type = CMD_CALLBACK;
        cmd.callback = (void(*)())args;
        QueueCommand(cmd);
    } else {
        Command cmd(type, args);
        QueueCommand(cmd);
    }
}

internal_function
void ScriptRunQueue(ScriptState *state) {
    ScriptRunNextLine(state);
}

internal_function
void ScriptRunNextLine(ScriptState *state) {
    state->curr_command = CMD_NONE;
    if (state->event_queue.size() > 0) {
        Command cmd = state->event_queue.front();
        state->event_queue.pop();
        RunCommand(cmd);
        return;
    // } if (callbacks.size() > 0) {
        // auto callback = callbacks.front();
        // callbacks.pop();
        // callback(nullptr);
        // return;
    }
    if (!state->current_line_num || state->current_event == "") {
        state->current_line_num = 0;
        state->current_event = "";
        return;
    }
    state->current_line_num++;
    if (state->current_line_num >= state->file.commands.size() ||
        state->file.commands[state->current_line_num] == "") {
        state->current_line_num = 0;
        state->current_event = "";
        return;
    }
    RunCommand(state->file.commands[state->current_line_num]);
}

internal_function
void ScriptContinueCallback(ScriptState *state) {
    ScriptRunNextLine(state);
}



#define INLINE_COMMAND_START(); {void(*l)() = []() -> void {
#define INLINE_COMMAND_END(); }; QueueCommand(CMD_CALLBACK,l);}
