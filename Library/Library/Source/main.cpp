

#include <assert.h>
#include <tchar.h>
#include "../Header/StartingOver.h"
#include "../Header/Input.h"
#include "../Header/Blender.h"
#include "../../HideSeek/Header/GameScene.h"


INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{


    constexpr LPCTSTR name = L"StartingOver";
    StartingOver::Initialize(name, 1920, 1080, false);
    StartingOver::SetBlendMode(StartingOver::Blender::BS_NONE);


    Input input;


    while (StartingOver::GameLoop(TRUE))
    {
        input.Update();
       
     
        StartingOver::Present(0 ,0);
    }

    StartingOver::UnInitialize();
}