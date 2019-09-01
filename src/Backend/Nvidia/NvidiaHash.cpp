// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////////
#include "Backend/Nvidia/NvidiaHash.h"
//////////////////////////////////////

#include "Nvidia/Argon2.h"
#include "Config/Config.h"

/* Salt is not altered by nonce. We can initialize it once per job here. */
void NvidiaHash::init(const std::vector<uint8_t> &input, const NvidiaDevice &gpu)
{
    m_salt = std::vector<uint8_t>(input.begin(), input.begin() + 16);
    m_gpu = gpu;
}

std::vector<uint8_t> NvidiaHash::hash(
    std::vector<uint8_t> &input,
    const uint32_t localNonce,
    uint64_t *grids,
    uint8_t *results)
{
    return nvidiaHash(
        input,
        m_salt,
        m_memory,
        m_time,
        m_gpu.id,
        localNonce,
        grids,
        results
    ); 
}

NvidiaHash::NvidiaHash(
    const uint32_t memoryKB,
    const uint32_t iterations):
    m_memory(memoryKB),
    m_time(iterations)
{
}
