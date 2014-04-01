#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    DISPLAY_MODE_NORMAL = 0,
    DISPLAY_MODE_HORIZONTAL,
    DISPLAY_MODE_VERTICAL,
} DisplayMode;

class SushiSprite :  public Sprite
{
public:
    SushiSprite();
	static SushiSprite *create(int row, int col);
    static float getContentWidth();
    
    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(int, m_col, Col);
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
    CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove);
    CC_SYNTHESIZE(bool, m_ignoreCheck, IgnoreCheck);//新产生的4消寿司，本轮不被消除
    CC_SYNTHESIZE_READONLY(DisplayMode, m_displayMode, DisplayMode);
    void setDisplayMode(DisplayMode mode);
};

#endif /* defined(__SushiSprite_H__) */
