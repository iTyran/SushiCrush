//
//  GameTile.cpp
//  CandyCrush
//
//  Created by 郭 一鸣 on 14-3-19.
//
//

#include "GameTile.h"
#include <math.h>

GameTile::GameTile(int ax, int ay)
{
    _x = ax;
    _y = ay;
    _comboValue = 0;
}

GameTile::~GameTile()
{
    _sprite = nullptr;
}

/**
 * Checks if two tiles are neighbor horiz or vert
 */
bool GameTile::nearTile(GameTile* otherTile)
{
    return (_x == otherTile->_x && abs(_y - otherTile->_y) == 1) ||
    (_y == otherTile->_y && abs(_x - otherTile->_x) == 1);
}

void GameTile::trade(GameTile* otherTile)
{
    Sprite* tempSprite = _sprite;
    int tempValue = _value;
    int tempComboValue = _comboValue;
    
    _sprite = otherTile->_sprite;
    _value = otherTile->_value;
    _comboValue = otherTile->_comboValue;
    
    otherTile->_sprite = tempSprite;
    otherTile->_value = tempValue;
    otherTile->_comboValue = tempComboValue;
    
    tempSprite = nullptr;
}

void GameTile::changeComboTile(int orient)
{
    if(!_sprite)
    {
        return;
    }
    
    char name[20] = {0};
    if(orient == OrientationHori)
    {
        _comboValue = 1;
        sprintf(name, "%d_2.png", _comboValue);
    }
    else
    {
        _comboValue = 2;
        sprintf(name, "%d_3.png", _comboValue);
    }
    
    _sprite->setSpriteFrame(SpriteFrame::create(name, Rect(0, 0, 95, 95)));
}


Point GameTile::pixPosition()
{
    return Point(kStartX + _x * kTileSize + kTileSize / 2.0f, kStartY + _y * kTileSize + kTileSize / 2.0f);
}

void GameTile::refreshDebugInfo()
{
    if(true or !_sprite)
    {
        return;
    }
    
    _sprite->removeAllChildrenWithCleanup(true);
    
    char lab[3] = {0};
    sprintf(lab, "%d-%d", _value, _comboValue);
    
    auto labValue = LabelTTF::create(lab, "Arial", 40);
    labValue->setColor(Color3B(255, 0, 0));
    labValue->setPosition(Point(kTileSize / 2, kTileSize / 2));
    _sprite->addChild(labValue);
}





