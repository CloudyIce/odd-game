#pragma once

#include "cocos2d.h"
#include "Entity.h"

class Player : public Entity
{
public:
	Player(std::function<bool(int)> buttonDownFunction);
	virtual ~Player();
	
	void Knockback(const cocos2d::Vec2& hitDirection, const float force);

private:
	bool CanJump(const bool newJump) const;

private:
	virtual void OnTick(const float deltaTime) override;
	virtual void OnSetParent(cocos2d::Node* parent) override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
private:
	//cocos2d::Animation* mAnimation;
	//cocos2d::Animate* mCurrentAnimation;
	const float mMaxInvulnTime = 0.75f;
	float mInvulnTimer = -1.f;

	const float mJumpFrequency = 0.25f;
	const float mJumpLength = 0.7f;
	const float mJumpVelocity = 500.f;
	float mJumpTimer = -1.f;
	float mJumpDelayTimer = -1.f;
	bool mHoldingJump = false;

	bool mHasTouchedGroundSinceLastJumpEnded = false;

	cocos2d::Camera* mCamera = nullptr;

	std::function<bool(int)> mCheckButtonDownFunction = nullptr;
};