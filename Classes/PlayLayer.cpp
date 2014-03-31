#include "PlayLayer.h"
#include "SushiSprite.h"

#define MATRIX_WIDTH (7)
#define MATRIX_HEIGHT (9)

#define SUSHI_GAP (1)

PlayLayer::PlayLayer()
: spriteSheet(NULL)
, m_matrix(NULL)
, m_width(0)
, m_height(0)
, m_matrixLeftBottomX(0)
, m_matrixLeftBottomY(0)
, m_isAnimationing(true)//start with drop animation
, m_isTouchEnable(true)
, m_touchBeganSushi(NULL)
{
}

PlayLayer::~PlayLayer()
{
    if (m_matrix) {
        free(m_matrix);
    }
}

Scene *PlayLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    return scene;
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // background
    Size winSize = Director::getInstance()->getWinSize();
    auto background = Sprite::create("background.png");
    background->setAnchorPoint(Point(0, 1));
    background->setPosition(Point(0, winSize.height));
    this->addChild(background);
    
    // init spriteSheet
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
    spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
    addChild(spriteSheet);
    
    // Yes, you can load this value from other module.
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;
    
    // init position value
    m_matrixLeftBottomX = (winSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
    m_matrixLeftBottomY = (winSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;
    
    // init point array
    int arraySize = sizeof(SushiSprite *) * m_width * m_height;
    m_matrix = (SushiSprite **)malloc(arraySize);
    memset((void*)m_matrix, 0, arraySize);
    
    initMatrix();
    scheduleUpdate();
    
    // bind touch event
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    return true;
}

SushiSprite *PlayLayer::sushiOfPoint(Point *point)
{
    SushiSprite *sushi = NULL;
    Rect rect = Rect(0, 0, 0, 0);
    
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (sushi) {
            rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);
            rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);
            rect.size = sushi->getContentSize();
            if (rect.containsPoint(*point)) {
                return sushi;
            }
        }
    }
    
    return NULL;
}

bool PlayLayer::onTouchBegan(Touch *touch, Event *unused)
{
    if (m_isTouchEnable) {
        auto location = touch->getLocation();
        m_touchBeganSushi = sushiOfPoint(&location);
    }
    return m_isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch *touch, Event *unused)
{
    if (!m_touchBeganSushi || !m_isTouchEnable) {
        return;
    }
    
    SushiSprite *destSushi = NULL;
    int row = m_touchBeganSushi->getRow();
    int col = m_touchBeganSushi->getCol();
    
    auto location = touch->getLocation();
    auto halfSushiWidth = m_touchBeganSushi->getContentSize().width / 2;
    auto halfSushiHeight = m_touchBeganSushi->getContentSize().height / 2;
    
    auto  upRect = Rect(m_touchBeganSushi->getPositionX() - halfSushiWidth,
                        m_touchBeganSushi->getPositionY() + halfSushiHeight,
                        m_touchBeganSushi->getContentSize().width,
                        m_touchBeganSushi->getContentSize().height);
    
    if (upRect.containsPoint(location)) {
        row++;
        if (row < m_height) {
            destSushi = m_matrix[row * m_width + col];
        }
        swapSushi(m_touchBeganSushi, destSushi);
        return;
    }
    
    auto  downRect = Rect(m_touchBeganSushi->getPositionX() - halfSushiWidth,
                        m_touchBeganSushi->getPositionY() - (halfSushiHeight * 3),
                        m_touchBeganSushi->getContentSize().width,
                        m_touchBeganSushi->getContentSize().height);
    
    if (downRect.containsPoint(location)) {
        row--;
        if (row >= 0) {
            destSushi = m_matrix[row * m_width + col];
        }
        swapSushi(m_touchBeganSushi, destSushi);
        return;
    }
    
    auto  leftRect = Rect(m_touchBeganSushi->getPositionX() - (halfSushiWidth * 3),
                          m_touchBeganSushi->getPositionY() - halfSushiHeight,
                          m_touchBeganSushi->getContentSize().width,
                          m_touchBeganSushi->getContentSize().height);
    
    if (leftRect.containsPoint(location)) {
        col--;
        if (col >= 0) {
            destSushi = m_matrix[row * m_width + col];
        }
        swapSushi(m_touchBeganSushi, destSushi);
        return;
    }
    
    auto  rightRect = Rect(m_touchBeganSushi->getPositionX() + halfSushiWidth,
                          m_touchBeganSushi->getPositionY() - halfSushiHeight,
                          m_touchBeganSushi->getContentSize().width,
                          m_touchBeganSushi->getContentSize().height);
    
    if (rightRect.containsPoint(location)) {
        col++;
        if (col < m_width) {
            destSushi = m_matrix[row * m_width + col];
        }
        swapSushi(m_touchBeganSushi, destSushi);
        return;
    }
    
    // not a useful movement
}

