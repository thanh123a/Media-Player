#ifndef METADATASERVICE_H
#define METADATASERVICE_H

#include <vector>
#include <memory>
#include <string>
#include "Playlist.h"
#include "AudioFile.h"
#include "VideoFile.h"
#include "MetadataUtils.h"

class MetadataService {
private:
    std::vector<std::shared_ptr<Playlist>>& playlists;

public:
    MetadataService(std::vector<std::shared_ptr<Playlist>>& playlists);

    // Chỉnh sửa metadata của file media
    void editMediaMetadata(const std::shared_ptr<MediaFile>& mediaFile);
};

#endif
