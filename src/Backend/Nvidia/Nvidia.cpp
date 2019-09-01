// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////
#include "Backend/Nvidia/Nvidia.h"
//////////////////////////////////

#include <iostream>

#include "ArgonVariants/Variants.h"
#include "Backend/Nvidia/NvidiaHash.h"
#include "Utilities/ColouredMsg.h"
#include "Nvidia/Argon2.h"

Nvidia::Nvidia(
    const HardwareConfig &hardwareConfig,
    const std::function<void(const JobSubmit &jobSubmit)> &submitHashCallback):
    m_hardwareConfig(hardwareConfig),
    m_submitHash(submitHashCallback)
{
    std::copy_if(
        hardwareConfig.nvidia.devices.begin(),
        hardwareConfig.nvidia.devices.end(),
        std::back_inserter(m_availableDevices),
        [](const NvidiaDevice &device)
        {
            return device.enabled;
        }
    );

    m_numAvailableGPUs = m_availableDevices.size();
}

void Nvidia::start(const Job &job, const uint32_t initialNonce)
{
    if (!m_threads.empty())
    {
        stop();
    }

    m_shouldStop = false;

    m_nonce = initialNonce;

    m_currentJob = job;

    /* Indicate that there's no new jobs available to other threads */
    m_newJobAvailable = std::vector<bool>(m_numAvailableGPUs, false);

    for (uint32_t i = 0; i < m_numAvailableGPUs; i++)
    {
        m_threads.push_back(std::thread(&Nvidia::hash, this, m_availableDevices[i], i));
    }
}

void Nvidia::stop()
{
    m_shouldStop = true;

    for (int i = 0; i < m_numAvailableGPUs; i++)
    {
        m_newJobAvailable[i] = true;
    }

    /* Wait for all the threads to stop */
    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    /* Empty the threads vector for later re-creation */
    m_threads.clear();
}

void Nvidia::setNewJob(const Job &job, const uint32_t initialNonce)
{
    /* Set new nonce */
    m_nonce = initialNonce;

    /* Update stored job */
    m_currentJob = job;

    /* Indicate to each thread that there's a new job */
    for (int i = 0; i < m_numAvailableGPUs; i++)
    {
        m_newJobAvailable[i] = true;
    }
}

std::vector<PerformanceStats> Nvidia::getPerformanceStats()
{
    return {};
}

std::shared_ptr<NvidiaHash> getNvidiaMiningAlgorithm(const std::string &algorithm)
{
    switch(ArgonVariant::algorithmNameToCanonical(algorithm))
    {
        case ArgonVariant::Chukwa:
        {
            return std::make_shared<NvidiaHash>(512, 3);
        }
        case ArgonVariant::ChukwaWrkz:
        {
            return std::make_shared<NvidiaHash>(256, 4);
        }
        default:
        {
            throw std::runtime_error("Developer fucked up. Sorry!");
        }
    }
}

void Nvidia::hash(const NvidiaDevice gpu, const uint32_t threadNumber)
{
    const size_t THREADS = 256;

    uint64_t *grids = allocateScratchpads();
    uint8_t *results = allocateResults();

    while (!m_shouldStop)
    {
        /* Offset the nonce by our thread number so each thread has an individual
           nonce */
        uint32_t localNonce = m_nonce + (threadNumber * THREADS);

        Job job = m_currentJob;

        const bool isNiceHash = job.isNiceHash;

        auto algorithm = getNvidiaMiningAlgorithm(m_currentJob.algorithm);

        /* Let the algorithm perform any necessary initialization */
        algorithm->init(m_currentJob.rawBlob, gpu);

        while (!m_newJobAvailable[threadNumber])
        {
            try
            {
                const auto hashes = algorithm->hash(job.rawBlob, localNonce, grids, results);

                for (int i = 0; i < THREADS; i++)
                {
                    m_submitHash({ &hashes[i * 32], job.jobID, localNonce + i, job.target });
                }

                localNonce += (m_numAvailableGPUs * THREADS);
            }
            catch (const std::exception &e)
            {
                std::cout << WarningMsg("Caught unexpected error from GPU hasher: " + std::string(e.what())) << std::endl;
            }
        }

        /* Switch to new job. */
        m_newJobAvailable[threadNumber] = false;
    }

    freeMemory(grids, results);
}
