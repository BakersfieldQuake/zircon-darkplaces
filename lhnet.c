
// Written by Ashley Rose Hale (LadyHavoc) 2003-06-15 and placed into public domain.

#ifdef _WIN32
	#ifdef _MSC_VER
		#pragma comment(lib, "ws2_32.lib")
	#endif

		// Windows XP or higher is required for getaddrinfo, but the inclusion of wspiapi provides fallbacks for older versions
		# define _WIN32_WINNT 0x0501

	# include <winsock2.h>
	# include <ws2tcpip.h>
	# ifdef USE_WSPIAPI_H
		#  include <wspiapi.h>
	# endif
#endif // _ WIN32

#ifndef STANDALONETEST
	#include "darkplaces.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#ifndef _WIN32
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>

		#include <net/if.h>

#endif // ! _WIN32

#ifdef __MORPHOS__ // Baker: This is an Amiga like Power PC operating system from 1999
#include <proto/socket.h>
#endif

// for Z_Malloc/Z_Free in quake

#include "zone.h"
#include "sys.h"
#include "netconn.h"

#include "lhnet.h"

#ifdef STEAM_NETWORKING
	#include "steam_net.h"
#endif
#if defined(_WIN32)
	// as of Visual Studio 2015, EWOULDBLOCK and ECONNREFUSED are real things, with different values than we want when talking to WinSock, so we have to undef them here or change the rest of the code.
	#undef EWOULDBLOCK
	#undef ECONNREFUSED
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#define ECONNREFUSED WSAECONNREFUSED
	
	#define SOCKETERRNO WSAGetLastError()
	
	#define IOC_VENDOR 0x18000000
	#define _WSAIOW(x,y) (IOC_IN|(x)|(y))
	#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
	
	#define SOCKLEN_T int
	#elif defined(__MORPHOS__)
	#define ioctlsocket IoctlSocket
	#define closesocket CloseSocket
	#define SOCKETERRNO Errno()
	
	#define SOCKLEN_T int
#else
	// Baker: Not Windows ...
	#define ioctlsocket ioctl
	#define closesocket close
	#define SOCKETERRNO errno
	
	#define SOCKLEN_T socklen_t
#endif // ! _WIN32

// Baker: The MSG_DONTWAIT flag is defined in the <sys/socket.h> header file
#ifdef MSG_DONTWAIT
	#define LHNET_RECVFROM_FLAGS MSG_DONTWAIT
	#define LHNET_SENDTO_FLAGS 0
#else
	#define LHNET_RECVFROM_FLAGS 0
	#define LHNET_SENDTO_FLAGS 0
#endif

typedef struct lhnetaddressnative_s
{
	lhnetaddresstype_t addresstype;
	int port;
	union
	{
		struct sockaddr sock;
		struct sockaddr_in in;

		struct sockaddr_in6 in6;

	}
	addr;
	uintptr_t hConn; //HSteamNetConnection hConn; 
	unsigned long long steamid; //uint64 steamID;
}
lhnetaddressnative_t;

// to make LHNETADDRESS_FromString resolve repeated hostnames faster, cache them
#define MAX_NAMECACHE 64
static struct namecache_s
{
	lhnetaddressnative_t address;
	double expirationtime;
	char name[64];
}
namecache[MAX_NAMECACHE];
static int namecacheposition = 0;

// Baker: IPv6 (1 of 9 -- 100%)
int LHNETADDRESS_FromPort(lhnetaddress_t *vaddress, lhnetaddresstype_t addresstype, int port)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	if (!address) 
		return 0;
	switch(addresstype)
	{
	default:
		break;
	case LHNETADDRESSTYPE_LOOP:
		// local:port  (loopback)
		memset(address, 0, sizeof(*address));
		address->addresstype = LHNETADDRESSTYPE_LOOP;
		address->port = port;
		return 1;
	case LHNETADDRESSTYPE_STEAM_FAKEIP:
	case LHNETADDRESSTYPE_INET4:
		// 0.0.0.0:port  (INADDR_ANY, binds to all interfaces)
		memset(address, 0, sizeof(*address));
		address->addresstype = addresstype;
		address->port = port;
		address->addr.in.sin_family = AF_INET;
		address->addr.in.sin_port = htons((unsigned short)port);
		return 1;

	case LHNETADDRESSTYPE_INET6:
		// [0:0:0:0:0:0:0:0]:port  (IN6ADDR_ANY, binds to all interfaces)
		memset(address, 0, sizeof(*address));
		address->addresstype = LHNETADDRESSTYPE_INET6;
		address->port = port;
		address->addr.in6.sin6_family = AF_INET6;
		address->addr.in6.sin6_port = htons((unsigned short)port);
		return 1;

	}
	return 0;
} // LHNETADDRESS_FromPort

static int LHNETADDRESS_Resolve(lhnetaddressnative_t *address, const char *name, int port)
{
	char port_buff [16];
	struct addrinfo hints;
	struct addrinfo* addrinf;
	int err;

	dpsnprintf (port_buff, sizeof (port_buff), "%d", port);
	port_buff[sizeof (port_buff) - 1] = '\0';

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(name, port_buff, &hints, &addrinf);
	if (err != 0 || addrinf == NULL)
		return 0;
	if (addrinf->ai_addr->sa_family != AF_INET6 && addrinf->ai_addr->sa_family != AF_INET)
	{
		freeaddrinfo (addrinf);
		return 0;
	}

	// great it worked
	if (addrinf->ai_addr->sa_family == AF_INET6)
	{
		address->addresstype = LHNETADDRESSTYPE_INET6;
		memcpy(&address->addr.in6, addrinf->ai_addr, sizeof(address->addr.in6));
	}
	else
	{
		address->addresstype = LHNETADDRESSTYPE_INET4;
		memcpy(&address->addr.in, addrinf->ai_addr, sizeof(address->addr.in));
	}
	address->port = port;

	freeaddrinfo (addrinf);
	return 1;
} // LHNETADDRESS_Resolve

