////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Modified for Linux from lwip__Sockets.cpp on NETMF 4.1 by Device Drivers Limited.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EddyLinux_sockets.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <asm/ioctls.h>
#include <netdb.h>

extern const HAL_CONFIG_BLOCK   g_NetworkConfigHeader;
extern NETWORK_CONFIG           g_NetworkConfig;

//--// 

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
#endif

#if !defined(BUILD_RTM)
void printfd(char *s, fd_set* fds);
void printfd(char *s, fd_set* fds)
{
        unsigned int *p;
		p = (unsigned int *) fds;

		if (LINUXDBG <= 3) return;

        if (p != NULL) {
                printf("%s%08x %08x %08x %08x\r\n", s, *p, *(p+1), *(p+2), *(p+3));
        }
        else
                printf("%sNULL\r\n", s);
}

void printsockfd(char *s, SOCK_fd_set *fds);
void printsockfd(char *s, SOCK_fd_set *fds)
{
        int i;
        unsigned int *p = (unsigned int *) &fds->fd_array[0];
		int maxcnt = 0;
		if (LINUXDBG > 3)
			maxcnt = 1;
		else
			return;

        if (p != NULL) {
                printf("%s count=%d\n", s, fds->fd_count);
                for(i = 0; i < maxcnt; i++) {
                        printf("%08x %08x %08x %08x %08x %08x %08x %08x\r\n",
                               *p, *(p+1), *(p+2), *(p+3), *(p+4), *(p+5), *(p+6), *(p+7));
                        p += 8;
                }
        }
        else
                printf("%sNULL\r\n", s);
}

#define PRINTFD(a, b) printfd(a, b)
#define PRINTSOCKFD(a, b) printsockfd(a, b)

#else
#define PRINTFD(a, b)
#define PRINTSOCKFD(a, b)
#endif

int sockfdset_to_fdset(SOCK_fd_set *sockfds, fd_set *fds);
int sockfdset_to_fdset(SOCK_fd_set *sockfds, fd_set *fds)
{
        int i;
        for(i = 0; i < sockfds->fd_count; i++)
        {
                FD_SET(sockfds->fd_array[i], fds);
        }
        return(sockfds->fd_count);
}

int fdset_to_sockfdset(fd_set *fds, SOCK_fd_set *sockfds);
int fdset_to_sockfdset(fd_set *fds, SOCK_fd_set *sockfds)
{
        int i;
        for(i = 0; i < 128; i++)
        {
                if (FD_ISSET(i, fds))
                {
                        SOCK_FD_SET(i, sockfds);
                }
        }
        return(sockfds->fd_count);
}

//--//

BOOL LINUX_SOCKETS_Driver::Initialize()
{
    return TRUE;
}

BOOL LINUX_SOCKETS_Driver::Uninitialize()
{
    return TRUE;
}


