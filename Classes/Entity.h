#pragma once

#include <memory>
#include "cocos2d.h"

class Level;

class Entity
{
public:
	Entity();
	virtual ~Entity();

	void Load(std::shared_ptr<Level> level, const char *spriteName);
	void Unload();
	void Update(float deltaTime);

	void SetParent(cocos2d::Node* parent);

	bool IsAlive() const { return mAlive; };

	void SetPosition(const cocos2d::Vec2& newPos) { mPos = newPos; };
	const cocos2d::Vec2& GetPosition() const { return mPos; }

	const float GetSpeed() const { return mSpeed; };
	void SetSpeed(const float NewSpeed) { mSpeed = NewSpeed;};

	void AddVelocity(const cocos2d::Vec2& vel){ mDPos += vel; };

	void SetddPos(const cocos2d::Vec2& newPos){ mDDPos = newPos; };
	void SetdPos(const cocos2d::Vec2& newPos){ mDPos = newPos; };
	const cocos2d::Vec2& GetddPos() const { return mDDPos; };
	const cocos2d::Vec2& GetdPos() const { return mDPos; };

	void SetCollider(const cocos2d::Rect& newCollider){ mCollider = newCollider; };
	const cocos2d::Rect& GetCollider() const { return mCollider; };

	bool IsOnGround() const { return mIsGrounded; }
protected:
	virtual void OnTick(const float deltaTime) = 0;
	virtual void OnSetParent(cocos2d::Node* parent) = 0;
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;
protected:
	cocos2d::Sprite* mSprite = nullptr;

	cocos2d::Vec2 mColliderSize = { 24, 24 };
	cocos2d::Vec2 mColliderOffset = { 8, 8 };

	float mDefaultSpeed = 3000.f;
	cocos2d::Rect mDefaultSprite;

	std::shared_ptr<Level> mLevel;
private:
	cocos2d::Vec2 Move(const float deltaTime);
	void CheckTileMapCollision(cocos2d::Vec2& changeInMovement);
	void CheckCollisionOnAxis(float& changeInMovement, const cocos2d::Vec2& startPoint, const cocos2d::Vec2& endPoint, const bool xAxis);
	uint8_t GetClosestCollision(float& closestCollision, const cocos2d::Vec2& startPoint, const cocos2d::Vec2& endPoint, const int direction, const float changeInMovement, const bool xAxis) const;
	bool CheckIsCollisionClosest(bool collided, float& closestCollision, const float potentialCollision, const float potentialNewPosition, const bool movingForward) const;

private:
	cocos2d::Vec2 mPos;
	cocos2d::Vec2 mDPos;
	cocos2d::Vec2 mDDPos;
	cocos2d::Rect mCollider;

	float mSpeed = mDefaultSpeed;
	float mFriction = 15.0f;
	bool mAlive = false;

	bool mIsGrounded = false;
};
