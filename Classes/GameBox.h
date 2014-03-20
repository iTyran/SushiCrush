//
//  GameBox.h
//  CandyCrush
//
//  Created by 郭 一鸣 on 14-3-19.
//
//

#ifndef __CandyCrush__GameBox__
#define __CandyCrush__GameBox__

#include <iostream>
#include "GameTile.h"
#include "GameDefine.h"

USING_NS_CC;

class GameBox : public Node
{
public:
    GameBox(Size asize);
    GameTile* objectAtXandY(int posX, int posY);
    void useCombo(int posX, int posY, int comboValue);
    void checkWith(Orientation orient);
    bool check();
    void removeSprite(Node* sender);
    void afterAllMoveDone();
    bool haveMore();
    int repair();
    int repairSingleColumn(int columnIndex);
    
    inline void setLock(bool lock)
    {
        m_lock = lock;
    }
    inline bool isLocked()
    {
        return m_lock;
    }
    Layer* layer;
    
private:
    bool m_lock;
    GameTile* m_firstTile;
    GameTile* m_secondTile;
    GameTile* m_outBorderTile;
    Size m_size;
    __Array* m_content;
    __Array* m_readyToRemoveTiles;
};


#endif /* defined(__CandyCrush__GameBox__) */
