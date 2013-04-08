//
//  BYUtility.h
//  TableTest
//
//  Created by jason on 12-12-28.
//
//

#ifndef TableTest_BYUtility_h
#define TableTest_BYUtility_h

#include "BYMacros.h"
#include "BYTouchDelegate.h"
#include <iostream>

//class for detect double click
class BYDoubleClick
{
public:
    BYDoubleClick()
    {
        memset( &m_Prev, 0, sizeof( m_Prev ) );
        memset( &m_Now, 0, sizeof( m_Now ) );
    }
    
    //is there a double click
    bool detection()
    {
        bool bDoubleClick = false;
        
        gettimeofday( &m_Now, NULL );
        
        float fTimeDelta = ( m_Now.tv_sec - m_Prev.tv_sec ) + ( m_Now.tv_usec - m_Prev.tv_usec ) * 0.000001f;
        
        if( fTimeDelta < BY_DOUBLE_CLICK_DELTA )
        {
            bDoubleClick = true;
        }
        
        m_Prev = m_Now;
        
        return bDoubleClick;
    }
    
    timeval& getNow(){  return m_Now;   }
    timeval& getPrev(){ return m_Prev;  }
private:
    timeval m_Prev;
    timeval m_Now;
};

class BYClock
{
public:
    BYClock() : m_DeltaTime ( 0 )
    {
        memset( &m_BeginTime, 0, sizeof( m_BeginTime ) );
        memset( &m_EndTime, 0, sizeof( m_EndTime ) );
    }
    
    void begin()
    {
        gettimeofday( &m_BeginTime, NULL );
    }
    
    void end()
    {
        gettimeofday( &m_EndTime, NULL );
        m_DeltaTime = ( m_EndTime.tv_sec - m_BeginTime.tv_sec ) + ( m_EndTime.tv_usec - m_BeginTime.tv_usec ) * 0.000001;
    }
    
    float getDelta() { return m_DeltaTime; }
    void printDelta() { std::cout << "BYClock::PrintDelta = " << m_DeltaTime << " second." << std::endl; }
    
private:
    timeval m_BeginTime;
    timeval m_EndTime;
    float m_DeltaTime;
};

#endif
