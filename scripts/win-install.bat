copy CUDAVisualStudioIntegration\\extras\\visual_studio_integration\\MSBuildExtensions\\* "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\IDE\VC\VCTargets\BuildCustomizations\"

set PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\bin;%PATH%
set PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin;%PATH%

:: set CUDACXX=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\bin\nvcc
:: REM set CUDA_TOOLKIT_ROOT_DIR=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1
:: REM set CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1
set CudaToolkitDir=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1

echo %PATH%

nvcc --version

md build

cd build

cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_PLATFORM_TOOLSET_CUDA=10.1 .. || type CMakeFiles\CMakeError.log || type CMakeFiles\CMakeOutput.log || exit /b 1

MSBuild violetminer.sln /p:Configuration=Release /m /v:n || exit /b 1 

cd Release

argon2-cpp-test.exe
