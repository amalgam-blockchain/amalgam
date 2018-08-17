#pragma once

#include <amalgam/account_statistics/account_statistics_plugin.hpp>

#include <fc/api.hpp>

namespace amalgam{ namespace app {
   struct api_context;
} }

namespace amalgam { namespace account_statistics {

namespace detail
{
   class account_statistics_api_impl;
}

class account_statistics_api
{
   public:
      account_statistics_api( const amalgam::app::api_context& ctx );

      void on_api_startup();

   private:
      std::shared_ptr< detail::account_statistics_api_impl > _my;
};

} } // amalgam::account_statistics

FC_API( amalgam::account_statistics::account_statistics_api, )