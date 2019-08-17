// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <string>

namespace Config
{
    class Config
    {
      public:
        Config() {};

        std::string optimizationMethod;
    };

    extern Config config;
}
