#pragma once

#include <memory>
#include <vector>
#include "cocos2d.h"

class Level : public std::enable_shared_from_this<Level>
{
public:
	Level();
	virtual ~Level();

	void Load();
	void Unload();

	void SetParent(cocos2d::Node* parent);

	cocos2d::Point TileCoordinateToPosition(const cocos2d::Size& size, const cocos2d::Point& point) const;
	float TileCoordinateToPosition(const float position) const;
	cocos2d::Vec2 PositionToTileCoordinate(const cocos2d::Vec2& position) const;
	float PositionToTileCoordinate(const float position) const;
	float XPositionToTileCoordinate(const float x) const;
	float YPositionToTileCoordinate(const float y) const;

	const std::vector<cocos2d::Rect> GetTilesInRangeAlongRow(const int y, const int from, const int to) const;
	const std::vector<cocos2d::Rect> GetTilesInRangeAlongColumn(const int x, const int from, const int to) const;

	const cocos2d::Vec2 GetTileSize() const;

	void OnPlayerCameraUpdated(const cocos2d::Vec2& position);

	void Tick(const float deltaTime);
private:
	cocos2d::Rect&& GetTileCollider(const cocos2d::Vec2& pos, cocos2d::TMXLayer* targetLayer, const cocos2d::Vec2& offset) const;
private:
	cocos2d::TMXTiledMap* mMap;
	cocos2d::LayerGradient* mSky = nullptr;

	cocos2d::Size mTileSize;
};
