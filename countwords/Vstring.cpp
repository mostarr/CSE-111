/*
 * Vstring.cpp
 *
 *  Created on: Jan 9, 2020
 *      Author: max
 */

#include "Vstring.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

Vstring::Vstring()
{
  cap = 10;
  end = 1;
  word = (char *)calloc(cap, sizeof(char));
}

Vstring::~Vstring()
{
  free(word);
}

void Vstring::addChar(char x)
{
  if (end == cap)
  {
    cap = cap * 2;
    char *newWord = (char *)calloc(cap, sizeof(char));
    for (int i = 1; i <= end; i++)
    {
      newWord[i] = word[i];
    }
    free(word);
    word = newWord;
  }
  word[end] = x;
  end++;
}

void Vstring::print()
{
  for (int i = 1; i <= end; i++)
  {
    std::cout << word[i];
  }
  std::cout << std::endl;
}
