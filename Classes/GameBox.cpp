//
//  GameBox.cpp
//  CandyCrush
//
//  Created by 郭 一鸣 on 14-3-19.
//
//

#include "GameBox.h"

GameBox::GameBox(Size asize)
{
    m_size = asize;
    m_outBorderTile = new GameTile(-1, -1);
    m_content = __Array::createWithCapacity(m_size.height);
    m_content->retain();
    
    for(int y = 0; y < m_size.height; y++)
    {
        __Array* rowCount = __Array::createWithCapacity(m_size.width);
        for(int x = 0; x < m_size.width; x++)
        {
            GameTile* tile = new GameTile(x, y);
            rowCount->addObject(tile);
        }
        
        m_content->addObject(rowCount);
    }
    
    m_readyToRemoveTiles = __Array::createWithCapacity(m_size.height);
    m_readyToRemoveTiles->retain();
}

GameTile* GameBox::objectAtXandY(int posX, int posY)
{
    if(posX < 0 || posX >= kBoxWidth || posY < 0 || posY >= kBoxHeight)
    {
        return m_outBorderTile;
    }
    
    return (GameTile*)((__Array*)m_content->getObjectAtIndex(posY))->getObjectAtIndex(posX);
}

/**
 * add candy in the the same row or col to the remove stack
 */

void GameBox::useCombo(int posX, int posY, int comboValue)
{
    int iMax = (comboValue == 1) ? m_size.width : m_size.height;
    for(int i = 0; i < iMax; i++)
    {
        int tileX = comboValue == 1 ? i : posY;
        int tileY = comboValue == 1 ? posY : i;
        GameTile* tile = this->objectAtXandY(tileX, tileY);
        m_readyToRemoveTiles->addObject(tile);
    }
}

void GameBox::checkWith(Orientation orient)
{
    int iMax = (orient == OrientationHori) ? m_size.width : m_size.height;
    int jMax = (orient == OrientationHori) ? m_size.height : m_size.width;
    
    for(int i = 0; i < iMax; i++)
    {
        int count = 0;
        int value = -1;
        int comboValue = -1;
        m_firstTile = nullptr;
        m_secondTile = nullptr;
        for(int j = 0; j < jMax; j++)
        {
            int tileX = orient == OrientationHori ? i : j;
            int tileY = orient == OrientationHori ? j : i;
            GameTile* tile = this->objectAtXandY(tileX, tileY);
            tile->refreshDebugInfo();
            if(tile->_comboValue > 0)
            {
                comboValue = tile->_comboValue;
            }
            if(tile->_value == value)
            {
                count++;
                if(comboValue > 0 && count >= 3)
                {
                    useCombo(tileX, tileY, comboValue);
                }
                if(count > 4)
                {
                    m_readyToRemoveTiles->addObject(tile);
                }
                else if(count == 4)
                {
                    tile->changeComboTile(OrientationHori);
                }
                else if(count == 3)
                {
                    m_readyToRemoveTiles->addObject(m_firstTile);
                    m_readyToRemoveTiles->addObject(m_secondTile);
                    m_readyToRemoveTiles->addObject(tile);
                    m_firstTile = nullptr;
                    m_secondTile = nullptr;
                }
                else if( count == 2)
                {
                    m_secondTile = tile;
                }
            }
            else
            {
                count = 1;
                m_firstTile = tile;
                m_secondTile = nullptr;
                value = tile->_value;
            }
        }
    }
}

bool GameBox::check()
{
    checkWith(OrientationHori);
    checkWith(OrientationVert);
    
    int count = m_readyToRemoveTiles->count();
    if(count == 0)
    {
        return false;
    }
    for(int i = 0; i < count; i++)
    {
        GameTile* tile = (GameTile*)m_readyToRemoveTiles->getObjectAtIndex(i);
        tile->_value =  0;
        if(tile->_sprite)
        {
            tile->_sprite->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.3f, 0), CallFuncN::create(CC_CALLBACK_1(GameBox::removeSprite, this))));
        }
    }
    
    m_readyToRemoveTiles->removeAllObjects();
    int maxCount = this->repair();
    
    layer->runAction(Sequence::create(DelayTime::create(kMoveTileTime * maxCount + 0.03f), CallFunc::create(CC_CALLBACK_0(GameBox::afterAllMoveDone, this)), NULL));
    
    return true;
}

void GameBox::removeSprite(Node* sender)
{
    GameTile* tile = (GameTile*)sender;
    layer->removeChild(tile, true);
}

