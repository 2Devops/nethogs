#ifndef __PROCESS_H
#define __PROCESS_H

#include <assert.h>
#include "nethogs.h"
#include "connection.h"

extern bool tracemode;

class ConnList
{
public:
	ConnList (Connection * m_val, ConnList * m_next)
	{
		if (DEBUG)
			assert (m_val != NULL);
		val = m_val; next = m_next;
	}
	~ConnList ()
	{
		/* does not delete its value, to allow a connection to
		 * remove itself from the global connlist in its destructor */
	}
	Connection * getVal ()
	{
		return val;
	}
	void setNext (ConnList * m_next)
	{
		next = m_next;
	}
	ConnList * getNext ()
	{
		return next;
	}
private:
	Connection * val;
	ConnList * next;
};

class Process
{
public:
	/* the process makes a copy of the device name and name. */
	Process (unsigned long m_inode, char * m_devicename, char * m_name = NULL)
	{
		if (DEBUG)
			std::cout << "PROC: Process created at " << this << std::endl;
		inode = m_inode;

		if (m_name == NULL)
			name = NULL;
		else
			name = strdup(m_name);

		devicename = strdup(m_devicename);
		connections = NULL;
		pid = 0;
		uid = 0;
	}
	/* TODO free m_name and m_devicename again in constructor */
	~Process ()
	{
		if (DEBUG)
			std::cout << "PROC: Process deleted at " << this << std::endl;
	}
	int getLastPacket ();

	const char * name;
	const char * devicename;
	int pid;

	unsigned long inode;
	ConnList * connections;
	uid_t getUid()
	{
		return uid;
	}

	void setUid(uid_t m_uid)
	{
		assert (m_uid >= 0);
		uid = m_uid;
	}
private:
	uid_t uid;
};

Process * getProcess (Connection * connection, char * devicename = NULL);
void do_refresh ();

void procclean ();

#endif
