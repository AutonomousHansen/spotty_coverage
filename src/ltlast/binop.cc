#include <cassert>
#include <utility>
#include "binop.hh"
#include "visitor.hh"

namespace spot
{
  namespace ltl
  {
    binop::binop(type op, formula* first, formula* second)
      : op_(op), first_(first), second_(second)
    {
    }

    binop::~binop()
    {
      // Get this instance out of the instance map.
      pairf pf(first(), second());
      pair p(op(), pf);
      map::iterator i = instances.find(p);
      assert (i != instances.end());
      instances.erase(i);
    }

    void
    binop::accept(visitor& v)
    {
      v.visit(this);
    }

    void
    binop::accept(const_visitor& v) const
    {
      v.visit(this);
    }

    const formula*
    binop::first() const
    {
      return first_;
    }

    formula*
    binop::first()
    {
      return first_;
    }

    const formula*
    binop::second() const
    {
      return second_;
    }

    formula*
    binop::second()
    {
      return second_;
    }

    binop::type
    binop::op() const
    {
      return op_;
    }

    const char*
    binop::op_name() const
    {
      switch (op_)
	{
	case Xor:
	  return "Xor";
	case Implies:
	  return "Implies";
	case Equiv:
	  return "Equiv";
	case U:
	  return "U";
	case R:
	  return "R";
	}
      // Unreachable code.
      assert(0);
      return 0;
    }

    binop::map binop::instances;

    binop*
    binop::instance(type op, formula* first, formula* second)
    {
      // Sort the operands of associative operators, so that for
      // example the formula instance for 'a xor b' is the same as
      // that for 'b xor a'.
      switch (op)
	{
	case Xor:
	case Equiv:
	  if (second < first)
	    std::swap(first, second);
	  break;
	case Implies:
	case U:
	case R:
	  // Non associative operators.
	  break;
	}

      pairf pf(first, second);
      pair p(op, pf);
      map::iterator i = instances.find(p);
      if (i != instances.end())
	{
	  return static_cast<binop*>(i->second->ref());
	}
      binop* ap = new binop(op, first, second);
      instances[p] = ap;
      return static_cast<binop*>(ap->ref());
    }

    unsigned
    binop::instance_count()
    {
      return instances.size();
    }
  }
}
