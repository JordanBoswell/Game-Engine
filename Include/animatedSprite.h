#ifndef AnimatedSprite_h
#define AnimatatedSprite_h



#include <vector>

#include <SFML\Graphics\Sprite>
#include <SFML\Graphics\Rect.hpp>
#include <SFML\System\Time.hpp>



/*----------------------------------------------------------------------------------
A sprite class that offers animation capabilities by allowing the underlying sprite's
texture to change over time.  A single animation is called an animationSequence, which
is a sequence of the locations in a texture that the sprite will cycle through each
animation frame.  An entity may have many animationSequences (e.g. run, jump, etc.)
and these are grouped together in an AnimationSequenceSet.  AnimatedSprite 
contains a static pointer to a vector of all AnimationSequenceSets, called sAnimations, 
allowing all AnimatedSprite objects to access any AnimationSequence.  sAnimations is 
implemented as a doubly-nested vector, so each level of access(Object, Animation, Frame) is 
accessed via an integer or enum.         
----------------------------------------------------------------------------------*/
class AnimatedSprite : public sf::Sprite
{
public:
    enum CallbackConditions {  //bitflags specifying when the callback is called
    Never = 0, 
    EachLoopEnd = 1, //at the end of each animation loop
    AnimationCompletion = 2, //at the end of all animation loops
    AnimationSwitch = 4 //if a new animation is set
    }

    using AnimationSequence = std::vector<IntRect>;
    using AnimationSequenceSet = std::vector<framePositions>;
    static vector<animationSequenceGroup> sAnimations;


public:
    AnimatedSprite(std::vector::size_type objectId);
    AnimatedSprite(std::vector::size_type objectId, const Texture& texture);
    AnimatedSprite(std::vector::size_type objectId, const Texture& texture, const sf::IntRect& textureSubRectangle);
        
    void setAnimation(std::vector::size_type animationId, sf::Time timePerFrame, unsigned numLoops, CallbackConditions callbackConditions = Never, std::function<void()> callback = [](){});
    void setContinuouslyLoopingAnimation(std::vector::size_type animationId, sf::Time timePerFrame, CallbackConditions callbackConditions = Never, std::function<void()> callback = [](){});
    //Update the accumulated frame time, and possibly update the sprite's texture rect.
    //Return true if the animation is finished; otherwise false.
    bool update(sf::Time deltaTime);
    //Manually call the callback.
    void executeCallback();
	
	
private:
    std::vector::size_type mObjectId;  //used to access the proper element in the sAnimations vector
    AnimationSequence* mCurrentAnimationSequence;
    AnimationSequence::iterator mCurrentFrame;
    sf::Time mTimePerFrame;
    sf::Time mAccumulatedFrameTime;
    bool mContinuouslyLooping;
    unsigned mNumLoopsRemaining;
    std::function<void()> mCallback;
    CallbackConditions mCallbackConditions;
};


constexpr CallbackConditions operator|(CallbackConditions firstOperand, CallbackConditions secondOperand);
constexpr CallbackConditions operator&(CallbackConditions firstOperand, CallbackConditions secondOperand);


#endif