#include "AppService.h"
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <cctype>

// Khởi tạo AppService với các service con
AppService::AppService() {
    playlistManager = std::make_unique<PlaylistManagerService>(playlists);
    playlistService = std::make_unique<PlaylistService>(playlists);
    metadataService = std::make_unique<MetadataService>(playlists);
    playMediaService = std::make_unique<PlayMediaService>(playlists);
}

// Getter cho các service con
PlaylistManagerService& AppService::getPlaylistManager() {
    return *playlistManager;
}

PlaylistService& AppService::getPlaylistService() {
    return *playlistService;
}

MetadataService& AppService::getMetadataService() {
    return *metadataService;
}

PlayMediaService& AppService::getPlayMediaService() {
    return *playMediaService;
}

void AppService::loadMediaFileToPlaylistFromFolder(const std::string& folderPath, std::string& playlistName) {
    // Tạo playlist "Default" qua PlaylistManagerService, nếu chưa tồn tại
    if(playlistName == ""){
        getPlaylistManager().addPlaylist("Default");
        playlistName = "Default";
    }

    // Duyệt các file trong folder sử dụng opendir/readdir
    DIR* dir = opendir(folderPath.c_str());
    if (!dir) {
        std::cerr << "Error: Unable to open folder: " << folderPath << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fileName = entry->d_name;
        if (fileName == "." || fileName == "..")
            continue;

        // Tạo đường dẫn đầy đủ cho file
        std::string fullPath = folderPath + "/" + fileName;

        // Kiểm tra file có phải là file thường không, sử dụng stat()
        struct stat s;
        if (stat(fullPath.c_str(), &s) == 0) {
            if (!S_ISREG(s.st_mode))
                continue;
        } else {
            continue;
        }

        // Lấy phần mở rộng của file, chuyển thành chữ thường để so sánh
        std::string extension;
        size_t pos = fileName.find_last_of('.');
        if (pos != std::string::npos) {
            extension = fileName.substr(pos + 1);
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        }

        // Kiểm tra định dạng file được hỗ trợ (audio và video)
        if (extension == "mp3" || extension == "wav" || extension == "flac" ||
            extension == "mp4" || extension == "avi" || extension == "mkv") {
            // Sử dụng PlaylistService để thêm media vào playlist "Default"
            getPlaylistService().addMediaToPlaylist(playlistName, fullPath);
            //std::cout << "Added file: " << fileName << " to playlist: Default" << std::endl;
        }
    }
    closedir(dir);
}
