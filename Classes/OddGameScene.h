#pragma once

#include "cocos2d.h"
#include <memory>
#include "Player.h"

class OddGameScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	virtual void update(float delta) override;
	CREATE_FUNC(OddGameScene);

	bool IsButtonDown(const uint8_t button) const;
	void SetButtonDown(const uint8_t button, const bool isDown);
private:
	std::unique_ptr<Player> mPlayer;

	uint32_t mInputBuffer = 0;
	cocos2d::Layer* mGameLayer;
	cocos2d::Layer* mUILayer;
};
