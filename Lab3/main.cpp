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

  cout << endl
       << "input: \"" << line << "\"" << endl;
  smatch result;
  if (regex_search(line, result, comment_regex))
  {
    cout << "Comment or empty line." << endl;
    return {"c", {"", ""}};
  }
  if (regex_search(line, result, key_value_regex))
  {
    cout << "key  : \"" << result[1] << "\"" << endl;
    cout << "value: \"" << result[2] << "\"" << endl;
    return {"kv", {result[1], result[2]}};
  }
  else if (regex_search(line, result, trimmed_regex))
  {
    cout << "query: \"" << result[1] << "\"" << endl;
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
    if (infile.eof())
    {
      break;
    }
    auto lineResult = matchLine(line);
    if (lineResult.first == "c")
      continue;
    if (lineResult.first == "kv")
    {
      if (lineResult.second.first == "" && lineResult.second.second == "")
      {
        for (str_str_pair pair : _map)
        {
          cout << pair << endl;
        }
        continue;
      }
      if (lineResult.second.second == "")
      {
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

str_str_map &iterFiles(int argc, char **argv, str_str_map &output)
{
  // if (&argv[optind] == &argv[argc])
  //   filenames.push_back(cin_name);
  for (char **argp = &argv[optind]; argp != &argv[argc]; ++argp)
  {
    if (*argp == cin_name)

      runFile(cin, output);
    else
    {
      ifstream infile(*argp);
      if (infile.fail())
      {
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

  return output;
}

int main(int argc, char **argv)
{
  sys_info::execname(argv[0]);
  scan_options(argc, argv);
  // int status = 0;
  str_str_map test;

  iterFiles(argc, argv, test);

  // for (str_str_map::iterator itor = test.begin();
  //      itor != test.end(); ++itor)
  // {
  //   cout << "&*itor: " << &*itor << endl;
  //   cout << "*itor: " << *itor << endl;
  // }

  for (char **argp = &argv[optind]; argp != &argv[argc]; ++argp)
  {
    str_str_pair pair(*argp, to_string<int>(argp - argv));
    cout << "Before insert: " << pair << endl;
    test.insert(pair);
  }

  for (str_str_map::iterator itor = test.begin();
       itor != test.end(); ++itor)
  {
    cout << "During iteration: " << *itor << endl;
  }

  str_str_map::iterator itor = test.begin();
  test.erase(itor);

  cout << "EXIT_SUCCESS" << endl;
  return EXIT_SUCCESS;
}
