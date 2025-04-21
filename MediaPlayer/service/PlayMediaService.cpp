#include "PlayMediaService.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

PlayMediaService* PlayMediaService::instance = nullptr;

PlayMediaService::PlayMediaService(std::vector<std::shared_ptr<Playlist>>& playlists)
    : playlists(playlists), currentPlaylistIndex(0), currentMediaIndex(0),
      isPaused(false), isShuffled(false), music(nullptr)
{
    instance = this; // để callback static có thể gọi non-static method
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    Mix_HookMusicFinished([]() {
        PlayMediaService::onMusicFinishedStatic();
    });
    refreshCurrentQueue();
}

void PlayMediaService::onMusicFinishedStatic() {
    if (instance) {
        instance->onMusicFinished();
    }
}

// Hàm xử lý logic khi bài hát kết thúc
void PlayMediaService::onMusicFinished() {
    std::cout << "Media finished. Moving to next...\n";
    currentMediaIndex++;
    if (currentMediaIndex >= static_cast<int>(currentQueue.size())) {
        currentMediaIndex = 0; // quay lại từ đầu
    }
    play();
}

PlayMediaService::~PlayMediaService() {
    if(music != nullptr) {
      Mix_FreeMusic(music);
      music = nullptr;
    }
    Mix_CloseAudio();
}

void PlayMediaService::refreshCurrentQueue() {
    currentQueue.clear();
    if (!playlists.empty() && currentPlaylistIndex < static_cast<int>(playlists.size())) {
        auto currentPlaylist = playlists[currentPlaylistIndex];
        const auto& mediaMap = currentPlaylist->getAllMediaFiles();
        for (const auto& pair : mediaMap) {
            currentQueue.push_back(pair.second);
        }
    }
}

// Hàm helper: chuyển đối tượng MediaFile thành PlayableMediaFile
// std::shared_ptr<PlayableMediaFile> PlayMediaService::convertToPlayable(const std::shared_ptr<MediaFile>& mediaFile) {
//     // Nếu mediaFile đã là PlayableMediaFile, trả về trực tiếp
//     std::shared_ptr<PlayableMediaFile> playable = std::dynamic_pointer_cast<PlayableMediaFile>(mediaFile);
//     if (!playable) {
//         playable = std::make_shared<PlayableMediaFile>();
//         playable->setFileName(mediaFile->getFileName());
//         playable->setPath(mediaFile->getPath());
//         if(mediaFile->getFileType() == MediaFileType::AUDIO){
//             playable->setFileType(MediaFileType::AUDIO);
//         } 
//         else if(mediaFile->getFileType() == MediaFileType::VIDEO){
//             playable->setFileType(MediaFileType::VIDEO);
//         } 
//     }
    
//     // Nếu danh sách playlist không rỗng và currentPlaylistIndex hợp lệ
//     if (!playlists.empty() && currentPlaylistIndex < static_cast<int>(playlists.size())) {
//         auto currentPlaylist = playlists[currentPlaylistIndex];
//         playable->setPlaylistName(currentPlaylist->getPlaylistName());
        
//         // Dựa vào currentQueue (đã refresh) để xác định vị trí của media hiện tại
//         if (!currentQueue.empty()) {
//             int index = -1;
//             for (int i = 0; i < static_cast<int>(currentQueue.size()); ++i) {
//                 if (currentQueue[i]->getFileName() == mediaFile->getFileName()) {
//                     index = i;
//                     break;
//                 }
//             }
//             if (index != -1) {
//                 if (!isShuffled) {
//                     // Theo thứ tự, sử dụng vòng tròn
//                     int prevIndex = (index - 1 + currentQueue.size()) % currentQueue.size();
//                     int nextIndex = (index + 1) % currentQueue.size();
//                     playable->setPreviousFilePath(currentQueue[prevIndex]->getPath());
//                     playable->setNextFilePath(currentQueue[nextIndex]->getPath());
//                 } else {
//                     // Nếu đang shuffle, chọn ngẫu nhiên (khác với current)
//                     std::srand(static_cast<unsigned int>(std::time(nullptr)));
//                     int mediaCount = static_cast<int>(currentQueue.size());
//                     if (mediaCount > 1) {
//                         int randPrev = std::rand() % mediaCount;
//                         while (currentQueue[randPrev]->getFileName() == mediaFile->getFileName())
//                             randPrev = std::rand() % mediaCount;
//                         int randNext = std::rand() % mediaCount;
//                         while (currentQueue[randNext]->getFileName() == mediaFile->getFileName())
//                             randNext = std::rand() % mediaCount;
//                         playable->setPreviousFilePath(currentQueue[randPrev]->getPath());
//                         playable->setNextFilePath(currentQueue[randNext]->getPath());
//                     } else {
//                         playable->setPreviousFilePath("");
//                         playable->setNextFilePath("");
//                     }
//                 }
//             }
//         }
//     }
//     return playable;
// }

