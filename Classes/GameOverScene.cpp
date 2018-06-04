#include "GameOverScene.h"

#include "OddGameScene.h"

#include "2d/CCDrawNode.h"
#include "2d/CCLabel.h"

cocos2d::Scene * GameOverScene::createScene()
{
	return GameOverScene::create();
}

bool GameOverScene::init()
{
	cocos2d::TTFConfig labelConfig;
	labelConfig.fontFilePath = "Helvetica-Regular.ttf";
	labelConfig.fontSize = 24;
	labelConfig.glyphs = cocos2d::GlyphCollection::DYNAMIC;
	labelConfig.outlineSize = 0;
	labelConfig.customGlyphs = nullptr;
	labelConfig.distanceFieldEnabled = false;
	
	const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto* gameOverText = cocos2d::Label::createWithTTF(labelConfig, "Too late! You got fired!");
	gameOverText->setPosition(cocos2d::Vec2(screenSize.width / 2, screenSize.height / 2));
	gameOverText->setColor(cocos2d::Color3B::WHITE);
	gameOverText->setScale(2.0f);
	addChild(gameOverText);

	// Go back to the title screen
	const auto timeToWait = 5;
	auto* delay = cocos2d::DelayTime::create(timeToWait);
	auto* backToTitle = cocos2d::CallFunc::create([]() {
		auto* director = cocos2d::Director::getInstance();
		director->replaceScene(OddGameScene::createScene());
	});
	auto* sequence = cocos2d::Sequence::create(delay, backToTitle, nullptr);
	this->runAction(sequence);

	return true;
}

void GameOverScene::update(float delta)
{
}
