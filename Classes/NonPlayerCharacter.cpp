#include "NonPlayerCharacter.h"
#include <string>

USING_NS_CC;

NonPlayerCharacter::NonPlayerCharacter()
{
	mColliderSize = { 32, 48 };
	mColliderOffset = { 8, 0 };
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

	SetddPos(ddPos);
}

void NonPlayerCharacter::OnSetParent(cocos2d::Node* parent)
{
}

void NonPlayerCharacter::OnLoad()
{

}

void NonPlayerCharacter::OnUnload() 
{
}
