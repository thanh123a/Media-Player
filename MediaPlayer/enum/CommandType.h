#ifndef COMMAND_TYPE_H
#define COMMAND_TYPE_H

enum class CommandType {
    NONE,
    PLAY,
    TOGGLE_PLAY,
    PAUSE,
    STOP,
    NEXT,
    PREV,
    PLAY_SELECTED,
    ADD_PLAYLIST,
    REMOVE_PLAYLIST,
    ADD_MEDIA,
    REMOVE_MEDIA,
    EDIT_METADATA,
    VOLUME_UP,  
    VOLUME_DOWN,
    SET_VOLUME,
    SEEK_TO,
    OPEN_FOLDER,
    UNKNOWN, 
};

#endif