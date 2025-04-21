#include "MainView.h"
#include <SDL_ttf.h>
#include <SDL.h>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include "AudioFile.h"
#include "VideoFile.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

MainView::MainView() 
    : window(nullptr), renderer(nullptr), 
      titleFont(nullptr), regularFont(nullptr), smallFont(nullptr),
      activeSection(UISection::PLAYLISTS), 
      selectedPlaylistIndex(0), selectedMediaIndex(0),
      showMetadataEditor(false) {}

MainView::~MainView() {
    if (titleFont) TTF_CloseFont(titleFont);
    if (regularFont) TTF_CloseFont(regularFont);
    if (smallFont) TTF_CloseFont(smallFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

bool MainView::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Media Player",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load fonts
    titleFont = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 26);
    regularFont = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 18);
    smallFont = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 14);
    
    if (!titleFont || !regularFont || !smallFont) {
        std::cerr << "Failed to load fonts: " << TTF_GetError() << std::endl;
        return false;
    }

    // Khởi tạo các nút điều khiển
    createPlayerButtons();
    createMenuButtons();

    return true;
}

void MainView::createPlayerButtons() {
    // Nút điều khiển phát nhạc
    int btnWidth = 60;
    int btnHeight = 40;
    int startX = (windowWidth - (btnWidth * 5 + 20)) / 2;  // Căn giữa 5 nút với khoảng cách 5px
    int y = windowHeight - 70;
    
    playerButtons.push_back(Button(startX, y, btnWidth, btnHeight, "Prev"));
    playerButtons.push_back(Button(startX + btnWidth + 5, y, btnWidth, btnHeight, "Play"));
    playerButtons.push_back(Button(startX + (btnWidth + 5) * 2, y, btnWidth, btnHeight, "Pause"));
    playerButtons.push_back(Button(startX + (btnWidth + 5) * 3, y, btnWidth, btnHeight, "Stop"));
    playerButtons.push_back(Button(startX + (btnWidth + 5) * 4, y, btnWidth, btnHeight, "Next"));
}

void MainView::createMenuButtons() {
    // Nút menu
    int btnWidth = 180;
    int btnHeight = 30;
    int x = 10;
    int startY = 60;
    
    menuButtons.push_back(Button(x, startY, btnWidth, btnHeight, "Add Playlist"));
    menuButtons.push_back(Button(x, startY + 40, btnWidth, btnHeight, "Remove Playlist"));
    menuButtons.push_back(Button(x, startY + 80, btnWidth, btnHeight, "Add Media"));
    menuButtons.push_back(Button(x, startY + 120, btnWidth, btnHeight, "Remove Media"));
    menuButtons.push_back(Button(x, startY + 160, btnWidth, btnHeight, "Edit Metadata"));
}

void MainView::render(const std::vector<DisplayItem<std::string>>& playlistItems,
    const std::vector<DisplayItem<std::string>>& mediaItems,
    const std::shared_ptr<MediaFile>& currentMedia,
    bool isPlaying, bool isPaused, int volumeLevel, int currentTime) {

    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    renderHeader();
    renderSidebar(playlistItems);
    renderMediaList(mediaItems);
    renderPlayerControls(isPlaying, isPaused, volumeLevel, currentTime, currentMedia->getDuration());

    if (currentMedia) renderMetadataPanel(currentMedia);

    SDL_RenderPresent(renderer);
}

