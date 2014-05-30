#include "SoundPlayer.h"



void SoundPlayer::playSound(sf::SoundBuffer& soundBuffer, SoundInfo soundInfo)
{
    assert(soundInfo.volume >= 0 && soundInfo.volume <= 100 && "Volume must be between 0-100 in SoundPlayer::playSound()");
    assert(soundInfo.pitch > 0 && "Pitch must be greater than 0 in SoundPlayer::playSound");
    assert(soundInfo.minDistance > 0 && "minDistance must be greater than 0 in SoundPlayer::playSound");
    assert(soundInfo.attenuation >= 0 && "attenuation must be greater than or equal to 0 in SoundPlayer::playSound");
    
    sf::Sound sound(soundBuffer);
    sound.setRelativeToListener(soundInfo.relativeToListener);
    sound.setPosition(soundInfo.position);
    sound.setVolume(soundInfo.volume);
    sound.setPitch(soundInfo.pitch);
    sound.setMinDistance(soundInfo.minDistance);
    sound.setAttenuation(soundInfo.attenuation);
    mSounds.push_back(sf::Sound(soundBuffer));
    mSounds.back().play();
}


void SoundPlayer::removeStoppedSounds()
{
    for(auto iter = mSounds.begin(); iter != mSounds.end(); ++iter)
    {
        if(iter->getStatus() == sf::Sound::Status::Stopped)
            mSounds.erase(iter);
    }
}