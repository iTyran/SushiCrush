#include "PlayLayer.h"
#include "SushiSprite.h"

#define MATRIX_WIDTH (6)
#define MATRIX_HEIGHT (8)

#define SUSHI_GAP (1)

PlayLayer::PlayLayer()
: spriteSheet(NULL)
, m_matrix(NULL)
, m_width(0)
, m_height(0)
, m_matrixLeftBottomX(0)
, m_matrixLeftBottomY(0)
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
    return true;
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