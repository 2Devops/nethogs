#ifndef __PACKET_H
#define __PACKET_H

#define _BSD_SOURCE 1
#include <net/ethernet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C"
{
	#include <pcap.h>
}

/* To initialise this module, call getLocal with the currently
 * monitored device (e.g. "eth0:1") */
void getLocal (const char *device);

class Packet
{
public:
	in_addr sip;
	in_addr dip;
	unsigned short sport;
	unsigned short dport;
	bpf_u_int32 len;
	timeval time;

	Packet (in_addr m_sip, unsigned short m_sport, in_addr m_dip, unsigned short m_dport, bpf_u_int32 m_len, timeval m_time);
	/* using default copy constructor */
	/* Packet (const Packet &old_packet); */
	/* copy constructor that turns the packet around */
	Packet * newInverted ();

	bool isOlderThan(timeval t);
	/* is this packet coming from the local host? */
	bool Outgoing ();

	bool match (Packet * other);
	/* returns '1.2.3.4:5-1.2.3.4:6'-style string */
	char * gethashstring();
};

Packet * getPacket (const struct pcap_pkthdr * header, const u_char * packet);

#endif
