#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <string>
#include "MediaFileType.h" 

class MediaFile {
protected:
    std::string fileName;  // Tên file (VD: "song.mp3")
    std::string path;      // Đường dẫn đầy đủ của file (VD: "/home/user/music/song.mp3")
    double duration;       // Thời lượg (giây)
    MediaFileType fileType;  // Loại file (audio/video)

public:
    // Constructor
    MediaFile(const std::string& name, const std::string& filePath, double fileDuration, const MediaFileType& type);
    virtual ~MediaFile() = default;

    // Getter & Setter
    std::string getFileName() const;
    void setFileName(const std::string& name);

    std::string getPath() const;
    void setPath(const std::string& filePath);

    double getDuration() const;
    void setDuration(double fileDuration);

    MediaFileType getFileType() const;
    void setFileType(MediaFileType type);
};

#endif
