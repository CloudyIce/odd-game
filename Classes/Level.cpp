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

float Level::XPositionToTileCoordinate(const float x) const
{
	const auto tileSize = mTileSize.width;
	return floorf(x / tileSize);
}

float Level::YPositionToTileCoordinate(const float y) const
{
	const auto tileSize = mTileSize.height;
	const float levelHeightInPixels = mMap->getMapSize().height * tileSize;
	return floorf((levelHeightInPixels - y) / tileSize);
}

const std::vector<cocos2d::Rect> Level::GetTilesInRangeAlongRow(const int y, const int from, const int to) const 
{
	std::vector<Rect> tiles;
	const auto walls = mMap->getLayer(sCollisionLayerName);
	CCASSERT(walls != nullptr, "Level::GetTilesInRange - Couldn't get collision layer!");
	const auto mapsize = walls->getLayerSize();

	//should be zero for our purposes, but implementing anyways
	const auto yOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().y));
	const int offsety = y - yOffset;

	if (y >= mapsize.height || y < 0)
	{
		tiles;
	}

	const int direction = from > to ? -1 : 1;

	const auto xOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().x));
	const int offsetFrom = direction == 1 ? 0 : mMap->getMapSize().width;
	const int offsetTo = direction == 1 ? mMap->getMapSize().width : 0;

	for (int x = offsetFrom; x != offsetTo; x += direction)
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

const std::vector<cocos2d::Rect> Level::GetTilesInRangeAlongColumn(const int x, const int from, const int to) const 
{
	std::vector<Rect> tiles;
	const auto walls = mMap->getLayer(sCollisionLayerName);
	CCASSERT(walls != nullptr, "Level::GetTilesInRange - Couldn't get collision layer!");
	const auto mapsize = walls->getLayerSize();

	const auto xOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().x));
	const int offsetX = x - xOffset;

	if (offsetX >= mapsize.width || offsetX < 0)
	{
		return tiles;
	}

	const int direction = from > to ? -1 : 1;

	const auto yOffset = static_cast<int>(PositionToTileCoordinate(mMap->getPosition().y));
	const int offsetFrom = direction == 1 ? 0 : mMap->getMapSize().height;
	const int offsetTo = direction == 1 ? mMap->getMapSize().height: 0 ;
		
	// have to include last tile here
	for (int y = offsetFrom; y != offsetTo + direction; y += direction)
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


