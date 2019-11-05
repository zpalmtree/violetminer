copy CUDAVisualStudioIntegration\\extras\\visual_studio_integration\\MSBuildExtensions\\* "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\IDE\VC\VCTargets\BuildCustomizations\"

dir C:\Program Files\NVIDIA Corporation

echo "test"

dir "C:\Program Files\NVIDIA Corporation"
dir "C:\Program Files"
dir "C:\Program Files\NVIDIA GPU Computing Toolkit"
dir "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA"
dir "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0"
dir "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin"

dir C:\Program Files\NVIDIA Corporation
dir C:\Program Files
dir C:\Program Files\NVIDIA GPU Computing Toolkit
dir C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA
dir C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
dir C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin

set PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin;%PATH%
set PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin;%PATH%

set CudaToolkitDir=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0

echo %PATH%

nvcc --version

md build

cd build

cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_PLATFORM_TOOLSET_CUDA=8.0 .. || type CMakeFiles\CMakeError.log || type CMakeFiles\CMakeOutput.log || exit /b 1

MSBuild violetminer.sln /p:Configuration=Release /m /v:n || exit /b 1 

cd Release

argon2-cpp-test.exe