void PlayLayer::swapSushi(SushiSprite *first, SushiSprite *second)
{
    m_isAnimationing = true;
    m_isTouchEnable = false;
    if (!first || !second) {
        return;
    }
    
    Point posOfFirst = first->getPosition();
    Point posOfSecond = second->getPosition();
    float time = 0.2;
    
    // 1.swap in matrix
    m_matrix[first->getRow() * m_width + first->getCol()] = second;
    m_matrix[second->getRow() * m_width + second->getCol()] = first;
    int tmpRow = first->getRow();
    int tmpCol = first->getCol();
    first->setRow(second->getRow());
    first->setCol(second->getCol());
    second->setRow(tmpRow);
    second->setCol(tmpCol);
    
    // 2.check for remove able
    std::list<SushiSprite *> colChainListOfFirst;
    getColChain(first, colChainListOfFirst);
    
    std::list<SushiSprite *> rowChainListOfFirst;
    getRowChain(first, rowChainListOfFirst);
    
    std::list<SushiSprite *> colChainListOfSecond;
    getColChain(second, colChainListOfSecond);
    
    std::list<SushiSprite *> rowChainListOfSecond;
    getRowChain(second, rowChainListOfSecond);
    
    if (colChainListOfFirst.size() >= 3
        || rowChainListOfFirst.size() >= 3
        || colChainListOfSecond.size() >= 3
        || rowChainListOfSecond.size() >= 3) {
        // just swap
        first->runAction(MoveTo::create(time, posOfSecond));
        second->runAction(MoveTo::create(time, posOfFirst));
        return;
    }
    
    // no chain, swap back
    m_matrix[first->getRow() * m_width + first->getCol()] = second;
    m_matrix[second->getRow() * m_width + second->getCol()] = first;
    tmpRow = first->getRow();
    tmpCol = first->getCol();
    first->setRow(second->getRow());
    first->setCol(second->getCol());
    second->setRow(tmpRow);
    second->setCol(tmpCol);
    
    first->runAction(Sequence::create(
                                      MoveTo::create(time, posOfSecond),
                                      MoveTo::create(time, posOfFirst),
                                      NULL));
    second->runAction(Sequence::create(
                                      MoveTo::create(time, posOfFirst),
                                      MoveTo::create(time, posOfSecond),
                                      NULL));
}

void PlayLayer::update(float dt)
{
    // check if animationing
    if (m_isAnimationing) {
        // init with false
        m_isAnimationing = false;
        for (int i = 0; i < m_height * m_width; i++) {
            SushiSprite *sushi = m_matrix[i];
            if (sushi && sushi->getNumberOfRunningActions() > 0) {
                m_isAnimationing = true;
                break;
            }
        }
    }
    
    // if sushi is moving, ignore use touch event
    m_isTouchEnable = !m_isAnimationing;
    
    if (!m_isAnimationing) {
        checkAndRemoveChain();
    }
}

//横向chain检查
void PlayLayer::getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
    chainList.push_back(sushi);// add first sushi
    
    int neighborCol = sushi->getCol() - 1;
    while (neighborCol >= 0) {
        SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborCol--;
        } else {
            break;
        }
    }
    
    neighborCol = sushi->getCol() + 1;
    while (neighborCol < m_width) {
        SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborCol++;
        } else {
            break;
        }
    }
}

void PlayLayer::getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
    chainList.push_back(sushi);// add first sushi
    
    int neighborRow = sushi->getRow() - 1;
    while (neighborRow >= 0) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborRow--;
        } else {
            break;
        }
    }
    
    neighborRow = sushi->getRow() + 1;
    while (neighborRow < m_height) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborRow++;
        } else {
            break;
        }
    }
}

