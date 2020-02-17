// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t, mapped_t, less_t>::~listmap()
{
  DEBUGF('l', reinterpret_cast<const void *>(this));
  node *current = anchor()->next;
  while (current != anchor())
  {
    node *temp = current;
    current = current->next;
    delete temp;
  }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::insert(const value_type &pair)
{
  DEBUGF('l', &pair << "->" << pair);

  node *insertBefore = anchor()->next;
  while (iterator(insertBefore) != end())
  {
    if (less(pair.first, insertBefore->value.first))
      break;
    insertBefore = insertBefore->next;
  }

  node *newNode = new node{insertBefore, insertBefore->prev, pair};

  insertBefore->prev->next = newNode;
  insertBefore->prev = newNode;

  return iterator();
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::find(const key_type &that)
{
  DEBUGF('l', that);
  for (listmap::iterator itor = begin(); itor != end(); ++itor)
  {
    if (itor->first == that)
      return itor;
  }
  return anchor();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::erase(iterator position)
{
  DEBUGF('l', &*position);
  iterator before = position.where->prev;
  iterator next = position.where->next;
  before.where->next = next.where;
  next.where->prev = before.where;
  delete position.where;
  return next;
}
