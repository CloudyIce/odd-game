#include "Level.h"
#include "Entity.h"

USING_NS_CC;

namespace 
{
	const char sCollisionLayerName[50] = "collision";
	const Size sSkyBuffer(30, 30);
	const float sTileMapZOrder = -2;
}

Level::Level()
{
}

Level::~Level()
{
	Unload();
}

void Level::Load()
{
	mMap = TMXTiledMap::create("testmap.tmx");
	mMap->retain();
	mMap->setPosition(0, 0);
	const auto startColor = std::strtol("66e8ef", nullptr, 16);
	const auto endColor = std::strtol("e4f5f7", nullptr, 16);
	const auto startColorColor = Color4B(((startColor >> 16) & 0xFF), ((startColor >> 8) & 0xFF), ((startColor) & 0xFF), 255);
	const auto endColorColor = Color4B(((endColor >> 16) & 0xFF), ((endColor >> 8) & 0xFF), ((endColor) & 0xFF), 255);
	mSky = LayerGradient::create(startColorColor, endColorColor);
	mSky->retain();
	mSky->setContentSize(Director::getInstance()->getVisibleSize() + sSkyBuffer);
	mTileSize = mMap->getTileSize();
}

void Level::Unload()
{
	mMap->release();
	mMap->removeFromParent();
	mMap = nullptr;
}

void Level::SetParent(cocos2d::Node* parent)
{
	parent->addChild(mSky, -100);
	parent->addChild(mMap, sTileMapZOrder);
}

cocos2d::Point Level::TileCoordinateToPosition(const cocos2d::Size& size, const cocos2d::Point& point) const
{
	const auto tileSize = mTileSize;

	const float x = floorf(size.width * 0.5f + point.x * tileSize.width);
	const float y = floorf(size.height * 0.5f + point.y * tileSize.height);

	return Point(x, y);
}

float Level::TileCoordinateToPosition(const float position) const
{
	const auto tileSize = mTileSize;

	const float x = floorf(position * tileSize.width);

	return x;
}

cocos2d::Vec2 Level::PositionToTileCoordinate(const cocos2d::Vec2& position) const
{
	const auto tileSize = mTileSize;
	const float x = floorf((position.x / tileSize.width) + 0.5f);
	const float y = floorf((position.y / tileSize.height) + 0.5f);
	return Vec2(x, y);
}

float Level::PositionToTileCoordinate(const float position) const
{
	const auto tileSize = mTileSize.width;
	return floorf((position / tileSize)+0.5f);
}

const std::vector<cocos2d::Rect> Level::GetTilesAlongRow(const int y, const int direction) const
{
	std::vector<Rect> tiles;
	const auto walls = mMap->getLayer(sCollisionLayerName);
	const auto size = mMap->getMapSize().width;

	const auto yOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().y));
	const int offsety = y - yOffset;

	const auto xOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().x));
	int from = direction > 0 ? 0 : size;
	int to = direction > 0 ? size : 0;
	from -= xOffset;
	to -= xOffset;

	for (int x = from; x != to; x += direction)
	{
		const auto tile = GetTileCollider(Vec2(x, offsety), walls, Vec2(xOffset, yOffset));
		if (tile.equals(Rect::ZERO))
		{
			continue;
		}
		tiles.push_back(std::move(tile));
	}

	return tiles;
}

const std::vector<cocos2d::Rect> Level::GetTilesAlongColumn(const int x, const int direction) const 
{
	std::vector<Rect> tiles;
	const auto walls = mMap->getLayer(sCollisionLayerName);
	const auto size = mMap->getMapSize().height;

	const auto xOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().x));
	const int offsetX = x - xOffset;

	const auto yOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().y));
	int from = direction > 0 ? 0 : size;
	int to = direction > 0 ? size : 0;
	from -= yOffset;
	to -= yOffset;

	// have to include last tile here
	for (int y = from; y != (to + direction); y += direction)
	{
		const auto tile = GetTileCollider(Vec2(offsetX, y), walls, Vec2(xOffset, yOffset));
		// Check if valid tile
		if (tile.equals(Rect::ZERO))
		{
			continue;
		}
		tiles.push_back(std::move(tile));
	}

	return tiles;
}

const cocos2d::Vec2 Level::GetTileSize() const
{
	return mTileSize;
}

void Level::OnPlayerCameraUpdated(const cocos2d::Vec2& position)
{
	mSky->setPosition(position - mSky->getContentSize()*0.5f);
}

void Level::Tick(const float deltaTime)
{
}

cocos2d::Rect&& Level::GetTileCollider(const Vec2& pos, TMXLayer* targetLayer, const Vec2& offset) const
{
	auto rect = Rect::ZERO;
	// Check if valid tile
	const auto mapsize = targetLayer->getLayerSize();
	// We have to inverse the y since the Tile Coordinates are x positive to the right and y positive down
	const auto tilePosition = Vec2(pos.x, (mapsize.height - 1) - pos.y);
	if (tilePosition.x >= mapsize.width || tilePosition.x < 0)
	{
		return std::move(rect);
	}

	if (tilePosition.y >= mapsize.height || tilePosition.y < 0)
	{
		return std::move(rect);
	}

	const auto tilepos = Vec2(tilePosition.x, tilePosition.y);
	const Sprite* tile = targetLayer->getTileAt(tilepos);

	// If it's an empty tile, don't add it
	if (tile == nullptr)
	{
		return std::move(rect);
	}

	const auto position = targetLayer->getPositionAt(tilepos);
	rect = Rect(position.x + (offset.x*mTileSize.width), position.y + (offset.y*mTileSize.height), mTileSize.width, mTileSize.height);
	return std::move(rect);
}


