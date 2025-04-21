#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <unordered_map>
#include <string>
#include <memory>
#include "AudioFile.h"
#include "VideoFile.h"
#include "MediaFileType.h"

class Playlist {
private:
    std::string playlistName;
    std::unordered_map<std::string, std::shared_ptr<MediaFile>> mediaFiles;
    std::string createdDate;
    int quantity; 

    public:
    Playlist(const std::string& name);

    std::string getPlaylistName() const;
    void setPlaylistName(const std::string& name);
    std::string getCreatedDate() const;
    void setCreatedDate(const std::string& date);
    int getQuantity() const;
    
    void addMediaFile(std::shared_ptr<MediaFile> mediaFile);
    void removeMediaFile(const std::string& fileName);
    std::shared_ptr<MediaFile> getMediaFile(const std::string& fileName) const;
    const std::unordered_map<std::string, std::shared_ptr<MediaFile>>& getAllMediaFiles() const;
    void clearPlaylist();
    bool editMediaFile(std::shared_ptr<MediaFile> mediaFile);
};

#endif
