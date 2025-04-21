#include "AudioFile.h"

// Constructor
AudioFile::AudioFile(const std::string& name, const std::string& filePath, double duration, 
                     const std::string& album, const std::string& artist, 
                     const std::string& genre, const std::string& publisher, int year)
    : MediaFile(name, filePath, duration, MediaFileType::AUDIO), album(album), artist(artist), genre(genre), 
      publisher(publisher), publishYear(year) {}

// Getter & Setter
std::string AudioFile::getAlbum() const { return album; }
void AudioFile::setAlbum(const std::string& albumName) { album = albumName; }

std::string AudioFile::getArtist() const { return artist; }
void AudioFile::setArtist(const std::string& artistName) { artist = artistName; }

std::string AudioFile::getGenre() const { return genre; }
void AudioFile::setGenre(const std::string& genreName) { genre = genreName; }

std::string AudioFile::getPublisher() const { return publisher; }
void AudioFile::setPublisher(const std::string& publisherName) { publisher = publisherName; }

int AudioFile::getPublishYear() const { return publishYear; }
void AudioFile::setPublishYear(int year) { publishYear = year; }
