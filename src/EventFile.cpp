#pragma once

struct EventFile {
	std::vector<std::string> commands;
	std::unordered_map<std::string, int> event_lines;
};

internal_function
EventFile EventFileLoad(const char* path) {
	EventFile events_file;
	std::ifstream file(path);
	if (!file.is_open()) {
		printf("Failed to open file!");
		return events_file;
	}
	std::string line;

	int line_num = 0;
	while (getline(file, line))
	{
		events_file.commands.push_back(line);
		if (line[0] == ':') {
			std::string event_name = line.substr(1, line.size() - 1);
			events_file.event_lines[event_name] = line_num;
		}
		line_num++;
	}
	events_file.commands.push_back("");
	file.close();
	return events_file;
}

internal_function
int EventFileGetEventLineNumber(EventFile file, std::string name) {
	if (file.event_lines.count(name) == 0) {
		std::cout << "Error: event " << name << " not found!";
		return -1;
	}
	int val = file.event_lines[name];
	return val;
}