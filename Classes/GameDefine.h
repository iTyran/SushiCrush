#ifndef CandyCrush_GameDefine_h
#define CandyCrush_GameDefine_h

#include "cocos2d.h"
USING_NS_CC;

#define kTileSize 50.0f
#define kMoveTileTime 0.25f
#define kBoxWidth 6
#define kBoxHeight 8
#define kStartX (Director::getInstance()->getWinSize().width - kTileSize * kBoxWidth) /2
#define kStartY (Director::getInstance()->getWinSize().height - kTileSize * kBoxHeight) /2
#define kKindCount 6

enum Orientation{
	OrientationHori,
	OrientationVert,
};

#endif
