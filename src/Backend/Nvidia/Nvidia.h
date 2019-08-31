// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include "Backend/IBackend.h"

class Nvidia : virtual public IBackend
{
  public:
    Nvidia();

    virtual void start(const Job &job);

    virtual void stop();

    virtual void setNewJob(const Job &job);

    virtual std::vector<PerformanceStats> getPerformanceStats();

  private:
};
