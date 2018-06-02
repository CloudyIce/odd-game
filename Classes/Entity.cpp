#include "Entity.h"
#include <vector>

USING_NS_CC;

namespace 
{
	const int sTravelDistance = 3;
	const float sEpsilon = 0.000001f;
	const float sGravity = -50.f;
	const float sEntityLayer = 10.f;
}

Entity::Entity()
{
	mSpeed = mDefaultSpeed;
	mCollider.setRect(mPos.x + mColliderOffset.x, mPos.y + mColliderOffset.y, mColliderSize.x, mColliderSize.y);
	mAlive = true;
}

Entity::~Entity()
{
}

void Entity::Load(const char *spriteSheetName, const cocos2d::Rect& defaultSprite)
{
	mSprite = defaultSprite.equals(Rect()) ? Sprite::create(spriteSheetName, mDefaultSprite) : Sprite::create(spriteSheetName, defaultSprite);
	mSprite->setAnchorPoint(Vec2(0, 0));
	OnLoad();
}

void Entity::Unload()
{
	OnUnload();
	mSprite->release();
}

void Entity::Update(const float deltaTime)
{
	if (!mAlive) 
	{
		return;
	}

	OnTick(deltaTime);

	auto movementDelta = Move(deltaTime);
	AddVelocity(Vec2(0, sGravity));

	CheckTileMapCollision(movementDelta);
	mIsGrounded = fabs(movementDelta.y) <= sEpsilon;
	mPos += movementDelta;
	
	mSprite->setPosition(mPos - mColliderOffset);
	mCollider.setRect(mPos.x, mPos.y, mColliderSize.x, mColliderSize.y);
}

void Entity::SetParent(cocos2d::Node* parent)
{
	parent->addChild(mSprite, sEntityLayer);
	OnSetParent(parent);
}

Vec2 Entity::Move(const float deltaTime)
{
	const float ddPosLength = mDDPos.lengthSquared();
	if (ddPosLength > 1.0f) 
	{
		mDDPos *= (1.0f / mDDPos.length());
	}

	mDDPos *= mSpeed;
	mDDPos += mDPos*(-mFriction);
	const Vec2 delta = (0.5f*mDDPos*(deltaTime*deltaTime)) + (mDPos*deltaTime);
	mDPos = (mDDPos*deltaTime) + (mDPos);
	
	return delta;
}

void Entity::CheckTileMapCollision(Vec2& changeInMovement)
{
	// TODO
	const int tileSize = 32;//static_cast<int>(tilemap->GetTileSize().x);
	if(changeInMovement.x > 0)
	{
		CheckCollisionOnAxis(changeInMovement.x, Vec2(mCollider.getMaxX(), mPos.y), Vec2(mPos.x + sTravelDistance * tileSize, mCollider.getMaxY()), true);
	}
	else if (changeInMovement.x < 0) {
		CheckCollisionOnAxis(changeInMovement.x, Vec2(mCollider.getMinX(), mPos.y), Vec2(mPos.x - sTravelDistance * tileSize, mCollider.getMaxY()), true);
	}
	
	if (changeInMovement.y > 0) {
		CheckCollisionOnAxis(changeInMovement.y, Vec2(mCollider.getMaxY(), mPos.x), Vec2(mPos.y + sTravelDistance * tileSize, mCollider.getMaxX()), false);
	}
	else if (changeInMovement.y < 0) {
		CheckCollisionOnAxis(changeInMovement.y, Vec2(mCollider.getMinY(), mPos.x), Vec2(mPos.y - sTravelDistance * tileSize, mCollider.getMaxX()), false);
	}
}

void Entity::CheckCollisionOnAxis(float& changeInMovement, const cocos2d::Vec2& startPoint, const cocos2d::Vec2& endPoint, const bool xAxis)
{
	const int direction = changeInMovement > 0 ? 1 : -1;

	// If we're doing the X axis, we want to get every Tile x Row that the Entity's collider hits.
	// So if the Entity is 2 tiles tall (64 units) then we should fill up the above vector with 2-3 rows to look at.

	float closestCollision = 0.f;
	uint8_t collisions = GetClosestCollision(closestCollision, startPoint, endPoint, direction, changeInMovement, xAxis);

	// no collisions
	if (collisions == 0) 
	{
		return;
	}

	// our change in movement on the collided axis now needs to be reduced so it is lower then collidedAt
	const auto distanceToCollision = fabs(startPoint.x - closestCollision);

	if (fabs(changeInMovement) < distanceToCollision) 
	{
		return;
	}
	changeInMovement = (distanceToCollision - sEpsilon) * direction;
}


uint8_t Entity::GetClosestCollision(float& closestCollision, const cocos2d::Vec2& startPoint, const cocos2d::Vec2& endPoint, const int direction, const float changeInMovement, const bool xAxis) const 
{
	// TODO
	/*const int fromX = static_cast<int>(level->XPositionToTileCoordinate(startPoint.x));
	const int toX = static_cast<int>(level->XPositionToTileCoordinate(endPoint.x));
	const int fromY = static_cast<int>(level->XPositionToTileCoordinate(startPoint.y));
	const int toY = static_cast<int>(level->XPositionToTileCoordinate(endPoint.y));
	const int tileSize = static_cast<int>(level->GetTileSize().x);

	const float dPos = startPoint.x + changeInMovement;

	uint8_t collisions = 0;
	for (int row = fromY; row <= toY; ++row) {
		const auto tiles = xAxis ? level->GetTilesInRangeAlongRow(row, fromX, toX) : level->GetTilesInRangeAlongColumn(row, fromX, toX);
		// Find colliding tiles if any
		for (auto&& tile : tiles) {
			// Save the "closest" collided tile so we can use this for our collision
			const auto tileReferencePoint = (xAxis ? tile.getMidX() : tile.getMidY()) + static_cast<int>(tileSize*0.5f) * -direction;
			const bool collided = CheckIsCollisionClosest(collisions > 0, closestCollision, tileReferencePoint, dPos, direction > 0);
			collisions += static_cast<uint8_t>(collided);
		}
	}*/
	return 0; //collisions;
}

bool Entity::CheckIsCollisionClosest(bool collided, float& closestCollision, const float potentialCollision, const float potentialNewPosition, const bool movingForward) const
{
	//Check if we're even colliding with the point
	if (!(movingForward ? potentialNewPosition > potentialCollision : potentialNewPosition < potentialCollision)) 
	{
		return false;
	}
	// Now Check if our collision is farther from our other collision
	if (collided && (movingForward ? potentialCollision < closestCollision : potentialCollision > closestCollision)) 
	{
		return false;
	}

	closestCollision = potentialCollision;
	return true;
}
