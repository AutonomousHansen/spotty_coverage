// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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


#ifndef SPOT_MISC_FREELIST_HH
# define SPOT_MISC_FREELIST_HH

#include <list>
#include <utility>

namespace spot
{

  /// Manage list of free integers.
  class free_list
  {
  public:
    virtual ~free_list();

    /// \brief Find \a n consecutive integers.
    ///
    /// Browse the list of free integers until \a n consecutive
    /// integers are found.  Extend the list (using extend()) otherwise.
    /// \return the first integer of the range
    int register_n(int n);

    /// Release \a n consecutive integers starting at \a base.
    void release_n(int base, int n);

    /// Dump the list to \a os for debugging.
    std::ostream& dump_free_list(std::ostream& os) const;
  protected:

    /// Allocate \a n integer.
    ///
    /// This function is called by register_n() when the free list is
    /// empty or if \a n consecutive integers could not be found.  It
    /// should allocate more integers, possibly changing the list, and
    /// return the first integer on a range of n consecutive integer
    /// requested by the user.
    virtual int extend(int n) = 0;

    /// Such pairs describe \c second free integer starting at \first.
    typedef std::pair<int, int> pos_lenght_pair;
    typedef std::list<pos_lenght_pair> free_list_type;
    free_list_type fl; ///< Tracks unused BDD variables.
  };

}

#endif // SPOT_MISC_FREELIST_HH
