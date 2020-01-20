// $Id: ubigint.h,v 1.4 2020-01-06 13:39:55-08 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
using namespace std;

#include "debug.h"
#include "relops.h"

class ubigint
{
  friend ostream &operator<<(ostream &, const ubigint &);

private:
  using udigit_t = unsigned char;
  using ubigvalue_t = std::vector<udigit_t>;
  ubigvalue_t ubig_value;
  void trim();

public:
  void multiply_by_2();
  void divide_by_2();
  ubigint mod_by_2();

  ubigint() = default; // Need default ctor as well.
  ubigint(unsigned long);
  ubigint(const string &);

  ubigint operator+(const ubigint &) const;
  ubigint operator-(const ubigint &) const;
  ubigint operator*(const ubigint &)const;
  ubigint operator/(const ubigint &) const;
  ubigint operator%(const ubigint &) const;

  bool operator==(const ubigint &) const;
  bool operator<(const ubigint &) const;
};

#endif
