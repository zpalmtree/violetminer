# Add the files we want to link against
set(nvidia_manager_source_files
    NvidiaManager.cu
)

# Add the library to be linked against, with the previously specified source files
add_library(NvidiaManager ${nvidia_manager_source_files})

target_compile_features(NvidiaManager PUBLIC cxx_std_11)

set_target_properties(NvidiaManager PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_link_libraries(NvidiaManager Nvidia)
