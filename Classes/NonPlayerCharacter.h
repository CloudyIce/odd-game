#pragma once

#include "cocos2d.h"
#include "Entity.h"

class NonPlayerCharacter : public Entity
{
public:
	NonPlayerCharacter();
	virtual ~NonPlayerCharacter();

	void SetDirection(const int dir);
private:
	virtual void OnTick(const float deltaTime) override;
	virtual void OnSetParent(cocos2d::Node* parent) override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
private:
	//cocos2d::Animation* mAnimation;
	//cocos2d::Animate* mCurrentAnimation;
	float mIntentDirection = 1.f;

	const float mJumpLengthMin = 0.4f;
	const float mJumpLengthMax = 0.8f;
	float mJumpLength = mJumpLengthMin;
	const float mJumpVelocity = 500.f;
	float mJumpTimer = -1.f;

	int mChanceOfJumping = 200;
};