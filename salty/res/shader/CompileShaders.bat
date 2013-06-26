@echo off
@rem //------------------------------------------------------
@rem // File : CompileShaders.bat
@rem // Desc : Shader Compile Batch File.
@rem // Author : Pocol
@rem //-------------------------------------------------------

setlocal
set error=0

call :CompileShader LineShader vs VSFunc
call :CompileShader LineShader ps PSFunc
call :CompileShader SpriteShader vs VSFunc
call :CompileShader SpriteShader ps PSFunc


echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShader
set fxc=fxc /nologo %1.fx /T%2_4_0_level_9_1 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /FhCompiled\%1_%3.inc /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b