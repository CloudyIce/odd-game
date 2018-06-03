#include "2d/CCDrawNode.h"
#include "GL/glew.h"

#include "OddGameScene.h"
#include "Entity.h"
#include "Level.h"
#include "NonPlayerCharacter.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

namespace 
{
	constexpr float sEndY = 1920.f;
}


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
	if (!Scene::init())
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
	mPlayer->Load(mLevel, "CloseNormal.png");
	mPlayer->SetParent(mGameLayer);
	mPlayer->SetPosition(Vec2(300, 100));

	const GLchar* fragmentSource = "uniform float percent;\
		void main() \
	{ \
		float dotSize = 32.0;\
		vec2 pos = vec2(mod(gl_FragCoord.x, dotSize), mod(gl_FragCoord.y, dotSize));\
		float r = length(pos - vec2(dotSize / 2.0, dotSize / 2.0));\
		if (r < dotSize * percent) { \
			gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); \
		} else {\
			gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\
		} \
	}";
	GLProgram* p = GLProgram::createWithByteArrays(ccPositionTextureA8Color_vert, fragmentSource);
	p->link();
	CHECK_GL_ERROR_DEBUG();
	p->updateUniforms();
	CHECK_GL_ERROR_DEBUG();

	const auto screenSize = Director::getInstance()->getVisibleSize();
	mFadeInOutOverlay = cocos2d::DrawNode::create();
	Vec2 rectanglePoints[4];
	rectanglePoints[0] = Vec2(screenSize.width / 2, screenSize.height / 2) * -1;
	rectanglePoints[1] = Vec2(screenSize.width / 2, screenSize.height / 2 * -1);
	rectanglePoints[2] = Vec2(screenSize.width / 2, screenSize.height / 2);
	rectanglePoints[3] = Vec2(screenSize.width / 2 * -1, screenSize.height / 2);
	Color4F black(1, 0, 0, 1);
	mFadeInOutOverlay->drawPolygon(rectanglePoints, 4, black, 1, black);
	mFadeInOutOverlay->setGLProgram(p);
	mFadeInOutOverlay->getGLProgramState()->setUniformFloat("percent", 0);
	addChild(mFadeInOutOverlay);

	return true;
}

void OddGameScene::update(float delta) 
{
	mPlayer->Update(delta);

	static float spawnTimer = 0.f;
	spawnTimer -= delta;
	if (spawnTimer < 0.f) 
	{
		spawnTimer = cocos2d::RandomHelper::random_int(1, 4)*0.25f;
		mNPCs.emplace_back(std::unique_ptr<NonPlayerCharacter>(new NonPlayerCharacter()));
		auto& newNPC = mNPCs.back();
		newNPC->Load(mLevel, "CloseSelected.png");

		if(cocos2d::RandomHelper::random_int(-2,2) > 1) 
		{
			newNPC->SetPosition(Vec2((mLevel->GetMapSize().x*32)-10, (mLevel->GetMapSize().y * 32)+32));
			newNPC->SetDirection(-1);
		}
		else
		{
			newNPC->SetPosition(Vec2(10, (mLevel->GetMapSize().y * 32) + 32));
			newNPC->SetDirection(1);
		}
		
		newNPC->SetParent(mGameLayer);
	}

	for (auto npc = mNPCs.begin(); npc != mNPCs.end(); ++npc) 
	{

		(*npc)->Update(delta);
		
		if((*npc)->GetCollider().intersectsRect(mPlayer->GetCollider()))
		{
			mPlayer->Knockback((*npc)->GetPosition(), (*npc)->GetSpeed()*0.5f);
		}

		if ((*npc)->GetPosition().x < 0.f || (*npc)->GetPosition().x > (mLevel->GetMapSize().x*32) ) 
		{
			(*npc)->Unload();
			npc = mNPCs.erase(npc);

			if (npc == mNPCs.end()) 
			{
				break;
			}
		}
	}

	if (mPlayer->GetPosition().x >= sEndY && mWinDelay == nullptr)
	{
		mWinDelay = DelayTime::create(1.0f);
		mSecondsSinceWin = 0.f;
		auto* onComplete = CallFunc::create([&]() {
			mWinDelay = nullptr;

			// Win!!
			mPlayer->SetPosition(Vec2(300, 100));
			for (auto npc = mNPCs.begin(); npc != mNPCs.end(); ++npc)
			{
				(*npc)->Unload();
				npc = mNPCs.erase(npc);

				if (npc == mNPCs.end())
				{
					break;
				}
			}
		});
		
		auto* sequence = Sequence::create(mWinDelay, onComplete, nullptr);
		mFadeInOutOverlay->runAction(sequence);
	}

	const auto cameraPosition = getDefaultCamera()->getPosition();
	mFadeInOutOverlay->setPosition(cameraPosition.x, cameraPosition.y);
	if (mWinDelay != nullptr) {
		mSecondsSinceWin += delta;
		mFadeInOutOverlay->getGLProgramState()->setUniformFloat("percent", mSecondsSinceWin);
	} else {
		mSecondsSinceWin = std::max(mSecondsSinceWin - delta, 0.f);
		mFadeInOutOverlay->getGLProgramState()->setUniformFloat("percent", mSecondsSinceWin);
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