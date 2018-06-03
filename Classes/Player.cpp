#include "Player.h"
#include <string>

USING_NS_CC;

Player::Player(std::function<bool(int)> buttonDownFunction)
	: mCheckButtonDownFunction(buttonDownFunction)
{
	mColliderSize = { 32, 48 };
	mColliderOffset = { 8, 0 };
	mDefaultSpeed = 4000.f;
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
	mSprite->setOpacity(0.5f);
	
	auto hitForce = GetPosition()-hitDirection;
	hitForce*=-1.f;
	hitForce.normalize();
	hitForce.y = fabsf(hitForce.y);
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
	
	if(mInvulnTimer > 0.f) 
	{
		mInvulnTimer -= deltaTime;
		if(mInvulnTimer <= 0.f)
		{
			mSprite->setOpacity(1.0f);
			// Maybe have no input during this??
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

	if (ddPos.x != 0.f) 
	{
		mSprite->setFlippedX(ddPos.x < 0);
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
}

void Player::OnUnload() 
{
}
