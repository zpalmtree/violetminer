# Add the files we want to link against
set(nvidia_backend_source_files
    Nvidia.cpp
)

add_library(NvidiaBackend ${nvidia_backend_source_files})
