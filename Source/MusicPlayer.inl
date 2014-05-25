#include <utility>
#include <algorithm>
#include "MusicPlayer.h"


template<typename T_PlaylistID>
MusicPlayer<T_PlaylistID>::MusicPlayer()
:mStatus(Status::Empty)
{}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::storePlaylist(const T_PlaylistID& id, Playlist&& playlist)
{
	mStoredPlaylists.emplace(std::make_pair(id, std::move(playlist)));
}


template<typename T_PlaylistID>
float MusicPlayer<T_PlaylistID>::getVolume() const
{
	return mMusicStream.getVolume();
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::setVolume(float newVolume)
{
	mMusicStream.setVolume(newVolume);
}


template<typename T_PlaylistID>
typename MusicPlayer<T_PlaylistID>::Status MusicPlayer<T_PlaylistID>::getMusicStatus() const
{
	return mStatus;
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::update()
{
	if(mStatus == Status::Playing && mMusicStream.getStatus() == sf::Music::Stopped)
		nextSong();
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::play()
{
	switch(mStatus)
	{
	case Status::Playing:
	case Status::Paused:
	case Status::SongStopped:
	case Status::PlaylistStopped:
		mMusicStream.play();
		mStatus = Status::Playing;
		break;
	}
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::pause()
{
	if(mStatus == Status::Playing)
	{
		mMusicStream.pause();
		mStatus = Status::Paused;
	}
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::stopSong()
{
	if(mStatus != Status::Empty && mStatus != Status::PlaylistStopped)
	{
		mMusicStream.stop();
		mStatus = Status::SongStopped;
	}
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::stopPlaylist()
{
	if(mStatus != Status::Empty && mStatus != Status::PlaylistStopped)
	{
		if(mShuffle)
			std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
		mCurrentSong = mCurrentPlaylist.begin();
		mMusicStream.openFromFile(**mCurrentSong);
		mStatus = Status::PlaylistStopped;
	}
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::nextSong()
{
	if(mStatus != Status::Empty)
	{
		++mCurrentSong;
		if(mCurrentSong != mCurrentPlaylist.end())
		{
			mMusicStream.openFromFile(**mCurrentSong);
			if(mStatus == Status::Playing)
				mMusicStream.play();
		}
		else if(mLooped == true)
		{
			if(mShuffle)
				std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
			mCurrentSong = mCurrentPlaylist.begin();
			mMusicStream.openFromFile(**mCurrentSong);
			if(mStatus == Status::Playing)
				mMusicStream.play();
		}
		else
			stopPlaylist();
	}
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::previousSong()
{
	if(mStatus != Status::Empty)
	{
		if(mCurrentSong != mCurrentPlaylist.begin())
		{
			--mCurrentSong;
			mMusicStream.openFromFile(**mCurrentSong);
			if(mStatus == Status::Playing)
				mMusicStream.play();
		}
		else if(mLooped == true)
		{
			if(mShuffle)
				std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
			mCurrentSong = --mCurrentPlaylist.end();
			mMusicStream.openFromFile(**mCurrentSong);
			if(mStatus == Status::Playing)
				mMusicStream.play();
		}
		else
			stopPlaylist();
	}
}


template<typename T_PlaylistID>
bool MusicPlayer<T_PlaylistID>::loadPlaylist(T_PlaylistID id, bool looped, bool shuffle, bool saveCurrentMusic)
{
	std::map<T_PlaylistID, Playlist>::iterator playlistToLoadIter = mStoredPlaylists.find(id);
	bool playlistFound = playlistToLoadIter == mStoredPlaylists.end() ? false : true;
	if(playlistFound == true)
	{
		if(saveCurrentMusic == true && mCurrentPlaylist.size())
		{
			sf::Time elapsedTime = mMusicStream.getPlayingOffset();
			MusicState stateToBeSaved = std::make_tuple(mCurrentPlaylist, mCurrentSong, elapsedTime, mLooped, mShuffle);
			mSavedMusicStates.push(stateToBeSaved);
		}
		if(mMusicStream.getStatus() == sf::Music::Playing)
			mMusicStream.stop();
		mStatus = Status::PlaylistStopped;
		mCurrentPlaylist.clear();
		for(auto& song : playlistToLoadIter->second)
		{
			mCurrentPlaylist.push_back(&song);
		}
		if(shuffle)
			std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
		mCurrentSong = mCurrentPlaylist.begin();
		mLooped = looped;
		mShuffle = shuffle;
		mMusicStream.openFromFile(**mCurrentSong);
	}
	return playlistFound;
}


template<typename T_PlaylistID>
int MusicPlayer<T_PlaylistID>::getNumSavedPlaylists() const
{
	return mSavedMusicStates.size();
}


template<typename T_PlaylistID>
void MusicPlayer<T_PlaylistID>::popCurrentPlaylist()
{
	if(mSavedMusicStates.size())
	{
		mStatus = Status::PlaylistStopped;
		MusicState& mostRecentlySavedState = mSavedMusicStates.top();
		mCurrentPlaylist = std::get<0>(mostRecentlySavedState);
		mCurrentSong = std::get<1>(mostRecentlySavedState);
		sf::Time savedTime = std::get<2>(mostRecentlySavedState);
		mLooped = std::get<3>(mostRecentlySavedState);
		mShuffle = std::get<4>(mostRecentlySavedState);
		mSavedMusicStates.pop();
		mMusicStream.openFromFile(**mCurrentSong);
		mMusicStream.setPlayingOffset(savedTime);
	}
	else
	{
		mCurrentPlaylist.clear();
		mStatus = Status::Empty;
	}
}