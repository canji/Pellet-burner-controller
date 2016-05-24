// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za mini web server

#ifndef _MiniWebServer_h_
#define _MiniWebServer_h_

#include "__NetEthInternal_Stellaris.h"

//==============================================================================
// konstante

extern const code unsigned char httpHeader[];  // HTTP header
extern const code unsigned char httpMimeTypeHTML[];  // HTML MIME type

//------------------------------------------------------------------------------
// globalne promenljive

extern unsigned short pomWebBrojac; // pomocni brojac za inkrementiranje vremenskog brojava servera

extern unsigned char httpMethod[]; // HTML metod

extern unsigned char   myMacAddr[6];  // my MAC address
extern unsigned char   myIpAddr[4];  // my IP address
extern unsigned char   gwIpAddr[4];  // gateway (router) IP address
extern unsigned char   ipMask[4];  // network mask (for example : 255.255.255.0)
extern unsigned char   dnsIpAddr[4];  // DNS server IP address

extern unsigned char   getRequest[15];  // HTTP request buffer
extern unsigned char   dyna[31];  // buffer for dynamic response

extern char sendHTML_mark; // da li je aktivan HTML socket
extern SOCKET_Intern_Dsc *socketHTML; // socket
extern unsigned int pos ; // pozicija u sajtu

//==============================================================================
// funkcija za inicijalizaciju mini web servera
void initMiniWebServer();

//==============================================================================

#endif