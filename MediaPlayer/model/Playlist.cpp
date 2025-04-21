#include "Playlist.h"
#include <iostream>
#include <ctime>
#include "MediaFileType.h"

// Constructor
Playlist::Playlist(const std::string& name) : playlistName(name) {
    std::time_t now = std::time(nullptr);
    quantity = 0;
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&now));
    createdDate = buffer;
}

// Getter & Setter
std::string Playlist::getPlaylistName() const { return playlistName; }
void Playlist::setPlaylistName(const std::string& name) { playlistName = name; }

std::string Playlist::getCreatedDate() const { return createdDate; }
void Playlist::setCreatedDate(const std::string& date) { createdDate = date; }

int Playlist::getQuantity() const { return quantity; }

// Playlist Management
void Playlist::addMediaFile(std::shared_ptr<MediaFile> mediaFile) {
    if (!mediaFile) return;
    mediaFiles[mediaFile->getFileName()] = mediaFile;
    quantity += 1;
}

void Playlist::removeMediaFile(const std::string& fileName) {
    auto it = mediaFiles.find(fileName);
    if (it != mediaFiles.end()) {
        mediaFiles.erase(it);
        quantity = mediaFiles.size(); // Cập nhật số lượng file
    } else {
        std::cerr << "File not found: " << fileName << std::endl;
    }
}

std::shared_ptr<MediaFile> Playlist::getMediaFile(const std::string& fileName) const {
    auto it = mediaFiles.find(fileName);
    if (it != mediaFiles.end()) {
        return it->second;
    }
    return nullptr;
}

const std::unordered_map<std::string, std::shared_ptr<MediaFile>>& Playlist::getAllMediaFiles() const {
    return mediaFiles;
}

void Playlist::clearPlaylist() {
    mediaFiles.clear();
}

// ✨ **Hàm chỉnh sửa file media**
bool Playlist::editMediaFile(std::shared_ptr<MediaFile> mediaFile) {
    if (!mediaFile) {
        std::cout << "Invalid media file.\n";
        return false;
    }

    auto it = mediaFiles.find(mediaFile->getFileName());
    if (it == mediaFiles.end()) {
        std::cout << "File '" << mediaFile->getFileName() << "' not found in playlist.\n";
        return false;
    }

    std::shared_ptr<MediaFile> existingFile = it->second;
    
    // Xác định kiểu file
    switch (mediaFile->getFileType()) {
        case MediaFileType::AUDIO: {
            std::shared_ptr<AudioFile> audioFile = std::dynamic_pointer_cast<AudioFile>(existingFile);
            std::shared_ptr<AudioFile> newAudioFile = std::dynamic_pointer_cast<AudioFile>(mediaFile);
            if (audioFile && newAudioFile) {
                audioFile->setFileName(newAudioFile->getFileName());
                audioFile->setAlbum(newAudioFile->getAlbum());
                audioFile->setArtist(newAudioFile->getArtist());

                // Nếu tên file thay đổi, cập nhật key trong unordered_map
                if (existingFile->getFileName() != newAudioFile->getFileName()) {
                    mediaFiles.erase(it);
                    mediaFiles[newAudioFile->getFileName()] = audioFile;
                }
                return true;
            }
            break;
        }
        case MediaFileType::VIDEO: {
            std::shared_ptr<VideoFile> videoFile = std::dynamic_pointer_cast<VideoFile>(existingFile);
            std::shared_ptr<VideoFile> newVideoFile = std::dynamic_pointer_cast<VideoFile>(mediaFile);
            if (videoFile && newVideoFile) {
                videoFile->setFileName(newVideoFile->getFileName());
                videoFile->setBitrate(newVideoFile->getBitrate());
                videoFile->setCodec(newVideoFile->getCodec());

                // Nếu tên file thay đổi, cập nhật key trong unordered_map
                if (existingFile->getFileName() != newVideoFile->getFileName()) {
                    mediaFiles.erase(it);
                    mediaFiles[newVideoFile->getFileName()] = videoFile;
                }
                return true;
            }
            break;
        }
        default:
            std::cout << "Invalid media type.\n";
            return false;
    }

    std::cout << "Failed to edit file '" << mediaFile->getFileName() << "'.\n";
    return false;
}
