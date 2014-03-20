#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

USING_NS_CC;

class SushiSprite :  public Sprite
{
public:
	static SushiSprite *create(int row, int col);
    static float getContentWidth();
    
    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(int, m_col, Col);
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
};

#endif /* defined(__SushiSprite_H__) */
