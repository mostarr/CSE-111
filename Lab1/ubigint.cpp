// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
using namespace std;

#include "ubigint.h"
#include "debug.h"

//ubigint::ubigint (): ubig_value(0) {
//	ubig_value.push_back(0);
//}

ubigint::ubigint (unsigned long that): ubig_value (0) {
//   DEBUGF ('~', "" << this << " -> " << ubig_value);
    std::stringstream stream;
    stream << that;
    int length = stream.str().size();
	for(int i = 0; i < length; i++){
	   int digit = that % 10;
//	   if (digit == 0) break;
	   DEBUGF ('d', "long digit: " << digit);
	   that /= 10;
	   ubig_value.push_back(digit);
   }

}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(digit - '0');
   }
}

void ubigint::trim() {
	for ( ; ; ){
	   if(ubig_value.back() == 0){
		   ubig_value.pop_back();
		   if (ubig_value.size() < 1) {
			   return;
		   }
	   }else{
		   break;
	   }
	}
}

ubigint ubigint::operator+ (const ubigint& that) const {
	ubigvalue_t sum;
	auto thisIt = ubig_value.begin();
	auto thatIt = that.ubig_value.begin();
	int carry = 0;
	for ( ; thisIt != ubig_value.end(); ++thisIt ){
		int thatDigit;
		if (thatIt >= that.ubig_value.end()) {
			thatDigit = 0;
		}
		else {
			thatDigit = static_cast<int>(*(thatIt++));
		}
		int digitSum = static_cast<int>(*thisIt) + thatDigit + carry;

		if( digitSum >= 10) {
			carry = digitSum - (digitSum%10) - 10;
			digitSum = digitSum/10;
		}else{
			carry = 0;
		}
		sum.push_back(static_cast<unsigned char>(digitSum));
	}
	ubigint returnValue;
	returnValue.ubig_value = sum;
	returnValue.trim();
	return returnValue;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigvalue_t  diff;
	auto thisIt = ubig_value.begin();
	auto thatIt = that.ubig_value.begin();
	int carry = 0;

	for ( ; thisIt != ubig_value.end(); ++thisIt ){
		int thatDigit;
		if (thatIt >= that.ubig_value.end()) {
			thatDigit = 0;
		}
		else {
			thatDigit = static_cast<int>(*(thatIt++));
		}
		int digitDiff = static_cast<int>(*thisIt) - thatDigit + carry;

		cout << digitDiff << endl;

		if (digitDiff < 0) {
			digitDiff += 10;
			carry = -1;
		}else{
			carry = 0;
		}
		diff.push_back(static_cast<unsigned char>(digitDiff));
	}
	ubigint returnValue;
	returnValue.ubig_value = diff;
	returnValue.trim();
	return returnValue;
}

ubigint ubigint::operator* (const ubigint& that) const {
	ubigvalue_t prod(ubig_value.size() + that.ubig_value.size());
	auto thisIt = ubig_value.begin();
	size_t counterOut = 0;
	for ( ; thisIt != ubig_value.end(); ++thisIt ){
		int carry = 0;

		size_t counterIn = 0;

		auto thatIt = that.ubig_value.begin();
		for ( ; thatIt != that.ubig_value.end(); ++thatIt ){
			int digitAt = static_cast<int>(*(prod.begin() + counterIn + counterOut));
			int subProd = (static_cast<int>(*thisIt) * static_cast<int>(*(thatIt)));
			int digitProd = digitAt + subProd + carry;

			if(digitProd!=0){
				prod.erase(prod.begin() + counterOut + counterIn);
			 	prod.insert(prod.begin() + counterOut + counterIn,static_cast<unsigned char>(digitProd%10));
			}
			 counterIn++;
			carry = trunc(digitProd/10);

		}

		prod.insert((prod.begin() + (counterIn )+ (counterOut++)),static_cast<unsigned char>(carry));



	}
	ubigint returnValue;
	returnValue.ubig_value = prod;
	returnValue.trim();
	return returnValue;
}

void ubigint::multiply_by_2() {
	ubigvalue_t prod(ubig_value.size() + 1);
	auto thisIt = ubig_value.rbegin();
	int carry = 0;
	for ( ; thisIt != ubig_value.rend(); ++thisIt ){
		int digitProd = prod.back() + (static_cast<int>(*thisIt) * 2) + carry;
		prod.push_back(static_cast<unsigned char>(digitProd%10));
		carry = trunc(digitProd/10);
	}
   ubig_value = prod;
}

void ubigint::divide_by_2() {
	if (ubig_value.size() < 1) {
		return;
	}
	ubigvalue_t quot;
	auto thisIt = ubig_value.begin();
	int carry = 0;

	for ( ; thisIt != ubig_value.end() - 1; ++thisIt ){
	   DEBUGF ('d', "div digit: " << (static_cast<int>(*thisIt)));
		if(static_cast<int>(*(thisIt + 1)) != 0 && static_cast<int>(*(thisIt + 1)) % 2 != 0){
			carry = 5;
		}else{
			carry = 0;
		}
		quot.push_back(trunc((static_cast<int>(*thisIt) / 2) + carry));
	}
	quot.push_back(trunc((static_cast<int>(*thisIt) / 2) + carry));
	ubig_value = quot;
	this->trim();
}



struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();

      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
	if(ubig_value.size() == that.ubig_value.size()){
		auto thisIt = ubig_value.begin();
		auto thatIt = that.ubig_value.begin();
		for ( ; thisIt != ubig_value.end(); ++thisIt ){
			bool lt = static_cast<int>(*thisIt) < static_cast<int>(*(++thatIt));
			if(lt == true){
				return true;
			}
		}
	}
	return ubig_value.size() < that.ubig_value.size();
}

std::ostream& operator<< (std::ostream& out, const ubigint& that) {
	std::ostringstream output;

	for ( auto thisIt = that.ubig_value.rbegin(); thisIt != that.ubig_value.rend(); ++thisIt ){
		output << (static_cast<int>(*thisIt));
	}

   return out << "ubigint(" << output.str() << ")";
}
