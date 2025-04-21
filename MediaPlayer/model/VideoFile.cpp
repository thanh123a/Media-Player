#include "VideoFile.h"

// Constructor
VideoFile::VideoFile(const std::string& name, const std::string& filePath, double duration, 
                     double size, int bitrate, const std::string& codec)
    : MediaFile(name, filePath, duration, MediaFileType::VIDEO), fileSize(size), bitrate(bitrate), codec(codec) {}

// Getter & Setter
double VideoFile::getFileSize() const { return fileSize; }
void VideoFile::setFileSize(double size) { fileSize = size; }

int VideoFile::getBitrate() const { return bitrate; }
void VideoFile::setBitrate(int bitrateValue) { bitrate = bitrateValue; }

std::string VideoFile::getCodec() const { return codec; }
void VideoFile::setCodec(const std::string& codecType) { codec = codecType; }
