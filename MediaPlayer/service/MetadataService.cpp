#include "MetadataService.h"
#include <taglib/tag.h>

// Constructor nhận danh sách playlist từ AppService
MetadataService::MetadataService(std::vector<std::shared_ptr<Playlist>>& playlists) : playlists(playlists) {}

// Chỉnh sửa metadata của file media
void MetadataService::editMediaMetadata(const std::shared_ptr<MediaFile>& mediaFile) {
    if (!mediaFile) {
        std::cerr << "Invalid media file.\n";
        return;
    }

    const std::string& fileName = mediaFile->getFileName();

    for (const auto& playlist : playlists) {
        const auto& mediaMap = playlist->getAllMediaFiles();

        auto it = mediaMap.find(fileName);
        if (it != mediaMap.end()) {
            // Đã tìm thấy media file trong playlist
            MediaFileType type = mediaFile->getFileType();

            switch (type) {
                case MediaFileType::AUDIO: {
                    auto audio = std::dynamic_pointer_cast<AudioFile>(mediaFile);
                    if (audio) {
                        // Không cần setMetadata ở đây nếu đã được gán từ View
                        MetadataUtils::editMetadata(audio); // Ghi metadata ra file
                    }
                    break;
                }

                case MediaFileType::VIDEO: {
                    auto video = std::dynamic_pointer_cast<VideoFile>(mediaFile);
                    if (video) {
                        MetadataUtils::editMetadata(video);
                    }
                    break;
                }

                default:
                    std::cerr << "Unknown media file type.\n";
            }

            return;
        }
    }

    std::cerr << "Media file not found in any playlist.\n";
}