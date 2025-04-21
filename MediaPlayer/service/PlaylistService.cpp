#include "PlaylistService.h"
#include <taglib/tag.h>


// Constructor nhận danh sách playlist từ AppService
PlaylistService::PlaylistService(std::vector<std::shared_ptr<Playlist>>& playlists) : playlists(playlists) {}

// Thêm file vào playlist
void PlaylistService::addMediaToPlaylist(const std::string& playlistName, const std::string& filePath) {
    std::shared_ptr<MediaFile> mediaFile = MetadataUtils::createMediaFileFromPath(filePath);

    if (!mediaFile) {
        std::cerr << "Unsupported file type: " << filePath << std::endl;
        return;
    }

    // Tìm playlist và thêm vào
    for (auto& playlist : playlists) {
        if (playlist->getPlaylistName() == playlistName) {
            playlist->addMediaFile(mediaFile);
            std::cout << "Added " << mediaFile->getFileName() << " to playlist " << playlistName << std::endl;
            return;
        }
    }

    std::cerr << "Playlist not found: " << playlistName << std::endl;
}

// Xóa file khỏi playlist
void PlaylistService::removeMediaFromPlaylist(const std::string& playlistName, const std::string& fileName) {
    for (auto& playlist : playlists) {
        if (playlist->getPlaylistName() == playlistName) {
            playlist->removeMediaFile(fileName);
            return;
        }
    }
}


