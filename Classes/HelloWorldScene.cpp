#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene() {
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    //////////////////////////////
    // 1. super init first
    if (!Scene::init()) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    //创建一张贴图
    auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
    //从贴图中以像素单位切割，创建关键帧
    auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
    //使用第一帧创建精灵
    player = Sprite::createWithSpriteFrame(frame0);
    player->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2));
    addChild(player, 3);

    auto W_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("W", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::moveUp, this));
    auto A_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("A", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::moveLeft, this));
    auto S_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("S", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::moveDown, this));
    auto D_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("D", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::moveRight, this));
    auto X_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("X", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::reduceHP, this));
    auto Y_MenuItem = MenuItemLabel::create(
        Label::createWithTTF("Y", "fonts/arial.ttf", 36),
        CC_CALLBACK_1(HelloWorld::addHP, this));
    W_MenuItem->setPosition(Vec2(origin.x + 70, origin.y + 90));
    A_MenuItem->setPosition(Vec2(origin.x + 30, origin.y + 50));
    S_MenuItem->setPosition(Vec2(origin.x + 70, origin.y + 50));
    D_MenuItem->setPosition(Vec2(origin.x + 110, origin.y + 50));
    X_MenuItem->setPosition(Vec2(visibleSize.width - 30, origin.y + 90));
    Y_MenuItem->setPosition(Vec2(visibleSize.width - 70, origin.y + 50));
    auto menu = Menu::create(W_MenuItem, A_MenuItem, S_MenuItem, D_MenuItem, X_MenuItem, Y_MenuItem, NULL);
    menu->setPosition(origin);
    addChild(menu, 1);

    //hp条
    Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
    Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

    //使用hp条设置progressBar
    pT = ProgressTimer::create(sp);
    pT->setScaleX(90);
    pT->setAnchorPoint(Vec2(0, 0));
    pT->setType(ProgressTimerType::BAR);
    pT->setBarChangeRate(Point(1, 0));
    pT->setMidpoint(Point(0, 1));
    pT->setPercentage(100);
    pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
    addChild(pT, 1);
    sp0->setAnchorPoint(Vec2(0, 0));
    sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
    addChild(sp0, 0);

    // 静态动画
    idle.reserve(1);
    idle.pushBack(frame0);

    // 攻击动画
    attack.reserve(18);
    for (int i = 0; i < 17; i++) {
        auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
        attack.pushBack(frame);
    }
    attack.pushBack(frame0);

    // 可以仿照攻击动画
    // 死亡动画(帧数：22帧，高：90，宽：79）
    auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
    dead.reserve(23);
    for (int i = 0; i < 22; i++) {
        auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
        dead.pushBack(frame);
    }
    dead.pushBack(frame0);

    // 运动动画(帧数：8帧，高：101，宽：68）
    auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
    run.reserve(3);
    for (int i = 0; i < 3; i++) {
        auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
        run.pushBack(frame);
    }

    dtime = 180;
    char timeStr[10];
    itoa(dtime, timeStr, 10);
    time = Label::createWithTTF(timeStr, "fonts/arial.ttf", 36);
    time->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 50));
    addChild(time, 2);
    schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);

    auto myKeyListener = EventListenerKeyboard::create(); //创建事件监听器监听键盘事件
                                                          //键盘按键按下时的响应
    myKeyListener->onKeyPressed = [this](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
    {
        int keyNum = (int)keycode;
        switch (keyNum) {
            case 146: 
            case 28: this->moveUp(nullptr); break;
            case 124: 
            case 26: this->moveLeft(nullptr); break;
            case(142): 
            case 29: this->moveDown(nullptr); break;
            case 127: 
            case 27: this->moveRight(nullptr); break;
            case(147): this->addHP(nullptr); break;
            case(148): this->reduceHP(nullptr); break;
            default: break;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(myKeyListener, this);

    ifAction = false;
    return true;
}

void HelloWorld::moveUp(Ref* pSender) {
    Vec2 pos = player->getPosition();
    if (pos.y < visibleSize.height - 50)
        pos.y += 30;
    
    auto runAnimation = Animation::createWithSpriteFrames(run, 0.07f);
    auto runAnimate = Animate::create(runAnimation);
    auto move_to = MoveTo::create(0.21f, pos);
    player->runAction(runAnimate);
    player->runAction(move_to);
}

void HelloWorld::moveDown(Ref* pSender) {
    Vec2 pos = player->getPosition();
    if (pos.y > origin.y + 50)
        pos.y -= 30;

    auto runAnimation = Animation::createWithSpriteFrames(run, 0.07f);
    auto runAnimate = Animate::create(runAnimation);
    auto move_to = MoveTo::create(0.21f, pos);
    player->runAction(move_to);
    player->runAction(runAnimate);
}

void HelloWorld::moveLeft(Ref* pSender) {
    Vec2 pos = player->getPosition();
    if (pos.x > origin.x + 50)
        pos.x -= 30;

    auto runAnimation = Animation::createWithSpriteFrames(run, 0.07f);
    auto runAnimate = Animate::create(runAnimation);
    auto move_to = MoveTo::create(0.21f, pos);
    player->runAction(move_to);
    player->runAction(runAnimate);
}

void HelloWorld::moveRight(Ref* pSender) {
    Vec2 pos = player->getPosition();
    if (pos.x < visibleSize.width - 50)
        pos.x += 30;

    auto runAnimation = Animation::createWithSpriteFrames(run, 0.07f);
    auto runAnimate = Animate::create(runAnimation);
    auto move_to = MoveTo::create(0.21f, pos);
    player->runAction(move_to);
    player->runAction(runAnimate);
}

void HelloWorld::reduceHP(Ref* pSender) {
    if (ifAction)
        return;
    ifAction = true;
    if (pT->getPercentage() > 20)
        pT->setPercentage(pT->getPercentage() - 20);
    else
        pT->setPercentage(0);

    auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
    auto deadAnimate = Animate::create(deadAnimation);
    auto signal = CallFunc::create([this]() {
        this->ifAction = false;
    });
    auto seq = Sequence::create(deadAnimate, signal, nullptr);
    player->runAction(seq);
}

void HelloWorld::addHP(Ref* pSender) {
    if (ifAction)
        return;
    ifAction = true;
    if (pT->getPercentage() < 80)
        pT->setPercentage(pT->getPercentage() + 20);
    else
        pT->setPercentage(100);

    auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
    auto attackAnimate = Animate::create(attackAnimation);
    auto signal = CallFunc::create([this]() {
        this->ifAction = false;
    });
    auto seq = Sequence::create(attackAnimate, signal, nullptr);
    player->runAction(seq);
}

void HelloWorld::updateTime(float dt) {
    if (dtime == 0)
        return;
    dtime--;
    char timeStr[10];
    itoa(dtime, timeStr, 10);
    time->setString(timeStr);
}