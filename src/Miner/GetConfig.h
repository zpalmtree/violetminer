// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <thread>

#include "Types/Pool.h"
#include "Argon2/Constants.h"

#if defined(NVIDIA_ENABLED)
#include "Backend/Nvidia/NvidiaUtils.h"
#endif

struct NvidiaDevice
{
    bool enabled = true;

    std::string name;

    uint16_t id;

    uint32_t nonceOffset;

    uint32_t getNoncesPerRound(const size_t scratchpadSize)
    {
        #if defined(NVIDIA_ENABLED)
        return getNoncesPerRun(scratchpadSize, id);
        #else
        return 0;
        #endif
    }
};

struct AmdDevice
{
    bool enabled = true;

    std::string name;

    uint16_t id;

    uint32_t nonceOffset;

    uint32_t getNoncesPerRound(const size_t scratchpadSize)
    {
        return 0;
    }
};

struct CpuConfig
{
    bool enabled = true;

    uint32_t threadCount = std::thread::hardware_concurrency();

    Constants::OptimizationMethod optimizationMethod = Constants::OptimizationMethod::AUTO;
};

struct NvidiaConfig
{
    std::vector<NvidiaDevice> devices;
};

struct AmdConfig
{
    std::vector<AmdDevice> devices;
};

struct HardwareConfig
{
    CpuConfig cpu;
    NvidiaConfig nvidia;
    AmdConfig amd;

    uint32_t noncesPerRound;

    void initNonceOffsets(const size_t scratchpadSize)
    {
        uint32_t tmpNoncesPerRound = 0;

        if (cpu.enabled)
        {
            tmpNoncesPerRound += cpu.threadCount;
        }

        for (auto &gpu : nvidia.devices)
        {
            if (gpu.enabled)
            {
                gpu.nonceOffset = noncesPerRound;
                tmpNoncesPerRound += gpu.getNoncesPerRound(scratchpadSize);
            }
        }

        for (auto &gpu : amd.devices)
        {
            if (gpu.enabled)
            {
                gpu.nonceOffset = noncesPerRound;
                tmpNoncesPerRound += gpu.getNoncesPerRound(scratchpadSize);
            }
        }

        noncesPerRound = tmpNoncesPerRound;
    }
};

struct MinerConfig
{
    std::vector<Pool> pools;

    std::string configLocation;

    bool interactive = false;

    HardwareConfig hardwareConfiguration;
};

void to_json(nlohmann::json &j, const MinerConfig &config);

void from_json(const nlohmann::json &j, MinerConfig &config);

std::vector<Constants::OptimizationMethod> getAvailableOptimizations();

Constants::OptimizationMethod getAutoChosenOptimization();

Pool getPool();

std::vector<Pool> getPools();

MinerConfig getConfigInteractively();

MinerConfig getConfigFromJSON(const std::string &configLocation);

MinerConfig getMinerConfig(int argc, char **argv);
