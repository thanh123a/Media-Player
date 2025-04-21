#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include "MediaFile.h"

class AudioFile : public MediaFile {
private:
    std::string album;
    std::string artist;
    std::string genre;
    std::string publisher;
    int publishYear;

public:
    // Constructor
    AudioFile(const std::string& name, const std::string& filePath, double duration, 
              const std::string& album, const std::string& artist, 
              const std::string& genre, const std::string& publisher, int year);

    // Getter & Setter
    std::string getAlbum() const;
    void setAlbum(const std::string& albumName);

    std::string getArtist() const;
    void setArtist(const std::string& artistName);

    std::string getGenre() const;
    void setGenre(const std::string& genreName);

    std::string getPublisher() const;
    void setPublisher(const std::string& publisherName);

    int getPublishYear() const;
    void setPublishYear(int year);
};

#endif
