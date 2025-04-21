#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include "MediaFile.h"
#include "TextRenderer.h"
#include "DisplayItem.h"
#include <sstream>
#include "tinyfiledialogs.h"

enum class UISection {
    PLAYLISTS,
    MEDIA_LIST,
    PLAYER_CONTROLS,
    METADATA_PANEL
};

struct Button {
    SDL_Rect rect;
    std::string text;
    bool isHovered;
    
    Button(int x, int y, int w, int h, const std::string& btnText) 
        : rect{x, y, w, h}, text(btnText), isHovered(false) {}
};

class MainView {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* titleFont;
    TTF_Font* regularFont;
    TTF_Font* smallFont;

    // Real index
    std::vector<DisplayItem<std::string>> playlistItemsOnScreen;
    std::vector<DisplayItem<std::string>> mediaItemsOnScreen;
    
    // UI components
    std::vector<Button> playerButtons;  // Play, Pause, Stop, Next, Prev, etc.
    std::vector<Button> menuButtons;    // Add playlist, Remove playlist, etc.
    
    // UI state
    UISection activeSection;
    int selectedPlaylistIndex;
    int selectedMediaIndex;
    bool showMetadataEditor;
    
    
    // UI dimensions & colors
    const int windowWidth = 1024;
    const int windowHeight = 768;
    SDL_Color bgColor = {30, 30, 30, 255};         // Dark gray
    SDL_Color textColor = {255, 255, 255, 255};    // White
    SDL_Color highlightColor = {65, 105, 225, 255}; // Royal blue
    SDL_Color buttonColor = {50, 50, 50, 255};     // Medium gray
    SDL_Color buttonHoverColor = {70, 70, 70, 255}; // Lighter gray
    
    // Hiển thị từng phần của UI
    void renderHeader();
    void renderSidebar(const std::vector<DisplayItem<std::string>>& playlistItems);
    void renderMediaList(const std::vector<DisplayItem<std::string>>& mediaItems);
    void renderPlayerControls(bool isPlaying, bool isPaused, int volumeLevel, int currentTime, int duration);
    void renderMetadataPanel(const std::shared_ptr<MediaFile>& currentMedia);
    void renderButton(const Button& button);
    
    // Helper methods
    void createPlayerButtons();
    void createMenuButtons();

public:
    MainView();
    ~MainView();

    bool init();
    void render(const std::vector<DisplayItem<std::string>>& playlistItems,
        const std::vector<DisplayItem<std::string>>& mediaItems,
        const std::shared_ptr<MediaFile>& currentMedia,
        bool isPlaying = false, bool isPaused = false, int volumeLevel = 50, int currentTime = 0);
                
    std::string openTextInputDialog(std::string msg);
    std::string openFolderDialog(std::string messageText);
    std::string openFileDialog(const std::string& title, const std::string& filter);
    bool handleEvents(bool& quit, bool& folderSelected, 
                     int& selectedPlaylist, int& selectedMedia,
                     std::string& actionCommand, std::shared_ptr<MediaFile> currentMediaFile);
                     
    // Trả về renderer để các component khác sử dụng
    SDL_Renderer* getRenderer() const { return renderer; }
    
    // Getter & Setter cho UI state
    void setSelectedPlaylistIndex(int index) { selectedPlaylistIndex = index; }
    int getSelectedPlaylistIndex() const { return selectedPlaylistIndex; }
    
    void setSelectedMediaIndex(int index) { selectedMediaIndex = index; }
    int getSelectedMediaIndex() const { return selectedMediaIndex; }
    
    void setActiveSection(UISection section) { activeSection = section; }
    UISection getActiveSection() const { return activeSection; }
    
    void toggleMetadataEditor() { showMetadataEditor = !showMetadataEditor; }
    bool isMetadataEditorVisible() const { return showMetadataEditor; }

    void setPlaylistItems(const std::vector<DisplayItem<std::string>>& items);
    void setMediaItems(const std::vector<DisplayItem<std::string>>& items);
    bool showConfirmDialog(const std::string& messageText);
};

#endif