// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2014 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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

#include "ltlast/atomic_prop.hh"
#include "ltlast/constant.hh"
#include "tgtaexplicit.hh"
#include "tgba/formula2bdd.hh"
#include "ltlvisit/tostring.hh"

#include "tgba/bddprint.hh"

namespace spot
{

  tgta_explicit::tgta_explicit(const const_tgba_ptr& tgba,
			       unsigned n_acc,
			       ta_graph_state* artificial_initial_state) :
    tgta(tgba->get_dict()),
    ta_(make_ta_explicit(tgba, n_acc, artificial_initial_state))
  {
  }

  state*
  tgta_explicit::get_init_state() const
  {
    assert(false);
    return nullptr;
      // FIXME
      //ta_->get_artificial_initial_state();
  }

  tgba_succ_iterator*
  tgta_explicit::succ_iter(const spot::state* state) const
  {
    return ta_->succ_iter(state);
  }

  bdd
  tgta_explicit::compute_support_conditions(const spot::state* in) const
  {
    const ta_graph_state* s = down_cast<const ta_graph_state*>(in);
    assert(s);
    return ta_->get_tgba()->support_conditions(s->get_tgba_state());
  }

  bdd_dict_ptr
  tgta_explicit::get_dict() const
  {
    return ta_->get_dict();
  }

  std::string
  tgta_explicit::format_state(const spot::state* s) const
  {
    return ta_->format_state(s);
  }

  spot::tgba_succ_iterator*
  tgta_explicit::succ_iter_by_changeset(const spot::state* s, bdd /* FIXME chngset*/ ) const
  {
    return ta_->succ_iter(s/*, chngset*/);
  }

}
