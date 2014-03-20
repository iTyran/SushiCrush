//
//  GameTile.h
//  CandyCrush
//
//  Created by 郭 一鸣 on 14-3-19.
//
//

#ifndef __CandyCrush__GameTile__
#define __CandyCrush__GameTile__

#include <iostream>
#include "cocos2d.h"
#include "GameDefine.h"

USING_NS_CC;

class GameTile : public Node
{
public:
    GameTile(int ax, int ay);
    ~GameTile();
    bool nearTile(GameTile* otherTile);
    void trade(GameTile* otherTile);
    void changeComboTile(int orient);
    Point pixPosition();
    void refreshDebugInfo();
    
public:
    int _x;
    int _y;
    int _value;             // six kinds of candy
    int _comboValue;        // 1:orientationHori combo candy, 2:orientationVert combo candy
    Sprite* _sprite;

};

#endif /* defined(__CandyCrush__GameTile__) */