void PlayLayer::fillVacancies()
{
    Size size = CCDirector::getInstance()->getWinSize();
    int *colEmptyInfo = (int *)malloc(sizeof(int) * m_width);
    memset((void *)colEmptyInfo, 0, sizeof(int) * m_width);
    
    // 1. drop exist sushi
    SushiSprite *sushi = NULL;
    for (int col = 0; col < m_width; col++) {
        int removedSushiOfCol = 0;
        // from buttom to top
        for (int row = 0; row < m_height; row++) {
            sushi = m_matrix[row * m_width + col];
            if (NULL == sushi) {
                removedSushiOfCol++;
            } else {
                if (removedSushiOfCol > 0) {
                    // evey item have its own drop distance
                    int newRow = row - removedSushiOfCol;
                    // switch in matrix
                    m_matrix[newRow * m_width + col] = sushi;
                    m_matrix[row * m_width + col] = NULL;
                    // move to new position
                    Point startPosition = sushi->getPosition();
                    Point endPosition = positionOfItem(newRow, col);
                    float speed = (startPosition.y - endPosition.y) / size.height;
                    sushi->stopAllActions();// must stop pre drop action
                    sushi->runAction(CCMoveTo::create(speed, endPosition));
                    // set the new row to item
                    sushi->setRow(newRow);
                }
            }
        }
        
        // record empty info
        colEmptyInfo[col] = removedSushiOfCol;
    }
    
    // 2. create new item and drop down.
    for (int col = 0; col < m_width; col++) {
        for (int row = m_height - colEmptyInfo[col]; row < m_height; row++) {
            createAndDropSushi(row, col);
        }
    }
    
    free(colEmptyInfo);
}

void PlayLayer::removeSushi(std::list<SushiSprite *> &sushiList)
{
    // make sequence remove
    m_isAnimationing = true;
    
    std::list<SushiSprite *>::iterator itList;
    for (itList = sushiList.begin(); itList != sushiList.end(); itList++) {
        SushiSprite *sushi = (SushiSprite *)*itList;
        // remove sushi from the metrix
        m_matrix[sushi->getRow() * m_width + sushi->getCol()] = NULL;
        explodeSushi(sushi);
    }
    
    // drop to fill empty
    fillVacancies();
}

void PlayLayer::explodeSushi(SushiSprite *sushi)
{
    float time = 0.3;
    
    // 1. action for sushi
    sushi->runAction(Sequence::create(
                                      ScaleTo::create(time, 0.0),
                                      CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sushi)),
                                      NULL));
    
    // 2. action for circle
    auto circleSprite = Sprite::create("circle.png");
	addChild(circleSprite, 10);
	circleSprite->setPosition(sushi->getPosition());
	circleSprite->setScale(0);// start size
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
                                             NULL));

    // 3. particle effect
	auto particleStars = ParticleSystemQuad::create("stars.plist");
    particleStars->setAutoRemoveOnFinish(true);
	particleStars->setBlendAdditive(false);
	particleStars->setPosition(sushi->getPosition());
	particleStars->setScale(0.3);
    addChild(particleStars, 20);
}

void PlayLayer::checkAndRemoveChain()
{
    for (int i = 0; i < m_height * m_width; i++) {
        SushiSprite *sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }
        
        // start count chain
        std::list<SushiSprite *> colChainList;
        getColChain(sushi, colChainList);
        
        std::list<SushiSprite *> rowChainList;
        getRowChain(sushi, rowChainList);
        
        std::list<SushiSprite *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
        if (longerList.size() == 3) {
            removeSushi(longerList);
            return;
        }
        if (longerList.size() > 3) {
            //TODO: make a special sushi can clean a line, and remove others
            removeSushi(longerList);
            return;
        }
    }
}

void PlayLayer::initMatrix()
{
    for (int row = 0; row < m_height; row++) {
		for (int col = 0; col < m_width; col++) {
            createAndDropSushi(row, col);
        }
    }
}

void PlayLayer::createAndDropSushi(int row, int col)
{
    Size size = Director::getInstance()->getWinSize();
    
    SushiSprite *sushi = SushiSprite::create(row, col);
    
    // create animation
    Point endPosition = positionOfItem(row, col);
    Point startPosition = Point(endPosition.x, endPosition.y + size.height / 2);
    sushi->setPosition(startPosition);
    float speed = startPosition.y / (2 * size.height);
    sushi->runAction(MoveTo::create(speed, endPosition));
    // add to BatchNode
    spriteSheet->addChild(sushi);

    m_matrix[row * m_width + col] = sushi;
}

Point PlayLayer::positionOfItem(int row, int col)
{
    float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
    float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
    return Point(x, y);
}
