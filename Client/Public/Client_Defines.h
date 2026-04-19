#pragma once

#include <Windows.h>
#include <process.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

#pragma push_macro("new")
#undef new
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma pop_macro("new")

namespace Client 
{
	const unsigned int		g_iWinSizeX = { 2560 };
	const unsigned int		g_iWinSizeY = { 1440 };

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, DUNGEON, BOSSPATH, BOSS, END };


}



using namespace std;
using namespace Client;



/* 1. 똥싼놈(남). */
/* 1. 피해의식갑.(여) */
/* 1. 추천빌런.(여) */
/* 1. 여미새, 남미새 */
/* 1. 5기 호로새끼.(남) */
/* 1. 변태범죄자새끼 1, 2 .(남) */
/* 1. 야겜만든중2병.(남) */
/* 1. 착한쁘락지.(남) */
/* 1. 여왕벌과 아이들(남들) */
/* 1. 게이.(남) */
/* 1. 코스프레.(남, 여) */
/* 1. 서울게임아카데미 */
/* 1. 쥬신의 과거 */