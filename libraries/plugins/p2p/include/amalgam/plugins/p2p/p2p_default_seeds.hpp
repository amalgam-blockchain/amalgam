#pragma once

#include <vector>

namespace amalgam{ namespace plugins { namespace p2p {

#ifdef IS_TEST_NET
const std::vector< std::string > default_seeds;
#else
const std::vector< std::string > default_seeds = {
   "seed-rh.amalgam.money:2001",
   "seed-ny.amalgam.money:2001",
   "seed-tor.amalgam.money:2001"
};
#endif

} } } // amalgam::plugins::p2p
