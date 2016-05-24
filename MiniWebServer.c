// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za mini web server

#include "GNPfunkcije.h"
#include "Temperatura.h"
#include "Parametri.h"
#include "Sat.h"
#include "DIulazi.h"
#include "Komunikacija.h"
#include "MiniWebServer.h"
#include "__NetEthInternal_Stellaris.h"
#include "sajt.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// konstante

const code unsigned char httpHeader[] = "HTTP/1.1 200 OK\nConnection: close\nContent-type: ";  // HTTP header
const code unsigned char httpMimeTypeHTML[] = "text/html\n\n";  // HTML MIME type

//------------------------------------------------------------------------------
// globalne promenljive

unsigned short pomWebBrojac = 0; // pomocni brojac za inkrementiranje vremenskog brojava servera

unsigned char httpMethod[] = "GET /"; // HTML metod

unsigned char   myMacAddr[6] = {0x00, 0x14, 0xA5, 0x76, 0x19, 0x3f};   // my MAC address
unsigned char   myIpAddr[4]  = {192, 168,  20, 60 };                   // my IP address
unsigned char   gwIpAddr[4]  = {192, 168,  20,  6 };                   // gateway (router) IP address
unsigned char   ipMask[4]    = {255, 255, 255,  0 };                   // network mask (for example : 255.255.255.0)
unsigned char   dnsIpAddr[4] = {192, 168,  20,  1 };                   // DNS server IP address

unsigned char   getRequest[15];  // HTTP request buffer
unsigned char   dyna[31] ;  // buffer for dynamic response

char sendHTML_mark = 0; // da li je aktivan HTML socket
SOCKET_Intern_Dsc *socketHTML; // socket
unsigned int pos = 0; // pozicija u sajtu

