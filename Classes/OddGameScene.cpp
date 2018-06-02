#include "OddGameScene.h"
#include "Entity.h"
#include "Level.h"
#include "NonPlayerCharacter.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;


Scene* OddGameScene::createScene()
{
    return OddGameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

// on "init" you need to initialize your instance
bool OddGameScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
		const auto controller = static_cast<OddGameScene*>(event->getCurrentTarget());
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			case EventKeyboard::KeyCode::KEY_A:
				controller->SetButtonDown(0, true);
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			case EventKeyboard::KeyCode::KEY_D:
				controller->SetButtonDown(1, true);
				break;
			case EventKeyboard::KeyCode::KEY_UP_ARROW:
			case EventKeyboard::KeyCode::KEY_W:
				controller->SetButtonDown(2, true);
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			case EventKeyboard::KeyCode::KEY_S:
				controller->SetButtonDown(3, true);
				break;
		}
	};
	eventListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
		const auto controller = static_cast<OddGameScene*>(event->getCurrentTarget());
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			case EventKeyboard::KeyCode::KEY_A:
				controller->SetButtonDown(0, false);
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			case EventKeyboard::KeyCode::KEY_D:
				controller->SetButtonDown(1, false);
				break;
			case EventKeyboard::KeyCode::KEY_UP_ARROW:
			case EventKeyboard::KeyCode::KEY_W:
				controller->SetButtonDown(2, false);
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			case EventKeyboard::KeyCode::KEY_S:
				controller->SetButtonDown(3, false);
				break;
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

	scheduleUpdate();

	mGameLayer = Layer::create();
	this->addChild(mGameLayer, 0);

	mUILayer = Layer::create();
	this->addChild(mUILayer, 2);


	mLevel = std::shared_ptr<Level>(new Level());
	mLevel->Load();
	mLevel->SetParent(mGameLayer);

	mPlayer = std::unique_ptr<Player>(new Player([this](int button) {return IsButtonDown(button); }));
	mPlayer->Load(mLevel,"CloseNormal.png");
	mPlayer->SetParent(mGameLayer);
	mPlayer->SetPosition(Vec2(100, 100));


	return true;
}

void OddGameScene::update(float delta) 
{
	mPlayer->Update(delta);

	static float spawnTimer = 0.f;
	spawnTimer -= delta;
	if (spawnTimer < 0.f) 
	{
		spawnTimer = cocos2d::RandomHelper::random_int(1, 2)*0.5f;
		mNPCs.emplace_back(std::unique_ptr<NonPlayerCharacter>(new NonPlayerCharacter()));
		auto& newNPC = mNPCs.back();
		newNPC->Load(mLevel, "CloseSelected.png");
		newNPC->SetParent(mGameLayer);
		newNPC->SetPosition(Vec2(640, 192));
	}

	for (auto npc = mNPCs.begin(); npc != mNPCs.end(); ++npc) {

		(*npc)->Update(delta);

		if ((*npc)->GetPosition().x < 0.f) {
			(*npc)->Unload();
			npc = mNPCs.erase(npc);

			if (npc == mNPCs.end()) {
				break;
			}
		}
	}

}

bool OddGameScene::IsButtonDown(const uint8_t button) const 
{
	return ((mInputBuffer >> button) & 1) == 1;
}

void OddGameScene::SetButtonDown(const uint8_t button, const bool isDown) 
{
	if (isDown) {
		mInputBuffer |= 1 << button;
	}
	else {
		mInputBuffer &= ~(1 << button);
	}
}