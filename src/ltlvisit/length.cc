// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "length.hh"
#include "ltlast/allnodes.hh"
#include "ltlast/visitor.hh"

namespace spot
{
  namespace ltl
  {
    namespace
    {
      class length_visitor: public const_visitor
      {
      public:

	length_visitor()
	{
	  result_ = 0;
	}

	virtual
	~length_visitor()
	{
	}

	int
	result() const
	{
	  return result_;
	}

	void
	visit(const atomic_prop*)
	{
	  result_ = 1;
	}

	void
	visit(const constant*)
	{
	  result_ = 1;
	}

	void
	visit(const unop* uo)
	{
	  result_ = 1 + length(uo->child());
	}

	void
	visit(const binop* bo)
	{
	  result_ = 1 + length(bo->first()) + length(bo->second());
	}

	void
	visit(const multop* mo)
	{
	  unsigned mos = mo->size();
	  ++result_;
	  for (unsigned i = 0; i < mos; ++i)
	    result_ += length(mo->nth(i));
	}

      protected:
	int result_; // size of the formula
      };
    }

    int
    length(const formula* f)
    {
      length_visitor v;
      const_cast<formula*>(f)->accept(v);
      return v.result();
    }

  }
}
