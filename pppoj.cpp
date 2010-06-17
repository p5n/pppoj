/*
 * pppoj.cpp - PPP over Jabber
 *
 * You may do whatever you want with this code.
 *
 * Usage: pppoj <JID> <password> <other-JID> -master|-slave
 */

#include "gloox/client.h"
#include "gloox/messagehandler.h"
#include "gloox/message.h"
#include "gloox/base64.h"
#include "gloox/presence.h"
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

using namespace gloox;

class Bot : public MessageHandler
{
public:
  Bot(int fd, const char *jid, const char *password, const char *other)
  {
    pipefd = fd;
    child_pid = 1;
    partner = other;
    JID myjid( jid );
    j = new Client( myjid, password );
    j->registerMessageHandler( this );
    j->setPresence( Presence::Available, 5 );
    j->connect(false);
    while (! j->authed())
      j->recv();
  }

  virtual void handleMessage( const Message& msg, MessageSession* session = 0 )
  {
    if (msg.from().bare() != partner)
      return;
    std::string s = msg.body();
    if (s == "hello")
    {
      printf("New client\n");
      if(child_pid != 1)
      {
        printf("pppd(%d) SIGHUP\n", child_pid);
        kill(child_pid, SIGHUP);
      }
      return;
    }
    std::string dec = Base64::decode64(s);
    write(pipefd, dec.c_str(), dec.length());
  }

  void recv()
  {
    j->recv(5000);
  }

  void set_child_pid(int pid)
  {
    child_pid = pid;
  }

  void send(const void *buf, int count)
  {
    std::string s((char *)buf, count);
    std::string enc = Base64::encode64(s);
    const Message msg(Message::Normal, partner, enc);
    j->send( msg );
  }

  void send_raw(const void *buf, int count)
  {
    std::string s((char *)buf, count);
    const Message msg(Message::Normal, partner, s);
    j->send( msg );
  }

private:
  std::string partner;
  int pipefd;
  Client* j;
  int child_pid;
};

void usage()
{
  printf("Usage: pppoj <JID> <password> <other-JID> -master|-slave\n");
}

int start_pppd(int ismaster)
{
}

int main( int argc, char* argv[] )
{
  if (argc != 5) {
    usage();
    return 0;
  }
  int readpipe[2], writepipe[2];
  if (pipe(readpipe) < 0) {
    perror("pipe");
    return 1;
  }
  if (pipe(writepipe) < 0) {
    perror("pipe");
    return 1;
  }
  printf("Connecting...\n");
  Bot b(readpipe[1], argv[1], argv[2], argv[3]);
  printf("Connected\n");
  int readfd = writepipe[0];
  int ismaster = !strcmp(argv[4], "-master");
  int ret = fork();
  if (ret < 0) {
    perror("fork");
    return 1;
  }
  else if (ret == 0)
  {
    dup2(readpipe[0], 0);
    dup2(writepipe[1],1);
    // close fds
    close(readpipe[0]);
    close(readpipe[1]);
    close(writepipe[0]);
    close(writepipe[1]);
    if(ismaster)
      execl(PPPD_PATH, "updetach", "persist", "noauth", "passive", "notty", "ipparam", "pppoj", "10.0.0.1:10.0.0.2", NULL);
    else
      execl(PPPD_PATH, "nodetach", "notty", "noauth", NULL);
  }
  else
  {
    int hello_sent = 0;
    b.set_child_pid(ret);
    while (1) {
      b.recv();
      if(!ismaster && !hello_sent)
      {
        hello_sent = 1;
        b.send_raw("hello", 5);
      }
      struct timeval tmo;
      fd_set rset, eset;
      FD_ZERO(&rset);
      FD_ZERO(&eset);
      FD_SET(readfd,&rset);
      tmo.tv_sec = 0;
      tmo.tv_usec= 5000;
      int ret = select(readfd + 1,&rset,NULL,&eset,&tmo);
      if (ret < 0)
        perror("select");
      if (ret == 0)
        continue;
      if(FD_ISSET(readfd,&rset))
      {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int count = read(readfd, buf, sizeof(buf));
        if (count > 0)
        {
          b.send(buf, count);
        }
      }
    }
  }
}