void MainView::renderHeader() {
    // Thanh tiêu đề
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect headerRect = {0, 0, windowWidth, 50};
    SDL_RenderFillRect(renderer, &headerRect);
    
    // Tiêu đề ứng dụng
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(titleFont, "Media Player", textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect dstRect = {20, 10, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void MainView::renderSidebar(const std::vector<DisplayItem<std::string>>& playlistItems) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect sidebarRect = {0, 50, 200, windowHeight - 50};
    SDL_RenderFillRect(renderer, &sidebarRect);

    for (const auto& button : menuButtons) renderButton(button);

    // Title
    SDL_Surface* headerSurface = TTF_RenderUTF8_Solid(regularFont, "Playlists", textColor);
    if (headerSurface) {
        SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
        if (headerTexture) {
            SDL_Rect dstRect = {10, 250, headerSurface->w, headerSurface->h};
            SDL_RenderCopy(renderer, headerTexture, nullptr, &dstRect);
            SDL_DestroyTexture(headerTexture);
        }
        SDL_FreeSurface(headerSurface);
    }

    int y = 280;
    for (size_t i = 0; i < playlistItems.size(); i++) {
        const auto& item = playlistItems[i];
        if (item.realIndex == selectedPlaylistIndex) {
            SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
            SDL_Rect selectedRect = {5, y - 2, 190, 30};
            SDL_RenderFillRect(renderer, &selectedRect);
        }

        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(regularFont, item.data.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect dstRect = {10, y, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        y += 30;
    }
}

void MainView::renderMediaList(const std::vector<DisplayItem<std::string>>& mediaItems) {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_Rect mediaListRect = {200, 50, windowWidth - 450, windowHeight - 150};
    SDL_RenderFillRect(renderer, &mediaListRect);

    SDL_Surface* headerSurface = TTF_RenderUTF8_Solid(regularFont, "Media Files", textColor);
    if (headerSurface) {
        SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
        if (headerTexture) {
            SDL_Rect dstRect = {210, 60, headerSurface->w, headerSurface->h};
            SDL_RenderCopy(renderer, headerTexture, nullptr, &dstRect);
            SDL_DestroyTexture(headerTexture);
        }
        SDL_FreeSurface(headerSurface);
    }

    int y = 100;
    for (size_t i = 0; i < mediaItems.size(); i++) {
        const auto& item = mediaItems[i];
        if (item.realIndex == selectedMediaIndex) {
            SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
            SDL_Rect selectedRect = {205, y - 2, windowWidth - 460, 30};
            SDL_RenderFillRect(renderer, &selectedRect);
        }

        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(regularFont, item.data.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect dstRect = {210, y, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        y += 30;
    }
}

void MainView::renderPlayerControls(bool isPlaying, bool isPaused, int volumeLevel, int currentTime, int duration) {
    // Player controls background
    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_Rect controlsRect = {0, windowHeight - 100, windowWidth, 100};
    SDL_RenderFillRect(renderer, &controlsRect);

    // Progress bar — đặt ở cao hơn
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect progressBarBg = {220, windowHeight - 90, 500, 15};
    SDL_RenderFillRect(renderer, &progressBarBg);

    float progressPercent = (duration > 0) ? (static_cast<float>(currentTime) / duration) : 0.0f;
    SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
    SDL_Rect progressBarFg = {220, windowHeight - 90, static_cast<int>(500 * progressPercent), 15};
    SDL_RenderFillRect(renderer, &progressBarFg);

    // Time text bên phải progress bar
    std::string timeText = std::to_string(currentTime) + " / " + std::to_string(duration) + "s";
    SDL_Surface* timeSurface = TTF_RenderUTF8_Solid(smallFont, timeText.c_str(), textColor);
    if (timeSurface) {
        SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_Rect timeRect = {730, windowHeight - 92, timeSurface->w, timeSurface->h};
        SDL_RenderCopy(renderer, timeTexture, nullptr, &timeRect);
        SDL_DestroyTexture(timeTexture);
        SDL_FreeSurface(timeSurface);
    }

    // Nút bấm player
    for (auto& button : playerButtons) {
        if ((button.text == "Play" && isPlaying && !isPaused) || 
            (button.text == "Pause" && isPaused)) {
            SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
            SDL_RenderFillRect(renderer, &button.rect);
        } else {
            renderButton(button);
        }
    }

    // Volume bar giữ nguyên
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect volumeBarBg = {windowWidth - 200, windowHeight - 70, 150, 20};
    SDL_RenderFillRect(renderer, &volumeBarBg);

    SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
    SDL_Rect volumeBarFg = {windowWidth - 200, windowHeight - 70, static_cast<int>(1.5 * volumeLevel), 20};
    SDL_RenderFillRect(renderer, &volumeBarFg);

    std::string volumeText = "Volume: " + std::to_string(volumeLevel) + "%";
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(smallFont, volumeText.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect dstRect = {windowWidth - 200, windowHeight - 90, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
    }
}


void MainView::renderMetadataPanel(const std::shared_ptr<MediaFile>& currentMedia) {
    if (!currentMedia) return;
    
    // Metadata panel background
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_Rect metadataRect = {windowWidth - 250, 50, 250, windowHeight - 150};
    SDL_RenderFillRect(renderer, &metadataRect);
    
    // Tiêu đề
    SDL_Surface* headerSurface = TTF_RenderUTF8_Solid(regularFont, "Metadata", textColor);
    if (headerSurface) {
        SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
        if (headerTexture) {
            SDL_Rect dstRect = {windowWidth - 240, 60, headerSurface->w, headerSurface->h};
            SDL_RenderCopy(renderer, headerTexture, nullptr, &dstRect);
            SDL_DestroyTexture(headerTexture);
        }
        SDL_FreeSurface(headerSurface);
    }
    
    // Thông tin về file
    int y = 100;
    std::vector<std::string> metadata;
    
    metadata.push_back("File: " + currentMedia->getFileName());
    metadata.push_back("Duration: " + std::to_string(currentMedia->getDuration()) + "s");
    
    // Check if it's an AudioFile
    auto audioFile = std::dynamic_pointer_cast<AudioFile>(currentMedia);
    if (audioFile) {
        metadata.push_back("Type: Audio");
        metadata.push_back("Artist: " + audioFile->getArtist());
        metadata.push_back("Album: " + audioFile->getAlbum());
        metadata.push_back("Genre: " + audioFile->getGenre());
        metadata.push_back("Year: " + std::to_string(audioFile->getPublishYear()));
    }
    
    // Check if it's a VideoFile
    auto videoFile = std::dynamic_pointer_cast<VideoFile>(currentMedia);
    if (videoFile) {
        metadata.push_back("Type: Video");
        metadata.push_back("Size: " + std::to_string(videoFile->getFileSize()) + " MB");
        metadata.push_back("Bitrate: " + std::to_string(videoFile->getBitrate()) + " kbps");
        metadata.push_back("Codec: " + videoFile->getCodec());
    }
    
    // Render metadata
    for (const auto& line : metadata) {
        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(smallFont, line.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect dstRect = {windowWidth - 240, y, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        y += 25;
    }
}

void MainView::renderButton(const Button& button) {
    // Button background
    if (button.isHovered) {
        SDL_SetRenderDrawColor(renderer, buttonHoverColor.r, buttonHoverColor.g, buttonHoverColor.b, buttonHoverColor.a);
    } else {
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    }
    SDL_RenderFillRect(renderer, &button.rect);
    
    // Button text
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(regularFont, button.text.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            int textX = button.rect.x + (button.rect.w - textSurface->w) / 2;
            int textY = button.rect.y + (button.rect.h - textSurface->h) / 2;
            SDL_Rect dstRect = {textX, textY, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

bool MainView::handleEvents(bool& quit, bool& folderSelected, 
                           int& selectedPlaylist, int& selectedMedia, 
                           std::string& actionCommand, std::shared_ptr<MediaFile> currentMediaFile) {
    SDL_Event e;
    bool eventHandled = false;
    
    // Reset command
    actionCommand = "";
    
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
            eventHandled = true;
        } else if (e.type == SDL_KEYDOWN) {
            // Keyboard navigation
            switch (e.key.keysym.sym) {
                case SDLK_o:
                    folderSelected = true;
                    eventHandled = true;
                    actionCommand = "OPEN_FOLDER";
                    break;
                    
                case SDLK_UP:
                    if (activeSection == UISection::PLAYLISTS && selectedPlaylistIndex > 0) {
                        selectedPlaylistIndex--;
                        selectedPlaylist = selectedPlaylistIndex;
                        eventHandled = true;
                    } else if (activeSection == UISection::MEDIA_LIST && selectedMediaIndex > 0) {
                        selectedMediaIndex--;
                        selectedMedia = selectedMediaIndex;
                        eventHandled = true;
                    }
                    break;
                    
                case SDLK_DOWN:
                    if (activeSection == UISection::PLAYLISTS) {
                        selectedPlaylistIndex++;
                        selectedPlaylist = selectedPlaylistIndex;
                        eventHandled = true;
                    } else if (activeSection == UISection::MEDIA_LIST) {
                        selectedMediaIndex++;
                        selectedMedia = selectedMediaIndex;
                        eventHandled = true;
                    }
                    break;
                    
                case SDLK_TAB:
                    // Switch between sections
                    if (activeSection == UISection::PLAYLISTS) {
                        activeSection = UISection::MEDIA_LIST;
                    } else if (activeSection == UISection::MEDIA_LIST) {
                        activeSection = UISection::PLAYER_CONTROLS;
                    } else if (activeSection == UISection::PLAYER_CONTROLS) {
                        activeSection = UISection::PLAYLISTS;
                    }
                    eventHandled = true;
                    break;
                    
                case SDLK_SPACE:
                    // Play/Pause
                    actionCommand = "TOGGLE_PLAY";
                    eventHandled = true;
                    break;
                    
                case SDLK_RIGHT:
                    // Next track
                    actionCommand = "NEXT";
                    eventHandled = true;
                    break;
                    
                case SDLK_LEFT:
                    // Previous track
                    actionCommand = "PREV";
                    eventHandled = true;
                    break;
                    
                // case SDLK_PLUS:
                // case SDLK_EQUALS:
                //     // Increase volume
                //     actionCommand = "VOLUME_UP";
                //     eventHandled = true;
                //     break;
                    
                // case SDLK_MINUS:
                //     // Decrease volume
                //     actionCommand = "VOLUME_DOWN";
                //     eventHandled = true;
                //     break;
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                
                // Check player buttons
                for (const auto& button : playerButtons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        
                        if (button.text == "Play") {
                            actionCommand = "PLAY";
                        } else if (button.text == "Pause") {
                            actionCommand = "PAUSE";
                        } else if (button.text == "Stop") {
                            actionCommand = "STOP";
                        } else if (button.text == "Next") {
                            actionCommand = "NEXT";
                        } else if (button.text == "Prev") {
                            actionCommand = "PREV";
                        }
                        
                        eventHandled = true;
                        break;
                    }
                }
                
                // Check menu buttons
                for (const auto& button : menuButtons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        
                        if (button.text == "Add Playlist") {
                            actionCommand = "ADD_PLAYLIST";
                        } else if (button.text == "Remove Playlist") {
                            actionCommand = "REMOVE_PLAYLIST";
                        } else if (button.text == "Add Media") {
                            actionCommand = "ADD_MEDIA";
                        } else if (button.text == "Remove Media") {
                            actionCommand = "REMOVE_MEDIA";
                        } else if (button.text == "Edit Metadata") {
                            actionCommand = "EDIT_METADATA";
                        }
                        
                        eventHandled = true;
                        break;
                    }
                }
                
                // Check if clicked on playlist list
                if (mouseX >= 5 && mouseX <= 195 && mouseY >= 280) {
                    int newIndex = (mouseY - 280) / 30;
                    if (newIndex >= 0 && newIndex < static_cast<int>(playlistItemsOnScreen.size())) {
                        selectedPlaylistIndex = playlistItemsOnScreen[newIndex].realIndex;
                        selectedPlaylist = selectedPlaylistIndex;
                        activeSection = UISection::PLAYLISTS;
                        eventHandled = true;
                
                        std::cout << "[CLICK PLAYLIST] realIndex: " << selectedPlaylistIndex << std::endl;
                    }
                }
                
                // Check if clicked on media list
                if (mouseX >= 205 && mouseX <= windowWidth - 255 && mouseY >= 100) {
                    int newIndex = (mouseY - 100) / 30;
                    if (newIndex >= 0 && newIndex < static_cast<int>(mediaItemsOnScreen.size())) {
                        selectedMediaIndex = mediaItemsOnScreen[newIndex].realIndex;
                        selectedMedia = selectedMediaIndex;
                        activeSection = UISection::MEDIA_LIST;
                        eventHandled = true;

                        if (currentMediaFile) {
                            renderMetadataPanel(currentMediaFile);
                        }
                
                        std::cout << "[CLICK MEDIA] realIndex: " << selectedMediaIndex << std::endl;
                
                        // Double click check
                        static Uint32 lastClickTime = 0;
                        static int lastClickIndex = -1;
                        Uint32 currentTime = SDL_GetTicks();
                        if (lastClickIndex == newIndex && currentTime - lastClickTime < 500) {
                            actionCommand = "PLAY_SELECTED";
                        }
                        lastClickTime = currentTime;
                        lastClickIndex = newIndex;
                    }
                }
                
                // Check volume control
                if (mouseX >= windowWidth - 200 && mouseX <= windowWidth - 50 &&
                    mouseY >= windowHeight - 70 && mouseY <= windowHeight - 50) {
                    int volumeLevel = ((mouseX - (windowWidth - 200)) * 100) / 150;
                    actionCommand = "SET_VOLUME:" + std::to_string(volumeLevel);
                    eventHandled = true;
                }
                
                // Check time media scontrol
                if (mouseX >= 220 && mouseX <= 720 &&
                    mouseY >= windowHeight - 90 && mouseY <= windowHeight - 75)
                {
                    float progressPercent = (mouseX - 220) / 500.0f;
                    int seekTime = 0;
                    if (currentMediaFile) {
                        seekTime = static_cast<int>(progressPercent * currentMediaFile->getDuration());
                    }
                    actionCommand = "SEEK_TO:" + std::to_string(seekTime);
                    eventHandled = true;
                }
            }
        } else if (e.type == SDL_MOUSEMOTION) {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            
            // Update hover state for buttons
            for (auto& button : playerButtons) {
                button.isHovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                                   mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
            }
            
            for (auto& button : menuButtons) {
                button.isHovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                                   mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
            }
        }
    }
    
    return eventHandled;
}

std::string MainView::openTextInputDialog(std::string messageText) {
    SDL_StartTextInput();

    bool done = false;
    std::string inputBuffer = "";
    bool dirty = true;  // flag báo cần render lại

    SDL_Event e;
    Uint32 frameDelay = 1000 / 60;  // 60fps
    Uint32 frameStart, frameTime;

    while (!done) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_StopTextInput();
                return "";
            }
            if (e.type == SDL_TEXTINPUT) {
                inputBuffer += e.text.text;
                dirty = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    dirty = true;
                }
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
                    done = true;
                }
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_StopTextInput();
                    return "";
                }
            }
        }

        // Chỉ render nếu có thay đổi
        if (dirty) {
            // Clear màn hình
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);

            // Vẽ message text
            SDL_Surface* messageSurface = TTF_RenderUTF8_Solid(regularFont, messageText.c_str(), textColor);
            if (messageSurface) {
                SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
                if (messageTexture) {
                    SDL_Rect messageRect = { 100, windowHeight / 2 - 80, messageSurface->w, messageSurface->h };
                    SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
                    SDL_DestroyTexture(messageTexture);
                }
                SDL_FreeSurface(messageSurface);
            }

            // Vẽ input box
            SDL_Rect inputRect = { 100, windowHeight / 2 - 25, windowWidth - 200, 50 };
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &inputRect);

            // Render text đang nhập
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(regularFont, inputBuffer.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect = { 110, windowHeight / 2 - textSurface->h / 2, textSurface->w, textSurface->h };
                    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }

            SDL_RenderPresent(renderer);
            dirty = false;
        }

        // Cap 60fps
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_StopTextInput();
    return inputBuffer;
}


void MainView::setPlaylistItems(const std::vector<DisplayItem<std::string>>& items) {
    playlistItemsOnScreen = items;
}

void MainView::setMediaItems(const std::vector<DisplayItem<std::string>>& items) {
    mediaItemsOnScreen = items;
}


// Hàm chia message thành từng dòng nếu vượt maxWidth
std::vector<std::string> wrapText(TTF_Font* font, const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word, line;

    while (iss >> word) {
        std::string testLine = line.empty() ? word : line + " " + word;
        int w = 0, h = 0;
        TTF_SizeUTF8(font, testLine.c_str(), &w, &h);
        if (w > maxWidth) {
            if (!line.empty()) lines.push_back(line);
            line = word;
        } else {
            line = testLine;
        }
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}

bool MainView::showConfirmDialog(const std::string& messageText) {
    bool done = false;
    bool result = false;

    SDL_Event e;
    Uint32 frameDelay = 1000 / 60;
    Uint32 frameStart, frameTime;

    int dialogWidth = 450;
    int dialogHeight = 220;
    SDL_Rect dialogRect = {
        (windowWidth - dialogWidth) / 2,
        (windowHeight - dialogHeight) / 2,
        dialogWidth,
        dialogHeight
    };

    // Chia message thành từng dòng nếu dài quá
    int textMaxWidth = dialogWidth - 60;
    auto lines = wrapText(regularFont, messageText, textMaxWidth);

    // Cấu hình nút Yes và No
    int buttonWidth = 100;
    int buttonHeight = 40;
    int buttonSpacing = 20;
    int buttonY = dialogRect.y + dialogHeight - 60;

    SDL_Rect yesButtonRect = {
        dialogRect.x + (dialogWidth - 2 * buttonWidth - buttonSpacing) / 2,
        buttonY,
        buttonWidth,
        buttonHeight
    };

    SDL_Rect noButtonRect = {
        yesButtonRect.x + buttonWidth + buttonSpacing,
        buttonY,
        buttonWidth,
        buttonHeight
    };

    while (!done) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Point mousePoint = { e.button.x, e.button.y };
                if (SDL_PointInRect(&mousePoint, &yesButtonRect)) {
                    result = true;
                    done = true;
                } else if (SDL_PointInRect(&mousePoint, &noButtonRect)) {
                    result = false;
                    done = true;
                }
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_y) {
                    result = true;
                    done = true;
                }
                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_n) {
                    result = false;
                    done = true;
                }
            }
        }

        // Vẽ nền tối
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, nullptr);

        // Vẽ dialog box
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &dialogRect);

        // Vẽ message text từng dòng
        int lineY = dialogRect.y + 30;
        for (const auto& line : lines) {
            SDL_Surface* lineSurface = TTF_RenderUTF8_Solid(regularFont, line.c_str(), textColor);
            SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
            SDL_Rect lineRect = {
                dialogRect.x + (dialogWidth - lineSurface->w) / 2,
                lineY,
                lineSurface->w,
                lineSurface->h
            };
            SDL_RenderCopy(renderer, lineTexture, nullptr, &lineRect);
            lineY += lineSurface->h + 5;
            SDL_FreeSurface(lineSurface);
            SDL_DestroyTexture(lineTexture);
        }

        // Vẽ nút Yes
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
        SDL_RenderFillRect(renderer, &yesButtonRect);
        SDL_Surface* yesSurface = TTF_RenderUTF8_Solid(regularFont, "Yes", { 255, 255, 255 });
        SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
        SDL_Rect yesTextRect = {
            yesButtonRect.x + (buttonWidth - yesSurface->w) / 2,
            yesButtonRect.y + (buttonHeight - yesSurface->h) / 2,
            yesSurface->w, yesSurface->h
        };
        SDL_RenderCopy(renderer, yesTexture, nullptr, &yesTextRect);
        SDL_FreeSurface(yesSurface);
        SDL_DestroyTexture(yesTexture);

        // Vẽ nút No
        SDL_SetRenderDrawColor(renderer, 178, 34, 34, 255);
        SDL_RenderFillRect(renderer, &noButtonRect);
        SDL_Surface* noSurface = TTF_RenderUTF8_Solid(regularFont, "No", { 255, 255, 255 });
        SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
        SDL_Rect noTextRect = {
            noButtonRect.x + (buttonWidth - noSurface->w) / 2,
            noButtonRect.y + (buttonHeight - noSurface->h) / 2,
            noSurface->w, noSurface->h
        };
        SDL_RenderCopy(renderer, noTexture, nullptr, &noTextRect);
        SDL_FreeSurface(noSurface);
        SDL_DestroyTexture(noTexture);

        SDL_RenderPresent(renderer);

        // Giới hạn FPS
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) SDL_Delay(frameDelay - frameTime);
    }

    return result;
}

std::string MainView::openFolderDialog(std::string messageText) {
    const char* folderPath = tinyfd_selectFolderDialog(messageText.c_str(), nullptr);
    if (folderPath) {
        return std::string(folderPath);
    } else {
        return "";
    }
}

std::string MainView::openFileDialog(const std::string& title, const std::string& filter) {
    const char* filePath = tinyfd_openFileDialog(
        title.c_str(),
        nullptr,
        0,
        nullptr,  // filter pattern list (có thể set nếu muốn lọc file media)
        nullptr,
        0);

    if (filePath) {
        return std::string(filePath);
    } else {
        return "";
    }
}

