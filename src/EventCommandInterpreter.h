#pragma once

enum CommandType {
	CMD_NONE,
	CMD_SAY,
	CMD_FADEIN,
	CMD_FADEOUT,
	CMD_WAIT,
	CMD_ENTER_ROOM,
	CMD_CHANGESCENE,
	CMD_SCENE_ANIM,
	CMD_SCENE_FRAME,
	CMD_CALLBACK,
	CMD_EVENT,
	CMD_ANIM
};


struct Command {
	Command() {}
	Command(CommandType, void*);
	CommandType type=CMD_NONE;
	union {
		char text[256];
		char target_scene[256];
		char event_name[32];
		double len;
		int anim_num;
		int frame_num;
		void (*callback)();
		void (*load_func)(Scene*);
	};
};

struct ScriptState {
	CommandType curr_command=CMD_NONE;
	EventFile file;
	std::string current_event="";
	
	std::queue<Command> event_queue;
	std::queue<void(*)(void*)> callbacks;
    std::unordered_map<std::string,int> scripting_vars;
	std::unordered_map<std::string,int> event_runcount;
	std::stack<int> choice_stack;
	u32 current_line_num=0;
	
	double waitLen = 0;
	bool isWaiting = false;
};

// void ScriptSetEventFile(ScriptState *state, EventFile file);
// void ScriptRunEvent(ScriptState *state, const char* name);
// void ScriptContinueCallback(ScriptState *state);
// void ScriptAddEventCallback(ScriptState *state, void(*callback)(void*));
// void ScriptUpdate(ScriptState *state, float timestep);
// void ScriptRunQueue(ScriptState *state);

// void RunCommand(std::string cmd);
// void RunCommand(Command &cmd);
// void QueueCommand(CommandType, double);
// void QueueCommand(CommandType, std::string);
// void QueueCommand(CommandType, int);
// void QueueCommand(CommandType, void*);
// // void QueueCommand(CommandType, void(*)(void*));
// void QueueCommand(Command &cmd);
