#include "SushiSprite.h"

USING_NS_CC;

#define TOTAL_SUSHI (6)

static const char *sushiNormal[TOTAL_SUSHI] = {
	"sushi_1n.png",
	"sushi_2n.png",
	"sushi_3n.png",
	"sushi_4n.png",
	"sushi_5n.png",
    "sushi_6n.png"
};

static const char *sushiVertical[TOTAL_SUSHI] = {
	"sushi_1v.png",
	"sushi_2v.png",
	"sushi_3v.png",
	"sushi_4v.png",
	"sushi_5v.png",
    "sushi_6v.png"
};

static const char *sushiHorizontal[TOTAL_SUSHI] = {
	"sushi_1h.png",
	"sushi_2h.png",
	"sushi_3h.png",
	"sushi_4h.png",
	"sushi_5h.png",
    "sushi_6h.png"
};

float SushiSprite::getContentWidth()
{
    static float itemWidth = 0;
    if (0 == itemWidth) {
        Sprite *sprite = CCSprite::createWithSpriteFrameName(sushiNormal[0]);
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

SushiSprite::SushiSprite()
: m_col(0)
, m_row(0)
, m_imgIndex(0)
, m_isNeedRemove(false)
, m_ignoreCheck(false)
, m_displayMode(DISPLAY_MODE_NORMAL)
{
}

SushiSprite *SushiSprite::create(int row, int col)
{
	SushiSprite *sushi = new SushiSprite();
	sushi->m_row = row;
	sushi->m_col = col;
    sushi->m_imgIndex = arc4random() % TOTAL_SUSHI;
    sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
	sushi->autorelease();
	return sushi;
}

void SushiSprite::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
    
    SpriteFrame *frame;
    switch (mode) {
        case DISPLAY_MODE_VERTICAL:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiVertical[m_imgIndex]);
            break;
        case DISPLAY_MODE_HORIZONTAL:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiHorizontal[m_imgIndex]);
            break;
        default:
            return;
    }
    setDisplayFrame(frame);
}