// Baker: IPv6 (2 of 9 -- 100% - but needs test for sure)
int LHNETADDRESS_FromString(lhnetaddress_t *vaddress, const char *string, int defaultport)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	int i, port, d1, d2, d3, d4, resolved;
	size_t namelen;
	unsigned char *a;
	char name[128];
	const char *addr_start;
	const char *addr_end = NULL;
	const char *port_name = NULL;
	int addr_family = AF_UNSPEC;

	if (!address || !string || !*string)
		return 0;
	memset(address, 0, sizeof(*address));
	address->addresstype = LHNETADDRESSTYPE_NONE;
	port = 0;

	// If it's a bracketed IPv6 address
	if (string[0] == '[')
	{
		const char *end_bracket = strchr(string, ']');

		if (end_bracket == NULL)
			return 0;

		if (end_bracket[1] == ':')
			port_name = end_bracket + 2;
		else if (end_bracket[1] != '\0')
			return 0;

		addr_family = AF_INET6;
		addr_start = &string[1];
		addr_end = end_bracket;
	}
	else
	{
		const char *first_colon;

		addr_start = string;

		// If it's a numeric non-bracket IPv6 address (-> no port),
		// or it's a numeric IPv4 address, or a name, with a port
		first_colon = strchr(string, ':');
		if (first_colon != NULL)
		{
			const char *last_colon = strrchr(first_colon + 1, ':');

			// If it's an numeric IPv4 address, or a name, with a port
			if (last_colon == NULL)
			{
				addr_end = first_colon;
				port_name = first_colon + 1;
			}
			else
				addr_family = AF_INET6;
		}
	}

	if (addr_end != NULL)
		namelen = addr_end - addr_start;
	else
		namelen = strlen (addr_start);

	if (namelen >= sizeof(name))
		namelen = sizeof(name) - 1;
	memcpy (name, addr_start, namelen);
	name[namelen] = 0;

	if (port_name)
		port = atoi(port_name);

	if (port == 0)
		port = defaultport;

	// handle loopback
	if (String_Match(name, "local"))
	{
		address->addresstype = LHNETADDRESSTYPE_LOOP;
		address->port = port;
		return 1;
	}
	// try to parse as dotted decimal ipv4 address first
	// note this supports partial ip addresses
	d1 = d2 = d3 = d4 = 0;
#if _MSC_VER >= 1400
	#define sscanf sscanf_s
#endif
	if (addr_family != AF_INET6 &&
		sscanf(name, "%d.%d.%d.%d", &d1, &d2, &d3, &d4) >= 1 && (unsigned int)d1 < 256 && (unsigned int)d2 < 256 && (unsigned int)d3 < 256 && (unsigned int)d4 < 256)
	{
		// parsed a valid ipv4 address
		address->addresstype = LHNETADDRESSTYPE_INET4;
		address->port = port;
		address->addr.in.sin_family = AF_INET;
		address->addr.in.sin_port = htons((unsigned short)port);
		a = (unsigned char *)&address->addr.in.sin_addr;
		a[0] = d1;
		a[1] = d2;
		a[2] = d3;
		a[3] = d4;
#ifdef STEAM_NETWORKING // Baker: (2.1)
		if (g_steamNetInitialized)
		{
			if (SteamNet_IsFakeIPAddr((lhnetaddress_t *)address))
			{
				address->addresstype = LHNETADDRESSTYPE_STEAM_FAKEIP;
			}
		}
#endif // STEAM_NETWORKING

		return 1;
	}
	for (i = 0;i < MAX_NAMECACHE;i++)
		if (String_Match(namecache[i].name, name))
			break;
	if (i < MAX_NAMECACHE && host.realtime < namecache[i].expirationtime)
	{
		*address = namecache[i].address;
		address->port = port;
		if (address->addresstype == LHNETADDRESSTYPE_INET6)
		{
			address->addr.in6.sin6_port = htons((unsigned short)port);
			return 1;
		}
// Baker: (2.2)
		else if (isin2(address->addresstype, 
			LHNETADDRESSTYPE_INET4, LHNETADDRESSTYPE_STEAM_FAKEIP))
		{
			address->addr.in.sin_port = htons((unsigned short)port);
#ifdef STEAM_NETWORKING
			if (g_steamNetInitialized)
			{
				if (SteamNet_IsFakeIPAddr((lhnetaddress_t *)address))
				{
					address->addresstype = LHNETADDRESSTYPE_STEAM_FAKEIP;
				}
			}
#endif // STEAM_NETWORKING

			return 1;
		}
		return 0;
	}

	for (i = 0;i < (int)sizeof(namecache[namecacheposition].name)-1 && name[i];i++)
		namecache[namecacheposition].name[i] = name[i];
	namecache[namecacheposition].name[i] = 0;
	namecache[namecacheposition].expirationtime = host.realtime + 12 * 3600; // 12 hours

	// try resolving the address (handles dns and other ip formats)
	resolved = LHNETADDRESS_Resolve(address, name, port);
	if (resolved)
	{
		namecache[namecacheposition].address = *address;
// Baker: Not sure if this needed, does not look like it hurts.
// Baker: (2.3)
#ifdef STEAM_NETWORKING
			if (g_steamNetInitialized)
			{
				if (SteamNet_IsFakeIPAddr((lhnetaddress_t *)address))
				{
					address->addresstype = LHNETADDRESSTYPE_STEAM_FAKEIP;
				}
			}
#endif // STEAM_NETWORKING

	}
	else
	{
#ifdef STANDALONETEST
		printf("name resolution failed on address " QUOTED_S NEWLINE, name);
#endif
		namecache[namecacheposition].address.addresstype = LHNETADDRESSTYPE_NONE;
	}

	namecacheposition = (namecacheposition + 1) % MAX_NAMECACHE;
	return resolved;
} // LHNETADDRESS_FromString

