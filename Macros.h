#ifndef __MACROS_H__
#define __MACROS_H__

//Map Type
#define MAP_TYPE_ISO 0
#define MAP_TYPE_ORT 1

#define WM_TYPE 1

//Speed Control
#if (WM_TYPE == MAP_TYPE_ISO)
#define VS_LEFT		 b2Vec2(-4,2)
#define VS_STOP		 b2Vec2(0,0)
#define VS_RIGHT	 b2Vec2(4,-2)
#define VS_UP		 b2Vec2(4,2)
#define VS_DOWN		 b2Vec2(-4,-2)
#else
#define VS_LEFT		 b2Vec2(-4,0)
#define VS_STOP		 b2Vec2(0,0)
#define VS_RIGHT	 b2Vec2(4,0)
#define VS_UP		 b2Vec2(0,4)
#define VS_DOWN		 b2Vec2(0,-4)
#endif


//Debug Draw on Map
#define DEBUGDRAW				

#endif
