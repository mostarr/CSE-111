// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <regex>
#include <cassert>

#include <unistd.h>
#include <libgen.h>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string, string>;
using str_str_pair = str_str_map::value_type;

void scan_options(int argc, char **argv)
{
  opterr = 0;
  for (;;)
  {
    int option = getopt(argc, argv, "@:");
    if (option == EOF)
      break;
    switch (option)
    {
    case '@':
      debugflags::setflags(optarg);
      break;
    default:
      complain() << "-" << char(optopt) << ": invalid option"
                 << endl;
      break;
    }
  }
}

const string cin_name = "-";

xpair<string, str_str_pair> matchLine(string &line)
{
  regex comment_regex{R"(^\s*(#.*)?$)"};
  regex key_value_regex{R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex{R"(^\s*([^=]+?)\s*$)"};

  smatch result;
  if (regex_search(line, result, comment_regex))
  {
    return {"c", {"", ""}};
  }
  if (regex_search(line, result, key_value_regex))
  {
    return {"kv", {result[1], result[2]}};
  }
  else if (regex_search(line, result, trimmed_regex))
  {
    return {"q", {result[1], ""}};
  }
  else
  {
    assert(false and "This can not happen.");
  }
}

str_str_map &runFile(istream &infile, str_str_map &_map)
{
  for (;;)
  {
    string line;
    getline(infile, line);
    cout << line << endl;
    if (infile.eof())
    {
      break;
    }
    auto lineResult = matchLine(line);
    if (lineResult.first == "c")
      continue;
    if (lineResult.first == "kv")
    {
      if (lineResult.second.first == "" &&
          lineResult.second.second == "")
      {
        for (str_str_pair pair : _map)
        {
          cout << pair << endl;
        }
        continue;
      }
      if (lineResult.second.second == "")
      {
        if (_map.find(lineResult.second.first) != _map.end())
          _map.erase(_map.find(lineResult.second.first));
        continue;
      }
      if (lineResult.second.first == "")
      {

        for (str_str_pair pair : _map)
        {
          if (pair.second == lineResult.second.second)
          {
            cout << pair << endl;
          }
        }
        continue;
      }
      _map.insert(lineResult.second);
    }
    if (lineResult.first == "q")
    {
      auto itor = _map.find(lineResult.second.first);
      if (itor == _map.end())
        cout << lineResult.first << ": key not found" << endl;
      else
        cout << *itor << endl;
    }
  }
  return _map;
}

int iterFiles(int argc, char **argv, str_str_map &output)
{
  int status = 0;
  for (char **argp = &argv[optind]; argp != &argv[argc]; ++argp)
  {
    if (*argp == cin_name)

      runFile(cin, output);
    else
    {
      ifstream infile(*argp);
      if (infile.fail())
      {
        status = 1;
        cerr << argv[0] << ": " << *argp << ": "
             << strerror(errno) << endl;
      }
      else
      {
        runFile(infile, output);
        infile.close();
      }
    }
  }

  return status;
}

int main(int argc, char **argv)
{
  sys_info::execname(argv[0]);
  scan_options(argc, argv);
  int status = 0;
  str_str_map test;

  status = iterFiles(argc, argv, test);

  return status;
}