// Baker: IPv6 (3 of 9 -- 100%)
int LHNETADDRESS_ToString(const lhnetaddress_t *vaddress, char *string, int stringbuffersize, int includeport)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	const unsigned char *a;
	if (!address || !string || stringbuffersize < 1)
		return 0;
	*string = 0;
	switch(address->addresstype)
	{
	default:
		break;
	case LHNETADDRESSTYPE_LOOP:
		if (includeport)
		{
			if (stringbuffersize >= 12)
			{
				dpsnprintf(string, stringbuffersize, "local:%d", address->port);
				return 1;
			}
		}
		else
		{
			if (stringbuffersize >= 6)
			{
				memcpy(string, "local", 6);
				return 1;
			}
		}
		break;
	case LHNETADDRESSTYPE_STEAM_FAKEIP:
	case LHNETADDRESSTYPE_INET4:
		a = (const unsigned char *)(&address->addr.in.sin_addr);
		if (includeport)
		{
			if (stringbuffersize >= 22)
			{
				dpsnprintf(string, stringbuffersize, "%d.%d.%d.%d:%d", a[0], a[1], a[2], a[3], address->port);
				return 1;
			}
		}
		else
		{
			if (stringbuffersize >= 16)
			{
				dpsnprintf(string, stringbuffersize, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
				return 1;
			}
		}
		break;
	case LHNETADDRESSTYPE_INET6:
		a = (const unsigned char *)(&address->addr.in6.sin6_addr);
		if (includeport)
		{
			if (stringbuffersize >= 88)
			{
				dpsnprintf(string, stringbuffersize, "[%x:%x:%x:%x:%x:%x:%x:%x]:%d", a[0] * 256 + a[1], a[2] * 256 + a[3], a[4] * 256 + a[5], a[6] * 256 + a[7], a[8] * 256 + a[9], a[10] * 256 + a[11], a[12] * 256 + a[13], a[14] * 256 + a[15], address->port);
				return 1;
			}
		}
		else
		{
			if (stringbuffersize >= 80)
			{
				dpsnprintf(string, stringbuffersize, "%x:%x:%x:%x:%x:%x:%x:%x", a[0] * 256 + a[1], a[2] * 256 + a[3], a[4] * 256 + a[5], a[6] * 256 + a[7], a[8] * 256 + a[9], a[10] * 256 + a[11], a[12] * 256 + a[13], a[14] * 256 + a[15]);
				return 1;
			}
		}
		break;
	}
	return 0;
} // LHNETADDRESS_ToString

int LHNETADDRESS_GetAddressType(const lhnetaddress_t *address)
{
	if (address)
		return address->addresstype;
	else
		return LHNETADDRESSTYPE_NONE;
}

// Baker: IPv6 (4 of 9 -- 100%)
const char *LHNETADDRESS_GetInterfaceName(const lhnetaddress_t *vaddress, char *ifname, size_t ifnamelength)
{

	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;

	if (address && address->addresstype == LHNETADDRESSTYPE_INET6)
	{
#ifndef _WIN32
		// Baker: Linux, etc.
		if (if_indextoname(address->addr.in6.sin6_scope_id, ifname) == ifname)
			return ifname;

#else
		// Baker: Win 32 ..
		// The Win32 API doesn't have if_indextoname() until Windows Vista,
		// but luckily it just uses the interface ID as the interface name

		if (dpsnprintf(ifname, ifnamelength, "%lu", address->addr.in6.sin6_scope_id) > 0)
			return ifname;

#endif // #ifndef _WIN32
	}

	return NULL;
}

int LHNETADDRESS_GetPort(const lhnetaddress_t *address)
{
	if (!address)
		return -1;
	return address->port;
}

// Baker: IPv6 (5 of 9 -- 100%)
int LHNETADDRESS_SetPort(lhnetaddress_t *vaddress, int port)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	if (!address)
		return 0;
	address->port = port;
	switch(address->addresstype)
	{
	case LHNETADDRESSTYPE_LOOP:
		return 1;
	case LHNETADDRESSTYPE_STEAM_FAKEIP:
	case LHNETADDRESSTYPE_INET4:
		address->addr.in.sin_port = htons((unsigned short)port);
		return 1;

	case LHNETADDRESSTYPE_INET6:
		address->addr.in6.sin6_port = htons((unsigned short)port);
		return 1;

	default:
		return 0;
	}
}

// Baker: IPv6 (6 of 9 -- 100%)
int LHNETADDRESS_Compare(const lhnetaddress_t *vaddress1, const lhnetaddress_t *vaddress2)
{
	lhnetaddressnative_t *address1 = (lhnetaddressnative_t *)vaddress1;
	lhnetaddressnative_t *address2 = (lhnetaddressnative_t *)vaddress2;
	if (!address1 || !address2)
		return 1;
	if (address1->addresstype != address2->addresstype)
		return 1;
	switch(address1->addresstype)
	{
	case LHNETADDRESSTYPE_LOOP:
		if (address1->port != address2->port)
			return -1;
		return 0;
	case LHNETADDRESSTYPE_STEAM_FAKEIP:
	case LHNETADDRESSTYPE_INET4:
		if (address1->addr.in.sin_family != address2->addr.in.sin_family)
			return 1;
		if (memcmp(&address1->addr.in.sin_addr, &address2->addr.in.sin_addr, sizeof(address1->addr.in.sin_addr)))
			return 1;
		if (address1->port != address2->port)
			return -1;
		return 0;

	case LHNETADDRESSTYPE_INET6:
		if (address1->addr.in6.sin6_family != address2->addr.in6.sin6_family)
			return 1;
		if (memcmp(&address1->addr.in6.sin6_addr, &address2->addr.in6.sin6_addr, sizeof(address1->addr.in6.sin6_addr)))
			return 1;
		if (address1->port != address2->port)
			return -1;
		return 0;

	default:
		return 1;
	}
}

typedef struct lhnetpacket_s
{
	void *data;
	int length;
	int sourceport;
	int destinationport;
	time_t timeout;

	double sentdoubletime;

	llist_t list;
}
lhnetpacket_t;

static int lhnet_active;
static lhnetsocket_t lhnet_socketlist; // Baker: The socket list equiv of netv4_broadcastsocket
static lhnetpacket_t lhnet_packetlist;
static int lhnet_default_dscp = 0;
#ifdef _WIN32
	static int lhnet_didWSAStartup = 0;
	static WSADATA lhnet_winsockdata;
#endif

