// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"
#include "util.h"

bigint::bigint(long that) : uvalue(that), is_negative(that < 0){
                                              DEBUGF('~', this << " -> " << uvalue)}

                            bigint::bigint(const ubigint &uvalue_, bool is_negative_) : uvalue(uvalue_), is_negative(is_negative_)
{
}

bigint::bigint(const string &that)
{
  is_negative = that.size() > 0 && that[0] == '_';
  uvalue = ubigint(that.substr(is_negative ? 1 : 0));
}

bigint bigint::operator+() const
{
  return *this;
}

bigint bigint::operator-() const
{
  return {uvalue, !is_negative};
}

bigint bigint::operator+(const bigint &that) const
{
  if (is_negative != that.is_negative)
  {
    if (uvalue > that.uvalue)
    {
      if (is_negative)
      {
        return bigint(uvalue - that.uvalue, true);
      }
      return bigint(uvalue - that.uvalue);
    }
    else
    {
      if (is_negative)
      {
        return bigint{that.uvalue - uvalue};
      }
      return bigint{that.uvalue - uvalue, true};
    }
  }
  if (uvalue > that.uvalue)
  {
    return bigint(uvalue + that.uvalue);
  }
  else
  {
    return bigint(that.uvalue + uvalue);
  }
  //   return result;
}

bigint bigint::operator-(const bigint &that) const
{
  bigint result;
  if (is_negative != that.is_negative)
  {
    if (is_negative && !(that.is_negative))
    {
      result = uvalue + that.uvalue;
    }
    if (!is_negative && that.is_negative)
    {
      result.is_negative = true;
      result.uvalue = uvalue + that.uvalue;
    }
  }
  else
  {
    if (uvalue < that.uvalue)
    {
      result.is_negative = true;
      result.uvalue = that.uvalue - uvalue;
    }
    else
    {
      result.uvalue = uvalue - that.uvalue;
    }
  }
  return result;
}

bigint bigint::operator*(const bigint &that) const
{
  bigint zero = 0;
  if (uvalue == zero.uvalue || that.uvalue == zero.uvalue)
  {
    return 0;
  }
  bigint result = uvalue * that.uvalue;
  result.is_negative = is_negative != that.is_negative;
  return result;
}

bigint bigint::operator/(const bigint &that) const
{
  bigint result = uvalue / that.uvalue;
  result.is_negative = is_negative != that.is_negative;
  return result;
}

bigint bigint::operator%(const bigint &that) const
{
  bigint result = uvalue % that.uvalue;
  return result;
}

bool bigint::operator==(const bigint &that) const
{
  return is_negative == that.is_negative && uvalue == that.uvalue;
}

bool bigint::operator<(const bigint &that) const
{
  if (is_negative != that.is_negative)
    return is_negative;
  return is_negative ? uvalue > that.uvalue
                     : uvalue < that.uvalue;
}

ostream &operator<<(ostream &out, const bigint &that)
{
  return out << (that.is_negative ? "-" : "") << that.uvalue;
}
