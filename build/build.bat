@echo off
pushd "C:\dev\Prototypes\engine\build"
REM Optimization switches /O2 /Oi /fp:fast
REM set CommonCompilerFlags=/O2 /MTd /nologo /fp:fast /Gm- /GR- /EHa /Zo /Oi /WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4127 /DHANDMADE_INTERNAL=1 /DHANDMADE_SLOW=1 /DHANDMADE_WIN32 /Z7 /FC /F4194304
REM set CommonLinkerFlags= -incremental:no -opt:ref  user32.lib gdi32.lib winmm.lib
set ReleaseCompilerFlags= /O2 /Oi
set DebugCompilerFlags= /Zi /Zo
set CommonCompilerFlags= /MP /GR- /WX /EHsc %ReleaseCompilerFlags%
set Includes= /I "../" /I "C:/frameworks/SDL2-2.28.4/include" /I "C:/frameworks/SDL2_ttf-2.20.2/include" /I "C:\frameworks\SDL2_mixer-2.8.0\include" /I "C:\frameworks\SDL2_image-2.6.3\include" /I "C:\frameworks\glew-2.1.0\include" /I "C:\frameworks\glm-0.9.9.8\includes" /I "../vendor/imgui" /I "../vendor/imgui/backends" /I "B:\frameworks\glew-2.1.0\include" /I "B:\frameworks\glm-0.9.9.8\includes"
set Libraries= SDL2main.lib SDL2.lib SDL2_image.lib SDL2_ttf.lib SDL2_mixer.lib
set LinkerFlags= /DEBUG -incremental:no /LIBPATH:C:\frameworks\glew-2.1.0\lib\Release\x64 winmm.lib user32.lib gdi32.lib ws2_32.lib opengl32.lib glew32.lib %Libraries%
set ImguiSources= "../vendor/imgui/imgui.cpp" "../vendor/imgui/imgui_draw.cpp" "../vendor/imgui/imgui_tables.cpp" "../vendor/imgui/imgui_widgets.cpp" "../vendor/imgui/backends/imgui_impl_sdl2.cpp" "../vendor/imgui/backends/imgui_impl_sdlrenderer2.cpp" "../vendor/imgui/misc/cpp/imgui_stdlib.cpp"
@REM %date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
@REM cl %CommonCompilerFlags% %ImguiSources% %Includes% -LD /link %LinkerFlags%
del game_*.pdb
@REM set PDB=game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb
@REM /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb
@REM /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
cl %CommonCompilerFlags% "../src/DynamicGameCode.cpp" %Includes% -LD /link %LinkerFlags% imgui.obj imgui_impl_sdl2.obj imgui_impl_sdlrenderer2.obj imgui_stdlib.obj imgui_tables.obj imgui_draw.obj imgui_widgets.obj /EXPORT:GameUpdateAndRender /EXPORT:GameHandleEvent /OUT:DynamicGameCode.dll
cl %CommonCompilerFlags% /Fe:engine.exe "../src/Application.cpp" %Includes% /link %LinkerFlags% imgui.obj imgui_impl_sdl2.obj imgui_impl_sdlrenderer2.obj imgui_stdlib.obj imgui_tables.obj imgui_draw.obj imgui_widgets.obj
popd

