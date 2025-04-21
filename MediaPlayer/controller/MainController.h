#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <string>
#include <memory>
#include <SDL.h>
#include "MainView.h"
#include "AppService.h"
#include "MediaFile.h"

class MainController {
private:
    MainView mainView;
    AppService& appService;
    
    // Application state
    bool isPlaying;
    bool isPaused;
    int currentVolumeLevel;
    int currentSeekTime;
    std::shared_ptr<MediaFile> currentMediaFile;
    std::string nameOfSelectedPlaylist = "";
    
    // Selected indices
    int selectedPlaylistIndex;
    int selectedMediaIndex;
    
    // Helper methods
    void handleCommand(const std::string& command);
    void loadSelectedMedia();
    void updateMediaList();
    void playSelectedMedia();
    void updateCurrentMediaInfo();
    
    // UI Handlers
    void handleAddPlaylist();
    void handleRemovePlaylist();
    void handleAddMedia();
    void handleRemoveMedia();
    void handleEditMetadata();
    
    // Playback control
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void setVolume(int level);
    void setSeekTime(int time);
    
    // Utility methods
    //std::string promptUserInput(const std::string& prompt);

public:
    MainController(AppService& service);

    void run();  // Vòng lặp chính

    std::shared_ptr<MediaFile> getCurrentMediaFile();
};

#endif