//==============================================================================
// funkcija za inicijalizaciju mini web servera
void initMiniWebServer()
{
  // podesavanje IP adrese
  myIpAddr[3] = parametarA[IPadresa];

  Net_Ethernet_Intern_stackInitTCP();
  Net_Ethernet_Intern_Init(myMacAddr, myIpAddr, _ETHERNET_AUTO_NEGOTIATION); // init ethernet board
  Net_Ethernet_Intern_confNetwork(ipMask, gwIpAddr, dnsIpAddr);
  // Enable ethernet LED signal on PF3 and PF2.
  GPIO_Config(&GPIO_PORTF, _GPIO_PINMASK_2 | _GPIO_PINMASK_3, _GPIO_DIR_NO_CHANGE,
              _GPIO_CFG_ALT_FUNCTION | _GPIO_CFG_DRIVE_8mA | _GPIO_CFG_DIGITAL_ENABLE, _GPIO_PINCODE_1);
}
//==============================================================================
// funkcija za UDT, ne koristi se
unsigned int Net_Ethernet_Intern_UserUDP(UDP_Intern_Dsc *udpDsc)
{
  return 0;
}
//==============================================================================
// funkcija za TCP
void Net_Ethernet_Intern_UserTCP(SOCKET_Intern_Dsc *socket)
{
  unsigned int    len;
  unsigned int    offset = 0;     // possition in payload buffer

  // I listen only to web request on port 80
  if(socket->destPort != 80) { return; }

  // get 10 first bytes only of the request, the rest does not matter here
  for(len = 0; len < 10; len++)
  {
    getRequest[len] = Net_Ethernet_Intern_readPayloadByte(offset++);
  }
  getRequest[len] = 0;

  // only GET method is supported here
  if(memcmp(getRequest, httpMethod, 5)&&(socket->state != 3)) { return; }

  if( (memcmp(getRequest, httpMethod, 5)==0)&&( (getrequest[7]=='H')||(getRequest[8]=='H')||(getRequest[6]=='H')||(getRequest[7]=='=') ) )
  {
    sendHTML_mark = 1;
    socketHTML = socket;
  }

  //............................................................................
  // Send html page.
  if((sendHTML_mark == 1)&&(socketHTML == socket))
  {
    if(pos==0)
    {
      char txt6[6]; // pomocna promenljive
      char txt4[4]; // pomocna promenljiva
      unsigned int drzi = 0; // pomocna promenljiva
      
      // izbor promene rada gorionika na osnovu linka
      switch (getRequest[5])
      {
        case 'o': { paliGasi(); break; } // pali ili gasi gorionik
        case 'm': { radManf(); break; } // prebacuje se na manuelni rad
        case 'a': { radAuto(); break; } // prebaci na automatski rad
        case 'P': // podesavanje jacine rada
        {
          switch (getRequest[6])
          {
            case '1': { klikP1(); break; }
            case '2': { klikP2(); break; }
            case '3': { klikP3(); break; }
            case '4': { klikP4(); break; }
            case '5': { klikP5(); break; }
            default: { break; }
          }
          break;
        }
        default: { break; }
      }
      
      // proveriti da li treba podesiti zadatu temperaturu
      if ( (getRequest[5]=='?')&&(getRequest[6]=='t')&&(getRequest[7]=='=') )
      { // treba podesiti zadatu temperaturu
        drzi = (getRequest[8]-48)*10 + (getRequest[9]-48);
        // proveriti opseg temperature
        if ( (drzi>10)&&(drzi<parametar[maxTV]) ) { parametarA[zadTV] = drzi; ispisRada(); upisAparametra(zadTV); }
      }
    
      // ubacivanje IP adrese
      WordToStr(parametarA[IPadresa], txt6);
      if ( parametarA[IPadresa]>99 ) // da li je broj dvocifren ili trocifren
      { // trocifren
        promWeb1[23] =  txt6[2];
        promWeb1[24] =  txt6[3];
        promWeb1[25] =  txt6[4];
        promWeb1[26] =  '"';
        promWeb1[27] =  ';';
        promWeb1[28] =  '\0';
      }
      else
      { // dvocifren
        promWeb1[23] =  txt6[3];
        promWeb1[24] =  txt6[4];
        promWeb1[25] =  '"';
        promWeb1[26] =  ';';
        promWeb1[27] =  '\0';
      }
      
      // ubacivanje faze rada
      ByteToStr(stanje.fazaRada, txt4);
      promWeb2[13] = txt4[2];
      
      // ubacivanje temperature dima
      WordToStrWithZeros(Tdimnjace, txt6);
      promWeb3[20] = txt6[2];
      promWeb3[21] = txt6[3];
      promWeb3[22] = txt6[4];
      
      // ubacivanje temperature ambijenta
      WordToStr(Tambijenta, txt6);
      promWeb4[20] = txt6[3];
      promWeb4[21] = txt6[4];
      
      // ubacivanje jacine rada
      ByteToStr(stanje.jacinaRada, txt4);
      promWeb5[15] = txt4[2];
      
      // ubacivanje zadate temperature vode
      WordToStr(parametarA[zadTV], txt6);
      promWeb6[19] = txt6[3];
      promWeb6[20] = txt6[4];
    
      // salji HTTP header i vrednosti promenljivih
      Net_Ethernet_Intern_putConstStringTCP(httpHeader, socket);
      Net_Ethernet_Intern_putConstStringTCP(httpMimeTypeHTML, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb0, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb1, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb2, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb3, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb4, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb5, socket);
      Net_Ethernet_Intern_putStringTCP(promWeb6, socket);
      Net_Ethernet_Intern_putStringTCP(promWebKraj, socket);
    }
    while(webSajt[pos]!='\0')
    {
      if(Net_Ethernet_Intern_putByteTCP(webSajt[pos++], socket) == 0)
      {
        pos--;
        break;
      }
    }

    if( Net_Ethernet_Intern_bufferEmptyTCP(socket) && (webSajt[pos]=='\0') )
    {
      Net_Ethernet_Intern_disconnectTCP(socket);
      sendHTML_mark = 0;
      pos = 0;
    }
  }
}
//==============================================================================