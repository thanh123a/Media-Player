#include "MetadataUtils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unistd.h>

std::shared_ptr<MediaFile> MetadataUtils::createMediaFileFromPath(const std::string& filePath) {
    if (!MetadataUtils::fileExists(filePath)) return nullptr;

    std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    TagLib::FileRef fileRef(filePath.c_str());
    if (fileRef.isNull() || !fileRef.tag()) return nullptr;

    TagLib::Tag* tag = fileRef.tag();
    std::string title = tag->title().to8Bit(true);
    if (title.empty()) {
        size_t lastSlash = filePath.find_last_of("/\\");
        size_t lastDot = filePath.find_last_of('.');
        if (lastDot != std::string::npos && lastSlash != std::string::npos && lastDot > lastSlash) {
            title = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else {
            title = filePath;  // fallback nếu lỡ không có dấu / hoặc .
        }
    }
    std::string artist = tag->artist().to8Bit(true);
    std::string album = tag->album().to8Bit(true);
    std::string genre = tag->genre().to8Bit(true);
    int year = tag->year();
    int duration = fileRef.audioProperties() ? fileRef.audioProperties()->length() : 0;

    if (extension == "mp3" || extension == "wav" || extension == "flac") {
        return std::make_shared<AudioFile>(title, filePath, duration, album, artist, genre, "", year);
    } else if (extension == "mp4" || extension == "avi" || extension == "mkv") {
        int bitrate = fileRef.audioProperties() ? fileRef.audioProperties()->bitrate() : 0;
        return std::make_shared<VideoFile>(title, filePath, duration, 0.0, bitrate, extension);
    }

    return nullptr;
}

void MetadataUtils::displayMetadata(const std::shared_ptr<MediaFile>& mediaFile) {
    if (!mediaFile) {
        std::cerr << "No media file provided.\n";
        return;
    }

    std::cout << "Title: " << mediaFile->getFileName() << "\n";
    std::cout << "Duration: " << mediaFile->getDuration() << "s\n";

    if (mediaFile->getFileType() == MediaFileType::AUDIO) {
        auto audio = std::dynamic_pointer_cast<AudioFile>(mediaFile);
        std::cout << "Album: " << audio->getAlbum() << "\n";
        std::cout << "Artist: " << audio->getArtist() << "\n";
        std::cout << "Genre: " << audio->getGenre() << "\n";
        std::cout << "Year: " << audio->getPublishYear() << "\n";
    } else if (mediaFile->getFileType() == MediaFileType::VIDEO) {
        auto video = std::dynamic_pointer_cast<VideoFile>(mediaFile);
        std::cout << "Bitrate: " << video->getBitrate() << " kbps\n";
        std::cout << "Codec: " << video->getCodec() << "\n";
    }
}

void MetadataUtils::editMetadata(const std::shared_ptr<MediaFile>& mediaFile) {
    if (!mediaFile) {
        std::cerr << "Invalid media file.\n";
        return;
    }

    TagLib::FileRef fileRef(mediaFile->getPath().c_str());
    if (fileRef.isNull() || !fileRef.tag()) {
        std::cerr << "Failed to open file with TagLib: " << mediaFile->getPath() << std::endl;
        return;
    }

    TagLib::Tag* tag = fileRef.tag();
    MediaFileType type = mediaFile->getFileType();

    switch (type) {
        case MediaFileType::AUDIO: {
            auto audio = std::dynamic_pointer_cast<AudioFile>(mediaFile);
            if (audio && tag) {
                tag->setTitle(audio->getFileName());
                tag->setArtist(audio->getArtist());
                tag->setAlbum(audio->getAlbum());
                tag->setGenre(audio->getGenre());
                tag->setYear(audio->getPublishYear());
                fileRef.save();
            }
            break;
        }

        case MediaFileType::VIDEO: {
            auto video = std::dynamic_pointer_cast<VideoFile>(mediaFile);
            if (video && tag) {
                tag->setTitle(video->getFileName());
                // Có thể thêm các tag video khác nếu cần
                fileRef.save();
            }
            break;
        }

        default:
            std::cerr << "Unsupported media type for metadata editing.\n";
    }
}

bool MetadataUtils::fileExists(const std::string& filePath) {
    return access(filePath.c_str(), F_OK) == 0;
}
