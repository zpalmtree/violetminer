@echo off

copy CUDAVisualStudioIntegration\\extras\\visual_studio_integration\\MSBuildExtensions\\* "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\v150\BuildCustomizations"

set PATH="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\bin\";%PATH%
set CUDACXX="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\bin\nvcc"

md build

cd build

cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release .. || exit /b 1

MSBuild violetminer.sln /p:Configuration=Release /m || exit /b 1 

cd Release

argon2-cpp-test.exe
