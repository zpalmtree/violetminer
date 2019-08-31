// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>
#include <random>
#include <thread>

#include "MinerManager/HashManager.h"
#include "Miner/GetConfig.h"
#include "PoolCommunication/PoolCommunication.h"
#include "Types/IHashingAlgorithm.h"

class MinerManager
{
  public:
    /* CONSTRUCTOR */
    MinerManager(
        const std::shared_ptr<PoolCommunication> pool,
        const HardwareConfig hardwareConfig);

    /* DESTRUCTOR */
    ~MinerManager();

    /* PUBLIC METHODS */
    void start();

    void stop();

    void printStats();

  private:

    /* PRIVATE METHODS */
    void hash(uint32_t threadNumber);

    void setNewJob(const Job &job);

    void pauseMining();

    void resumeMining();

    void startNvidiaMining();

    void statPrinter();

    /* PRIVATE VARIABLES */
    std::vector<std::thread> m_threads;

    /* Should we stop the worker funcs */
    std::atomic<bool> m_shouldStop = false;

    /* Threads to launch, whether CPU/GPU is enabled, etc */
    const HardwareConfig m_hardwareConfig;

    /* Pool connection */
    const std::shared_ptr<PoolCommunication> m_pool;

    /* Handles submitting shares and tracking hashrate statistics */
    HashManager m_hashManager;

    /* Current job to be working on */
    Job m_currentJob;

    /* Nonce to begin hashing at */
    uint32_t m_nonce;

    /* Handles creating random nonces */
    std::random_device m_device;

    std::mt19937 m_gen;

    std::uniform_int_distribution<uint32_t> m_distribution {0, std::numeric_limits<uint32_t>::max()};

    /* A bool for each thread indicating if they should swap to a new job */
    std::vector<bool> m_newJobAvailable;

    /* Thread that periodically prints hashrate, etc */
    std::thread m_statsThread;
};
