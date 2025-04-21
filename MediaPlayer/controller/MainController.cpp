#include "MainController.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <dirent.h>

bool directoryExists(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

MainController::MainController(AppService& service)
    : appService(service),
      isPlaying(false),
      isPaused(false),
      currentVolumeLevel(50),
      selectedPlaylistIndex(0),
      selectedMediaIndex(0),
      currentSeekTime(0),
      nameOfSelectedPlaylist("") {}

void MainController::run() {
    if (!mainView.init()) {
        std::cerr << "Failed to initialize MainView\n";
        return;
    }

    // Set initial volume
    appService.getPlayMediaService().setVolume(currentVolumeLevel);

    bool quit = false;
    bool folderSelected = false;
    std::string actionCommand;

    // Auto-load default music folder at startup for better user experience
    std::string defaultMusicFolder = "./Music";
    if (directoryExists(defaultMusicFolder)) {
        appService.loadMediaFileToPlaylistFromFolder(defaultMusicFolder, nameOfSelectedPlaylist);
        updateMediaList();
    }

    // Keep track of the current state of playlists/media
    std::vector<DisplayItem<std::string>> playlistItems;
    std::vector<DisplayItem<std::string>> mediaItems;
    Uint32 lastSeekUpdateTick = SDL_GetTicks();

    while (!quit) {
        bool needsUpdate = false;

        // Kiểm tra xem có cần cập nhật lại playlist hoặc media không
        auto playlists = appService.getPlaylistManager().getAllPlaylists();
        if (playlists.size() != playlistItems.size()) {
            needsUpdate = true; // Playlist changed
        }

        // Cập nhật danh sách media nếu playlist đã thay đổi hoặc index khác
        if (selectedPlaylistIndex < static_cast<int>(playlists.size())) {
            auto selectedPlaylist = playlists[selectedPlaylistIndex];
            if (selectedPlaylist) {
                const auto& mediaMap = selectedPlaylist->getAllMediaFiles();
                if (mediaMap.size() != mediaItems.size()) {
                    needsUpdate = true; // Media changed
                }
            }
        }

        if (needsUpdate) {
            // Cập nhật playlist và media nếu có thay đổi
            playlistItems.clear();
            for (size_t i = 0; i < playlists.size(); ++i) {
                if (playlists[i]) {
                    playlistItems.push_back({static_cast<int>(i), playlists[i]->getPlaylistName()});
                }
            }

            mediaItems.clear();
            if (!playlists.empty() && selectedPlaylistIndex < static_cast<int>(playlists.size())) {
                auto selectedPlaylist = playlists[selectedPlaylistIndex];
                if (selectedPlaylist) {
                    const auto& mediaMap = selectedPlaylist->getAllMediaFiles();
                    int index = 0;
                    for (const auto& mediaPair : mediaMap) {
                        mediaItems.push_back({index++, mediaPair.first});
                    }
                }
            }
        }

        // Kiểm tra xem có cần cập nhật selectedMediaIndex không
        if (selectedMediaIndex >= static_cast<int>(mediaItems.size())) {
            selectedMediaIndex = mediaItems.empty() ? 0 : mediaItems.size() - 1;
        }

        // Xử lý các sự kiện từ view
        if (mainView.handleEvents(quit, folderSelected, selectedPlaylistIndex, selectedMediaIndex, actionCommand, currentMediaFile)) {
            // if (folderSelected) {
            //     std::string folderPath = mainView.openFolderDialog("Input folder path");
            //     if (!folderPath.empty()) {
            //         appService.loadDefaultPlaylistFromFolder(folderPath);
            //         updateMediaList();
            //     }
            //     folderSelected = false;
            // }

            if (!actionCommand.empty()) {
                handleCommand(actionCommand);
            }
        }

        if (isPlaying && !isPaused) {
            Uint32 currentTick = SDL_GetTicks();
            if (currentTick - lastSeekUpdateTick >= 1000) {
                currentSeekTime += 1;
                lastSeekUpdateTick = currentTick;
            }
        }

        // Render view với state hiện tại
        mainView.setPlaylistItems(playlistItems);
        mainView.setMediaItems(mediaItems);
        mainView.render(playlistItems, mediaItems, currentMediaFile, isPlaying, isPaused, currentVolumeLevel, currentSeekTime);
        SDL_Delay(16);  // ~60fps
    }
}


void MainController::handleCommand(const std::string& commandStr) {
    ParsedCommand parsed = parseCommandString(commandStr);
    CommandType command = parsed.type;

    switch (command) {
        case CommandType::PLAY:
        case CommandType::TOGGLE_PLAY:
            if (!isPlaying) {
                play();
            } else if (isPaused) {
                appService.getPlayMediaService().resume();
                isPaused = false;
            } else {
                pause();
            }
            break;

        case CommandType::PAUSE:
            pause();
            break;

        case CommandType::STOP:
            stop();
            break;

        case CommandType::NEXT:
            next();
            break;

        case CommandType::PREV:
            previous();
            break;

        case CommandType::PLAY_SELECTED:
            playSelectedMedia();
            break;

        case CommandType::ADD_PLAYLIST:
            handleAddPlaylist();
            break;

        case CommandType::REMOVE_PLAYLIST:
            handleRemovePlaylist();
            break;

        case CommandType::ADD_MEDIA:
            handleAddMedia();
            break;

        case CommandType::REMOVE_MEDIA:
            handleRemoveMedia();
            break;

        case CommandType::EDIT_METADATA:
            handleEditMetadata();
            break;

        // case CommandType::VOLUME_UP:
        //     setVolume(std::min(currentVolumeLevel + 5, 100));
        //     break;

        // case CommandType::VOLUME_DOWN:
        //     setVolume(std::max(currentVolumeLevel - 5, 0));
        //     break;

        case CommandType::SET_VOLUME:
            if (parsed.volumeLevel != -1) {
                setVolume(std::max(0, std::min(parsed.volumeLevel, 100)));
            }
            break;
        
        case CommandType::SEEK_TO:
            if (parsed.seekTime != -1) {
                setSeekTime(parsed.seekTime);
            }
            break;

        case CommandType::OPEN_FOLDER: {
            std::string folderPath = mainView.openFolderDialog("Input path to folder");
            if (!folderPath.empty()) {
                std::cout << nameOfSelectedPlaylist;
                appService.loadMediaFileToPlaylistFromFolder(folderPath, nameOfSelectedPlaylist);
                updateMediaList();
            }
            break;
        }

        case CommandType::NONE:
        default:
            break;
    }
}

void MainController::loadSelectedMedia() {
    auto playlists = appService.getPlaylistManager().getAllPlaylists();
    if (playlists.empty() || selectedPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }
    
    auto selectedPlaylist = playlists[selectedPlaylistIndex];
    if (!selectedPlaylist) return;
    
    const auto& mediaMap = selectedPlaylist->getAllMediaFiles();
    if (mediaMap.empty()) return;
    
    std::vector<std::string> mediaNames;
    for (const auto& mediaPair : mediaMap) {
        mediaNames.push_back(mediaPair.first);
    }
    
    if (selectedMediaIndex >= 0 && selectedMediaIndex < static_cast<int>(mediaNames.size())) {
        const std::string& selectedMediaName = mediaNames[selectedMediaIndex];
        currentMediaFile = selectedPlaylist->getMediaFile(selectedMediaName);
    }
}

void MainController::updateMediaList() {
    auto playlists = appService.getPlaylistManager().getAllPlaylists();
    if (playlists.empty()) {
        selectedPlaylistIndex = 0;
        selectedMediaIndex = 0;
        return;
    }
    
    if (selectedPlaylistIndex >= static_cast<int>(playlists.size())) {
        selectedPlaylistIndex = playlists.size() - 1;
    }
    
    mainView.setSelectedPlaylistIndex(selectedPlaylistIndex);
    selectedMediaIndex = 0;
    mainView.setSelectedMediaIndex(selectedMediaIndex);
    nameOfSelectedPlaylist = playlists[selectedPlaylistIndex]->getPlaylistName();
    loadSelectedMedia();
}

void MainController::playSelectedMedia() {
    loadSelectedMedia();
    if (currentMediaFile) {
        // TODO: Set current playlist and media in PlayMediaService
        appService.getPlayMediaService().resetCurrentFile(selectedMediaIndex, selectedPlaylistIndex);
        isPlaying = true;
        isPaused = false;
        currentSeekTime = 0;
    }
}

void MainController::updateCurrentMediaInfo() {
    // This would be called when the currently playing media changes
    // For now, we just ensure currentMediaFile is up to date
    loadSelectedMedia();
}

void MainController::play() {
    if (!isPlaying) {
        appService.getPlayMediaService().play();
        isPlaying = true;
        isPaused = false;
    }
}

void MainController::pause() {
    if (isPlaying && !isPaused) {
        appService.getPlayMediaService().pause();
        isPaused = true;
    }
}

void MainController::stop() {
    if (isPlaying) {
        appService.getPlayMediaService().pause();  // No stop function, we use pause
        isPlaying = false;
        isPaused = false;
        currentSeekTime = 0;
    }
}

void MainController::next() {
    appService.getPlayMediaService().next();
    updateCurrentMediaInfo();
}

void MainController::previous() {
    appService.getPlayMediaService().previous();
    updateCurrentMediaInfo();
}

void MainController::setVolume(int level) {
    currentVolumeLevel = level;
    appService.getPlayMediaService().setVolume(level);
}

void MainController::setSeekTime(int time){
    currentSeekTime = time;
    appService.getPlayMediaService().seekTo(time);
}

// UI Handler implementations
void MainController::handleAddPlaylist() {
    std::string playlistName = mainView.openTextInputDialog("Enter the name of playlist: ");
    if (!playlistName.empty()) {
        appService.getPlaylistManager().addPlaylist(playlistName);
        // Select the newly created playlist
        auto playlists = appService.getPlaylistManager().getAllPlaylists();
        selectedPlaylistIndex = playlists.size() - 1;
        updateMediaList();
    }
}

void MainController::handleRemovePlaylist() {
    auto playlists = appService.getPlaylistManager().getAllPlaylists();
    if (playlists.empty() || selectedPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }
    
    std::string playlistName = playlists[selectedPlaylistIndex]->getPlaylistName();
    // std::string confirm = promptUserInput("Are you sure you want to remove playlist '" + 
    //                                         playlistName + "'? (y/n): ");
    bool confirm = mainView.showConfirmDialog("Are you sure you want to remove playlist '" + playlistName + "'? (y/n): ");
    if (confirm == 1 || confirm == true) {
        appService.getPlaylistManager().removePlaylist(playlistName);
        updateMediaList();
    }
}

void MainController::handleAddMedia() {
    auto playlists = appService.getPlaylistManager().getAllPlaylists();
    if (playlists.empty() || selectedPlaylistIndex >= static_cast<int>(playlists.size())) {
        std::cout << "Please select or create a playlist first.\n";
        return;
    }
    
    std::string filePath = mainView.openFileDialog("Chọn file media", "*.mp3;*.mp4;*.wav");

    if (!filePath.empty()) {
        std::string playlistName = playlists[selectedPlaylistIndex]->getPlaylistName();
        appService.getPlaylistService().addMediaToPlaylist(playlistName, filePath);
        // Update and select the newly added media
        updateMediaList();
    }
}

void MainController::handleRemoveMedia() {
    auto playlists = appService.getPlaylistManager().getAllPlaylists();
    if (playlists.empty() || selectedPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }
    
    auto selectedPlaylist = playlists[selectedPlaylistIndex];
    if (!selectedPlaylist) return;
    
    const auto& mediaMap = selectedPlaylist->getAllMediaFiles();
    if (mediaMap.empty()) return;
    
    std::vector<std::string> mediaNames;
    for (const auto& mediaPair : mediaMap) {
        mediaNames.push_back(mediaPair.first);
    }
    
    if (selectedMediaIndex < 0 || selectedMediaIndex >= static_cast<int>(mediaNames.size())) {
        return;
    }
    
    std::string mediaName = mediaNames[selectedMediaIndex];
    bool confirm = mainView.showConfirmDialog("Are you sure you want to remove '" + 
                                            mediaName + "' from playlist? (y/n): ");
    
    if (confirm == 1 || confirm == true) {
        std::string playlistName = selectedPlaylist->getPlaylistName();
        appService.getPlaylistService().removeMediaFromPlaylist(playlistName, mediaName);
        
        // Update media list
        if (selectedMediaIndex >= static_cast<int>(mediaNames.size() - 1)) {
            selectedMediaIndex = mediaNames.size() - 2;
            if (selectedMediaIndex < 0) selectedMediaIndex = 0;
        }
        updateMediaList();
    }
}

void MainController::handleEditMetadata() {
    // loadSelectedMedia();
    // if (currentMediaFile) {
    //     appService.getMetadataService().editMediaMetadata(currentMediaFile);
    // } else {
    //     std::cout << "No media selected.\n";
    // }
}

std::shared_ptr<MediaFile> MainController::getCurrentMediaFile(){
    return currentMediaFile;
}