#pragma once

#include "cocos2d.h"

class GameOverScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	virtual void update(float delta) override;
	CREATE_FUNC(GameOverScene);

};

