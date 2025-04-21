#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include "MediaFile.h"

class VideoFile : public MediaFile {
private:
    double fileSize;  // Kích thước file (MB)
    int bitrate;      // Bitrate (kbps)
    std::string codec;

public:
    // Constructor
    VideoFile(const std::string& name, const std::string& filePath, double duration, 
              double size, int bitrate, const std::string& codec);

    // Getter & Setter
    double getFileSize() const;
    void setFileSize(double size);

    int getBitrate() const;
    void setBitrate(int bitrateValue);

    std::string getCodec() const;
    void setCodec(const std::string& codecType);

};

#endif
