#include "CommandUtils.h"
#include <algorithm>

ParsedCommand parseCommandString(const std::string& str) {
    ParsedCommand cmd;

    if (str == "PLAY") cmd.type = CommandType::PLAY;
    else if (str == "TOGGLE_PLAY") cmd.type = CommandType::TOGGLE_PLAY;
    else if (str == "PAUSE") cmd.type = CommandType::PAUSE;
    else if (str == "STOP") cmd.type = CommandType::STOP;
    else if (str == "NEXT") cmd.type = CommandType::NEXT;
    else if (str == "PREV") cmd.type = CommandType::PREV;
    else if (str == "PLAY_SELECTED") cmd.type = CommandType::PLAY_SELECTED;
    else if (str == "ADD_PLAYLIST") cmd.type = CommandType::ADD_PLAYLIST;
    else if (str == "REMOVE_PLAYLIST") cmd.type = CommandType::REMOVE_PLAYLIST;
    else if (str == "ADD_MEDIA") cmd.type = CommandType::ADD_MEDIA;
    else if (str == "REMOVE_MEDIA") cmd.type = CommandType::REMOVE_MEDIA;
    else if (str == "EDIT_METADATA") cmd.type = CommandType::EDIT_METADATA;
    else if (str == "VOLUME_UP") cmd.type = CommandType::VOLUME_UP;
    else if (str == "VOLUME_DOWN") cmd.type = CommandType::VOLUME_DOWN;
    else if (str == "OPEN_FOLDER") cmd.type = CommandType::OPEN_FOLDER;
    else if (str.rfind("SET_VOLUME:", 0) == 0) {
        cmd.type = CommandType::SET_VOLUME;
        try {
            cmd.volumeLevel = std::stoi(str.substr(11));
        } catch (...) {
            cmd.volumeLevel = -1;
        }
    }
    else if (str.rfind("SEEK_TO:", 0) == 0) {
        cmd.type = CommandType::SEEK_TO;
        try {
            cmd.seekTime = std::stoi(str.substr(8));
        } catch (...) {
            cmd.seekTime = -1;
        }
    }
    return cmd;
}
