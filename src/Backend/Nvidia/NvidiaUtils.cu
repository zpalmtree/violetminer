// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>
#include <map>
#include <vector>

#include "Nvidia/Argon2.h"
#include "Utilities/ColouredMsg.h"

std::vector<std::tuple<std::string, bool, int>> getNvidiaDevicesActual()
{
    std::vector<std::tuple<std::string, bool, int>> devices;

    int numberDevices;
    cudaGetDeviceCount(&numberDevices);

    for (int i = 0; i < numberDevices; i++)
    {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);

        devices.push_back(std::make_tuple(prop.name, true, i));
    }

    return devices;
}

void printNvidiaHeader()
{
    std::cout << InformationMsg<std::string>("* ") << WhiteMsg<std::string>("NVIDIA DEVICES", 25);

    int numberDevices;
    cudaGetDeviceCount(&numberDevices);

    if (numberDevices == 0)
    {
        std::cout << WarningMsg<std::string>("None found") << std::endl;
        return;
    }

    std::map<std::string, std::vector<cudaDeviceProp>> gpus;

    for (int i = 0; i < numberDevices; i++)
    {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);

        std::string deviceName = prop.name;

        gpus[deviceName].push_back(prop);
    }

    int i = 0;

    for (const auto gpu : gpus)
    {
        const auto name = gpu.first;
        const auto properties = gpu.second;

        if (i != 0)
        {
            std::cout << ", ";
        }

        size_t numDevices = properties.size();

        std::cout << SuccessMsg<size_t>(numDevices) << SuccessMsg<std::string>("x ") << SuccessMsg<std::string>(name);

        i++;
    }

    std::cout << std::endl;
}

uint32_t getNoncesPerRun(const size_t scratchpadSize, const uint32_t gpuIndex)
{
    cudaDeviceProp properties;

    /* Figure out how much memory we have available */
    cudaGetDeviceProperties(&properties, gpuIndex);

    const size_t ONE_MB = 1024 * 1024;
    const size_t ONE_GB = ONE_MB * 1024;

    size_t memoryAvailable = (properties.totalGlobalMem / ONE_GB - 1) * (ONE_GB / ONE_MB);

    /* The amount of nonces we're going to try per kernel launch */
    uint32_t noncesPerRun = (memoryAvailable * ONE_MB) / (1024 * scratchpadSize);
    noncesPerRun = (noncesPerRun / 128) * 128;

    return noncesPerRun;
}
