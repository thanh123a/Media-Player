#ifndef METADATA_UTILS_H
#define METADATA_UTILS_H

#include <string>
#include <memory>
#include "MediaFile.h"
#include "AudioFile.h"
#include "VideoFile.h"

class MetadataUtils {
public:
    // Tạo MediaFile (Audio/Video) từ đường dẫn
    static std::shared_ptr<MediaFile> createMediaFileFromPath(const std::string& filePath);

    // Hiển thị metadata (in ra console)
    static void displayMetadata(const std::shared_ptr<MediaFile>& mediaFile);

    // Sửa metadata từ input
    static void editMetadata(const std::shared_ptr<MediaFile>& mediaFile);

    // Check existed
    static bool fileExists(const std::string& filePath);
};

#endif
