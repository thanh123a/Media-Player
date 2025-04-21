#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H

#include "CommandType.h"
#include <string>

struct ParsedCommand {
    CommandType type = CommandType::NONE;
    int volumeLevel = -1;  // Chỉ dùng cho SET_VOLUME
    int seekTime = -1;
};
ParsedCommand parseCommandString(const std::string& command);

#endif
