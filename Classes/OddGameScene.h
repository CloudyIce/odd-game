#pragma once

#include "cocos2d.h"
#include <memory>
#include "Player.h"
#include "NonPlayerCharacter.h"

class DrawNode;
class Level;

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
	std::shared_ptr<Level> mLevel;
	std::vector<std::unique_ptr<NonPlayerCharacter>> mNPCs;

	cocos2d::DrawNode* mFadeInOutOverlay;
	cocos2d::DelayTime* mWinDelay = nullptr;
	float mSecondsSinceWin = 0.f;

	int mScore = 0;
	cocos2d::Label* mScoreText;

	uint32_t mInputBuffer = 0;
	cocos2d::Layer* mGameLayer;
	cocos2d::Layer* mUILayer;

	void updateScore(int newScore);
};