bool PlayMediaService::loadCurrentMedia() {
    if (playlists.empty()) {
        std::cerr << "No playlist available.\n";
        return false;
    }
    refreshCurrentQueue();
    if (currentQueue.empty()) {
        std::cerr << "Current playlist is empty.\n";
        return false;
    }
    
    if (currentMediaIndex < 0 || currentMediaIndex >= static_cast<int>(currentQueue.size())) {
        std::cerr << "Invalid media index.\n";
        return false;
    }
    
    // Lấy đối tượng MediaFile từ vector nội bộ
    std::shared_ptr<MediaFile> baseMedia = currentQueue[currentMediaIndex];
    //std::shared_ptr<PlayableMediaFile> playableMedia = convertToPlayable(baseMedia);
    
    if(music != nullptr) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    
    music = Mix_LoadMUS(baseMedia->getPath().c_str());
    if (music == nullptr) {
        std::cerr << "Failed to load music: " << Mix_GetError() << "\n";
        return false;
    }
    return true;
}

void PlayMediaService::play() {
    if (!loadCurrentMedia()) return;
    
    if (Mix_PlayMusic(music, 1) == -1) {
        std::cerr << "Error playing music: " << Mix_GetError() << "\n";
        return;
    }
    isPaused = false;
    std::cout << "Playing: " << currentQueue[currentMediaIndex]->getFileName() << "\n";
}

void PlayMediaService::pause() {
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
        isPaused = true;
        std::cout << "Paused.\n";
    }
}

void PlayMediaService::resume() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        isPaused = false;
        std::cout << "Resumed.\n";
    }
}

void PlayMediaService::next() {
    currentMediaIndex++;
    if (currentMediaIndex >= static_cast<int>(currentQueue.size()))
        currentMediaIndex = 0;
    play();
}

void PlayMediaService::previous() {
    currentMediaIndex = std::max(0, currentMediaIndex - 1);
    play();
}

void PlayMediaService::shuffle() {
    if (playlists.empty()) return;
    
    refreshCurrentQueue();
    if (currentQueue.empty()) return;
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::random_shuffle(currentQueue.begin(), currentQueue.end());
    
    currentMediaIndex = 0;
    isShuffled = true;
    std::cout << "Playlist shuffled.\n";
}

void PlayMediaService::setVolume(int level) {
    // Mix_VolumeMusic: level là giá trị từ 0 đến MIX_MAX_VOLUME (128)
    Mix_VolumeMusic(level);
    std::cout << "Set volume to: " << level << "\n";
}

void PlayMediaService::setCurrentPlaylistIndex(int index) {
    currentPlaylistIndex = index;
}

void PlayMediaService::setCurrentMediaIndex(int index) {
    currentMediaIndex = index;
}

void PlayMediaService::resetCurrentFile(int indexMediaFile, int indexPlaylist){
    if(indexMediaFile != currentMediaIndex || indexPlaylist != currentPlaylistIndex){
        pause();
        setCurrentMediaIndex(indexMediaFile);
        setCurrentPlaylistIndex(indexPlaylist);
        play();
    }
    

}

void PlayMediaService::seekTo(double seconds) {
    if (!Mix_PlayingMusic()) {
        std::cerr << "No music is playing to seek.\n";
        return;
    }

    if (Mix_SetMusicPosition(seconds) == -1) {
        std::cerr << "Failed to seek: " << Mix_GetError() << "\n";
        return;
    }
    std::cout << "Seeked to " << seconds << " seconds.\n";
}

