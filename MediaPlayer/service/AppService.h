#ifndef APPSERVICE_H
#define APPSERVICE_H

#include <vector>
#include <memory>
#include "Playlist.h"
#include "PlaylistManagerService.h"
#include "PlaylistService.h"
#include "MetadataService.h"
#include "PlayMediaService.h"  
#include "CommandUtils.h"
#include "CommandType.h"

class AppService {
private:
    std::vector<std::shared_ptr<Playlist>> playlists;
    std::unique_ptr<PlaylistManagerService> playlistManager;
    std::unique_ptr<PlaylistService> playlistService;
    std::unique_ptr<MetadataService> metadataService;
    std::unique_ptr<PlayMediaService> playMediaService;
public:
    // Constructor
    AppService();

    // Getter để các service khác có thể sử dụng
    PlaylistManagerService& getPlaylistManager();
    PlaylistService& getPlaylistService();
    MetadataService& getMetadataService();
    PlayMediaService& getPlayMediaService();
    void loadMediaFileToPlaylistFromFolder(const std::string& folderPath, std::string& playlistName);

};

#endif
