#include <iostream>
#include <assert.h>
#include <malloc.h>
#include "nethogs.h"
#include "connection.h"

class ConnList
{
public:
	ConnList (Connection * m_val = NULL, ConnList * m_next = NULL)
	{
	    val = m_val; next = m_next;
	}
	Connection * val;
	ConnList * next;
};

ConnList * connections = NULL;

void PackList::add (Packet * p)
{
	if (content == NULL)
	{
		content = new PackListNode (p);
		return;
	}

	if (content->val->time.tv_sec == p->time.tv_sec)
	{
		content->val->len += p->len;
		return;
	}

	content = new PackListNode(p, content);
}

/* sums up the total bytes used and removes 'old' packets */
bpf_u_int32 PackList::sumanddel (timeval t)
{
	bpf_u_int32 retval = 0;
	PackListNode * current = content;
	PackListNode * previous = NULL;

	while (current != NULL) 
	{
		//std::cout << "Comparing " << current->val->time.tv_sec << " <= " << t.tv_sec - PERIOD << endl;
		if (current->val->time.tv_sec <= t.tv_sec - PERIOD)
		{
			if (current == content)
				content = NULL;
			else if (previous != NULL)
				previous->next = NULL;
			delete current;
			return retval;
		}
		retval += current->val->len;
		previous = current;
		current = current->next;
	}
	return retval;
}

Connection::Connection (Packet * packet)
{
	if (DEBUG)
		assert (packet != NULL);
	connections = new ConnList (this, connections);
	sent_packets = new PackList ();
	recv_packets = new PackList ();
	if (packet->Outgoing())
	{
		sent_packets->add(packet);
		refpacket = new Packet (*packet);
	} else {
		recv_packets->add(packet);
		refpacket = packet->newInverted();
	}
	lastpacket = packet->time.tv_sec;
	if (DEBUG)
		std::cout << "New reference packet created at " << refpacket << std::endl;
}

Connection::~Connection ()
{
	if (DEBUG)
		std::cout << "Deleting connection" << std::endl;
	/* refpacket is a pointer to one of the packets in the lists,
	 * so not deleted */
	//delete refpacket;
    	if (sent_packets != NULL)
		delete sent_packets;
    	if (recv_packets != NULL)
		delete recv_packets;

	ConnList * curr_conn = connections;
	ConnList * prev_conn = NULL;
	while (curr_conn != NULL)
	{
		if (curr_conn->val == this)
		{
			ConnList * todelete = curr_conn;
			curr_conn = curr_conn->next;
			if (prev_conn == NULL)
			{
				connections = curr_conn;
			} else {
				prev_conn->next = curr_conn;
			}
			delete (todelete);
		}
		else
		{
			prev_conn = curr_conn;
			curr_conn = curr_conn->next;
		}
	}
}

void Connection::add (Packet * packet)
{
	lastpacket = packet->time.tv_sec;
	if (packet->Outgoing())
	{
		sent_packets->add (packet);
	} else {
		recv_packets->add (packet);
	}
}

/* finds connection to which this packet belongs.
 * a packet belongs to a connection if it matches
 * to its reference packet */
Connection * findConnection (Packet * packet)
{
	ConnList * current = connections;
	Packet * invertedPacket = packet->newInverted();
	while (current != NULL)
	{
		/* the reference packet is always *outgoing* */
		if ((packet->match(current->val->refpacket))
		  || (invertedPacket->match(current->val->refpacket)))
			return current->val;

		current = current->next;
	}
	return NULL;
}

/*
 * Connection::sumanddel
 *	
 * sums up the total bytes used
 * and removes 'old' packets. 
 *
 * Returns sum of sent packages (by address)
 *	   sum of recieved packages (by address)
 */
void Connection::sumanddel (timeval t, bpf_u_int32 * sent, bpf_u_int32 * recv)
{
    (*sent)=(*recv)=0;

    *sent = sent_packets->sumanddel(t);
    *recv = recv_packets->sumanddel(t);
}