void LHNET_Init(void)
{
	if (lhnet_active) 
		return;
	List_Create(&lhnet_socketlist.list);
	List_Create(&lhnet_packetlist.list);
	lhnet_active = 1;
#ifdef _WIN32
	lhnet_didWSAStartup = !WSAStartup(MAKEWORD(1, 1), &lhnet_winsockdata);
	if (!lhnet_didWSAStartup)
		Con_PrintLinef("LHNET_Init: WSAStartup failed, networking disabled");
#endif // _WIN32
}

int LHNET_DefaultDSCP(int dscp)
{
#ifdef IP_TOS // Baker: 3 .. defined at least on Windows
	// Baker: This is the norm for at least Windows
	int prev = lhnet_default_dscp;
	if (dscp >= 0) 
		lhnet_default_dscp = dscp;
	return prev;
#else
	return -1;
#endif
}

void LHNET_Shutdown(void)
{
	lhnetsocket_t *s, *snext;
	lhnetpacket_t *p, *pnext;
	if (!lhnet_active)
		return;
	List_For_Each_Entry_Safe(s, snext, &lhnet_socketlist.list, lhnetsocket_t, list)
		LHNET_CloseSocket(s);
	List_For_Each_Entry_Safe(p, pnext, &lhnet_packetlist.list, lhnetpacket_t, list)
	{
		List_Delete(&p->list);
		Z_Free(p);
	}
#ifdef _WIN32
	if (lhnet_didWSAStartup)
	{
		lhnet_didWSAStartup = 0;
		WSACleanup();
	}
#endif // _WIN32
	lhnet_active = 0;
}

static const char *LHNETPRIVATE_StrError(void)
{
#ifdef _WIN32
	int i = WSAGetLastError();
	switch (i)
	{
		case WSAEINTR:           return "WSAEINTR";
		case WSAEBADF:           return "WSAEBADF";
		case WSAEACCES:          return "WSAEACCES";
		case WSAEFAULT:          return "WSAEFAULT";
		case WSAEINVAL:          return "WSAEINVAL";
		case WSAEMFILE:          return "WSAEMFILE";
		case WSAEWOULDBLOCK:     return "WSAEWOULDBLOCK";
		case WSAEINPROGRESS:     return "WSAEINPROGRESS";
		case WSAEALREADY:        return "WSAEALREADY";
		case WSAENOTSOCK:        return "WSAENOTSOCK";
		case WSAEDESTADDRREQ:    return "WSAEDESTADDRREQ";
		case WSAEMSGSIZE:        return "WSAEMSGSIZE";
		case WSAEPROTOTYPE:      return "WSAEPROTOTYPE";
		case WSAENOPROTOOPT:     return "WSAENOPROTOOPT";
		case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
		case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
		case WSAEOPNOTSUPP:      return "WSAEOPNOTSUPP";
		case WSAEPFNOSUPPORT:    return "WSAEPFNOSUPPORT";
		case WSAEAFNOSUPPORT:    return "WSAEAFNOSUPPORT";
		case WSAEADDRINUSE:      return "WSAEADDRINUSE";
		case WSAEADDRNOTAVAIL:   return "WSAEADDRNOTAVAIL";
		case WSAENETDOWN:        return "WSAENETDOWN";
		case WSAENETUNREACH:     return "WSAENETUNREACH";
		case WSAENETRESET:       return "WSAENETRESET";
		case WSAECONNABORTED:    return "WSAECONNABORTED";
		case WSAECONNRESET:      return "WSAECONNRESET";
		case WSAENOBUFS:         return "WSAENOBUFS";
		case WSAEISCONN:         return "WSAEISCONN";
		case WSAENOTCONN:        return "WSAENOTCONN";
		case WSAESHUTDOWN:       return "WSAESHUTDOWN";
		case WSAETOOMANYREFS:    return "WSAETOOMANYREFS";
		case WSAETIMEDOUT:       return "WSAETIMEDOUT";
		case WSAECONNREFUSED:    return "WSAECONNREFUSED";
		case WSAELOOP:           return "WSAELOOP";
		case WSAENAMETOOLONG:    return "WSAENAMETOOLONG";
		case WSAEHOSTDOWN:       return "WSAEHOSTDOWN";
		case WSAEHOSTUNREACH:    return "WSAEHOSTUNREACH";
		case WSAENOTEMPTY:       return "WSAENOTEMPTY";
		case WSAEPROCLIM:        return "WSAEPROCLIM";
		case WSAEUSERS:          return "WSAEUSERS";
		case WSAEDQUOT:          return "WSAEDQUOT";
		case WSAESTALE:          return "WSAESTALE";
		case WSAEREMOTE:         return "WSAEREMOTE";
		case WSAEDISCON:         return "WSAEDISCON";
		case 0:                  return "no error";
		default:                 return "unknown WSAE error";
	}
#else // _WIN32
	return strerror(errno);
#endif // !_WIN32
}

void LHNET_SleepUntilPacket_Microseconds(int microseconds)
{
#ifdef FD_SET
	fd_set fdreadset;
	struct timeval tv;
	int lastfd;
	lhnetsocket_t *s;
	FD_ZERO(&fdreadset);
	lastfd = 0;
	List_For_Each_Entry(s, &lhnet_socketlist.list, lhnetsocket_t, list)
	{
		if (s->address.addresstype == LHNETADDRESSTYPE_INET4 || s->address.addresstype == LHNETADDRESSTYPE_INET6)
		{
			if (lastfd < s->inetsocket)
				lastfd = s->inetsocket;
#if defined(_WIN32) && !defined(_MSC_VER)
			FD_SET((int)s->inetsocket, &fdreadset);
#else
			FD_SET((unsigned int)s->inetsocket, &fdreadset);
#endif
		}
	}
	tv.tv_sec = microseconds / 1000000;
	tv.tv_usec = microseconds % 1000000;
	select(lastfd + 1, &fdreadset, NULL, NULL, &tv);
#else
	Sys_Sleep(microseconds);
#endif
}

