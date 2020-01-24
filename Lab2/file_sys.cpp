// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr{1};

struct file_type_hash
{
  size_t operator()(file_type type) const
  {
    return static_cast<size_t>(type);
  }
};

ostream &operator<<(ostream &out, file_type type)
{
  static unordered_map<file_type, string, file_type_hash> hash{
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
  };
  return out << hash[type];
}

inode_state::inode_state()
{
  root = make_shared<inode>(file_type::DIRECTORY_TYPE, "");
  root->contents->init_dirents(root, root);
  cwd_ = root;
  DEBUGF('i', "root = " << root << ", cwd = " << cwd()
                        << ", prompt = \"" << prompt() << "\"");
}

const inode_ptr &inode_state::cwd() const { return cwd_; }
void inode_state::cwd(inode_ptr newCwd) { cwd_ = newCwd; }
const string &inode_state::prompt() const { return prompt_; }
void inode_state::prompt(const string &prompt) { prompt_ = prompt; }

ostream &operator<<(ostream &out, const inode_state &state)
{
  out << "inode_state: root = " << state.root
      << ", cwd = " << state.cwd();
  return out;
}

inode::inode(file_type type, const string &name) : inode_nr(next_inode_nr++)
{
  switch (type)
  {
  case file_type::PLAIN_TYPE:
    contents = make_shared<plain_file>();
    break;
  case file_type::DIRECTORY_TYPE:
    contents = make_shared<directory>();
    break;
  }
  contents->name(name);
  DEBUGF('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const
{
  DEBUGF('i', "inode = " << inode_nr);
  return inode_nr;
}

file_error::file_error(const string &what) : runtime_error(what)
{
}

const wordvec &base_file::readfile() const
{

  throw file_error("is a " + error_file_type());
}

void base_file::writefile(const wordvec &)
{
  throw file_error("is a " + error_file_type());
}

void base_file::remove(const string &)
{
  throw file_error("is a " + error_file_type());
}

inode_ptr base_file::mkdir(const string &)
{
  throw file_error("is a " + error_file_type());
}

inode_ptr base_file::mkfile(const string &)
{
  throw file_error("is a " + error_file_type());
}

void base_file::init_dirents(inode_ptr, inode_ptr)
{
  throw file_error("is a " + error_file_type());
}

inode_ptr base_file::getDirent(const string &)
{
  throw file_error("is a " + error_file_type());
}

void base_file::name(const string name)
{
  name_ = name;
}

string base_file::name()
{
  return name_;
}

void base_file::ls()
{
  cout << name_ << endl;
}

void base_file::lsr()
{
  cout << name_ << endl;
}

size_t plain_file::size() const
{
  size_t size{0};
  DEBUGF('i', "size = " << size);
  return size;
}

const wordvec &plain_file::readfile() const
{
  DEBUGF('i', data);
  return data;
}

void plain_file::writefile(const wordvec &words)
{
  DEBUGF('i', words);
}

void plain_file::ls()
{
  cout << name_ << endl;
}

void plain_file::lsr()
{
  cout << name_ << endl;
}

size_t directory::size() const
{
  size_t size{dirents.size()};
  DEBUGF('i', "size = " << size);
  return size;
}

void directory::init_dirents(inode_ptr parent, inode_ptr self)
{
  dirents.insert({".", self});
  dirents.insert({"..", parent});
}

void directory::remove(const string &filename)
{
  DEBUGF('i', filename);
}

inode_ptr directory::mkdir(const string &dirname)
{
  DEBUGF('i', dirname);
  auto dir = make_shared<inode>(file_type::DIRECTORY_TYPE, dirname);
  dir->getContents()->init_dirents(dirents.find(".")->second, dir);
  dirents.insert({dirname, dir});
  return dir;
}

inode_ptr directory::mkfile(const string &filename)
{
  DEBUGF('i', filename);
  return nullptr;
}

inode_ptr directory::getDirent(const string &name)
{
  return dirents.find(name)->second;
}

void directory::ls()
{
  cout << "/" << name_ << ":" << endl;
  for (auto dirent : dirents)
  {
    cout << "\t";
    cout << dirent.second->get_inode_nr() << "  ";
    cout << dirent.second->getContents()->size() << "  ";
    cout << dirent.first;
    if (dirent.second->getContents()->type() == "dir")
    {
      cout << "/";
    }
    cout << endl;
  }
}

void directory::lsr()
{
  cout << "/" << name_ << ":" << endl;
  for (auto dirent : dirents)
  {
    cout << "\t";
    cout << dirent.second->get_inode_nr() << "  ";
    cout << dirent.second->getContents()->size() << "  ";
    cout << dirent.first;
    if (dirent.second->getContents()->type() == "dir")
    {
      cout << "/";
    }
    cout << endl;
  }
  for (auto dirent : dirents)
  {
    if (dirent.second->getContents()->type() == "dir" &&
        dirent.first != "." && dirent.first != "..")
    {
      wordvec pathname;
      inode_ptr parent = dirents.find(".")->second;
      while (parent->getContents()->name() != "")
      {
        pathname.insert(pathname.begin(), parent->getContents()->name());
        parent = parent->getContents()->getDirent("..");
      }
      for (const string &dirName : pathname)
      {
        cout << "/" << dirName;
      }
      dirent.second->getContents()->lsr();
    }
  }
}