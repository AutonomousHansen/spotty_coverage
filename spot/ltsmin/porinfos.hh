// -*- coding: utf-8 -*-
// Copyright (C)  2015, 2017 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE)
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <vector>
#include <unordered_set>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <set>
#include <assert.h>
#include <spot/ltsmin/spins.hh>
#include <spot/misc/common.hh>

namespace spot
{
  // \brief Store statistics about partial order
  class SPOT_API porinfos_stats
  {
  public:
    porinfos_stats();

    void cumul(unsigned red, unsigned en);

    void dump();

  private:
    unsigned red_;
    unsigned en_;
    unsigned cumul_;
    unsigned min_red_;
    unsigned max_red_;
    unsigned min_en_;
    unsigned max_en_;
  };

  // \brief Store configuration informations for partial order
  class SPOT_API porinfos_options
  {
  public:
    enum class porinfos_method { stubborn_set, base };

    porinfos_options(porinfos_method method = porinfos_method::base)
      : method_(method)
    {
    }

    porinfos_method method_;
  };

  // \brief Store informations about partial order
  class SPOT_API porinfos
  {
  public:
    porinfos(const spins_interface* si);

    porinfos(const spins_interface* si, const porinfos_options& opt);

    std::vector<bool> compute_reduced_set(const std::vector<int>& enabled,
                                          const int* for_spins_state);

    inline bool non_maybecoenabled(int t1, int t2);

    int transitions();

    int variables();

    void dump_read_dependency();

    void dump_write_dependency();

    void dump_nes_guards();

    void dump_mbc_guards();

    porinfos_stats& stats();

    void spin_fashion()
    {
      spin_ = true;
    }

  private:
    const porinfos_options opt;

    bool base(int t, std::vector<int>& t_work,
              const std::vector<int>& t_s,
              const std::vector<int>& enabled,
              const int* for_spins_state);

    bool stubborn_set(int t, std::vector<int>& t_work,
                      const std::vector<int>& t_s,
                      const std::vector<int>& enabled,
                      const int* for_spins_state);

    const spins_interface* d_;
    unsigned transitions_;
    unsigned variables_;
    unsigned guards_;
    porinfos_stats stats_;
    std::vector<std::vector<bool>> m_read;
    std::vector<std::vector<bool>> m_write;
    std::vector<std::vector<bool>> m_nes;
    std::vector<std::vector<bool>> m_mbc;
    std::vector<std::vector<int>>  m_guards;
    std::vector<std::vector<bool>> m_guard_variables;

    // Develop caches to reduce computation time
    std::vector<std::vector<bool>> m_dep_tr;
    std::vector<std::vector<bool>> m_dep_state;
    std::vector<std::vector<bool>> m_dep_process;
    std::vector<std::vector<bool>> m_conflict_tr;
    std::vector<std::vector<bool>> non_mbc_tr;
    std::vector<std::set<unsigned>> t_processes;
    bool spin_ = false;

    bool (porinfos::*f_not_enabled_transition)
      (int, std::vector<int>&, const std::vector<int>&, const std::vector<int>&,
       const int*);
  };
}