// Baker: IPv6 (7 of 9 -- 100%)
lhnetsocket_t *LHNET_OpenSocket_Connectionless(lhnetaddress_t *address)
{
	lhnetsocket_t *lhnetsocket, *s;
	if (!address)
		return NULL;
	lhnetsocket = (lhnetsocket_t *)Mem_Alloc(netconn_mempool, sizeof(*lhnetsocket)); // CMDCVARMEM: Baker changed to netconn mempool
	if (lhnetsocket)
	{
		memset(lhnetsocket, 0, sizeof(*lhnetsocket));
		lhnetsocket->address = *address;
		switch(lhnetsocket->address.addresstype)
		{
		case LHNETADDRESSTYPE_LOOP:
			if (lhnetsocket->address.port == 0)
			{
				// allocate a port dynamically
				// this search will always terminate because there is never
				// an allocated socket with port 0, so if the number wraps it
				// will find the port is unused, and then refuse to use port
				// 0, causing an intentional failure condition
				lhnetsocket->address.port = 1024;
				for (;;)
				{
					List_For_Each_Entry(s, &lhnet_socketlist.list, lhnetsocket_t, list)
						if (s->address.addresstype == lhnetsocket->address.addresstype && s->address.port == lhnetsocket->address.port)
							break;
					if (s == &lhnet_socketlist)
						break;
					lhnetsocket->address.port++;
				}
			}
			// check if the port is available
			List_For_Each_Entry(s, &lhnet_socketlist.list, lhnetsocket_t, list)
				if (s->address.addresstype == lhnetsocket->address.addresstype && s->address.port == lhnetsocket->address.port)
					break;
			if (s == &lhnet_socketlist && lhnetsocket->address.port != 0)
			{
				List_Add_Tail(&lhnetsocket->list, &lhnet_socketlist.list);
				return lhnetsocket;
			}
			break;
		case LHNETADDRESSTYPE_STEAM_FAKEIP:
		case LHNETADDRESSTYPE_INET4:
// Baker: (7.x) December 6 2025 - 
		case LHNETADDRESSTYPE_INET6:
#ifdef _WIN32
			if (lhnet_didWSAStartup)
			{
#endif // _WIN32

// Baker: (7.1)
				if ((lhnetsocket->inetsocket = socket(address->addresstype == LHNETADDRESSTYPE_INET6 ? PF_INET6 : PF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1)
				// Baker: Not this I don't think ... 
				//if ((lhnetsocket->inetsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1)
				{
#ifdef _WIN32
					u_long _false = 0;
#endif // _WIN32
#ifdef MSG_DONTWAIT
					if (1)
#else
	#ifdef _WIN32
					u_long _true = 1;
	#else
					char _true = 1;
	#endif // _WIN32
					// Baker: FIONBIO sets blocking, non-zero means Non-Blocking
					if (ioctlsocket(lhnetsocket->inetsocket, FIONBIO, &_true) != -1)
#endif // !MSG_DONTWAIT
					{
#ifdef IPV6_V6ONLY
						// We need to set this flag to tell the OS that we only listen on IPv6. If we don't
						// most OSes will create a dual-protocol socket that also listens on IPv4. In this case
						// if an IPv4 socket is already bound to the port we want, our bind() call will fail.
						int ipv6_only = 1;
						if (address->addresstype != LHNETADDRESSTYPE_INET6
							|| setsockopt (lhnetsocket->inetsocket, IPPROTO_IPV6, IPV6_V6ONLY,
										   (const char *)&ipv6_only, sizeof(ipv6_only)) == 0
#ifdef _WIN32
							// The Win32 API only supports IPV6_V6ONLY since Windows Vista, but fortunately
							// the default value is what we want on Win32 anyway (IPV6_V6ONLY = true)
							|| SOCKETERRNO == WSAENOPROTOOPT
#endif // _WIN32
							)
#endif // IPV6_V6ONLY
						{
							lhnetaddressnative_t *localaddress = (lhnetaddressnative_t *)&lhnetsocket->address;
							SOCKLEN_T namelen;
							int bindresult;

#if defined(SOL_RFC1149) && defined(RFC1149_1149ONLY)
							// we got reports of massive lags when this protocol was chosen as transport
							// so better turn it off
							{
								int rfc1149only = 0;
								int rfc1149enabled = 0;
								if (setsockopt(lhnetsocket->inetsocket, SOL_RFC1149, RFC1149_1149ONLY, &rfc1149only))
									Con_Printf (CON_ERROR "LHNET_OpenSocket_Connectionless: warning: setsockopt(RFC1149_1149ONLY) returned error: %s\n", LHNETPRIVATE_StrError());
								if (setsockopt(lhnetsocket->inetsocket, SOL_RFC1149, RFC1149_ENABLED, &rfc1149enabled))
									Con_Printf (CON_ERROR "LHNET_OpenSocket_Connectionless: warning: setsockopt(RFC1149_ENABLED) returned error: %s\n", LHNETPRIVATE_StrError());
							}
#endif

// Baker: (7.2)
							if (address->addresstype == LHNETADDRESSTYPE_INET6)
							{
								namelen = sizeof(localaddress->addr.in6);
								bindresult = bind(lhnetsocket->inetsocket, &localaddress->addr.sock, namelen);
								if (bindresult != -1)
								{
									if (getsockname(lhnetsocket->inetsocket, &localaddress->addr.sock, &namelen))
									{
										// If getsockname failed, we can assume the bound socket is useless.
										bindresult = -1;
									}
								}
							}
							else
							{
								namelen = sizeof(localaddress->addr.in);
								bindresult = bind(lhnetsocket->inetsocket, &localaddress->addr.sock, namelen);
								if (bindresult != -1)
								{
									if (getsockname(lhnetsocket->inetsocket, &localaddress->addr.sock, &namelen))
									{
										// If getsockname failed, we can assume the bound socket is useless.
										bindresult = -1;
									}
								}
							}
							if (bindresult != -1)
							{
								int i = 1;
								// int setsockopt(int socket, int level,
								// int option_name,
								// const void *option_value, socklen_t option_len);
								// enable broadcast on this socket
								//static void UDP4_GetLocalAddress (void)
								//{
								//	if (myAddrv4 == INADDR_ANY) {
								//		myAddrv4 = UDP4_GetHostNameIP (NULL, 0, my_ipv4_address, sizeof(my_ipv4_address));
								//	}
								//}

								// int res =
								setsockopt(lhnetsocket->inetsocket, 
									SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i));
#ifdef IP_TOS // Baker: 3 .. For certain this is defined on Windows
	// Baker: This is the norm on Windows at a minimum
								{
									// enable DSCP for ToS support
									int tos = lhnet_default_dscp << 2;
									if (setsockopt(lhnetsocket->inetsocket, IPPROTO_IP, IP_TOS, (char *) &tos, sizeof(tos)))
									{
										// Error in setsockopt - fine, we'll simply set no TOS then.
									}
								}
#endif
								List_Add_Tail(&lhnetsocket->list, &lhnet_socketlist.list);
#ifdef _WIN32
								if (ioctlsocket(lhnetsocket->inetsocket, SIO_UDP_CONNRESET, &_false) == -1)
									Con_DPrintf ("LHNET_OpenSocket_Connectionless: ioctlsocket SIO_UDP_CONNRESET returned error: %s\n", LHNETPRIVATE_StrError());
#endif
								return lhnetsocket;
							}
							else
								Con_Printf ("LHNET_OpenSocket_Connectionless: bind returned error: %s\n", LHNETPRIVATE_StrError());
						}
// Baker:  IPV6_V6ONLY constant is defined in the 
// <netinet/in.h> header file
#ifdef IPV6_V6ONLY
						else
							Con_Printf ("LHNET_OpenSocket_Connectionless: setsockopt(IPV6_V6ONLY) returned error: %s\n", LHNETPRIVATE_StrError());
#endif
					}
					else
						Con_Printf ("LHNET_OpenSocket_Connectionless: ioctlsocket returned error: %s\n", LHNETPRIVATE_StrError());
					closesocket(lhnetsocket->inetsocket);
				}
				else
					Con_Printf ("LHNET_OpenSocket_Connectionless: socket returned error: %s\n", LHNETPRIVATE_StrError());
#ifdef _WIN32
			}
			else
				Con_Print("LHNET_OpenSocket_Connectionless: can't open a socket (WSAStartup failed during LHNET_Init)\n");
#endif
			break;
		default:
			break;
		}
		Z_Free(lhnetsocket);
	}
	return NULL;
} // LHNET_OpenSocket_Connectionless

//KleskBY Fix me
// Baker: IPv6 (8 of 9 -- 100%)
void LHNET_CloseSocket(lhnetsocket_t *lhnetsocket)
{
// Baker: LHNETADDRESSTYPE_LOOP does not close socket.
	if (lhnetsocket)
	{
		List_Delete(&lhnetsocket->list);
		// no special close code for loopback, just inet
		// Baker: LHNETADDRESSTYPE_LOOP is the loopback type
		if (isin3(lhnetsocket->address.addresstype,
		 LHNETADDRESSTYPE_INET4,
		 LHNETADDRESSTYPE_INET6,
		 LHNETADDRESSTYPE_STEAM_FAKEIP))
		{
			closesocket(lhnetsocket->inetsocket);
		}
		Z_Free(lhnetsocket);
	}
} // LHNET_CloseSocket

lhnetaddress_t *LHNET_AddressFromSocket(lhnetsocket_t *sock)
{
	if (sock)
		return &sock->address;
	else
		return NULL;
}

// Baker: IPv6 (9 of 9 -- 100%)
int LHNET_Read(lhnetsocket_t *lhnetsocket, void *content, int maxcontentlength, lhnetaddress_t *vaddress)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	int value = 0;
	if (!lhnetsocket || !address || !content || maxcontentlength < 1)
		return -1;
	if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_LOOP)
	{
		time_t currenttime;
		lhnetpacket_t *p, *pnext;
		// scan for any old packets to timeout while searching for a packet
		// that is waiting to be delivered to this socket
		currenttime = time(NULL);
		List_For_Each_Entry_Safe(p, pnext, &lhnet_packetlist.list, lhnetpacket_t, list)
		{
			if (p->timeout < currenttime)
			{
				// unlink and free
				List_Delete(&p->list);
				Z_Free(p);
				continue;
			}

			if (net_fakelag.value && (host.realtime - net_fakelag.value * (1.0 / 2000.0)) < p->sentdoubletime)
				continue;

			if (value == 0 && p->destinationport == lhnetsocket->address.port)
			{
				if (p->length <= maxcontentlength)
				{
					lhnetaddressnative_t *localaddress = (lhnetaddressnative_t *)&lhnetsocket->address;
					*address = *localaddress;
					address->port = p->sourceport;
					memcpy(content, p->data, p->length);
					value = p->length;
				}
				else 
					value = -1;

				// unlink and free
				List_Delete(&p->list);
				Z_Free(p);
			}
		}
	}
	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_INET4)
	{
		SOCKLEN_T inetaddresslength;
		address->addresstype = LHNETADDRESSTYPE_NONE;
		inetaddresslength = sizeof(address->addr.in);
		value = recvfrom(lhnetsocket->inetsocket, (char*)content, maxcontentlength, LHNET_RECVFROM_FLAGS, &address->addr.sock, &inetaddresslength);
		if (value > 0)
		{
			address->addresstype = LHNETADDRESSTYPE_INET4;
			address->port = ntohs(address->addr.in.sin_port);
			return value;
		}
		else if (value < 0)
		{
			int e = SOCKETERRNO;
			if (e == EWOULDBLOCK)
				return 0;
			switch (e)
			{
			case ECONNREFUSED:
				Con_PrintLinef("Connection refused");
				return 0;
			}
			Con_DPrintLinef("LHNET_Read: recvfrom returned error: %s", LHNETPRIVATE_StrError());
		}
	}

	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_INET6)
	{
		SOCKLEN_T inetaddresslength;
		address->addresstype = LHNETADDRESSTYPE_NONE;
		inetaddresslength = sizeof(address->addr.in6);
		value = recvfrom(lhnetsocket->inetsocket, (char *)content, maxcontentlength, LHNET_RECVFROM_FLAGS, &address->addr.sock, &inetaddresslength);
		if (value > 0)
		{
			address->addresstype = LHNETADDRESSTYPE_INET6;
			address->port = ntohs(address->addr.in6.sin6_port);
			return value;
		}
		else if (value == -1)
		{
			int e = SOCKETERRNO;
			if (e == EWOULDBLOCK)
				return 0;
			switch (e)
			{
				case ECONNREFUSED:
					Con_PrintLinef("Connection refused");
					return 0;
			}
			Con_DPrintf ("LHNET_Read: recvfrom returned error: %s\n", LHNETPRIVATE_StrError());
		}
	}


