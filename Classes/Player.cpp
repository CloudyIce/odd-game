#include "Player.h"
#include <string>
#include "Level.h"

USING_NS_CC;

namespace {

	constexpr float sBonusUpHitForce = 0.5f;
	constexpr float sCameraYOffset = 120.f;
}

Player::Player(std::function<bool(int)> buttonDownFunction)
	: mCheckButtonDownFunction(buttonDownFunction)
	, mJumpAnimation	(nullptr)
	, mIdleAnimation	(nullptr)
	, mRunAnimation		(nullptr)
	, mCurrentAnimation	(nullptr)
{
	mColliderSize = { 32.f, 64 };
	mColliderOffset = { 32, 0 };
	mDefaultSpeed = 7000.f;
}

Player::~Player()
{

}

void Player::Knockback(const cocos2d::Vec2& hitDirection, const float force)
{
	if(mInvulnTimer > 0.f) 
	{
		return;
	}
	
	mInvulnTimer = mMaxInvulnTime;
	// StartBlinking or change sprite
	mSprite->setOpacity(50);
	
	auto hitForce = GetPosition()-hitDirection;
	hitForce.normalize();
	hitForce.y = fabsf(hitForce.y) + sBonusUpHitForce;
	hitForce*=force;
	
	AddVelocity(hitForce);
}

bool Player::CanJump(const bool newJump) const 
{
	//// if we're in the air above water return false
	if (!IsOnGround() && newJump) 
	{
		return false;
	}
	const bool heldButtonForTooLong = mJumpTimer <= 0.f;
	const bool stillWaitingForJumpDelay = (mJumpDelayTimer > 0.f);
	if ((stillWaitingForJumpDelay && newJump) || (heldButtonForTooLong && !newJump)) 
	{
		return false;
	}
	return true;
}

void Player::OnTick(const float deltaTime) 
{
	const auto newCameraPosition = Vec2(GetPosition().x, GetPosition().y + sCameraYOffset);
	mCamera->setPosition(newCameraPosition);
	mLevel->OnPlayerCameraUpdated(newCameraPosition);
	
	// Jump Timers
	if (IsOnGround()) {
		mJumpDelayTimer = -1.f;
		mJumpTimer = mJumpLength;
		if (mHasTouchedGroundSinceLastJumpEnded == false) {
			mHasTouchedGroundSinceLastJumpEnded = true;
		}
	}
	else {
		// Air Logic
		mJumpDelayTimer -= deltaTime;
		if (mHoldingJump) {
			mJumpTimer -= deltaTime;
		}
	}
	
	mInvulnTimer -= deltaTime;
	if (mInvulnTimer <= 0.f)
	{
		mSprite->setOpacity(256);
	}

	//
	Vec2 ddPos(0.0f, 0.0f);
	if (mCheckButtonDownFunction(0)) {
		ddPos.x = -1.0f;
	}
	else if (mCheckButtonDownFunction(1)) {
		ddPos.x = 1.0f;
	}

	// Jump Logic
	const bool wasHoldingJump = mHoldingJump;
	mHoldingJump = mCheckButtonDownFunction(2) && CanJump(!wasHoldingJump);
	if (mHoldingJump) {
		// This means it's a new jump!
		if (!wasHoldingJump) {
			// OnNewJump()
			mJumpTimer = mJumpLength;
		}

		// We want to slow it when we get to the peak, so we use mJumpTimer/Length to give us a slope
		AddVelocity(Vec2(0, mJumpVelocity*(mJumpTimer / mJumpLength)));
		mJumpDelayTimer = mJumpFrequency;

	}
	else if (wasHoldingJump) {
		mHasTouchedGroundSinceLastJumpEnded = false;
		mJumpTimer = -1.f;
	}

	if (ddPos.x != 0.f) 
	{
		mSprite->setFlippedX(ddPos.x < 0);
		if (mCurrentAnimation != nullptr) {
			if (mCurrentAnimation->getAnimation() != mRunAnimation && IsOnGround()) {
				mSprite->stopAction(mCurrentAnimation);
				mCurrentAnimation = Animate::create(mRunAnimation);
				mSprite->runAction(mCurrentAnimation);
			}
		}
		else {
			mCurrentAnimation = Animate::create(mRunAnimation);
			mSprite->runAction(mCurrentAnimation);
		}
	}
	else if (!IsOnGround()) {
		if (mCurrentAnimation != nullptr) {
			if (mCurrentAnimation->getAnimation() != mJumpAnimation) {
				mSprite->stopAction(mCurrentAnimation);
				mCurrentAnimation = Animate::create(mJumpAnimation);
				mSprite->runAction(mCurrentAnimation);
			}
		}
		else {
			mCurrentAnimation = Animate::create(mJumpAnimation);
			mSprite->runAction(mCurrentAnimation);
		}
	}
	else
	{
		if (mCurrentAnimation != nullptr) {
			if (mCurrentAnimation->getAnimation() != mIdleAnimation) {
				mSprite->stopAction(mCurrentAnimation);
				mCurrentAnimation = Animate::create(mIdleAnimation);
 				mSprite->runAction(mCurrentAnimation);
			}
		}
		else {
			mCurrentAnimation = Animate::create(mIdleAnimation);
			mSprite->runAction(mCurrentAnimation);
		}
	}

	

	SetddPos(ddPos);
}

void Player::OnSetParent(cocos2d::Node* parent)
{
	CCASSERT(parent != nullptr, "Player::OnSetParent - Passed in a null parent");
	mCamera = static_cast<Scene*>(parent->getParent())->getDefaultCamera();
	CCASSERT(mCamera != nullptr, "Player::OnSetParent - No camera created!");
}

void Player::OnLoad()
{
	mSprite->setScale(0.5f);

	// Build Animations
	mRunAnimation = cocos2d::Animation::create();
	mIdleAnimation = cocos2d::Animation::create();
	mJumpAnimation = cocos2d::Animation::create();
	mRunAnimation->init();
	mIdleAnimation->init();
	mJumpAnimation->init();
	mRunAnimation ->retain();
	mIdleAnimation->retain();
	mJumpAnimation->retain();

	const auto rect = Rect(0, 0, 184, 201);
	mIdleAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexidle.png", rect));

	mRunAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexwalk1.png", rect));
	mRunAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexwalk2.png", rect));
	mRunAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexwalk3.png", rect));
	mRunAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexwalk2.png", rect));

	mJumpAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("alexwalk1.png", rect));

	mRunAnimation->setDelayPerUnit(0.1f);
	mIdleAnimation->setDelayPerUnit(0.25f);
	mJumpAnimation->setDelayPerUnit(0.25f);
	mRunAnimation->setLoops(-1);
	mIdleAnimation->setLoops(-1);
	mJumpAnimation->setLoops(-1);
}

void Player::OnUnload() 
{
	mSprite->stopAction(mCurrentAnimation);
	mRunAnimation->release();
	mIdleAnimation->release();
	mJumpAnimation->release();
}
