#ifndef PLAYLISTMANAGERSERVICE_H
#define PLAYLISTMANAGERSERVICE_H

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Playlist.h"

class PlaylistManagerService {
private:
    std::vector<std::shared_ptr<Playlist>>& playlists;

public:
    PlaylistManagerService(std::vector<std::shared_ptr<Playlist>>& playlists);

    // CRUD playlist
    void addPlaylist(const std::string& name);
    void removePlaylist(const std::string& name);
    std::shared_ptr<Playlist> findPlaylist(const std::string& name);
    std::vector<std::shared_ptr<Playlist>> getAllPlaylists();
};

#endif
