////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// modified for Linux from lwip_socket.h on NETMF 4.1 by Device Drivers Limited.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX_SOCKETS_H_
#define _LINUX_SOCKETS_H_

#ifdef PLATFORM_ARM_LINUX
#include "tinyclr_application.h"
#include "tinyhal.h"
#endif
#include "linuxAPI.h"

//////////////////////////////////////////////////////////////////////////////
// DEBUG stubs
// 

#if !defined(BUILD_RTM)
#define DBGP(fmt, args...) \
    do { if (LINUXDBG > 3) debug_printf("*** %s: " fmt "\r\n", __FUNCTION__, ## args); } while(0)
#else
#define DBGP(fmt, args...)
#endif

//////////////////////////////////////////////////////////////////////////////
// SOCKET driver
// 
#define NETIF_MAX_HWADDR_LEN 6

struct LINUX_DRIVER_INTERFACE_DATA
{
    int          m_interfaceNumber;
};

struct ip_addr {
  unsigned int addr;
};

struct netif {
  /** pointer to next in linked list */
  struct netif *next;

  /** IP address configuration in network byte order */
  struct ip_addr ip_addr;
  struct ip_addr netmask;
  struct ip_addr gw;

  /** This function is called by the network device driver
   *  to pass a packet up the TCP/IP stack. */
  int (* input)(struct pbuf *p, struct netif *inp);
  /** This function is called by the IP module when it wants
   *  to send a packet on the interface. This function typically
   *  first resolves the hardware address, then sends the packet. */
  int (* output)(struct netif *netif, struct pbuf *p,
       struct ip_addr *ipaddr);
  /** This function is called by the ARP module when it wants
   *  to send a packet on the interface. This function outputs
   *  the pbuf as-is on the link medium. */
  int (* linkoutput)(struct netif *netif, struct pbuf *p);
  /** This field can be set by the device driver and could point
   *  to state information for the device. */
  void *state;
  /* the hostname for this netif, NULL is a valid value */
  char*  hostname;
  /** maximum transfer unit (in bytes) */
  unsigned short mtu;
  /** number of bytes used in hwaddr */
  unsigned char hwaddr_len;
  /** link level hardware address of this interface */
  unsigned char hwaddr[NETIF_MAX_HWADDR_LEN];
  /** flags (see NETIF_FLAG_ above) */
  unsigned char flags;
  /** descriptive abbreviation */
  char name[2];
  /** number of this interface */
  unsigned char num;
};

/** The list of network interfaces. */
extern struct netif *netif_list;
/** The default network interface. */
extern struct netif *netif_default;

struct LINUX_SOCKETS_Driver
{
    static BOOL Initialize();
    
    static BOOL Uninitialize();
    
    static SOCK_SOCKET Socket( int family, int type, int protocol );
    
    static int Bind( SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen  );

    static int Connect(SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen);

    static int Send(SOCK_SOCKET socket, const char* buf, int len, int flags);

    static int Recv(SOCK_SOCKET socket, char* buf, int len, int flags);

    static int Close(SOCK_SOCKET socket);

    static int Listen(SOCK_SOCKET socket, int backlog );

    static SOCK_SOCKET Accept(SOCK_SOCKET socket, SOCK_sockaddr* address, int* addressLen );


    static int Shutdown(SOCK_SOCKET socket, int how );

    static int GetAddrInfo(const char* nodename, 
                  char* servname, 
                  const SOCK_addrinfo* hints, 
                  SOCK_addrinfo** res );

    static void FreeAddrInfo(SOCK_addrinfo* ai );

    static int Ioctl(SOCK_SOCKET socket, int cmd, int* data );

    static int GetLastError();

    static int GetNativeTcpOption (int optname);

    static int GetNativeSockOption (int optname);

    static int GetNativeError( int error );

    static int GetNativeIPOption (int optname);

    static int Select(int nfds, SOCK_fd_set* readfds, SOCK_fd_set* writefds, SOCK_fd_set* exceptfds, const SOCK_timeval* timeout );

    static int SetSockOpt(SOCK_SOCKET socket, int level, int optname, const char* optval, int  optlen );

    static int GetSockOpt(SOCK_SOCKET socket, int level, int optname, char* optval, int* optlen );

    static int GetPeerName(SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen );
    
    static int GetSockName(SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen );

    static int RecvFrom(SOCK_SOCKET s, char* buf, int len, int flags, SOCK_sockaddr* from, int* fromlen );
    
    static int SendTo(SOCK_SOCKET s, const char* buf, int len, int flags, const SOCK_sockaddr* to, int tolen );

    static UINT32 GetAdapterCount();

    static HRESULT LoadAdapterConfiguration( UINT32 interfaceIndex, SOCK_NetworkConfiguration* config );
    
    static HRESULT UpdateAdapterConfiguration( UINT32 interfaceIndex, UINT32 updateFlags, SOCK_NetworkConfiguration* config );

    static HRESULT LoadWirelessConfiguration( UINT32 interfaceIndex, SOCK_WirelessConfiguration* wirelessConfig );


private:
    LINUX_DRIVER_INTERFACE_DATA m_interfaces[NETWORK_INTERFACE_COUNT];
};

//
// SOCKET driver
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _LINUX_SOCKETS_H_ 1

