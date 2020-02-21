// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog(cout);
struct cix_exit : public exception
{
};

unordered_map<string, cix_command> command_map{
    {"exit", cix_command::EXIT},
    {"help", cix_command::HELP},
    {"ls", cix_command::LS},
    {"put", cix_command::PUT},
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help()
{
  cout << help;
}

void cix_ls(client_socket &server)
{
  cix_header header;
  header.command = cix_command::LS;
  outlog << "sending header " << header << endl;
  send_packet(server, &header, sizeof header);
  recv_packet(server, &header, sizeof header);
  outlog << "received header " << header << endl;
  if (header.command != cix_command::LSOUT)
  {
    outlog << "sent LS, server did not return LSOUT" << endl;
    outlog << "server returned " << header << endl;
  }
  else
  {
    auto buffer = make_unique<char[]>(header.nbytes + 1);
    recv_packet(server, buffer.get(), header.nbytes);
    outlog << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    cout << buffer.get();
  }
}

void cix_put(client_socket &server, string &filename)
{
  ifstream file(filename, ifstream::binary);
  if (file)
  {

    // get length of file: (from cplusplus.org example)
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);

    auto buffer = make_unique<char[]>(length);

    outlog << "Reading " << length << " characters... " << endl;
    // read data as a block:
    file.read(buffer.get(), length);
    outlog << "read: " << buffer.get() << endl;
    cix_header header;
    header.command = cix_command::PUT;
    header.nbytes = length;

    strcpy(header.filename, filename.c_str());
    outlog << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    // send_packet(server, &header, sizeof header);
    send_packet(server, buffer.get(), length);
    outlog << "sent data" << endl;

    recv_packet(server, &header, sizeof header);

    outlog << "received header " << header << endl;
  }
  else
  {
    outlog << "Could not load file: " << filename << endl;
  }
}

void usage()
{
  cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
  throw cix_exit();
}

int main(int argc, char **argv)
{
  outlog.execname(basename(argv[0]));
  outlog << "starting" << endl;
  vector<string> args(&argv[1], &argv[argc]);
  if (args.size() > 2)
    usage();
  string host = get_cix_server_host(args, 0);
  in_port_t port = get_cix_server_port(args, 1);
  outlog << to_string(hostinfo()) << endl;
  try
  {
    outlog << "connecting to " << host << " port " << port << endl;
    client_socket server(host, port);
    outlog << "connected to " << to_string(server) << endl;
    for (;;)
    {
      string line;
      getline(cin, line);
      if (cin.eof())
        throw cix_exit();
      outlog << "command " << line << endl;

      stringstream stream(line);
      string intermediate;
      vector<string> tokens;
      while (getline(stream, intermediate, ' '))
      {
        tokens.push_back(intermediate);
      }

      const auto &itor = command_map.find(tokens.at(0));
      cix_command cmd = itor == command_map.end()
                            ? cix_command::ERROR
                            : itor->second;
      switch (cmd)
      {
      case cix_command::EXIT:
        throw cix_exit();
        break;
      case cix_command::HELP:
        cix_help();
        break;
      case cix_command::LS:
        cix_ls(server);
        break;
      case cix_command::PUT:
        cix_put(server, tokens.at(1));
        break;
      default:
        outlog << line << ": invalid command" << endl;
        break;
      }
    }
  }
  catch (socket_error &error)
  {
    outlog << error.what() << endl;
  }
  catch (cix_exit &error)
  {
    outlog << "caught cix_exit" << endl;
  }
  outlog << "finishing" << endl;
  return 0;
}