#ifdef STEAM_NETWORKING
	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_STEAM_FAKEIP)
	{
		value = SteamNet_Read(lhnetsocket, content, maxcontentlength, vaddress);
	}
#endif // STEAM_NETWORKING
	return value;
}

// Baker: IPv6 (10 of 9 -- 100%)
int LHNET_Write(lhnetsocket_t *lhnetsocket, const void *content, int contentlength, const lhnetaddress_t *vaddress)
{
	lhnetaddressnative_t *address = (lhnetaddressnative_t *)vaddress;
	int value = -1;
	if (!lhnetsocket || !address || !content || contentlength < 1)
	{
		return -1;
	}
	if (lhnetsocket->address.addresstype != address->addresstype)
	{
		return -1;
	}
	if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_LOOP)
	{
		lhnetpacket_t *p;
		p = (lhnetpacket_t *)Mem_Alloc(netconn_mempool, sizeof(*p) + contentlength); // CMDCVARMEM
		p->data = (void *)(p + 1);
		memcpy(p->data, content, contentlength);
		p->length = contentlength;
		p->sourceport = lhnetsocket->address.port;
		p->destinationport = address->port;
		p->timeout = time(NULL) + 10;
		List_Add_Tail(&p->list, &lhnet_packetlist.list);


		p->sentdoubletime = host.realtime;

		value = contentlength;
	}
	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_INET4)
	{
		value = sendto(lhnetsocket->inetsocket, (char*)content, contentlength, LHNET_SENDTO_FLAGS, (struct sockaddr*)&address->addr.in, sizeof(struct sockaddr_in));
		if (value == -1)
		{
			if (SOCKETERRNO == EWOULDBLOCK)
				return 0;
			Con_DPrintf("LHNET_Write: sendto returned error: %s\n", LHNETPRIVATE_StrError());
		}
	}

	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_INET6)
	{
		value = sendto(lhnetsocket->inetsocket, (char *)content, contentlength, 0, (struct sockaddr *)&address->addr.in6, sizeof(struct sockaddr_in6));
		if (value == -1)
		{
			if (SOCKETERRNO == EWOULDBLOCK)
				return 0;
			Con_DPrintLinef ("LHNET_Write: sendto returned error: %s", LHNETPRIVATE_StrError());
		}
	}


