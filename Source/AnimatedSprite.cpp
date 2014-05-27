#include <cassert>
#include "AnimatedSprite.h"


AnimatedSprite::AnimatedSprite(std::vector::size_type objectId)
:mObjectId(objectId),
mCurrentAnimationSequence(nullptr),
mCallback([](){})
{
	assert(objectId >= 0 && objectId < sAnimations.length() && "AnimatedSprite() failed due to objectId being out of vector bounds.");
}


AnimatedSprite::AnimatedSprite(std::vector::size_type objectId, const Texture& texture)
:sf::Sprite(texture),
mObjectId(objectId),
mCurrentAnimationSequence(nullptr),
mCallback([](){})
{
	assert(objectId >= 0 && objectId < sAnimations.length() && "AnimatedSprite() failed due to objectId being out of vector bounds.");
}


AnimatedSprite(std::vector::size_type objectId, const Texture& texture, const sf::IntRect& textureSubRectangle)
:sf::Sprite(texture, textureSubRectangle),
mObjectId(objectId),
mCurrentAnimationSequence(nullptr),
mCallback([](){})
{
	assert(objectId >= 0 && objectId < sAnimations.length() && "AnimatedSprite() failed due to objectId being out of vector bounds.");
}


void AnimatedSprite::setAnimation(std::vector::size_type animationId, sf::Time timePerFrame, unsigned numLoops, CallbackConditions callbackConditions, std::function<void()> callback)
{
	assert(animationId >= 0 && animationId < sAnimations[mId].length() && "animationId is out of vector bounds in AnimatedSprite::setAnimation()");
	assert(timePerFrame > sf::Time::Zero && "encountered: timePerFrame less than or equal to zero in AnimatedSprite::setAnimation");
	
	if(mCallbackConditions & AnimationSwitch)
		mCallback();
	mCurrentFrame = sAnimations[mObjectId][animationId].begin();
	mTimePerFrame = timePerFrame;
	mAccumulatedTime = sf::Time::Zero;
	mContinuouslyLooping = false;
	mNumLoopsRemaining = numLoopsRemaining;
	mCallback = callback;
}


void AnimatedSprite::setContinuouslyLoopingAnimation(std::vector::size_type animationId, sf::Time timePerFrame, CallbackConditions callbackConditions, std::function<void()> callback)
{
	assert(animationId >= 0 && animationId < sAnimations[mId].length() && "animationId is out of vector bounds in AnimatedSprite::setContinuouslyLoopingAnimation()");
	assert(timePerFrame > sf::Time::Zero && "encountered: timePerFrame less than or equal to zero in AnimatedSprite::setContinuouslyLoopingAnimation");
	
	if(mCallbackConditions & AnimationSwitch)
		mCallback();
	mCurrentFrame = sAnimations[mObjectId][animationId].begin();
	mTimePerFrame = timePerFrame;
	mAccumulatedTime = sf::Time::Zero;
	mContinuouslyLooping = true;
	mCallback = callback;
}


bool AnimatedSprite::update(sf::Time deltaTime)
{
	assert(mCurrentAnimationSequence != nullptr && "AnimatedSprite::update() called before an animation sequence was set.");
	assert(deltaTime < sf::Time::Zero && "AnimatedSprite::update() passed sf::Time arg with a negative value");
	
	mAccumulatedFrameTime += deltaTime;
	if(mAccumulatedFrameTime >= mTimePerFrame)
	{
		++mCurrentFrame;
		mAccumulatedFrameTime = sf::Time::Zero;
		if(mCurrentFrame == mCurrentAnimationSequence->end())
		{
			if(mContinuouslyLooping == true || --mNumLoopsRemaining > 0)
			{
				mCurrentFrame = mCurrentAnimationSequence->begin();
				setTextureRect(*mCurrentFrame);
				if(mCallbackConditions & EachLoopEnd)
					callback();
			}
			else
			{
				if(mCallbackConditions & EachLoopEnd | AnimationCompletion)
					callback();
				return true;
			}
		}
		else
			setTextureRect(*mCurrentFrame);
	}
	return false;
}


void AnimatedSprite::executeCallback()
{
	callback();
}