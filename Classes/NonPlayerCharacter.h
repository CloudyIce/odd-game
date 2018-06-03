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

	const float mJumpFrequency = 0.25f;
	const float mJumpLength = 0.5f;
	const float mJumpVelocity = 200.f;
};