#include "PlaylistManagerService.h"

// Constructor nhận danh sách playlist từ AppService
PlaylistManagerService::PlaylistManagerService(std::vector<std::shared_ptr<Playlist>>& playlists) : playlists(playlists) {}

// Thêm playlist mới
void PlaylistManagerService::addPlaylist(const std::string& name) {
    playlists.push_back(std::make_shared<Playlist>(name));
}

// Tìm playlist theo tên
std::shared_ptr<Playlist> PlaylistManagerService::findPlaylist(const std::string& name) {
    for (auto& playlist : playlists) {
        if (playlist->getPlaylistName() == name) {
            return playlist;
        }
    }
    return nullptr;
}

// Xóa playlist theo tên
void PlaylistManagerService::removePlaylist(const std::string& name) {
    playlists.erase(std::remove_if(playlists.begin(), playlists.end(),
        [&name](const std::shared_ptr<Playlist>& playlist) {
            return playlist->getPlaylistName() == name;
        }),
        playlists.end());
}

// Lấy toàn bộ danh sách playlist
std::vector<std::shared_ptr<Playlist>> PlaylistManagerService::getAllPlaylists() {
    return playlists;
}
