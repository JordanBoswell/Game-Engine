#include "MusicPlayer.h"


template<typename T_PlaylistId>
MusicPlayer<T_PlaylistId>::MusicPlayer()
:mStatus(Status::Empty)
{
    mMusicStream.setRelativeToListener(true);
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::storePlaylist(const T_PlaylistId& id, Playlist&& playlist)
{
    bool success = mStoredPlaylists.emplace(std::make_pair(id, std::move(playlist))).second;
    assert(success == true && "emplace() failed in MusicPlayer::storePlaylist()");
}


template<typename T_PlaylistId>
float MusicPlayer<T_PlaylistId>::getVolume() const
{
    return mMusicStream.getVolume();
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::setVolume(float newVolume)
{
    mMusicStream.setVolume(newVolume);
}


template<typename T_PlaylistId>
typename MusicPlayer<T_PlaylistId>::Status MusicPlayer<T_PlaylistId>::getMusicStatus() const
{
    return mStatus;
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::update()
{
    if(mStatus == Status::Playing && mMusicStream.getStatus() == sf::Music::Stopped)
        nextSong();
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::play()
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


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::pause()
{
    if(mStatus == Status::Playing)
    {
        mMusicStream.pause();
        mStatus = Status::Paused;
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::stopSong()
{
    if(mStatus != Status::Empty && mStatus != Status::PlaylistStopped)
    {
        mMusicStream.stop();
        mStatus = Status::SongStopped;
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::stopPlaylist()
{
    if(mStatus != Status::Empty && mStatus != Status::PlaylistStopped)
    {
        if(mShuffle)
            std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
        mCurrentSong = 0;
        openCurrentSongFile();
        mStatus = Status::PlaylistStopped;
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::nextSong()
{
    if(mStatus != Status::Empty)
    {
        ++mCurrentSong;
        if(mCurrentSong < mCurrentPlaylist.size())
        {
            openCurrentSongFile();
            if(mStatus == Status::Playing)
                mMusicStream.play();
        }
        else if(mLooped == true)
        {
            if(mShuffle)
                std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
            mCurrentSong = 0;
            openCurrentSongFile();
            if(mStatus == Status::Playing)
                mMusicStream.play();
        }
        else
            stopPlaylist();
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::previousSong()
{
    if(mStatus != Status::Empty)
    {
        if(mCurrentSong != 0)
        {
            --mCurrentSong;
            openCurrentSongFile();
            if(mStatus == Status::Playing)
                mMusicStream.play();
        }
        else if(mLooped == true)
        {
            if(mShuffle)
                std::random_shuffle(mCurrentPlaylist.begin(), mCurrentPlaylist.end());
            mCurrentSong = mCurrentPlaylist.size() - 1;
            openCurrentSongFile();
            if(mStatus == Status::Playing)
                mMusicStream.play();
        }
        else
            stopPlaylist();
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::loadPlaylist(T_PlaylistId id, bool looped, bool shuffle, bool saveCurrentMusic)
{
    std::map<T_PlaylistId, Playlist>::iterator playlistToLoadIter = mStoredPlaylists.find(id);
    bool playlistFound = playlistToLoadIter == mStoredPlaylists.end() ? false : true;
    assert(playlistFound == true && "Playlist not found in mStoredPlaylists in MusicPlayer::loadPlaylist()");
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
    mCurrentSong = 0;
    mLooped = looped;
    mShuffle = shuffle;
    openCurrentSongFile();
}


template<typename T_PlaylistId>
int MusicPlayer<T_PlaylistId>::getNumSavedPlaylists() const
{
    return mSavedMusicStates.size();
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::popCurrentPlaylist()
{
    if(mSavedMusicStates.size())
    {
        MusicState& mostRecentlySavedState = mSavedMusicStates.top();
        mCurrentPlaylist = std::get<0>(mostRecentlySavedState);
        mCurrentSong = std::get<1>(mostRecentlySavedState);
        sf::Time savedTime = std::get<2>(mostRecentlySavedState);
        mLooped = std::get<3>(mostRecentlySavedState);
        mShuffle = std::get<4>(mostRecentlySavedState);
        mSavedMusicStates.pop();
        openCurrentSongFile();
        mMusicStream.setPlayingOffset(savedTime);
        mStatus = Status::Paused;
    }
    else
    {
        mCurrentPlaylist.clear();
        if(mMusicStream.getStatus() == sf::Music::Playing)
            mMusicStream.stop();
        mStatus = Status::Empty;
    }
}


template<typename T_PlaylistId>
void MusicPlayer<T_PlaylistId>::openCurrentSongFile()
{
    bool success = mMusicStream.openFromFile(*mCurrentPlaylist[mCurrentSong]);
    if(success == false)
        throw std::runtime_error("The music stream failed to open " + *mCurrentPlaylist[mCurrentSong] + " in MusicPlayer::openCurrentSongFile()." );
}