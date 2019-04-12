#ifndef SOUNDS_H_INCLUDED
#define SOUNDS_H_INCLUDED


void PlayMusicLooping(const std::string& file){
    std::cout << "sounds init was called\n";

    sf::SoundBuffer buffer;
    bool status = buffer.loadFromFile(file);
    if(!status){
        std::cout << "file not found " << file << std::endl;
    }

    std::cout <<"loadFromFile status: "<< status << std::endl;

    sf::Sound audio(buffer);

    audio.setLoop(true);
    audio.setVolume(10.f);
    audio.play();

    // Loop while the sound is playing
    while (audio.getStatus() == sf::Sound::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));

        // Display the playing position
        //std::cout << "\rPlaying... " << audio.getPlayingOffset().asSeconds() << " sec        ";
        //std::cout << std::flush;
    }
};

void PrintSoundStatus(sf::SoundSource::Status s){
    switch(s)
    {
        case sf::Sound::Paused:
            std::cout << "Paused\n";
        break;
        case sf::Sound::Playing:
            std::cout << "Playing\n";
        break;
        case sf::Sound::Stopped:
             std::cout << "Stopped\n";
        break;
    }
};

void PlaySoundEffect(const std::string& file){
    std::cout <<"PlaySoundEffect " << std::endl;

    sf::SoundBuffer buffer;
    bool status = buffer.loadFromFile(file);
    if(!status){
        std::cout << "file not found " << file << std::endl;
    }

    std::cout <<"loadFromFile status: "<< status << std::endl;

    sf::Sound audio(buffer);

    audio.setVolume(30.f);
    audio.play();

    while (audio.getStatus() == sf::Sound::Playing)
    {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));
        // Display the playing position
        //std::cout << "\rPlaying... " << audio.getPlayingOffset().asSeconds() << " sec        ";
        //std::cout << std::flush;
    }
}

bool MusicInit(const std::string& musicFile){
    sf::Music music;
    if (!music.openFromFile(musicFile)){
        std::cout << "audio file not found:" << musicFile << std::endl;
        return false; // error
    }
    music.play();

    return true;
};

#endif // SOUNDS_H_INCLUDED
