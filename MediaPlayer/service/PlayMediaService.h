#ifndef PLAY_MEDIA_SERVICE_H
#define PLAY_MEDIA_SERVICE_H

#include <vector>
#include <memory>
#include <string>
#include <SDL_mixer.h>
#include "Playlist.h"
#include "PlayableMediaFile.h"
#include "MediaFile.h"

class PlayMediaService {
private:
    // Danh sách playlist dùng chung (truyền tham chiếu từ AppService)
    std::vector<std::shared_ptr<Playlist>>& playlists;
    
    // Vector nội bộ để thao tác theo thứ tự phát
    std::vector<std::shared_ptr<MediaFile>> currentQueue;

   
    bool stopPlaybackThread = false;
    
    // Chỉ số của playlist và chỉ số media hiện tại
    int currentPlaylistIndex;
    int currentMediaIndex;
    
    bool isPaused;
    bool isShuffled;
    
    // SDL_mixer Music pointer (dùng để phát nhạc)
    Mix_Music* music;
    
    // Hàm helper: làm mới currentQueue từ playlist hiện tại
    void refreshCurrentQueue();
    
    // Hàm helper: chuyển đối tượng MediaFile thành PlayableMediaFile (sao chép thông tin cơ bản nếu cần)
    //std::shared_ptr<PlayableMediaFile> convertToPlayable(const std::shared_ptr<MediaFile>& mediaFile);

public:
    // Constructor nhận tham chiếu đến danh sách playlist dùng chung
    PlayMediaService(std::vector<std::shared_ptr<Playlist>>& playlists);
    
    // Destructor
    ~PlayMediaService();
    
    // Các chức năng điều khiển phát nhạc
    void play();
    void pause();
    void resume();
    void next();
    void previous();
    void shuffle();
    void setVolume(int level);  // Điều chỉnh âm lượng thực tế
    
    // Hàm nạp lại media của bài hiện tại, trả về true nếu thành công
    bool loadCurrentMedia();

    static void onMusicFinishedStatic();
    static PlayMediaService* instance; // singleton hoặc tham chiếu toàn cục
    void onMusicFinished(); 
    void setCurrentPlaylistIndex(int index);
    void setCurrentMediaIndex(int index);
    void resetCurrentFile(int indexMediaFile, int indexPlaylist);
    void seekTo(double seconds);

};

#endif
