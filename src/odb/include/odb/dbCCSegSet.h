// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2025, The OpenROAD Authors

#pragma once

#include "dbObject.h"
#include "dbSet.h"

namespace odb {

class dbCCSeg;

template <>
class dbSetIterator<dbCCSeg>
{
  friend class dbSet<dbCCSeg>;

  dbIterator* _itr;
  uint _cur;
  uint _pid;

  dbSetIterator(dbIterator* itr, uint id, uint pid)
  {
    _itr = itr;
    _cur = id;
    _pid = pid;
  }

 public:
  using value_type = dbCCSeg*;
  using difference_type = std::ptrdiff_t;
  using pointer = dbCCSeg**;
  using reference = dbCCSeg*&;
  using iterator_category = std::input_iterator_tag;

  dbSetIterator()
  {
    _itr = nullptr;
    _cur = 0;
    _pid = 0;
  }

  dbSetIterator(const dbSetIterator& it)
  {
    _itr = it._itr;
    _cur = it._cur;
    _pid = it._pid;
  }

  bool operator==(const dbSetIterator<dbCCSeg>& it)
  {
    return (_itr == it._itr) && (_cur == it._cur) && (_pid == it._pid);
  }

  bool operator!=(const dbSetIterator<dbCCSeg>& it)
  {
    return (_itr != it._itr) || (_cur != it._cur) || (_pid != it._pid);
  }

  dbCCSeg* operator*() { return (dbCCSeg*) _itr->getObject(_cur); }

  dbCCSeg* operator->() { return (dbCCSeg*) _itr->getObject(_cur); }

  dbSetIterator<dbCCSeg>& operator++()
  {
    _cur = _itr->next(_cur, _pid);
    return *this;
  }

  dbSetIterator<dbCCSeg> operator++(int)
  {
    dbSetIterator it(*this);
    _cur = _itr->next(_cur, _pid);
    return it;
  }
};

template <>
class dbSet<dbCCSeg>
{
  dbIterator* _itr;
  dbObject* _parent;
  uint _pid;

 public:
  using iterator = dbSetIterator<dbCCSeg>;

  dbSet()
  {
    _itr = nullptr;
    _parent = nullptr;
    _pid = 0;
  }

  dbSet(dbObject* parent, dbIterator* itr)
  {
    _parent = parent;
    _itr = itr;
    _pid = parent->getId();
  }

  dbSet(const dbSet<dbCCSeg>& c)
  {
    _itr = c._itr;
    _parent = c._parent;
    _pid = c._pid;
  }

  ///
  /// Returns the number of items in this set.
  ///
  uint size() { return _itr->size(_parent); }

  ///
  /// Return a begin() iterator.
  ///
  iterator begin() { return iterator(_itr, _itr->begin(_parent), _pid); }

  ///
  /// Return an end() iterator.
  ///
  iterator end() { return iterator(_itr, _itr->end(_parent), _pid); }

  ///
  /// Returns the maximum number sequential elements the this set
  /// may iterate.
  ///
  uint sequential() { return _itr->sequential(); }

  ///
  /// Returns true if this set is reversible.
  ///
  bool reversible() { return _itr->reversible(); }

  ///
  /// Returns true if the is iterated in the reverse order that
  /// it was created.
  ///
  bool orderReversed() { return _itr->orderReversed(); }

  ///
  /// Reverse the order of this set.
  ///
  void reverse() { _itr->reverse(_parent); }
};

}  // namespace odb
