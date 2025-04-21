// #include "PlayableMediaFile.h"

// PlayableMediaFile::PlayableMediaFile()
//     : MediaFile("", "", 0.0, MediaFileType::UNKNOWN)  // hoặc giá trị mặc định tùy bạn định nghĩa
// {
//     // Có thể để trống hoặc đặt giá trị mặc định cho các trường riêng của PlayableMediaFile
// }

// PlayableMediaFile::PlayableMediaFile(const std::string& name, const std::string& filePath, double fileDuration, const MediaFileType& type)
//     : MediaFile(name, filePath, duration, type), previousFilePath(""), nextFilePath(""), volumeLevel(50) {}

// std::string PlayableMediaFile::getPreviousFilePath() const {
//     return previousFilePath;
// }

// void PlayableMediaFile::setPreviousFilePath(const std::string& path) {
//     previousFilePath = path;
// }

// std::string PlayableMediaFile::getNextFilePath() const {
//     return nextFilePath;
// }

// void PlayableMediaFile::setNextFilePath(const std::string& path) {
//     nextFilePath = path;
// }

// int PlayableMediaFile::getVolumeLevel() const {
//     return volumeLevel;
// }

// void PlayableMediaFile::setVolumeLevel(int volume) {
//     volumeLevel = std::max(0, std::min(100, volume)); // Giới hạn từ 0 - 100
// }

// std::string PlayableMediaFile::getPlaylistName() const{
//     return playlistName;
// }

// void PlayableMediaFile::setPlaylistName(const std::string& name){
//     playlistName = name;
// }
