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
        
        CCTime::gettimeofdayCocos2d( &m_Now, NULL );
        
        double fTimeDelta = CCTime::timersubCocos2d((struct cc_timeval *)&m_Prev, (struct cc_timeval *)&m_Now);

        if( fTimeDelta < BY_DOUBLE_CLICK_DELTA )
        {
            bDoubleClick = true;
        }
        
        m_Prev = m_Now;
        
        return bDoubleClick;
    }
    
    cc_timeval& getNow(){  return m_Now;   }
    cc_timeval& getPrev(){ return m_Prev;  }
private:
    cc_timeval m_Prev;
    cc_timeval m_Now;
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
        CCTime::gettimeofdayCocos2d( &m_BeginTime, NULL );
    }
    
    void end()
    {
        CCTime::gettimeofdayCocos2d( &m_EndTime, NULL );
        m_DeltaTime = ( m_EndTime.tv_sec - m_BeginTime.tv_sec ) + ( m_EndTime.tv_usec - m_BeginTime.tv_usec ) * 0.000001;
    }
    
    float getDelta() { return m_DeltaTime; }
    void printDelta() { std::cout << "BYClock::PrintDelta = " << m_DeltaTime << " second." << std::endl; }
    
private:
    cc_timeval m_BeginTime;
    cc_timeval m_EndTime;
    float m_DeltaTime;
};

#endif
