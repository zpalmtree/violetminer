// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include "Argon2/Argon2.h"
#include "Types/IHashingAlgorithm.h"
#include "Miner/GetConfig.h"

class NvidiaHash
{
  public:
    NvidiaHash(
        const uint32_t memoryKB,
        const uint32_t iterations);

    void init(const std::vector<uint8_t> &initialInput, const NvidiaDevice &gpu);

    std::vector<uint8_t> hash(
        std::vector<uint8_t> &input,
        const uint32_t localNonce,
        uint64_t *grids,
        uint8_t *threads);

  private:

    const uint32_t m_memory;
    const uint32_t m_time;

    std::vector<uint8_t> m_salt;

    NvidiaDevice m_gpu;
};
