@echo off
pushd "B:\dev\Prototypes\engine\build"
REM Optimization switches /O2 /Oi /fp:fast
REM set CommonCompilerFlags=/O2 /MTd /nologo /fp:fast /Gm- /GR- /EHa /Zo /Oi /WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4127 /DHANDMADE_INTERNAL=1 /DHANDMADE_SLOW=1 /DHANDMADE_WIN32 /Z7 /FC /F4194304
REM set CommonLinkerFlags= -incremental:no -opt:ref  user32.lib gdi32.lib winmm.lib

set build=DEBUG

set DebugCompilerFlags= /Zi /Zo /DDEBUG
set ReleaseCompilerFlags= /O2 /Oi /DNDEBUG
set CommonCompilerFlags= /MP /GR- /EHsc /WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4127
set CompilerFlags= 

set Includes= /I "../" /I "%FRAMEWORKS%/SDL2-2.28.4/include" /I "%FRAMEWORKS%\SDL2_ttf-2.20.2/include" /I "%FRAMEWORKS%\SDL2_mixer-2.8.0\include" /I "%FRAMEWORKS%\SDL2_image-2.6.3\include" /I "%FRAMEWORKS%\glew-2.1.0\include" /I "%FRAMEWORKS%\glm-0.9.9.8\includes" /I "../vendor/imgui" /I "../vendor/imgui/backends"
set Libraries= SDL2main.lib SDL2.lib SDL2_image.lib SDL2_ttf.lib SDL2_mixer.lib

set DebugLinkerFlags= /SUBSYSTEM:Console
set ReleaseLinkerFlags= /SUBSYSTEM:Windows
set CommonLinkerFlags= -incremental:no /LIBPATH:%FRAMEWORKS%\glew-2.1.0\lib\Release\x64 winmm.lib user32.lib gdi32.lib ws2_32.lib opengl32.lib glew32.lib %Libraries%
set LinkerFlags=

if %build% == RELEASE (
    set CompilerFlags= %CommonCompilerFlags% %ReleaseCompilerFlags%
    set LinkerFlags= %CommonLinkerFlags% %ReleaseLinkerFlags%
    echo Release
)
if %build% == DEBUG (
    set CompilerFlags= %DebugCompilerFlags% %CommonCompilerFlags% 
    set LinkerFlags= %DebugLinkerFlags% %CommonLinkerFlags%
    echo Debug
)


set ImguiSources= "../vendor/imgui/imgui.cpp" "../vendor/imgui/imgui_draw.cpp" "../vendor/imgui/imgui_tables.cpp" "../vendor/imgui/imgui_widgets.cpp" "../vendor/imgui/backends/imgui_impl_sdl2.cpp" "../vendor/imgui/backends/imgui_impl_sdlrenderer2.cpp" "../vendor/imgui/misc/cpp/imgui_stdlib.cpp"
@REM %date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
@REM cl %CommonCompilerFlags% %ImguiSources% %Includes% -LD /link %LinkerFlags%
del game_*.pdb
@REM set PDB=game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb
@REM /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb
@REM /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
cl %CompilerFlags% "../src/DynamicGameCode.cpp" %Includes% -LD /link %LinkerFlags% imgui.obj imgui_impl_sdl2.obj imgui_impl_sdlrenderer2.obj imgui_stdlib.obj imgui_tables.obj imgui_draw.obj imgui_widgets.obj /EXPORT:GameUpdateAndRender /EXPORT:GameHandleEvent /OUT:DynamicGameCode.dll
cl %CompilerFlags% /Fe:engine.exe "../src/Application.cpp" %Includes% /link %LinkerFlags% imgui.obj imgui_impl_sdl2.obj imgui_impl_sdlrenderer2.obj imgui_stdlib.obj imgui_tables.obj imgui_draw.obj imgui_widgets.obj
popd

