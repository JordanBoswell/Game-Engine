#ifndef SoundPlayer_h
#define SoundPlayer_h



#include <SFML\Audio\Sound.hpp>
#include <SFML\System\Vector3.hpp>
#include <cassert>
#include <list>


//P.O.D. that contains all information needed to specify a sound's properties.
struct SoundInfo
{
    bool relativeToListener{true};
    sf::Vector3f position{0.0f, 0.0f, 0.0f};
    float volume{100.0f};
    float pitch{1.0f};
    float minDistance{200.0f};
    float attenuation{8.0f};
};


/*----------------------------------------------------------------------------------
Manager and player of sounds.  Sounds must be periodically removed via a call to 
removeStoppedSounds in order to stay under the limit of simultaneous sounds and music.
The sounds can be positioned in 3d space.
 ----------------------------------------------------------------------------------*/
class SoundPlayer : sf::NonCopyable
{
public:
    void playSound(sf::SoundBuffer& soundBuffer, SoundInfo soundInfo = SoundInfo());
    //Should be called periodically(though not necessarily each frame) to remove finished sounds
    void removeStoppedSounds();
    
private:
    std::list<sf::Sound> mSounds;
};



#endif