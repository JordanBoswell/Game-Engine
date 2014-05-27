#ifndef MusicPlayer_h
#define MusicPlayer_h



#include <map>
#include <tuple>
#include <vector>
#include <stack>
#include <SFML\System\Time.hpp>
#include <SFML\System\NonCopyable.hpp>


/*----------------------------------------------------------------------------------
A robust music player that stores Playlists and retrieves and plays them.  Playlists
are moved to the MusicPlayer, not copied, so they should not be used outside of the 
MusicPlayer after adding them. Due to the limit on the number of simultaneous Music 
and sf::Sound instances, MusicPlayer uses only one Music instance.  The option 
exists for saving the current music state in a stack when loading a new Playlist, 
which allows the user to resume the previous Playlist once finished with the current 
one.  update() must be called frequently to check if the current song has ended,
at which point the function will play the next song.
----------------------------------------------------------------------------------*/
template<typename T_PlaylistId> //Key used to id the Playlists.
class MusicPlayer : public sf::NonCopyable
{
public:
	enum class Status{Playing, Paused, SongStopped, PlaylistStopped, Empty};

	using Playlist = std::vector<std::string>; //Filenames of the comprising songs.
	//The required information needed to cache and later retrieve a Playlist's state. 
	using MusicState = std::tuple
						<std::vector<std::string*>, //Playlist 
						int, //Song position in Playlist
						sf::Time, //Elapsed time
						bool, //Looped Playlist or not.
						bool>; //Set to shuffle or not.

public:
	MusicPlayer();
	//Store Playlist for possible future retrieval
	void storePlaylist(const T_PlaylistId& id, Playlist&& playlist);
	//Volume is in the range 0-100
	float getVolume() const;
	void setVolume(float newVolume);
	Status getMusicStatus() const;
	//Check if song is finished and, if so, play the next song.  Should be called often.
	void update();
	void play();
	void pause();
	//Stop music and go to beginning of current song.
	void stopSong();
	//Stop music, shuffle Playlist if shuffle is on, and go to beginning of current Playlist.
	void stopPlaylist();
	//Go to the next song in the Playlist, keeping previous status.
	void nextSong();
	void previousSong();
	//Load a stored Playlist which will start with PlaylistStopped status.  Shuffles it if shuffle argument is true.
	void loadPlaylist(T_PlaylistId id, bool looped, bool shuffle, bool saveCurrentMusic = false);
	int getNumSavedPlaylists() const;
	//Clear the loaded playlist(but still keep it in storage).  Load the most recently stored Playlist(if any) with Paused status. 
	void popCurrentPlaylist();
	
private:
    void openCurrentSongFile();
    
	
private:
	Status mStatus;
	std::map<T_PlaylistId, Playlist> mStoredPlaylists;
	//Playlist iterators.  Used to shuffle without affecting source.
	std::vector<std::string*> mCurrentPlaylist;
	int mCurrentSong;
	sf::Music mMusicStream;
	bool mLooped;
	bool mShuffle;
	std::stack<MusicState> mSavedMusicStates;
};

#include "MusicPlayer.inl"
//will we need to add code to ensure the music is played unaffected by listener movement.

#endif