#ifdef STEAM_NETWORKING
	else if (lhnetsocket->address.addresstype == LHNETADDRESSTYPE_STEAM_FAKEIP)
	{
		value = SteamNet_Write(lhnetsocket, content, contentlength, vaddress);
	}
#endif // STEAM_NETWORKING
	return value;
}


// Baker: I added all of the following code .. why?
// Is this reverse DNS?  Explain ...
// Or was this required to get MinGW to compile Windows version?

#define MAXHOSTNAMELEN_256	256
#define NET_NAMELEN_64		64
#ifndef in_addr_t
	#define in_addr_t u_long
#endif


int UDP4_GetHostNameIP (char *namebuf, size_t namebuf_size, char * ipbuf, size_t ipbuf_size)
{
	char buff[MAXHOSTNAMELEN_256];

    if (gethostname(buff, sizeof(buff)) == -1 /*SOCKET_ERROR*/) {
		int err = SOCKETERRNO;
		Con_PrintLinef ("UDP4_GetHostName: gethostname failed (%d)", err); //socketerror(err));
		return false;
	}

	buff[sizeof(buff) - 1] = 0;


	do {
		struct hostent *local = gethostbyname(buff);
		in_addr_t netaddr;
		if (local == NULL) {
			int errcode = SOCKETERRNO;
			Con_PrintLinef ("UDP4_GetHostName: gethostbyname failed for hostname [%s](%d)", buff, errcode /*socketerror(err)*/); // This probably never happens.
			return 0;
		}
		else if (local->h_addrtype != AF_INET) {
			Con_PrintLinef ("UDP4_GetHostName: address from gethostbyname not IPv4");
			return 0;
		}

		// Success
		netaddr = *(in_addr_t *)local->h_addr_list[0];

		if (namebuf) {
			// Copy out the name buf if we have one
			strlcpy (namebuf, buff, namebuf_size);
		}

		if (ipbuf) {
			// If ip address, fill that in.
			// alert ("%p: %d", ipbuf, (int)ipbuf_size);
			in_addr_t	haddr = ntohl(netaddr); // Net byte order to host order
			// int is the right tool for this job.
			//c_snprintfc (ipbuf, ipbuf_size,  "%ld.%ld.%ld.%ld", (haddr >> 24) & 0xff, (haddr >> 16) & 0xff, (haddr >> 8) & 0xff, haddr & 0xff);
			dpsnprintf (ipbuf, ipbuf_size,  "%d.%d.%d.%d", (int)((haddr >> 24) & 0xff), (int)((haddr >> 16) & 0xff), (int)((haddr >> 8) & 0xff), (int)(haddr & 0xff));
		}
		return netaddr; // In network form.
	} while (0);
}

struct qsockaddr
{
#if defined(HAVE_SA_LEN)
	unsigned char qsa_len;
	unsigned char qsa_family;
#else
	short qsa_family;
#endif	/* BSD, sockaddr */
	unsigned char qsa_data[NET_NAMELEN_64]; // NETQ 3.1 NET_NAMELEN instead?
};