SOCK_SOCKET LINUX_SOCKETS_Driver::Socket(int family, int type, int protocol) 
{  
    NATIVE_PROFILE_PAL_NETWORK();

	int ret = socket(family, type, protocol);
	DBGP("family=%d, type=%d, protocol=%d, ret = %d", family, type, protocol, ret);
	if (ret >= 0) return (SOCK_SOCKET) ret;

	debug_printf("socket() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Bind(SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();

	int ret = bind(socket, (const sockaddr*) address, addressLen);
	DBGP("socket=%d, address=%p, addressLen=%d, ret=%d", socket, address, addressLen, ret);
	if (ret >= 0) return ret;

	debug_printf("bind() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Connect(SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    
	int ret = connect(socket, (const sockaddr*) address, addressLen);
	DBGP("socket=%d, address=%p, addressLen=%d, ret=%d, errno=%d", socket, address, addressLen, ret, errno);
	if (ret >= 0) return ret;

	//if (errno == EINPROGRESS || errno == EAGAIN)
	if (errno == EINPROGRESS)
	{
		DBGP("connect() return: %s(%d), connection process has started.\r\n",
			strerror(errno), errno);

		return SOCK_EWOULDBLOCK;
	}

	debug_printf("connect() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Send(SOCK_SOCKET socket, const char* buf, int len, int flags) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();
        
	int ret = send(socket, buf, len, flags);
	DBGP("socket=%d, buf=%p, len=%d, flags=%x, ret=%d", socket, buf, len, flags, ret);
	if (ret >= 0) return ret;

	debug_printf("send() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}


int LINUX_SOCKETS_Driver::Recv(SOCK_SOCKET socket, char* buf, int len, int flags)
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int     nativeFlag;

    switch (flags)
    {
        case SOCKET_READ_PEEK_OPTION:
            nativeFlag = MSG_PEEK;
            break;
        default:
            nativeFlag = flags;
            break;
    }
    
	int ret = recv(socket, buf, len, nativeFlag);
	DBGP("socket=%d, buf=%p, len=%d, nativeFlag=%d, ret=%d", socket, buf, len, nativeFlag, ret);
	if (ret >= 0) return ret;
	if (errno == EAGAIN)
	{
		if (LINUXDBG > 3)
		{
			DBGP("recv() returns: EAGAIN (%d)\r\n", errno);
		}
	}
	else
	{
		debug_printf("recv() error: %s (%d)\r\n",
		strerror(errno), errno);
	}
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Close(SOCK_SOCKET socket)
{ 
    NATIVE_PROFILE_PAL_NETWORK();

	int ret = close(socket);
	DBGP("socket=%d, ret=%d", socket, ret);

	if (ret >= 0)
	{
		return ret;
	}
	debug_printf("close() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Listen(SOCK_SOCKET socket, int backlog)
{    
    NATIVE_PROFILE_PAL_NETWORK();
    
	int ret = listen(socket, backlog);
	DBGP("socket=%d, backlog=%x, ret=%d", socket, backlog, ret);
	if (ret >= 0) return ret;

	debug_printf("listen() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

SOCK_SOCKET LINUX_SOCKETS_Driver::Accept(SOCK_SOCKET socket, SOCK_sockaddr* address, int* addressLen)
{ 
    NATIVE_PROFILE_PAL_NETWORK();

	DBGP("Enter, socket=%d", socket);
	int ret = accept(socket, (struct sockaddr*) address, (socklen_t *) addressLen);
	DBGP("socket=%d, address=%p, addressLen=%p(%d), ret=%d", socket, address, addressLen,
		addressLen != NULL ? addressLen : 0, ret);
	if (ret >= 0) return (SOCK_SOCKET) ret;

	debug_printf("accept() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::Shutdown( SOCK_SOCKET socket, int how )
{    
    NATIVE_PROFILE_PAL_NETWORK();
    
	int ret = shutdown(socket, how);
	DBGP("socket=%d, how=%d, ret=%d", socket, how, ret);
	if (ret >= 0) return ret;

	debug_printf("shutdown() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

//////////////////////////////////////

typedef struct _SOCK_addrinfo
{  
	SOCK_addrinfo info;

	struct addrinfo *_addrinfo;

} _SOCK_addrinfo;

void SOCK_addrinfo_to_addrinfo(const SOCK_addrinfo* in, struct addrinfo *out)
{
	out->ai_flags = in->ai_flags;
	out->ai_family = in->ai_family;
	out->ai_socktype = in->ai_socktype;
	out->ai_protocol = in->ai_protocol;
	out->ai_addrlen = in->ai_addrlen;
	out->ai_addr = (struct sockaddr*) in->ai_addr;
	out->ai_canonname = in->ai_canonname;
	out->ai_next = NULL; //in->ai_next;
}

void change_addrinfolist_to_SOCK_addrinfolist(struct addrinfo *a, SOCK_addrinfo **p);
void change_addrinfolist_to_SOCK_addrinfolist(struct addrinfo *a, SOCK_addrinfo **p)
{
	_SOCK_addrinfo *base_ps;
    SOCK_addrinfo *ps;
    SOCK_sockaddr *pa;
    SOCK_addrinfo *pfirst;
    SOCK_addrinfo **plast;
#define error_check_mem(m) \
    if ((m) == NULL) { debug_printf("cannot malloc mem\r\n"); return; }

    plast = NULL;
    while(a != NULL)
    {
        base_ps = (_SOCK_addrinfo*) private_malloc(sizeof(_SOCK_addrinfo));
        error_check_mem(base_ps);//
        memset(base_ps, 0, sizeof(_SOCK_addrinfo));
		ps = &base_ps->info;

        ps->ai_addr = (SOCK_sockaddr*) private_malloc(sizeof(SOCK_sockaddr));
        error_check_mem(ps->ai_addr);
        memset(ps->ai_addr, 0, sizeof(SOCK_sockaddr));

        ps->ai_flags = a->ai_flags;
        ps->ai_family = a->ai_family;
        ps->ai_socktype = a->ai_socktype;
        ps->ai_protocol = a->ai_protocol;
        ps->ai_addrlen = a->ai_addrlen;

        if (a->ai_canonname != NULL)
        {
            int len = hal_strlen_s(a->ai_canonname) + 1;
            ps->ai_canonname = (char *) private_malloc(len);
            error_check_mem(ps->ai_canonname);
            hal_strcpy_s(ps->ai_canonname, len, a->ai_canonname);
        }
        memcpy(ps->ai_addr, a->ai_addr, sizeof(SOCK_sockaddr));
        if (plast == NULL) // first time ?
        {
            pfirst = ps;            // keep return address
			base_ps->_addrinfo = a; // keep start addrinfo
        }
        else
        {
            *plast = ps;
        }
        a = a->ai_next;
        plast = &ps->ai_next;
    }
    if (plast != NULL && p != NULL)
    {
        *p = pfirst;
    }
#undef error_check_mem
}

int LINUX_SOCKETS_Driver::GetAddrInfo(const char* nodename, char* servname, const SOCK_addrinfo* hints, SOCK_addrinfo** res)
{
    NATIVE_PROFILE_PAL_NETWORK();
	struct addrinfo ainfo;
	struct addrinfo *ares;
	SOCK_addrinfo *sres;
	int ret;

	SOCK_addrinfo_to_addrinfo(hints, &ainfo);
	ret = getaddrinfo(nodename, servname, (const struct addrinfo*) &ainfo, &ares);
	change_addrinfolist_to_SOCK_addrinfolist(ares, &sres);
	if (res) *res = sres;

	DBGP("nodename = %s, servname=%s, hints=%p, res=%p(%p), ret=%d", nodename, servname, hints, res, res ? *res : 0, ret);
	if (ret == 0) return ret;

	debug_printf("getaddrinfo() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR; 
}

void LINUX_SOCKETS_Driver::FreeAddrInfo( SOCK_addrinfo* ai )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
	_SOCK_addrinfo *base_ps;
    SOCK_addrinfo *ps;
    SOCK_addrinfo *ps_next;

	DBGP("ai=%p", ai);
	if (ai == NULL) return;
	base_ps = (_SOCK_addrinfo *) ai;

	for(ps = ai; ps != NULL; ps = ps_next)
	{
		ps_next = ps->ai_next; // save next address before free
		if (ps->ai_canonname)
		{
			private_free(ps->ai_canonname);
			ps->ai_canonname = NULL;
		}
		if (ps->ai_addr)
		{
			private_free(ps->ai_addr);
			ps->ai_addr = NULL;
		}
		private_free(ps);
		ps = NULL;
	}

	if (base_ps->_addrinfo != NULL)
	{
		DBGP("base_ps=%p", base_ps);
		freeaddrinfo(base_ps->_addrinfo);
	}
}

int LINUX_SOCKETS_Driver::Ioctl( SOCK_SOCKET socket, int cmd, int* data )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

	int ret = 0;
	int val = 0;
	if (!socket)
	{
		debug_printf("HAL_SOCK_ioctl() error: socket is 0\r\n");
		return SOCK_SOCKET_ERROR;
	}
	DBGP("socket=%d, cmd=%x(%s), data=%p(%d)", socket,
		cmd, cmd == SOCK_FIONREAD ? "FIONREAD" :
		  cmd == SOCK_FIONBIO ?
		    data && *(unsigned int *)data ? "NONBLOCK" : "BLOCK" :
			"Unknown",
		data, data != NULL ? *data : 0);

	switch(cmd)
	{
	case SOCK_FIONREAD:
		if (data == NULL)
		{
			debug_printf("HAL_SOCK_ioctl() error: data is NULL\r\n");
			return SOCK_SOCKET_ERROR; 
		}
		//
		// since asm/ioctls.h(34):#define FIONREAD	0x541B
		//
		*data = 0;
		ret = ioctl(socket, FIONREAD, data);
		if (ret >= 0)
		{
			DBGP("FIONREAD: socket=%d, data=%p(%d), ret=%d",
				socket, data, *data, ret);
			return ret;
		}
		break;

	case SOCK_FIONBIO:
		val = fcntl(socket, F_GETFL);
	    if (data && *(unsigned int *)data)
			val |= O_NONBLOCK;
	    else
			val &= ~O_NONBLOCK;

		ret = fcntl(socket, F_SETFL, val);
		if (ret >= 0)
		{
			return ret;
		}
		break;

	default:
		debug_printf("HAL_SOCK_ioctl(%d, UNIMPL: 0x%x, %p, 0x%d)\n",
			socket, cmd, data, data != NULL ? (int) *data : 0);
		break;
	} /* switch (cmd) */

    return SOCK_SOCKET_ERROR; 
}

int LINUX_SOCKETS_Driver::GetLastError()
{
    NATIVE_PROFILE_PAL_NETWORK();

	DBGP("errno = %d", errno);

	return GetNativeError(errno);
}

int LINUX_SOCKETS_Driver::Select( int nfds, SOCK_fd_set* readsockfds, SOCK_fd_set* writesockfds, SOCK_fd_set* exceptsockfds, const SOCK_timeval* timeout )
{
    NATIVE_PROFILE_PAL_NETWORK();
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

	PRINTSOCKFD("*** before readfds:", readsockfds);
	PRINTSOCKFD("*** before writefd:", writesockfds);
	PRINTSOCKFD("*** before except :", exceptsockfds);

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	sockfdset_to_fdset(readsockfds, &readfds);
	sockfdset_to_fdset(writesockfds, &writefds);
	sockfdset_to_fdset(exceptsockfds, &exceptfds);

	if (LINUXDBG > 5)
	{
		PRINTFD("** before readfds:", &readfds);
		PRINTFD("** before writefd:", &writefds);
		PRINTFD("** before except :", &exceptfds);
	}
	int sec = timeout != NULL ? timeout->tv_sec : 0;
	int usec = timeout != NULL ? timeout->tv_usec : 0;

	int ret = select(nfds, &readfds, &writefds, &exceptfds, (struct timeval *) timeout);

	if (LINUXDBG > 5)
	{
		PRINTFD("** after readfds :", &readfds);
		PRINTFD("** after writefd :", &writefds);
		PRINTFD("** after except  :", &exceptfds);
	}
	SOCK_FD_ZERO(readsockfds);
	SOCK_FD_ZERO(writesockfds);
	SOCK_FD_ZERO(exceptsockfds);
	fdset_to_sockfdset(&readfds, readsockfds);
	fdset_to_sockfdset(&writefds, writesockfds);
	fdset_to_sockfdset(&exceptfds, exceptsockfds);

	PRINTSOCKFD("*** after readfds :", readsockfds);
	PRINTSOCKFD("*** after writefd :", writesockfds);
	PRINTSOCKFD("*** after except  :", exceptsockfds);

	DBGP("nfds=%d, readfds=%p, writefds=%p, except=%p, timeout=%p(%d.%06d), ret=%d",
		nfds, readsockfds, writesockfds, exceptsockfds,
		timeout, (int) sec, (int) usec, ret);
	if (ret >= 0) return ret;

	debug_printf("select() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::SetSockOpt( SOCK_SOCKET socket, int level, int optname, const char* optval, int  optlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;
    int nativeLevel;
    int nativeOptionName;
    int nativeIntValue;
    char *pNativeOptionValue = (char*)optval;

    switch(level)
    {
        case SOCK_IPPROTO_IP:
            nativeLevel = IPPROTO_IP;
            nativeOptionName = GetNativeIPOption(optname);
            break;
        case SOCK_IPPROTO_TCP:
            nativeLevel = IPPROTO_TCP;
            nativeOptionName = GetNativeTcpOption(optname);
            break;
        case SOCK_IPPROTO_UDP: 
        case SOCK_IPPROTO_ICMP:
        case SOCK_IPPROTO_IGMP:
        case SOCK_IPPROTO_IPV4:
        case SOCK_SOL_SOCKET:
            nativeLevel      = SOL_SOCKET;
            nativeOptionName = GetNativeSockOption(optname);            

            switch(optname)
            {
                case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                case SOCK_SOCKO_NOCHECKSUM:
                case SOCK_SOCKO_DONTLINGER:
                    nativeIntValue     = !*(int*)optval;
                    pNativeOptionValue = (char*)&nativeIntValue;
                    break;
                default:
                    break;
            }
            break;
        default:
            nativeLevel         = 0;
            nativeOptionName    = 0;
            break;
    }

	ret = setsockopt(socket, nativeLevel, nativeOptionName,
		pNativeOptionValue, (socklen_t) optlen);
	DBGP("socket=%d, level=%d, optname=%d, optval=%s, optlen=%d, ret=%d",
		socket, nativeLevel, nativeOptionName, pNativeOptionValue, optlen, ret);
	if (ret >= 0) return ret;

	debug_printf("setsockopt() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR; 
}

int LINUX_SOCKETS_Driver::GetSockOpt( SOCK_SOCKET socket, int level, int optname, char* optval, int* optlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    int         nativeLevel;
	int         nativeOptionName;
    char*       pNativeOptval = optval;
    int         ret;
    
    switch(level)
    {
        case SOCK_IPPROTO_IP:
            nativeLevel         = IPPROTO_IP;
            nativeOptionName    = GetNativeIPOption(optname);
            break;
        case SOCK_IPPROTO_TCP:    
            nativeLevel         = IPPROTO_TCP;
            nativeOptionName    = GetNativeTcpOption(optname);
            break;
        case SOCK_IPPROTO_UDP: 
        case SOCK_IPPROTO_ICMP:
        case SOCK_IPPROTO_IGMP:
        case SOCK_IPPROTO_IPV4:
        case SOCK_SOL_SOCKET:
            nativeLevel         = SOL_SOCKET;
            nativeOptionName    = GetNativeSockOption(optname);
            break;
        default:
            nativeLevel         = level;
            nativeOptionName    = optname;
            break;
    }

	ret = getsockopt(socket, nativeLevel, nativeOptionName, optval, (socklen_t *) optlen);
	DBGP("socket=%d, level=%d, optname=%d, optval=%s, optlen=%p, ret=%d",
		socket, level, optname, optval, optlen, ret);

    if(ret >= 0)
    {
        switch(level)
        {
            case SOCK_SOL_SOCKET:
                switch(optname)
                {
                    case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    case SOCK_SOCKO_DONTLINGER:
                        *optval = !(*(int*)optval != 0);
                        break;
                        
                    case SOCK_SOCKO_ACCEPTCONNECTION:
                    case SOCK_SOCKO_BROADCAST:
                    case SOCK_SOCKO_KEEPALIVE:
                        *optval = (*(int*)optval != 0);
                        break;
                }
                break;
        }
	    return ret;    
    }

	debug_printf("getsockopt() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::GetPeerName( SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

	ret = getsockname(socket, (sockaddr*) name, (socklen_t *) namelen);
	DBGP("socket=%d, name=%p, namelen=%p, ret=%d", socket, name, namelen, ret);
	if (ret >= 0) return ret;

	debug_printf("getsockname() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR; 
}

int LINUX_SOCKETS_Driver::GetSockName( SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

	ret = getsockname(socket, (sockaddr*) name, (socklen_t *) namelen);
	DBGP("socket=%d, name=%p, namelen=%p, ret=%d", socket, name, namelen, ret);
	if (ret >= 0) return ret;

	debug_printf("getsockname() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::RecvFrom( SOCK_SOCKET socket, char* buf, int len, int flags, SOCK_sockaddr* from, int* fromlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    sockaddr_in addr;
    sockaddr *pFrom = NULL;
    int ret = recvfrom(socket, buf, (size_t) len, flags, (struct sockaddr *) from, (socklen_t *) fromlen);
	DBGP("socket=%d, buf=%p, len=%d, flags=0x%x, from=%p, fromlen=%p, ret=%d", socket, buf, len, flags, from, fromlen, ret);
	if (ret >= 0) return ret;

	debug_printf("recvfrom() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

int LINUX_SOCKETS_Driver::SendTo( SOCK_SOCKET socket, const char* buf, int len, int flags, const SOCK_sockaddr* to, int tolen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
	int ret = sendto(socket, buf, (size_t) len, flags, (const struct sockaddr *) to, (socklen_t) tolen);
	DBGP("socket=%d, buf=%p, len=%d, flags=0x%x, to=%p, tolen=%d, ret=%d", socket, buf, len, flags, to, tolen, ret);
	if (ret >= 0) return ret;

	debug_printf("sendto() error: %s (%d)\r\n",
		strerror(errno), errno);
    return SOCK_SOCKET_ERROR;
}

UINT32 LINUX_SOCKETS_Driver::GetAdapterCount()
{
    NATIVE_PROFILE_PAL_NETWORK();
	DBGP("ret=%d", NETWORK_INTERFACE_COUNT);
    return NETWORK_INTERFACE_COUNT;
}

HRESULT LINUX_SOCKETS_Driver::LoadAdapterConfiguration( UINT32 interfaceIndex, SOCK_NetworkConfiguration* config )
{
    return S_OK;
}

HRESULT LINUX_SOCKETS_Driver::LoadWirelessConfiguration( UINT32 interfaceIndex, SOCK_WirelessConfiguration* wirelessConfig )
{
    /// Load wireless specific settings if any. You must return S_OK, otherwise default values will be
    /// loaded by PAL.

    return CLR_E_FAIL;
}

struct dhcp_client_id
{
    UINT8 code;
    UINT8 length;
    UINT8 type;
    UINT8 clientId[6];
};

HRESULT LINUX_SOCKETS_Driver::UpdateAdapterConfiguration( UINT32 interfaceIndex, UINT32 updateFlags, SOCK_NetworkConfiguration* config )
{
    return S_OK;
}

int LINUX_SOCKETS_Driver::GetNativeTcpOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_TCP_NODELAY:
            nativeOptionName = TCP_NODELAY;
            break;

        // allow the C# user to specify LINUX options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    return nativeOptionName;
}

int LINUX_SOCKETS_Driver::GetNativeSockOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_SOCKO_DONTLINGER:
        case SOCK_SOCKO_LINGER:    
            nativeOptionName = SO_LINGER;
            break;
        case SOCK_SOCKO_SENDTIMEOUT:          
            nativeOptionName = SO_SNDTIMEO;
            break;
        case SOCK_SOCKO_RECEIVETIMEOUT:       
            nativeOptionName = SO_RCVTIMEO;
            break;
        case SOCK_SOCKO_EXCLUSIVEADDRESSUSE: 
        case SOCK_SOCKO_REUSEADDRESS:         
            nativeOptionName = SO_REUSEADDR;
            break;
        case SOCK_SOCKO_KEEPALIVE:  
            nativeOptionName = SO_KEEPALIVE;
            break;
        case SOCK_SOCKO_ERROR:                  
            nativeOptionName = SO_ERROR;
            break;
        case SOCK_SOCKO_BROADCAST:              
            nativeOptionName = SO_BROADCAST;
            break;
        case SOCK_SOCKO_RECEIVEBUFFER:
            nativeOptionName =  SO_RCVBUF;
            break;
        case SOCK_SOCKO_SENDBUFFER:
            nativeOptionName = SO_SNDBUF;
            break;
        case SOCK_SOCKO_ACCEPTCONNECTION:
            nativeOptionName = SO_ACCEPTCONN;
            break;
        case SOCK_SOCKO_TYPE:
            nativeOptionName = SO_TYPE;
            break;
            
        case SOCK_SOCKO_DONTROUTE:  
            nativeOptionName = SO_DONTROUTE;
            break;
        case SOCK_SOCKO_OUTOFBANDINLINE:
            nativeOptionName = SO_OOBINLINE;
            break;

        case SOCK_SOCKO_DEBUG:
            nativeOptionName = SO_DEBUG;
            break;
            
        case SOCK_SOCKO_SENDLOWWATER:
            nativeOptionName = SO_SNDLOWAT;
            break;
            
        case SOCK_SOCKO_RECEIVELOWWATER:
            nativeOptionName = SO_RCVLOWAT;
            break;
            
//        case SOCK_SOCKO_MAXCONNECTIONS:         //don't support
        case SOCK_SOCKO_USELOOPBACK: //
        case SOCK_SOCKO_UPDATE_ACCEPT_CTX:
        case SOCK_SOCKO_UPDATE_CONNECT_CTX:
            nativeOptionName = 0;
            break;
            
        // allow the C# user to specify LINUX options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
            
    }

    return nativeOptionName;
}

int LINUX_SOCKETS_Driver::GetNativeIPOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_IPO_TTL:           
            nativeOptionName = IP_TTL;
            break;
        case SOCK_IPO_TOS:    
            nativeOptionName = IP_TOS;
            break;
        case SOCK_IPO_MULTICAST_IF:
            nativeOptionName = IP_MULTICAST_IF;
            break;
        case SOCK_IPO_MULTICAST_TTL:  
            nativeOptionName = IP_MULTICAST_TTL;
            break;
        case SOCK_IPO_MULTICAST_LOOP: 
            nativeOptionName = IP_MULTICAST_LOOP;
            break;
        case SOCK_IPO_ADD_MEMBERSHIP:
            nativeOptionName = IP_ADD_MEMBERSHIP;
            break;
        case SOCK_IPO_DROP_MEMBERSHIP:
            nativeOptionName = IP_DROP_MEMBERSHIP;
            break;
		case SOCK_IPO_ADD_SOURCE_MEMBERSHIP:
            nativeOptionName = IP_ADD_SOURCE_MEMBERSHIP;
            break;
        case SOCK_IPO_DROP_SOURCE_MEMBERSHIP:
            nativeOptionName = IP_DROP_SOURCE_MEMBERSHIP;
            break;
        case SOCK_IPO_OPTIONS:
            nativeOptionName = IP_OPTIONS;
            break;
        case SOCK_IPO_HDRINCL:
            nativeOptionName = IP_HDRINCL;
            break;
        case SOCK_IPO_BLOCK_SOURCE:
            nativeOptionName = IP_OPTIONS;
            break;
        case SOCK_IPO_UBLOCK_SOURCE:
            nativeOptionName = IP_UNBLOCK_SOURCE;
            break;

        case SOCK_IPO_PACKET_INFO: 
        case SOCK_IPO_IP_DONTFRAGMENT:
            nativeOptionName = 0;
            break;

        // allow the C# user to specify LINUX options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    
    return nativeOptionName;
}   

int LINUX_SOCKETS_Driver::GetNativeError ( int error )
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    switch(error)
    {
        case EINTR:
            ret = SOCK_EINTR;
            break;

        case EACCES:
            ret = SOCK_EACCES;
            break;

        case EFAULT:
            ret = SOCK_EFAULT;
            break;

        case EINVAL:
            ret = SOCK_EINVAL;
            break;

        case EMFILE:
            ret = SOCK_EMFILE;
            break;

        case EAGAIN:
        case EBUSY:
        /* case EWOULDBLOCK: same as EINPROGRESS */ 
        case EINPROGRESS:
            ret = SOCK_EWOULDBLOCK;
            break;

        case EALREADY:
            ret = SOCK_EALREADY;
            break;

        case ENOTSOCK:
            ret = SOCK_ENOTSOCK;
            break;

        case EDESTADDRREQ:
            ret = SOCK_EDESTADDRREQ;
            break;

        case EMSGSIZE:
            ret = SOCK_EMSGSIZE;
            break;

        case EPROTOTYPE:
            ret = SOCK_EPROTOTYPE;
            break;

        case ENOPROTOOPT:
            ret = SOCK_ENOPROTOOPT;
            break;

        case EPROTONOSUPPORT:
            ret = SOCK_EPROTONOSUPPORT;
            break;

        case ESOCKTNOSUPPORT:
            ret = SOCK_ESOCKTNOSUPPORT;
            break;

        case EPFNOSUPPORT:
            ret = SOCK_EPFNOSUPPORT;
            break;

        case EAFNOSUPPORT:
            ret = SOCK_EAFNOSUPPORT;
            break;

        case EADDRINUSE:
            ret = SOCK_EADDRINUSE;
            break;

        case EADDRNOTAVAIL:
            ret = SOCK_EADDRNOTAVAIL;
            break;

        case ENETDOWN:
            ret = SOCK_ENETDOWN;
            break;

        case ENETUNREACH:
            ret = SOCK_ENETUNREACH;
            break;

        case ENETRESET:
            ret = SOCK_ENETRESET;
            break;

        case ECONNABORTED:
            ret = SOCK_ECONNABORTED;
            break;

        case ECONNRESET:
            ret = SOCK_ECONNRESET;
            break;

        case ENOBUFS:
        case ENOMEM:
            ret = SOCK_ENOBUFS;
            break;

        case EISCONN:
            ret = SOCK_EISCONN;
            break;

        case ENOTCONN:
            ret = SOCK_EISCONN;
            break;

        case ESHUTDOWN:
            ret = SOCK_ESHUTDOWN;
            break;

        case ETIMEDOUT:
            ret = SOCK_ETIMEDOUT;
            break;

        case ECONNREFUSED:
            ret = SOCK_ECONNREFUSED;
            break;

        case EHOSTDOWN:
            ret = SOCK_EHOSTDOWN;
            break;

        case EHOSTUNREACH:
            ret = SOCK_EHOSTUNREACH;
            break;

        case ENODATA:
            ret = SOCK_NO_DATA;
            break;

        default:
            ret = error;
            break;
    } 
    
    return (ret);   
}
