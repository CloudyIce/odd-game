#include "NonPlayerCharacter.h"
#include <string>

USING_NS_CC;

namespace 
{
	constexpr int sSpriteVariations = 1;
}

NonPlayerCharacter::NonPlayerCharacter()
{
	mColliderSize = { 32.f, 64 };
	mColliderOffset = { 32, 0 };
	mDefaultSpeed = 1000.f * cocos2d::RandomHelper::random_int(2, 5);
}

NonPlayerCharacter::~NonPlayerCharacter()
{

}

void NonPlayerCharacter::SetDirection(const int dir)
{
	mIntentDirection = dir;
}

void NonPlayerCharacter::OnTick(const float deltaTime) 
{
	Vec2 ddPos(0.0f, 0.0f);
	ddPos.x = mIntentDirection;

	if (ddPos.x != 0.f) 
	{
		mSprite->setFlippedX(ddPos.x < 0);
	}

	if (IsOnGround() && cocos2d::RandomHelper::random_int(1, mChanceOfJumping) == 1) {
		mJumpLength = cocos2d::RandomHelper::random_int(static_cast<int>(mJumpLengthMin * 10.f), static_cast<int>(mJumpLengthMax * 10.f))*0.1f;
		mJumpTimer = mJumpLength;
	}

	mJumpTimer -= deltaTime;
	if (mJumpTimer > 0.f) {
		// We want to slow it when we get to the peak, so we use mJumpTimer/Length to give us a slope
		AddVelocity(Vec2(0, mJumpVelocity*(mJumpTimer / mJumpLength)));
	}

	SetddPos(ddPos);
}

void NonPlayerCharacter::OnSetParent(cocos2d::Node* parent)
{
}

void NonPlayerCharacter::OnLoad()
{
	mSprite->setScale(0.5f);
	mSprite->setColor(Color3B(RandomHelper::random_int(155, 255), RandomHelper::random_int(155, 255), RandomHelper::random_int(155, 255)));
	mSprite->setOpacity(200);

	mAnimation = cocos2d::Animation::create();
	mAnimation->init();
	mAnimation->retain();

	const auto rect = Rect(0, 0, 184, 201);
	const auto variation = std::to_string(RandomHelper::random_int(1, sSpriteVariations));
	mAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("npc" + variation + "walk1.png", rect));
	mAnimation->addSpriteFrame(cocos2d::SpriteFrame::create("npc"+ variation+ "walk2.png", rect));

	mAnimation->setDelayPerUnit(0.1f/(mDefaultSpeed*0.0002f));
	mAnimation->setLoops(-1);

	mCurrentAnimation = Animate::create(mAnimation);
	mSprite->runAction(mCurrentAnimation);
}

void NonPlayerCharacter::OnUnload() 
{
	mCurrentAnimation->stop();
	mAnimation->release();
}
