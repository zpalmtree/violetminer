// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>

#include "Config/Config.h"
#include "Config/Constants.h"
#include "MinerManager/MinerManager.h"
#include "PoolCommunication/PoolCommunication.h"
#include "Types/Pool.h"
#include "Utilities/ColouredMsg.h"
#include "ArgonVariants/Variants.h"
#include "Miner/GetConfig.h"

#if defined(X86_OPTIMIZATIONS)
#include "cpu_features/include/cpuinfo_x86.h"
#endif

std::vector<Pool> getDevPools()
{
    std::vector<Pool> pools;

    Pool pool1;
    pool1.host = "donate.futuregadget.xyz";
    pool1.port = 3333;
    pool1.username = "donate";
    pool1.algorithm = "turtlecoin";
    pool1.niceHash = true;
    pool1.algorithmGenerator = ArgonVariant::Algorithms[pool1.algorithm];

    pools.push_back(pool1);

    return pools;
}

void printWelcomeHeader(MinerConfig config)
{
    std::cout << InformationMsg("* ") << WhiteMsg("ABOUT", 25) << InformationMsg("violetminer " + Constants::VERSION) << std::endl;
    std::cout << InformationMsg("* ") << WhiteMsg("THREADS", 25) << InformationMsg(config.threadCount) << std::endl;

#if defined(NO_OPTIMIZATIONS)
    std::cout << InformationMsg("* ") << WhiteMsg("OPTIMIZATION SUPPORT", 25) << WarningMsg("No optimizations available on this platform");
#elif defined(X86_OPTIMIZATIONS)

    static const cpu_features::X86Features features = cpu_features::GetX86Info().features;

    std::vector<std::tuple<std::string, bool>> availableOptimizations = {
        { "AVX-512", features.avx512f },
        { "AVX-2", features.avx2 },
        { "SSE3", features.sse3 },
        { "SSE2", features.sse2 }
    };

    std::cout << InformationMsg("* ") << WhiteMsg("OPTIMIZATION SUPPORT", 25);

    for (const auto &[optimization, enabled] : availableOptimizations)
    {
        if (enabled)
        {
            std::cout << SuccessMsg(optimization + " ");
        }
        else
        {
            std::cout << WarningMsg(optimization + " ");
        }
    }
#endif

    std::cout << std::endl << InformationMsg("* ") << WhiteMsg("CHOSEN OPTIMIZATION", 25);
    
    if (config.optimizationMethod != "None")
    {
        std::cout << SuccessMsg(config.optimizationMethod) << std::endl;
    }
    else
    {
        std::cout << WarningMsg("None") << std::endl;
    }

    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    /* Get the pools, algorithm, etc from the user in some way */
    MinerConfig config = getMinerConfig(argc, argv);

    /* Set the global config */
    Config::config.optimizationMethod = config.optimizationMethod;

    /* If not initial startup, print welcome header */
    if (!config.interactive)
    {
        printWelcomeHeader(config);
    }

    const auto userPoolManager = std::make_shared<PoolCommunication>(config.pools);

    /* Get the dev pools */
    std::vector<Pool> devPools = getDevPools();

    const auto devPoolManager = std::make_shared<PoolCommunication>(devPools);

    /* Setup a manager for the user pools and the dev pools */
    MinerManager userMinerManager(userPoolManager, config.threadCount);
    MinerManager devMinerManager(devPoolManager, config.threadCount);

    /* A cycle lasts 100 minutes */
    const auto cycleLength = std::chrono::minutes(100);

    /* We mine for the dev for DEV_FEE_PERCENT off the 100 minutes */
    const auto devMiningTime = std::chrono::seconds(static_cast<uint8_t>(60 * Constants::DEV_FEE_PERCENT));

    /* We mine for the user for the rest of the time */
    const auto userMiningTime = cycleLength - devMiningTime;

    if (Constants::DEV_FEE_PERCENT == 0)
    {
        std::cout << WarningMsg("Dev fee disabled :( Consider making a one off donation to TRTLv1c5XpYGdwHbcU94foRojzLiz3pQ4AJN6swsy514QrydgvYPCKhDZPt61JG5eVGJKrHsXJHSUHDmAhZ134q8QRN2kJwUbtB") << std::endl;

        /* No dev fee, just start the users mining */
        userMinerManager.start();

        /* Wait forever */
        std::promise<void>().get_future().wait();
    }
    else
    {
        /* 100 minute rounds, alternating between users pool and devs pool */
        while (true)
        {
            /* Start mining for the user */
            userMinerManager.start();

            /* Mine for userMiningTime minutes */
            std::this_thread::sleep_for(userMiningTime);

            /* Stop mining for the user */
            userMinerManager.stop();

            std::cout << InformationMsg("=== Started mining to the development pool - Thank you for supporting violetminer! ===") << std::endl;
            std::cout << InformationMsg("=== This will last for " + std::to_string(devMiningTime.count()) + " seconds. (Every 100 minutes) ===") << std::endl;

            /* Start mining for the dev */
            devMinerManager.start();

            /* Mine for devMiningTime seconds */
            std::this_thread::sleep_for(devMiningTime);

            /* Stop mining for the dev. */
            devMinerManager.stop();

            std::cout << InformationMsg("=== Regular mining resumed. Thank you for supporting violetminer! ===") << std::endl;
        }
    }
}
