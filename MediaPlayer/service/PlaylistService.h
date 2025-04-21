#ifndef PLAYLISTSERVICE_H
#define PLAYLISTSERVICE_H

#include <vector>
#include <memory>
#include <string>
#include "Playlist.h"
#include "MetadataUtils.h"

class PlaylistService {
private:
    std::vector<std::shared_ptr<Playlist>>& playlists;

public:
    PlaylistService(std::vector<std::shared_ptr<Playlist>>& playlists);

    // Quản lý file media trong playlist
    void addMediaToPlaylist(const std::string& playlistName, const std::string& filePath);
    void removeMediaFromPlaylist(const std::string& playlistName, const std::string& fileName);
};

#endif
