// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash{
    {"cat", fn_cat},
    {"cd", fn_cd},
    {"echo", fn_echo},
    {"exit", fn_exit},
    {"ls", fn_ls},
    {"lsr", fn_lsr},
    {"make", fn_make},
    {"mkdir", fn_mkdir},
    {"prompt", fn_prompt},
    {"pwd", fn_pwd},
    {"rm", fn_rm},
    {"rmr", fn_rmr},
};

command_fn find_command_fn(const string &cmd)
{
  // Note: value_type is pair<const key_type, mapped_type>
  // So: iterator->first is key_type (string)
  // So: iterator->second is mapped_type (command_fn)
  DEBUGF('c', "[" << cmd << "]");
  const auto result = cmd_hash.find(cmd);
  if (result == cmd_hash.end())
  {
    throw command_error(cmd + ": no such function");
  }
  return result->second;
}

command_error::command_error(const string &what) : runtime_error(what)
{
}

int exit_status_message()
{
  int status = exec::status();
  cout << exec::execname() << ": exit(" << status << ")" << endl;
  return status;
}

void fn_cat(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();
  wordvec files(words.cbegin() + 1, words.cend());
  for (auto file : files)
  {
    wordvec path = split(file, "/");
    auto newCwd = cwdPtr;
    for (auto level : path)
    {
      try
      {
        newCwd = newCwd->getContents()->getDirent(level);
      }
      catch (file_error &e)
      {
        cerr << "cat: " << level;
        cerr << ": No such file or directory." << endl;
        exec::status(1);
        continue;
      }
    }
    try
    {
      wordvec fileContents = newCwd->getContents()->readfile();
      if (fileContents.size() > 0)
      {
        string fileString = "";
        for (string word : fileContents)
        {
          fileString += word;
          if (word != words.back())
          {
            fileString += " ";
          }
        }
        cout << fileString << endl;
      }
    }
    catch (file_error &e)
    {
      cerr << "cat: " << words.at(1);
      cerr << ": No such file or directory." << endl;
      exec::status(1);
    }
  }
}

void fn_cd(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();
  wordvec path = split(words.at(1), "/");
  auto newCwd = cwdPtr;
  for (auto level : path)
  {
    try
    {
      newCwd = newCwd->getContents()->getDirent(level);
    }
    catch (file_error &e)
    {
      cerr << "cd: " << level;
      cerr << ": No such file or directory." << endl;
    }
  }
  state.cwd(newCwd);
}

void fn_echo(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  cout << word_range(words.cbegin() + 1, words.cend()) << endl;
}

void fn_exit(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  if (words.size() > 1)
  {
    exec::status(stoi(words.at(1)));
  }
  throw ysh_exit();
}

void fn_ls(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();

  if (words.size() < 2)
  {
    auto cwdCts = cwdPtr->getContents();
    cwdCts->ls();
    return;
  }

  wordvec files(words.cbegin() + 1, words.cend());
  for (auto file : files)
  {
    wordvec path = split(file, "/");

    auto newCwd = cwdPtr;
    for (auto level : path)
    {
      try
      {
        newCwd = newCwd->getContents()->getDirent(level);
      }
      catch (file_error &e)
      {
        cerr << "ls: " << level;
        cerr << ": No such file or directory." << endl;
      }
    }
    auto cwdCts = newCwd->getContents();
    cwdCts->ls();
  }
}

void fn_lsr(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();

  if (words.size() < 2)
  {
    auto cwdCts = cwdPtr->getContents();
    cwdCts->lsr();
    return;
  }

  wordvec files(words.cbegin() + 1, words.cend());
  for (auto file : files)
  {
    wordvec path = split(file, "/");

    auto newCwd = cwdPtr;
    for (auto level : path)
    {
      try
      {
        newCwd = newCwd->getContents()->getDirent(level);
      }
      catch (file_error &e)
      {
        cerr << "lsr: " << level;
        cerr << ": No such file or directory." << endl;
      }
    }
    auto cwdCts = newCwd->getContents();
    cwdCts->lsr();
  }
}

void fn_make(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  if (words.size() > 2)
  {

    auto fullPath = split(words.at(1), "/");
    wordvec parentDirPath(fullPath.cbegin(), fullPath.cend() - 1);

    auto newCwd = state.cwd();
    for (auto level : parentDirPath)
    {
      try
      {
        newCwd = newCwd->getContents()->getDirent(level);
      }
      catch (file_error &e)
      {
        cerr << "make: " << level;
        cerr << ": No such file or directory." << endl;
      }
    }
    wordvec sub(words.begin() + 2, words.end());
    try
    {
      newCwd->getContents()
          ->mkfile(
              fullPath.at(fullPath.size() - 1))
          ->getContents()
          ->writefile(sub);
    }
    catch (file_error &e)
    {
      cerr << "make: " << fullPath.at(fullPath.size() - 1);
      cerr << ": File or directory with specified";
      cerr << " name already exists." << endl;
    }
  }
}

void fn_mkdir(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  if (words.size() > 1)
  {

    wordvec files(words.cbegin() + 1, words.cend());
    for (auto file : files)
    {
      wordvec fullPath = split(file, "/");

      wordvec parentDirPath(fullPath.cbegin(), fullPath.cend() - 1);

      auto newCwd = state.cwd();
      for (auto level : parentDirPath)
      {
        try
        {
          newCwd = newCwd->getContents()->getDirent(level);
        }
        catch (file_error &e)
        {
          cerr << "mkdir: " << level;
          cerr << ": No such file or directory." << endl;
        }
      }
      try
      {
        newCwd->getContents()->mkdir(fullPath.at(fullPath.size() - 1));
      }
      catch (file_error &e)
      {
        cerr << "mkdir: " << fullPath.at(fullPath.size() - 1);
        cerr << ": File or directory with specified";
        cerr << " name already exists." << endl;
      }
    }
  }
}

void fn_prompt(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  if (words.size() > 1)
  {
    string prompt = "";
    wordvec sub(words.begin() + 1, words.end());
    for (string word : sub)
    {
      prompt += word + " ";
    }
    state.prompt(prompt);
  }
}

void fn_pwd(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();
  auto cwdCts = cwdPtr->getContents();
  if (cwdCts->getDirent(".") == cwdCts->getDirent(".."))
  {
    cout << "/" << endl;
  }
  else
  {
    cwdCts->pwd();
    cout << endl;
  }
}

void fn_rm(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();
  auto cwdCts = cwdPtr->getContents();

  if (cwdCts->getDirent(words.at(1))->getContents()->type() == "dir")
  {
    cerr << "rm: " << words.at(1) << ": must be a file." << endl;
    exec::status(1);
    return;
  }
  cwdCts->remove(words.at(1));
}

void fn_rmr(inode_state &state, const wordvec &words)
{
  DEBUGF('c', state);
  DEBUGF('c', words);
  auto cwdPtr = state.cwd();
  auto cwdCts = cwdPtr->getContents();
  if (cwdCts->getDirent(words.at(1))->getContents()->type() == "file")
  {
    cerr << "rm: " << words.at(1) << ": must be a directory." << endl;
    exec::status(1);
    return;
  }
  cwdCts->remove(words.at(1));
}
