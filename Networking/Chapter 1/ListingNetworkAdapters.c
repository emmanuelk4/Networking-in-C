//run with gcc ListingNetworkAdapters.c -o ListingNetworkAdapters.exe -liphlpapi -lws2_32
//Program to list newtork adapters in C
//Must initialize WInsock using WSAStartup

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
// _WIN32_WINNT macros defined first so proper header are included
//stdio.h is for printf function and stdlib.h is for memory allocation

int main() {
      //initalize Winsock with WSAStartup
      WSADATA d;
      if (WSAStartup(MAKEWORD(2, 2), &d)) {
          printf("Failed to initialize\n");
          return -1;
      }
      //Allocate memory for adapters in bytes using malloc function
      DWORD asize = 20000;
      PIP_ADAPTER_ADDRESSES adapters;
      do {
          adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);
          
          if(!adapters) {
          printf("Couldn't allocate %ld bytes for adapteers.\n", asize);
          WSACleanup();
          return -1;
          }
          
          //AF_UNSPEC says we want both IPv4 and IPv6 addresses, can use AF_INET or AF_INET6 to specify
          //GAA_FLAG_INCLUDE_PREFIX requests a list of addresses
          int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0,
                  adapters, &asize);
          if (r == ERROR_BUFFER_OVERFLOW) {
              printf("GetAdaptersAddresses wants %ld bytes.\n", r);
              free(adapters);
          } else if (r == ERROR_SUCCESS) {
              break;
          } else {
              printf("Error from GetAdapterDdresses: %d \n",r);
              free(adapters);
              WSACleanup();
              return -1;
          }
      } while (!adapters);
      
      //adapter is used to walk through linked list of adapters, addres i through linked list of addresses
      //First adapter is at beginning of adapters, at end of each loop we go to nextwith adapter->Next, loop aborts when adapter is 0
      //get adapter name from adapter->friendlyName
      //address of first adapter is adapter->FirstUnicastAddress, set address to it
      PIP_ADAPTER_ADDRESSES adapter = adapters;
      while (adapter) {
               printf("\nADapter name: %S\n", adapter->FriendlyName);
               
               PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
               while (address) {
                  printf("\t%s",
                         address->Address.lpSockaddr->sa_family == AF_INET ? // stores address family
                         "IPv4" : "IPv6");
                  char ap[100]; // allocate buffer to store text representation of address
                  
                  getnameinfo(address->Address.lpSockaddr,
                  address->Address.iSockaddrLength,
                  ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
                  printf("\t%s\n", ap);
                  
                  address = address->Next;
               }
               adapter = adapter->Next;
      }
      free(adapters);
      WSACleanup();
      return 0;          
}
