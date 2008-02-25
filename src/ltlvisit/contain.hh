// Copyright (C) 2006 Laboratoire d'Informatique de Paris 6 (LIP6),
// d�partement Syst�mes R�partis Coop�ratifs (SRC), Universit� Pierre
// et Marie Curie.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef SPOT_LTLVISIT_CONTAIN_HH
# define SPOT_LTLVISIT_CONTAIN_HH

#include "ltlast/formula.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "misc/hash.hh"
#include <set>

namespace spot
{
  namespace ltl
  {
    // Check containment of language represented by LTL formulae.
    class language_containment_checker
    {
      struct record_
      {
	const tgba* translation;
	typedef std::set<const record_*> incomp_map;
	incomp_map incompatible;
      };
      typedef Sgi::hash_map<const formula*,
			    record_, formula_ptr_hash> trans_map;
    public:
      /// This class uses spot::ltl_to_tgba_fm to translate LTL
      /// formulae.  See that class for the meaning of these options.
      language_containment_checker(bdd_dict* dict, bool exprop,
				   bool symb_merge,
				   bool branching_postponement,
				   bool fair_loop_approx);

      ~language_containment_checker();

      /// Check whether L(l) is a subset of L(g).
      bool contained(const formula* l, const formula* g);

      /// Check whether L(l) = L(g).
      bool equal(const formula* l, const formula* g);

    protected:
      const record_* register_formula_(const formula* f);

      /* Translation options */
      bdd_dict* dict_;
      bool exprop_;
      bool symb_merge_;
      bool branching_postponement_;
      bool fair_loop_approx_;
      /* Translation Maps */
      trans_map translated_;
    };
  }
}

#endif // SPOT_LTLVISIT_CONTAIN_HH
