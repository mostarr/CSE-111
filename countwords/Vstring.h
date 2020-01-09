/*
 * Vstring.h
 *
 *  Created on: Jan 9, 2020
 *      Author: max
 */

#ifndef VSTRING_H_
#define VSTRING_H_

class Vstring {
private:
	int cap;
	int end;
	char* word;
public:
	Vstring();
	virtual ~Vstring();
	char* getWord() { return word; }
	int getEnd() { return end; }
	void addChar(char x);
	void print();
};

#endif /* VSTRING_H_ */
