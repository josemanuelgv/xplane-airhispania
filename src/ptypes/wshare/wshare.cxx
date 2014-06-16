/*
 *
 *  C++ Portable Types Library (PTypes)
 *  Version 2.0.2  Released 17-May-2004
 *
 *  Copyright (C) 2001-2004 Hovik Melikyan
 *
 *  http://www.melikyan.com/ptypes/
 *
 */

#include <stdlib.h>
#include <limits.h>

#ifndef WIN32
#  include <pwd.h>
#  include <unistd.h>
#  include <grp.h>
#endif

#include "config.h"
#include "sysutils.h"
#include "log.h"
#include "utils.h"
#include "urlutils.h"
#include "request.h"
#include "clients.h"
#include "modules.h"


USING_PTYPES


class client_socket: public ipstream
{
public:
    client_socket();
};


client_socket::client_socket()
    : ipstream()
{
    // sets both input and output buffer sizes
    set_bufsize(SOCKET_BUF_SIZE);
}


void fail(const char* msg)
{
    syslog_write(SYSLOG_FATAL, "%s", msg);
    exit(2);
}


void run_server()
{
    ipstmserver server;
    client_socket* client = new client_socket();

    threads.set_capacity(cfg_max_clients * 2);

    try
    {
        // listen to all local addresses on cfg_port
        server.bindall(cfg_port);
        syslog_write(SYSLOG_WARNING, "started, user=%s, port=%d, dir=%s",
	    get_username(), cfg_port, pconst(cfg_document_root));

        // try to listen on socket to give an error message
        // before daemonizing
        server.poll(-1, 0);

        started = now();

        // daemonize (has no effect on WIN32)
        if (cfg_daemonize)
        {
            errlog = 0;
            daemon(0, 0);
        }

#ifndef WIN32
        // run-as (downgrade process priveleges)
        if (!isempty(cfg_user))
        {
            int uid = get_user_id(cfg_user);
            if (uid < 0)
                fail("Unknown user: " + cfg_user);

            int gid;
            if (isempty(cfg_group))
            {
                gid = get_user_gid(cfg_user);
                cfg_group = get_group_name(gid);
            }
            else
                gid = get_group_id(cfg_group);
            if (gid < 0)
                fail("Unknown group: " + cfg_group);

            if (setgid(gid) < 0)
                fail("Couldn't change effective user/group ID (not root?)");
            if (initgroups(cfg_user, gid))
                fail("initgroups() failed"); // what else can we say here?
            if (setuid(uid) < 0)
                fail("Couldn't change effective user ID (not root?)");

            syslog_write(SYSLOG_WARNING, "running as %s:%s",
                pconst(cfg_user), pconst(cfg_group));
        }
#endif

        while (true)
        {
            // wait infinitely for a connection request
            server.serve(*client);
            
            if (client->get_active())
            {
                if (thread_count >= cfg_max_clients * 2)
                {
                    // critical situation: the number of threads is twice as
                    // more than allowed
                    client->close();
                    syslog_write(SYSLOG_WARNING, "critical number of clients reached (%d * 2), aborting connection", cfg_max_clients);
                }

                else
                {
                    // start a new thread for this connection
                    client_thread* conn = new client_thread(client);
                    client = 0;
                    // start and forget; the thread object will free itself
                    conn->start();
                    client = new client_socket();
                }
            }
        }
    }
    catch(estream* e)
    {
        fail("Server socket error: " + e->get_message());
	delete e;
    }
}


void config_check()
{
    cfg_document_root = absolute_path(cfg_document_root);
    if (trail_char(cfg_document_root) == '/')
    {
#ifdef WIN32
        if (length(cfg_document_root) > 3)  // "c:/" or "///" ?
#else
        if (length(cfg_document_root) > 1)  // "/" ?
#endif
            trunc_trail_char(cfg_document_root);
    }
    if (isempty(cfg_document_root) || !is_directory(cfg_document_root))
        fail("Document root directory not found");

    if (cfg_port <= 0)
        fail("Invalid port number");

    if (!isempty(cfg_log_file))
    {
        htlog = new logfile(cfg_log_file, true);
        try
        {
            htlog->open();
        }
        catch (estream* e)
        {
            fail(e->get_message());
            delete e;
        }
    }

    else if (cfg_daemonize)
        syslog_write(SYSLOG_WARNING, "daemon started without a HTTP access log");

    if (cfg_max_clients <= 0)
        fail("Invalid maximum number of clients");

}


int main(int argc, char* argv[])
{
    cfg_server_name = get_nodename();

    config_init(argc, argv);
    log_init();
    config_check();
    init_handlers();

    run_server();

    // currently we never reach this point
    log_done();
    config_done();

    return 0;
}

