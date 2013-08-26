// -*- coding: utf-8 -*-
// Copyright (C) 2013 Laboratoire de Recherche et Développement
// de l'Epita.
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

#ifndef SPOT_TGBAALGOS_DTGBASAT_HH
# define SPOT_TGBAALGOS_DTGBASAT_HH

#include <iosfwd>
#include "tgba/tgba.hh"
#include "tgba/tgbaexplicit.hh"

namespace spot
{
  /// \brief Attempt to reduce a deterministic TGBA with a SAT solver.
  ///
  /// \param a the TGBA to reduce.  It should have only one acceptance
  ///  set and be deterministic.  I.e., it should be a deterministic TBA.
  ///
  /// \param cand_nacc is the number of acceptance sets in the result.
  ///
  /// This functions attempts to find the smallest TGBA with \a
  /// cand_nacc acceptance sets that is equivalent to \a a.  If no
  /// smaller TGBA is found, a null pointer is returned.
  SPOT_API tgba_explicit_number*
  dtgba_sat_minimize(const tgba* a, unsigned cand_nacc);
}

#endif // SPOT_TGBAALGOS_DTGBASAT_HH
