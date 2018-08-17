#pragma once

#include <amalgam/protocol/base.hpp>

#include <amalgam/follow/follow_plugin.hpp>

namespace amalgam { namespace follow {

using namespace std;
using amalgam::protocol::base_operation;

struct follow_operation : base_operation
{
    account_name_type follower;
    account_name_type following;
    set< string >     what; /// blog, mute

    void validate()const;

    void get_required_posting_authorities( flat_set<account_name_type>& a )const { a.insert( follower ); }
};

struct reblog_operation : base_operation
{
   account_name_type account;
   account_name_type author;
   string            permlink;

   void validate()const;

   void get_required_posting_authorities( flat_set<account_name_type>& a )const { a.insert( account ); }
};

typedef fc::static_variant<
         follow_operation,
         reblog_operation
      > follow_plugin_operation;

DEFINE_PLUGIN_EVALUATOR( follow_plugin, follow_plugin_operation, follow );
DEFINE_PLUGIN_EVALUATOR( follow_plugin, follow_plugin_operation, reblog );

} } // amalgam::follow

FC_REFLECT( amalgam::follow::follow_operation, (follower)(following)(what) )
FC_REFLECT( amalgam::follow::reblog_operation, (account)(author)(permlink) )

DECLARE_OPERATION_TYPE( amalgam::follow::follow_plugin_operation )

FC_REFLECT_TYPENAME( amalgam::follow::follow_plugin_operation )
