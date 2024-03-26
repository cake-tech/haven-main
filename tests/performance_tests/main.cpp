// Copyright (c) 2014-2022, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include <boost/regex.hpp>

#include "common/util.h"
#include "common/command_line.h"
#include "performance_tests.h"
#include "performance_utils.h"

// tests
#include "construct_tx.h"
#include "check_tx_signature.h"
#include "check_hash.h"
#include "cn_slow_hash.h"
#include "derive_public_key.h"
#include "derive_secret_key.h"
#include "derive_view_tag.h"
#include "ge_frombytes_vartime.h"
#include "ge_tobytes.h"
#include "generate_key_derivation.h"
#include "generate_key_image.h"
#include "generate_key_image_helper.h"
#include "generate_keypair.h"
#include "signature.h"
#include "is_out_to_acc.h"
#include "out_can_be_to_acc.h"
#include "subaddress_expand.h"
#include "sc_reduce32.h"
#include "sc_check.h"
#include "cn_fast_hash.h"
#include "rct_mlsag.h"
#include "equality.h"
#include "range_proof.h"
#include "bulletproof.h"
#include "bulletproof_plus.h"
#include "crypto_ops.h"
#include "multiexp.h"
#include "sig_mlsag.h"
#include "sig_clsag.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  TRY_ENTRY();
  tools::on_startup();
  set_process_affinity(1);
  set_thread_high_priority();

  mlog_configure(mlog_get_default_log_path("performance_tests.log"), true);

  po::options_description desc_options("Command line options");
  const command_line::arg_descriptor<std::string> arg_filter = { "filter", "Regular expression filter for which tests to run" };
  const command_line::arg_descriptor<bool> arg_verbose = { "verbose", "Verbose output", false };
  const command_line::arg_descriptor<bool> arg_stats = { "stats", "Including statistics (min/median)", false };
  const command_line::arg_descriptor<unsigned> arg_loop_multiplier = { "loop-multiplier", "Run for that many times more loops", 1 };
  const command_line::arg_descriptor<std::string> arg_timings_database = { "timings-database", "Keep timings history in a file" };
  command_line::add_arg(desc_options, arg_filter);
  command_line::add_arg(desc_options, arg_verbose);
  command_line::add_arg(desc_options, arg_stats);
  command_line::add_arg(desc_options, arg_loop_multiplier);
  command_line::add_arg(desc_options, arg_timings_database);

  po::variables_map vm;
  bool r = command_line::handle_error_helper(desc_options, [&]()
  {
    po::store(po::parse_command_line(argc, argv, desc_options), vm);
    po::notify(vm);
    return true;
  });
  if (!r)
    return 1;

  const std::string filter = tools::glob_to_regex(command_line::get_arg(vm, arg_filter));
  const std::string timings_database = command_line::get_arg(vm, arg_timings_database);
  Params p;
  if (!timings_database.empty())
    p.td = TimingsDatabase(timings_database);
  p.verbose = command_line::get_arg(vm, arg_verbose);
  p.stats = command_line::get_arg(vm, arg_stats);
  p.loop_multiplier = command_line::get_arg(vm, arg_loop_multiplier);

  performance_timer timer;
  timer.start();
  
  std::cout << "Tests finished. Elapsed time: " << timer.elapsed_ms() / 1000 << " sec" << std::endl;

  return 0;
  CATCH_ENTRY_L0("main", 1);
}
