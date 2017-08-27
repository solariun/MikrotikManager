 /*
    librouteros-api - Connect to RouterOS devices using official API protocol
    Copyright (C) 2012-2013, Håkon Nessjøen <haakon.nessjoen@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MIKROTIKLIB
#define MIKROTIKLIB

#define ROS_PORT 8728

#include <Util.h>
#include <Exception.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


struct ros_sentence {
	char **word;
	int words;
};

struct ros_result {
	struct ros_sentence *sentence;
	char done;
	char re;
	char trap;
	char fatal;
};

struct ros_event {
	char tag[100];
	void (*callback)(struct ros_result *result);
	char inuse;
};

enum ros_type {
    ROS_SIMPLE,
    ROS_EVENT
};


struct ros_connection {
	enum ros_type type;
#ifdef _WIN32
	SOCKET socket;
#else
	int socket;
#endif
	unsigned char buffer [512];
	struct ros_event **events;
	int max_events;
	struct ros_result *event_result;
	int expected_length;
	int length;
};

#ifdef __cplusplus
extern "C"
{
#endif
    /* event based functions */
    int ros_send_command(struct ros_connection *conn, char *command, ...);
    void ros_set_type(struct ros_connection *conn, enum ros_type type);
    int ros_runloop_once(struct ros_connection *conn, void (*callback)(struct ros_result *result));
    int ros_send_command_cb(struct ros_connection *conn, void (*callback)(struct ros_result *result), char *command, ...);
    int ros_send_sentence_cb(struct ros_connection *conn, void (*callback)(struct ros_result *result), struct ros_sentence *sentence, uint32_t nTag);
    
    /* blocking functions */
    struct ros_result *ros_send_command_wait(struct ros_connection *conn, char *command, ...);
    struct ros_result *ros_read_packet(struct ros_connection *conn);
    int ros_login(struct ros_connection *conn, char *username, char *password);
    int ros_cancel(struct ros_connection *conn, int id);
    
    /* common functions */
    struct ros_connection *ros_connect(char *address, int port);
    int ros_disconnect(struct ros_connection *conn);
    void ros_result_free(struct ros_result *result);
    char *ros_get(struct ros_result *result, char *key);
    char *ros_get_tag(struct ros_result *result);
    
    /* sentence functions */
    struct ros_sentence *ros_sentence_new();
    void ros_sentence_free(struct ros_sentence *sentence);
    void ros_sentence_add(struct ros_sentence *sentence, char *word);
    
#ifdef __cplusplus
}
#endif


class MikrotikLib : Exception
{
private:
    struct ros_connection *conn;
    struct ros_sentence *Sentence;
    fd_set read_fds;

    string strServer;
    uint   nPort;
    string strUser;
    string strPasswd;

    struct timeval timeout;

protected:
    
public:
    MikrotikLib (const char* pszServerAddress, uint nPortServer, const char* pszUser, const char* pszPassword);
    ~MikrotikLib ();

    void IfException ();
    
    /* Send command to Mikrotik and return the .tag given. */
    uint32_t SendCommand (const char* pszCommand, void (*pHandle)(struct ros_result*), uint32_t nTag);

    void ProcessLoopOnce ();
    
    void SetDebug (bool bdebug);
};

#endif




