// -*- coding: utf-8 -*-
// Copyright (C) 2011-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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

#include "config.h"
#include <iostream>
#include <spot/tl/match_word.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/tl/contain.hh>

namespace spot
{
  namespace
  {
    // recursive function called by get_vector_formula to recursively fill one
    // list of the result vector.
    // This function makes sure we start with a vector containing only single
    // formulas.

    static bool
    find_list(const std::list<formula>& list, const formula& f)
    {
      spot::language_containment_checker c;
      for (auto iter_list = list.begin(); iter_list != list.end(); ++iter_list)
        {
          if (iter_list->kind() != f.kind())
            continue;
          if (c.equal(*iter_list, f))
            {
              return true;
            }
        }
      return false;
    }

    //fills the lists contained in the vectors used everywhere
    static void
    rec_fill_list(std::list<formula>& list, formula f)
    {
      if (!f.is_literal())
        {
          for (auto iter_f = f.begin(); iter_f != f.end(); ++iter_f)
            rec_fill_list(list, *iter_f);
        }
      else if (f.kind() != op::Not)
        list.push_front(f);
    }

    // creates the vector that will be manipulated from the twa_word
    // is_prefix is used to proccess the prefix or the suffix at will
    std::vector<std::list<formula>>
    get_vector_formula(twa_word_ptr w, bool is_prefix)
    {
      std::vector<std::list<formula>> result;
      std::list<bdd> list_bdd = is_prefix ? w->prefix : w->cycle;
      for (auto iter_list = list_bdd.begin();
          iter_list != list_bdd.end(); ++iter_list)
        {
          std::list<formula> list;
          rec_fill_list(list, bdd_to_formula(*iter_list, w->get_dict()));
          result.push_back(list);
        }
      return result;
    }

    //function called on cycle and prefix
    static auto
    handle_until_loop(std::vector<std::list<formula>>& vec, const formula& f,
        op operation, const formula& f0, const formula& f1)
    {
      auto iter_mark = vec.end();
      //loop around vec
      //we suppose the test for aUb
      for (auto iter_vec = vec.begin(); iter_vec != vec.end(); ++iter_vec)
        {
          if (find_list(*iter_vec, f1))
            {
              if ((operation == op::U || operation == op::W)
                  || find_list(*iter_vec, f0))
                {
                  //if found b, loop from the mark to the current
                  //position and push_front
                  for (; iter_mark != vec.end()
                      && iter_mark != iter_vec; ++iter_mark)
                    iter_mark->push_front(f);
                  iter_vec->push_front(f);
                }
              //relist the mark
              iter_mark = vec.end();
            }
          else if (find_list(*iter_vec, f0))
            {
              //if found a for the first time, list a var to remember
              //if found for another time a, nothing
              if (iter_mark == vec.end())
                iter_mark = iter_vec;
            }
          else
            {
              //if different from a and b, go relist var to vec.end()
              if (iter_mark != vec.end())
                iter_mark = vec.end();
            }
        }
      //test case if condition circles around the vec
      return iter_mark;
    }

    //function to handle U, W, R, M operators
    //just pass the operation by argument
    static void
    handle_until(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, const formula& f, op operation)
    {
      formula f0;
      formula f1;
      if (operation == op::R || operation == op::M)
        {
          f0 = f[1];
          f1 = f[0];
        }
      else
        {
          f0 = f[0];
          f1 = f[1];
        }
      auto mark_cycle = handle_until_loop(cycle, f, operation, f0, f1);
      if (mark_cycle != cycle.end())
        {
          if (find_list(*cycle.begin(), f))
            {
              for (; mark_cycle != cycle.end(); ++mark_cycle)
                mark_cycle->push_front(f);
            }
          else if ((operation == op::W || operation == op::R)
              && find_list(*cycle.begin(), f0))
            {
              for (; mark_cycle != cycle.end(); ++mark_cycle)
                mark_cycle->push_front(f);
            }
        }
      auto mark_pref = handle_until_loop(prefix, f, operation, f0, f1);
      if (mark_pref != prefix.end())
        {
          if (find_list(*cycle.begin(), f))
            {
              for (; mark_pref != prefix.end(); ++mark_pref)
                mark_pref->push_front(f);
            }
        }
    }


    // Function to handle general LTL operator : a -> Ga
    static void
    handle_generally(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, const formula& f)
    {
      auto iter_cycle = cycle.begin();
      for (; iter_cycle != cycle.end()
          && find_list(*iter_cycle, f[0]); ++iter_cycle)
        continue;
      // if all letters in the cycle don't validate Gf, then there are
      // no letters to mark
      if (iter_cycle != cycle.end())
        return;
      for (iter_cycle = cycle.begin(); iter_cycle != cycle.end(); ++iter_cycle)
        iter_cycle->push_front(f);
      // from the end of the prefix, while we have f[0], we can mark them with
      // f
      for (auto riter_pref = prefix.rbegin(); riter_pref != prefix.rend()
          && find_list(*riter_pref, f[0]); ++riter_pref)
        riter_pref->push_front(f);
    }