void GameBox::afterAllMoveDone()
{
    if(check())
    {
        return;
    }
    if(haveMore())
    {
        setLock(false);
    }
    else
    {
        for(int y = 0; y < kBoxHeight; y++)
        {
            for(int x = 0; x < kBoxWidth; x++)
            {
                GameTile* tile = objectAtXandY(x, y);
                tile->_value = 0;
            }
        }
        check();
    }
}

int GameBox::repair()
{
    int maxCount = 0;
    for(int x = 0; x < m_size.width; x++)
    {
        int cout = repairSingleColumn(x);
        if(cout > maxCount)
        {
            maxCount = cout;
        }
    }
    
    return maxCount;
}

int GameBox::repairSingleColumn(int columnIndex)
{
    int extension = 0;
    for(int y = 0; y < m_size.height; y++)
    {
        GameTile* tile = objectAtXandY(columnIndex, y);
        if(tile->_value == 0)
        {
            extension++;
        }
        else if(extension == 0)
        {
            
        }
        else
        {
            GameTile* destTile = objectAtXandY(columnIndex, y - extension);
            tile->_sprite->stopAllActions();
            tile->_sprite->runAction(MoveBy::create(kMoveTileTime * extension, Point(0, -kTileSize * extension)));
            destTile->_value = tile->_value;
            destTile->_sprite = tile->_sprite;
            destTile->_comboValue = tile->_comboValue;
        }
    }
    
    for(int i = 0; i < extension; i++)
    {
        int value = arc4random() % kKindCount + 1;
        GameTile* destTile = objectAtXandY(columnIndex, kBoxHeight - extension + i);
        char name[10] = {0};
        sprintf(name, "%d.png", value);
        Sprite* sprite = Sprite::create(name);
        sprite->setPosition(Point(kStartX + columnIndex * kTileSize + kTileSize / 2, kStartY + (kBoxHeight + i) * kTileSize + kTileSize / 2));
        layer->addChild(sprite);
        sprite->runAction(MoveBy::create(kMoveTileTime * extension, Point(0, -kTileSize * extension)));
        destTile->_value = value;
        destTile->_comboValue = 0;
        destTile->_sprite = sprite;
    }
    
    return extension;
}

bool GameBox::haveMore()
{
    for(int y = 0; y < m_size.height; y++)
    {
        for(int x = 0; x < m_size.width; x++)
        {
            GameTile* aTile = objectAtXandY(x, y);
            // v 1 2
            if(aTile->_y - 1 >= 0)
            {
                GameTile* bTile = objectAtXandY(x, y + 2);
                if(aTile->_value == bTile->_value)
                {
                    {
                        GameTile* cTile = objectAtXandY(x - 1, y + 1);
                        if(cTile->_value == aTile->_value)
                        {
                            return true;
                        }
                    }
                    {
                        GameTile* cTile = objectAtXandY(x-1, y+1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x+1, y+1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x, y-3);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x-1, y-2);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x+1, y-2);
						if (cTile->_value == aTile->_value)
							return true;
					}
                }

            }
            // v 1 3
            if(aTile->_y - 2 >= 0)
            {
                GameTile* bTile = objectAtXandY(x, y - 2);
                if(aTile->_value == bTile->_value)
                {
                    {
                        GameTile* cTile = objectAtXandY(x, y + 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x, y - 3);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x - 1, y - 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 1, y - 1);
						if (cTile->_value == aTile->_value)
							return true;
					}

                }
            }
            // h 1 2
            if(aTile->_x + 1 < kBoxWidth)
            {
                GameTile* bTile = objectAtXandY(x + 1, y);
                if(aTile->_value == bTile->_value)
                {
                    {
                        GameTile* cTile = objectAtXandY(x - 2, y);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x - 1, y - 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x - 1, y + 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 3, y);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 2, y - 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 2, y + 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
                }
            }
            // h 1 3
            if(aTile->_x + 2 >= kBoxWidth)
            {
                GameTile* bTile = objectAtXandY(x + 2, y);
                if(aTile->_value == bTile->_value)
                {
                    {
                        GameTile* cTile = objectAtXandY(x + 3, y);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x - 1, y);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 1, y - 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
					{
                        GameTile* cTile = objectAtXandY(x + 1, y + 1);
						if (cTile->_value == aTile->_value)
							return true;
					}
                }
            }
        }
    }
    
    return false;
}




















