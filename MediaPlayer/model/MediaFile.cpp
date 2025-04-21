#include "MediaFile.h"

// Constructor
MediaFile::MediaFile(const std::string& name, const std::string& filePath, double fileDuration, const MediaFileType& type)
    : fileName(name), path(filePath), duration(fileDuration), fileType(type) {}

// Getter & Setter
std::string MediaFile::getFileName() const {
    return fileName;
}

void MediaFile::setFileName(const std::string& name) {
    fileName = name;
}

std::string MediaFile::getPath() const {
    return path;
}

void MediaFile::setPath(const std::string& filePath) {
    path = filePath;
}

double MediaFile::getDuration() const {
    return duration;
}

void MediaFile::setDuration(double fileDuration) {
    duration = fileDuration;
}

MediaFileType MediaFile::getFileType() const {
    return fileType;
}

void MediaFile::setFileType(MediaFileType type){
    fileType = type;
}
