#include "Player.h"
#include <string>

USING_NS_CC;

Player::Player(std::function<bool(int)> buttonDownFunction)
	: mCheckButtonDownFunction(buttonDownFunction)
{
	mColliderSize = { 16, 48 };
	mColliderOffset = { 8, 0 };
	mDefaultSpeed = 4000.f;
}

Player::~Player()
{

}

bool Player::CanJump(const bool newJump) const {
	//// if we're in the air above water return false
	if (!IsOnGround()) 
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

	mCamera->setPosition(GetPosition());
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

	mSprite->setFlippedX(ddPos.x < 0);

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
}

void Player::OnUnload() 
{
}