    // Function to handle eventual LTL operator : a -> Fa
    static void
    handle_eventually(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, const formula& f)
    {
      auto iter_cycle = cycle.begin();
      for (; iter_cycle != cycle.end()
          && !find_list(*iter_cycle, f[0]); ++iter_cycle)
        continue;
      // if there is at least one occurence of a the Fa is true for
      // all the cycle
      if (iter_cycle != cycle.end())
        {
          for (iter_cycle = cycle.begin(); iter_cycle != cycle.end();
              ++iter_cycle)
            iter_cycle->push_front(f);
          for (auto iter_pref = prefix.begin();
              iter_pref != prefix.end(); ++iter_pref)
            iter_pref->push_front(f);
        }
      else
        {
          auto riter_pref = prefix.rbegin();
          for (; riter_pref != prefix.rend()
              && !find_list(*riter_pref, f[0]); ++riter_pref)
            continue;
          // if we find an occurence of a, all letters before it can be marked
          // with Fa, that's why we want to iterate in reverse order
          if (riter_pref != prefix.rend())
            {
              for (; riter_pref != prefix.rend(); ++riter_pref)
                riter_pref->push_front(f);
            }
        }

    }

    //handle Not operator
    //loops through the word and push_front values
    template<typename Lambda>
    static void
    handle_map(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, const formula& f,
        Lambda lambda)
    {
      //loop on the prefix
      for (auto iter_pref = prefix.begin(); iter_pref != prefix.end();
          ++iter_pref)
        {
          if (lambda(*iter_pref, f))
            iter_pref->push_front(f);
        }
      //loop on the cycle
      for (auto iter_cycle = cycle.begin(); iter_cycle != cycle.end();
          ++iter_cycle)
        {
          if (lambda(*iter_cycle, f))
            iter_cycle->push_front(f);
        }
    }

    static void
    handle_next(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, const formula& f)
    {
      if (find_list(cycle[0], f[0]))
        cycle[cycle.size() - 1].push_front(f);
      for (size_t index = 1; index < cycle.size(); ++index)
        {
          if (find_list(cycle[index], f[0]))
            cycle[index - 1].push_front(f);
        }
      if (prefix.size() > 0)
        {
          //test if last letter of prefix should be marked
          if (find_list(cycle[0], f[0]))
            prefix[prefix.size() - 1].push_front(f);
          for (size_t index = prefix.size() - 1; index > 0; --index)
            {
              if (find_list(prefix[index], f[0]))
                prefix[index - 1].push_front(f);
            }
        }
    }


    //Function that evaluates every sub formulas on the word.
    //It will call itself on all the subformulas and then evaluate
    //its formula
    static void
    rec_match(std::vector<std::list<formula>>& prefix,
        std::vector<std::list<formula>>& cycle, formula f)
    {
      //stop case for recursion
      if (f.is_leaf())
        return;

      //recursive calls
      for (auto child = f.begin(); child != f.end(); ++child)
        rec_match(prefix, cycle, *child);

      //evaluation
      switch (f.kind())
        {
        case op::Not:
          handle_map(prefix, cycle, f,
              [](const std::list<formula>& list, const formula& f) -> bool
                {
                  return !find_list(list, f[0]);
                });
          break;

        case op::X:
          handle_next(prefix, cycle, f);
          break;

        case op::F:
          handle_eventually(prefix, cycle, f);
          break;

        case op::G:
          handle_generally(prefix, cycle, f);
          break;

        case op::U:
          handle_until(prefix, cycle, f, op::U);
          break;

        case op::W:
          handle_until(prefix, cycle, f, op::W);
          break;

        case op::M:
          handle_until(prefix, cycle, f, op::M);
          break;

        case op::R:
          handle_until(prefix, cycle, f, op::R);
          break;

        case op::Or:
          handle_map(prefix, cycle, f,
              [](const std::list<formula>& list, const formula& f) -> bool
                {
                  for (auto it_f = f.begin(); it_f != f.end(); ++it_f)
                    {
                      if (find_list(list, *it_f))
                        return true;
                    }
                  return false;
                });
          break;

        case op::And:
          handle_map(prefix, cycle, f,
              [](const std::list<formula>& list, const formula& f) -> bool
                {
                  for (auto it_f = f.begin(); it_f != f.end(); ++it_f)
                    {
                      if (!find_list(list, *it_f))
                        return false;
                    }
                  return true;
                });
          break;

        case op::tt:
          handle_map(prefix, cycle, f,
              [](const std::list<formula>& list, const formula& f) -> bool
                {
                  (void)list;
                  (void)f;
                  return true;
                });
          break;

        case op::ff:
          handle_map(prefix, cycle, f,
              [](const std::list<formula>& list, const formula& f) -> bool
                {
                  (void)list;
                  (void)f;
                  return false;
                });
          break;

        default:
          std::cerr << "Alert ! one operator isn't implemented" << std::endl;
        }
    }
  }


  //This function takes a formula and a word. Returns true if the word
  //corresponds to the formula.
  //It works by transforming the twa_word into two vectors of lists of
  //formulas.
  //
  bool
  match_word(formula f, twa_word_ptr w)
  {
    // transformation of the twa_word into some datastructure easier to use.
    std::vector<std::list<formula>> prefix = get_vector_formula(w, true);
    std::vector<std::list<formula>> cycle = get_vector_formula(w, false);

    // runs the recursive function that will proccess the vectors.
    rec_match(prefix, cycle, f);

    // Returns true if first letter of the prefix is recognized or if
    // the prefix is empty, applies on first letter of the cycle.
    if (prefix.size() > 0)
      {
        return find_list(prefix[0], f);
      }
    return find_list(cycle[0], f);
  }
}