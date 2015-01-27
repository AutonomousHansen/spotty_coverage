// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2014 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

#include <iostream>
#include "ta/ta.hh"
#include "stats.hh"
#include "reachiter.hh"

namespace spot
{
  namespace
  {
    class stats_bfs : public ta_reachable_iterator_breadth_first
    {
    public:
      stats_bfs(const const_ta_ptr a, ta_statistics& s) :
        ta_reachable_iterator_breadth_first(a), s_(s)
      {
      }

      void
      process_state(const state* s, int id)
      {
	// FIXME: to be consistent with previous code
	// only "REAL" initial states must be counted. In this implementation
	// there is ALWAYS one single artificial state
	if (t_automata_->get_artificial_initial_state() == s)
	  {
	    artificial_ = id;
	    return;
	  }
        ++s_.states;
        if (t_automata_->is_accepting_state(s)
            || t_automata_->is_livelock_accepting_state(s))
          ++s_.acceptance_states;
      }

      void
      process_link(int src, int, const ta_succ_iterator*)
      {
	if (artificial_ == src)
	  return;
        ++s_.transitions;
      }

    private:
      ta_statistics& s_;
      int artificial_;
      std::unordered_set<int> real_initials_;
    };
  } // anonymous


  std::ostream&
  ta_statistics::dump(std::ostream& out) const
  {
    out << "transitions: " << transitions << std::endl;
    out << "states: " << states << std::endl;
    return out;
  }

  ta_statistics
  stats_reachable(const const_ta_ptr& t)
  {
    ta_statistics s =
      { 0, 0, 0};
    stats_bfs d(t, s);
    d.run();
    return s;
  }
}