#if defined(__GNUC__) // && defined(_WIN32) //&& !defined(PLATFORM_OSX) // Fuck you mingw headers
    #define WIN32_LEAN_AND_MEAN
    #define IDE_COMES_WITH_GOOD_IPV6_HEADERS 0
	#pragma message ("Baker: MinGW headers bad for sockets/IPV6")
    // Sadly, MinGW version that comes with CodeBlocks the headers are not good
    // I want things to compile nice and easy out of the box, without errands and other foolishness.
#ifndef ULONG
	#define ULONG unsigned int
#endif
#ifndef USHORT
	#define USHORT unsigned short
#endif
#ifndef UCHAR
	#define UCHAR unsigned char
#endif

#ifndef _WIN32 // Baker: Jeez
    typedef struct {
        union {
            struct {
                ULONG Zone : 28;
                ULONG Level : 4;
            };
            ULONG Value;
        };
    } SCOPE_ID, *PSCOPE_ID;
#endif

  // This is what comes with mingw for CodeBlocks 13.12
	struct sockaddr_in6_mingw {
	    short   sin6_family;        /* AF_INET6 */
	    u_short sin6_port;          /* Transport level port number */
	    u_long  sin6_flowinfo;      /* IPv6 flow information */
	    struct in6_addr sin6_addr;  /* IPv6 address */
	    u_long sin6_scope_id;       /* set of interfaces for a scope */
	};



    typedef struct in6_addr_correctly {
        union {
            UCHAR       Byte[16];
            USHORT      Word[8];
        } u;
    } IN6_ADDR_EX; // , *PIN6_ADDR, FAR *LPIN6_ADDR;

    struct sockaddr_in6_EX {
        USHORT sin6_family; // AF_INET6.
        USHORT sin6_port;           // Transport level port number.
        ULONG  sin6_flowinfo;       // IPv6 flow information.
        IN6_ADDR_EX sin6_addr;         // IPv6 address.
        union {
            ULONG sin6_scope_id;     // Set of interfaces for a scope.
            SCOPE_ID sin6_scope_struct;
        };
    }; // SOCKADDR_IN6_LH, *PSOCKADDR_IN6_LH, FAR *LPSOCKADDR_IN6_LH;

    #define sockaddr_in6_HAX sockaddr_in6_EX
#else // !s6_addr

	#define IDE_COMES_WITH_GOOD_IPV6_HEADERS 1
	#define sockaddr_in6_HAX sockaddr_in6 // 'struct in6_addr' has no member named 'u'|
#endif

const char *UDP_AddrToString (struct qsockaddr *addr, int masked)
{
	//static char buffer[22]; // 192.168.100.100:26001 is 21 chars
	static char buffer[64];
	int		haddr;

	if (addr->qsa_family == AF_INET6)
	{
		if (masked)
		{
			c_dpsnprintf4 (buffer, "[%x:%x:%x:%x::]/64",
						ntohs((
             (struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[0]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[1]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[2]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[3]));
		}
		else
		{
			if (((struct sockaddr_in6 *)addr)->sin6_scope_id)
			{
				c_dpsnprintf10 (buffer, "[%x:%x:%x:%x:%x:%x:%x:%x%%%d]:%d",
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[0]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[1]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[2]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[3]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[4]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[5]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[6]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[7]),
						(int)((struct sockaddr_in6_HAX *)addr)->sin6_scope_id,
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_port));
			}
			else
			{
				c_dpsnprintf9 (buffer, "[%x:%x:%x:%x:%x:%x:%x:%x]:%d",
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[0]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[1]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[2]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[3]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[4]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[5]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[6]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_addr.u.Word[7]),
						ntohs(((struct sockaddr_in6_HAX *)addr)->sin6_port));
			}
		}
	}
	else
	{
		haddr = ntohl(((struct sockaddr_in *)addr)->sin_addr.s_addr);
		if (masked)
		{
			c_dpsnprintf3 (buffer, "%d.%d.%d.0/24", (haddr >> 24) & 0xff,
					  (haddr >> 16) & 0xff, (haddr >> 8) & 0xff);
		}
		else
		{
			c_dpsnprintf5 (buffer, "%d.%d.%d.%d:%d", (haddr >> 24) & 0xff,
					  (haddr >> 16) & 0xff, (haddr >> 8) & 0xff, haddr & 0xff,
					  ntohs(((struct sockaddr_in *)addr)->sin_port));
		}
	}

	return buffer;
}



int UDP6_GetHostNameIP (char *namebuf, size_t namebuf_size, char *ipbuf, size_t ipbuf_size)
{
	char		buff[MAXHOSTNAMELEN_256];
	struct addrinfo hints, *local = NULL;

//	if (myAddrv6 != IN6ADDR_ANY)
//		return;

	if (gethostname(buff, MAXHOSTNAMELEN_256) == -1 /*SOCKET_ERROR*/) {
		int	errcode = SOCKETERRNO;
		Con_PrintLinef ("UDP6_GetLocalAddress: gethostname failed (%d)", errcode /*socketerror(err)*/);
		return false;
	}
	buff[MAXHOSTNAMELEN_256 - 1] = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	if (getaddrinfo(buff, NULL, &hints, &local) == 0) {
		lhnetaddress_t addy;
		memset (&addy, 0, sizeof(addy));
		addy.addresstype = LHNETADDRESSTYPE_INET6;
		addy.port = 0;
	#if 0 // gcc unused
		int sz = sizeof(addy.storage);
	#endif
		memcpy (addy.storage, local, sizeof(addy.storage));
		int length;
		strlcpy (ipbuf, UDP_AddrToString((struct qsockaddr*)local->ai_addr, false), ipbuf_size);
#if 1
		length = (int)strlen(ipbuf);
		if (length > 2 && String_Match(ipbuf + length - 2, ":0"))
			ipbuf[length - 2] = 0;
#endif
		freeaddrinfo(local);
	}

	if (local == NULL) {
		int	errcode = SOCKETERRNO;
		if (errcode) { // Linux returns SOCKET_ERROR with error code 0 (success) for anything that resolves to 127.0.0.1
            Con_PrintLinef ("UDP6_GetLocalAddress: gethostbyname failed (#%d: %d)", errcode, errcode/* socketerror(err)*/);
            return false;
		}
	}
	return true;
}



