// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za komunikaciju putem ethernet i GSM

#include "Komunikacija.h"
#include "Parametri.h"
#include "GNPfunkcije.h"
#include "Sat.h"
#include "DIulazi.h"
#include "Temperatura.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// pinovi

sbit CTS at GPIO_PORTH_DATA0_bit; // CTS pin
sbit GPIO2 at GPIO_PORTC_DATA4_bit; // GPIO2 pin
sbit RTS at GPIO_PORTG_DATA0_bit; // RTS pin
sbit RST at GPIO_PORTE_DATA2_bit; // RST pin
sbit GSMon at GPIO_PORTD_DATA5_bit; // RST pin

//==============================================================================
// konstante

// AT komande
const char atc[] = "AT"; // provera GSM modula
const char atExoOff[] = "ATE0"; // exo iskljucen
const char formatTextSMS[] = "AT+CMGF=1"; // format za tekstualne poruke
const char resiveSMS[] = "AT+CMGR=1"; // citanje SMS poruke iz prve lokacije u inbox
const char erasingSMS[] = "AT+CMGD=1,4"; // brisanje svih SMS poruka u inbox

//------------------------------------------------------------------------------
// globalne promenljive

// struktura za rad se GSMom
GSMstatus GSMpodaci;

// broj bajtova koje primi UART od GSM modula
unsigned short brojacPrijema = 0;

// podaci primljeni od GSM modula putem UARTa
char baferUART[512];

// broj mobilnogtelefona za slanje alarmnih poruka
char alarmniBroj[20];

//==============================================================================
// funkcija za ispsis IP adrese
void ispisiIP ()
{
  char txt[4]; // pomocna promenljiva
  unsigned short zaUpis; // pomocna promenljiva
  
  zaUpis = (unsigned short)parametarA[IPadresa];
  
  DrawBox(&boksIP); // brisanje stare vrednosti
  ByteToStr (zaUpis, txt);
  labelAdresiranje_Caption[0] = txt[0];
  labelAdresiranje_Caption[1] = txt[1];
  labelAdresiranje_Caption[2] = txt[2];
  labelAdresiranje_Caption[3] = '\0';
  DrawLabel (&labelAdresiranje); // ispsi IP adresu
}
//==============================================================================
// funkcija za ispis broja mobilnog za alarmne poruke
void ispisiMob ()
{
  // ako je GSM ukljucen, a neispravan
  if ( (parametarA[GSMaktivnost])&&(GSMpodaci.ispravnostGSM) )
  {
    strcpy( labelBorjMob_Caption, "Neispravan!" );
    bokBrojMob.Color = 0xF800; // boks je crvene boje
  }
  else
  {
    labelBorjMob_Caption[0] = '0';
    labelBorjMob_Caption[1] = alarmniBroj[5];
    labelBorjMob_Caption[2] = alarmniBroj[6];
    labelBorjMob_Caption[3] = alarmniBroj[7];
    labelBorjMob_Caption[4] = alarmniBroj[8];
    labelBorjMob_Caption[5] = alarmniBroj[9];
    labelBorjMob_Caption[6] = alarmniBroj[10];
    labelBorjMob_Caption[7] = alarmniBroj[11];
    labelBorjMob_Caption[8] = alarmniBroj[12];
    labelBorjMob_Caption[9] = alarmniBroj[13];
    labelBorjMob_Caption[10] = alarmniBroj[14];
    labelBorjMob_Caption[11] = alarmniBroj[15];
    labelBorjMob_Caption[12] = '\0';
    bokBrojMob.Color = 0xFFFF; // je bele boje
  }
  DrawBox(&bokBrojMob); // brisanje stare vrednosti
  DrawLabel (&labelBorjMob);
}
//==============================================================================
// funkcija za inicijalizaciju GSM
void initGSM ()
{
  unsigned short sinGreska = 0; // brojac neuspelih pokusaja ostvarenja komunikacije sa GSM modulom

  if (!parametarA[GSmaktivnost]) { return; } // ake se ne koristi GSm nema inicijalizacije istog

  // inicijalizacija pinova
  GPIO_Digital_Input (&GPIO_PORTH, _GPIO_PINMASK_0); // CTS je ulazni
  GPIO_Digital_Output (&GPIO_PORTC, _GPIO_PINMASK_4); GPIO2 = 0; // GPIO2 izalzni
  GPIO_Digital_Output (&GPIO_PORTG, _GPIO_PINMASK_0); RTS = 0; // RTS izalzni
  GPIO_Digital_Output (&GPIO_PORTE, _GPIO_PINMASK_2); RST = 0; // RST izalzni
  GPIO_Digital_Input (&GPIO_PORTD, _GPIO_PINMASK_5); GSMon = 0; // GSMon ulazni

  // inicijalizacija promenljivih za GSM
  GSMpodaci.vremeGSM = 0;
  GSMpodaci.greskaGSM = 0;
  GSMpodaci.stanjeGSM = 0;
  GSMpodaci.ispravnostGSM = 0;

  // inicijalizacija UART-a
  UART0_Init (9600);
  // broj 21, 5
  UART0_LCRH.B4 = 0; // iskljucen FIFO
  UART0_IM.B4 = 1; // dozvoljavanje prekida prijema
  //UART0_RIS = 0x00000010; // slizi da se vidi status prekida
  UART0_ICR.B4 = 1; // brisanje flega
  NVIC_PRI5.B15 = 0; // prioritet , veci nego tajmer 3
  NVIC_PRI5.B14 = 1;
  NVIC_PRI5.B13 = 0;
  NVIC_IntEnable(IVT_INT_UART0); // dozvoljavanje prekida UARTa
  EnableInterrupts(); // dozvoljavanje prekida

  RTS = 0; // uvek je nula sto znaci da je kontroler uvek spreman da primi poruku od GSM modula

  RST = 1;
  Delay_ms(2500);
  RST = 0;

  Delay_ms(3000); // cekanje da se GSM modul inicijalizuje

  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi

  // ostvarenje komunikacije sa GSM modulom i podesavanje protoka
  while (1)
  {
    saljiAT (atc); // slanje AT
    Delay_ms(100); // cekanje odgovora
    if ( (brojacPrijema)&&(baferUART[0]=='A')&&(baferUART[1]=='T')&&(baferUART[2]==13)&&(baferUART[3]==13)
    &&(baferUART[4]==10)&&(baferUART[5]=='O')&&(baferUART[6]=='K')&&(baferUART[7]==13)&&(baferUART[8]==10) )
    { break; } // sinhronizacija uspesna
    else { sinGreska++; }
    if ( sinGreska > 20 ) { GSMpodaci.ispravnostGSM = 1; return; } // greska u sinhronizaciji
  }

  Delay_ms(1000);
  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  
  // slanje komnde za iskljucivanje exa
  saljiAT (atExoOff);
  Delay_ms(1000);
  if ( !( (brojacPrijema)&&(baferUART[5]==13)&&(baferUART[6]==10)&&(baferUART[7]=='O')&&(baferUART[8]=='K')&&(baferUART[9]==13)&&(baferUART[10]==10) ) )
  { GSMpodaci.ispravnostGSM = 1; return; } // nije uspelo iskljucivanje exa

  Delay_ms(1000);
  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  
  // slanje komande za tekst format SMS poruka
  saljiAT (formatTextSMS);
  Delay_ms(2000);
  if ( !( (brojacPrijema)&&(baferUART[0]==13)&&(baferUART[1]==10)&&(baferUART[2]=='O')&&(baferUART[3]=='K')&&(baferUART[4]==13)&&(baferUART[5]==10) ) )
  { GSMpodaci.ispravnostGSM = 1; return; } // nije uspelo tekst formatiranje poruka

  Delay_ms(1000);
  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  
  // brisanje svih SMS poruka u inbox GSM modula
  saljiAT (erasingSMS);
  Delay_ms(2000);
  if ( !( (brojacPrijema)&&(baferUART[0]==13)&&(baferUART[1]==10)&&(baferUART[2]=='O')&&(baferUART[3]=='K')&&(baferUART[4]==13)&&(baferUART[5]==10) ) )
  { GSMpodaci.ispravnostGSM = 1; return; } // nije uspelo brisanje poruka
  
  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
}
//==============================================================================
// slanje AT komande
// argument tipO, 0 ocekivani odgovor je kratak, 1 ocekivani odgovor je SMS poruka
void saljiAT(const char *s)
{
  brojacPrijema = 0;
  while( CTS == 1 ){} // cekanje da CTS bude 0
  while(*s)     // send command string
  {
    UART0_Write(*s++);
  }
  UART0_Write(0x0D); // terminate command with CR
}
//==============================================================================
// slanje AT komande iz RAM memorija
// argument tipO, 0 ocekivani odgovor je kratak, 1 ocekivani odgovor je SMS poruka
void saljiATram(char *s)
{
  brojacPrijema = 0;
  while( CTS == 1 ){} // cekanje da CTS bude 0
  while(*s)     // send command string
  {
    UART0_Write(*s++);
  }
  UART0_Write(0x0D); // terminate command with CR
}
//==============================================================================
// slanje SMS poruke
// vraca se 0 ako je SMS poslat, vrati se 1 ako SMS nije poslat
// agrumenti su broj mobilnog na koji se salje poruka i tekst poruke
void slanjeSMS (char* brojZaSlanje, char* tekst)
{
  char i = 0; // brojacka promenljiva
  // ubacivanje broja mobilnog za slanje AT komande
  char sendSMS[30] = "AT+CMGS="; // AT komanda za slanje SMS
  for (i=0; i<30; i++)
  {
    sendSMS[i+8] = brojZaSlanje[i];
    if (brojZaSlanje[i]=='\0') { break; }
  }
  saljiATram (sendSMS);
  GSMpodaci.stanjeGSM = 4; // stanje za slanje teksta poruke
  GSMpodaci.vremeGSM = 0; // resetuje se brojac za GSM
  // ubacivanje teksta poruke u promenljivu za test
  strcpy (GSMpodaci.tekstSMS, tekst);
}
//==============================================================================
// funkcija za rad sa GSM modulom
void radGSM ()
{
  unsigned int i = 0; // brojacka promenljiva

  // da li se koristi GSM
  if ( (!parametarA[GSMaktivnost])||(GSMpodaci.ispravnostGSM) ) { return; }

  // funkcija se radi na svakih 5 sekundi
  if ( GSMpodaci.vremeGSM > 50 ) { GSMpodaci.vremeGSM = 0; }  // resetovanje brojaca
  else { return; }

  // provera da li je doslo do greske
  if ( GSMpodaci.greskaGSM > 10 )
  {
    GSMpodaci.ispravnostGSM = 1; return; // greska sa GSM modulom, GSM vise nije aktivan
  }

  switch (GSMpodaci.stanjeGSM)
  {
    case 0: // slanje AT komande za proveru da li je stigla SMS poruka
    {
      saljiAT (resiveSMS); // slanje ATkomande za proveru inbox GSM modula
      GSMpodaci.stanjeGSM = 1; // proverava se odgovor u narednom pozivu funkcije
      break;
    }
    case 1: // provera odgovora modula da li je stigla SMS poruka i procesiranje poruke ako je stigla
    {
      if ( (brojacPrijema)&&(baferUART[2]=='+')&&(baferUART[3]=='C')&&(baferUART[7]==':')&&(baferUART[12]=='C') )
      { // stigla je SMS poruka, sledi njeno procesiranje
        // izvlacenje broja sa kog je poruka poslata
        i=0;
        while (1)
        {
          if (baferUART[22+i]==',') { GSmpodaci.brojMob[i]='\0'; break; }
          GSMpodaci.brojMob[i] = baferUART[22+i];
          i++;
        }

        // provera sadrzaja poruke
        if ( (baferUART[63]==13)&&(baferUART[64]==10)&&(baferUART[65]=='G')&&(baferUART[66]=='P')&&(baferUART[67]==' ')/*&&(baferUART[70]==13)*/ )
        { // stigla je odgovarajuca poruka
          switch (baferUART[68])
          {
            case 'S': { saljiPodatke(); break; } // slanje podataka
            case 'B': { podesavanjeBroja(); break; } // podesavanje broja za slanje alarmnih poruka
            case 'K': { daljinskaKomanda(); break; } // pali, gasi gorionik
            default: { slanjeSMS ( GSMpodaci.brojMob, "Postovani korisnice poruka je primljena ali je malo nejasna. Molim Vas pokusajte ponovo." ); break; }
          }
        }
        else
        { // nije stigla odgovarauca poruka
          slanjeSMS ( GSMpodaci.brojMob, "Postovani korisnice poruka je primljena ali je malo nejasna. Molim Vas pokusajte ponovo." );
        }
      }
      else
      { // nema SMS poruke
        GSMpodaci.stanjeGSM = 0; // u sledecem ciklusu se ponovo proverava da li ima poruke
      }
      break;
    }
    case 2: // slanje AT za brisanje inbox-a
    {
      saljiAT (erasingSMS); // slanje AT komande za brisanje svih SMS u inbox GSM modula
      GSMpodaci.stanjeGSM = 3; // u sledecem ciklusu se proverava da li je brisanje inbox uspelo
      break;
    }
    case 3: // proverava da li je brisanje inbox uspelo
    {
      if ( !( (brojacPrijema)&&(baferUART[0]==13)&&(baferUART[1]==10)&&(baferUART[2]=='O')&&(baferUART[3]=='K')&&(baferUART[4]==13)&&(baferUART[5]==10) ) )
      { GSMpodaci.greskaGSM++; } // doslo je do greske i brojac greske se inkrementira
      GSMpodaci.greskaGSM = 0;
      GSMpodaci.stanjeGSM = 0; // u sledecem ciklusu se ponovo proverava da li ima poruke
      break;
    }
    case 4: // provera da li je slanje SMS poruke prihvaceno i ako jeste salje se tekst poruke
    {
      if ( !( (brojacPrijema)&&(baferUART[0]==13)&&(baferUART[1]==10)&&(baferUART[2]=='>')&&(baferUART[3]==' ') ) )
      { GSMpodaci.greskaGSM++; GSMpodaci.stanjeGSM = 0; break; } // greska, nema slanja teksta
      // sve OK salje se tekst
      GSMpodaci.greskaGSM = 0;
      // pripremanje za slanje teksta SMS poruke
      brojacPrijema = 0;
      while( CTS == 1 ){}
      UART0_Write_Text (GSMpodaci.tekstSMS);
      UART0_Write (26);
      GSMpodaci.stanjeGSM = 5;
      break;
    }
    case 5: // provera da li je SMS poruka poslata
    {
      if ( !((baferUART[0]==13)&&(baferUART[1]==10)&&(baferUART[2]=='+')&&(baferUART[3]=='C')&&(baferUART[4]=='M')) )
      { GSMpodaci.greskaGSM++; }
      else { GSMpodaci.greskaGSM = 0; }
      GSMpodaci.stanjeGSM = 2;
      break;
    }
    default:
    {
      GSMpodaci.stanjeGSM = 0;
      break;
    }
  }
}
//==============================================================================
// funkcija koja salje SMS poruku o stanju gorionika
void saljiPodatke()
{
  char tekstAlarmnePoruke[250]; // string za tekst poruke
  char txt[7]; // string za vrednost senzora

  tekstAlarmnePoruke[0] = '\0'; // inicijalizacija teksta alarmne poruke

  // priprema teksta poruke
  strcpy ( tekstAlarmnePoruke, "Gorionik je " );
  
  if ( stanje.fazaRada > 1 ) // da li je gorionik ukljucen ili iskljucen
  { // gorionik je ukljucen
    strcat ( tekstAlarmnePoruke, "ukljucen na " );
    switch (stanje.jacinaRada)
    {
      case 0: // automatski rad
      {  
        strcat ( tekstAlarmnePoruke, "A " );
        IntToStr ( parametarA[zadTV], txt );
        strcat ( tekstAlarmnePoruke, txt );
        strcat ( tekstAlarmnePoruke, " oC. ");
        break;
      }
      case 1: { strcat ( tekstAlarmnePoruke, "P1. " ); break; }
      case 2: { strcat ( tekstAlarmnePoruke, "P2. " ); break; }
      case 3: { strcat ( tekstAlarmnePoruke, "P3. " ); break; }
      case 4: { strcat ( tekstAlarmnePoruke, "P4. " ); break; }
      case 5: { strcat ( tekstAlarmnePoruke, "P5. " ); break; }
      default: { break; }
    }
  }
  else { strcat ( tekstAlarmnePoruke, "iskljucen. " ); } // gorionik je iskljucen
  
  // slanje temperature dima ako se koristi sonda
  if (parametar[sondaDimnjace])
  {
    strcat ( tekstAlarmnePoruke, "Temperatura dima: " );
    IntToStr ( Tdimnjace, txt );
    strcat ( tekstAlarmnePoruke, txt );
    strcat ( tekstAlarmnePoruke, " oC. ");
  }
  
  // slanje temperature ambijenta ako se koristi sonda
  if (parametar[sondaAmbijenta])
  {
    strcat ( tekstAlarmnePoruke, "Temperatura ambijenta: " );
    IntToStr ( Tambijenta, txt );
    strcat ( tekstAlarmnePoruke, txt );
    strcat ( tekstAlarmnePoruke, " oC. ");
  }

  slanjeSMS ( GSMpodaci.brojMob, tekstAlarmnePoruke ); // slanje poruke
}
//==============================================================================
// funkcija koja podesava alarmni broj za SMS poruke
void podesavanjeBroja()
{
  char tekstAlarmnePoruke[250]; // string za tekst poruke
  unsigned short i = 0; // brojacka promenljiva

  tekstAlarmnePoruke[0] = '\0'; // inicijalizacija teksta alarmne poruke

  // broj sa kog je poslata poruka postaje broj za alarmne poruke
  for ( i=0; i<20; i++ ) { alarmniBroj[i] =  GSMpodaci.brojMob[i]; }

  // upisivanje alarmnog broja u eeprom
  for ( i=0; i<20; i++ )
  {
    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
    I2C0_Write( 90+i, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // slanje adrese eeprom-a za upis
    I2C0_Write( alarmniBroj[i], _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // slanje nizeg bajta parametra na visu adresu
    Delay_us(20);
  }

  strcpy ( tekstAlarmnePoruke, "Na ovaj broj mobilnog telefona ce se slati alarmne poruke." );

  slanjeSMS ( alarmniBroj, tekstAlarmnePoruke ); // slanje poruke
}
//==============================================================================
// funkcija koja podesava rad gorionika na osnovu SMS poruke
void daljinskaKomanda()
{
  char tekstAlarmnePoruke[250]; // string za tekst poruke
  char txt[7]; // string za vrednost senzora
  unsigned int i; // pomocna promenljiva

  tekstAlarmnePoruke[0] = '\0'; // inicijalizacija teksta alarmne poruke
  
  // provera da li je format poruke u redu
  if ( baferUART[69] != ' ' )
  { slanjeSMS ( GSMpodaci.brojMob, "Postovani korisnice poruka je primljena ali je malo nejasna. Molim Vas pokusajte ponovo." ); return; }
  
  // provera da li treba ugasiti gorionik
  if ( (baferUART[70]=='O')&&(baferUART[71]=='F')&&(baferUART[72]=='F') )
  { 
     stanje.fazaRada = 1; vremeRada = 0; podesavanjeRada (); ispisRada ();
     parametarA[pravilnoGasenje] = 1;  upisAparametra(pravilnoGasenje); // korisnik je ugasio gorionik
     saljiPodatke();
     return;
  }
  
  // podesavanje rada gorionika
  if ( (baferUART[70]=='A')||(baferUART[70]=='P') )
  {
    // provera da li treba ukljuciti gorionik
    if ( stanje.fazaRada < 2 ) // gorionik je ugasen i pali se
    {
      parametarA[pravilnoGasenje] = 0;  upisAparametra(pravilnoGasenje); // korisnik je upalio gorionik
      if (stanje.cekanje) // ako je cekanje aktivno, cekanje se deaktivira, a faza rada ostaje ista
      {
        stanje.cekanje = 0; vremeRada = 0; podesavanjeRada (); ispisRada ();
      }
      else
      {
        stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada ();
      }
    }
    // podesavanje rada
    if ( baferUART[70] == 'P' ) // ako je manuelni rad
    {
      // provera jacine rada
      i = baferUART[71] - 48; // konverzija char u broj
      if ( (i>0)&&(i<6) ) // provera, ako nije dobro poslata jacina rada postavlja se na P1
      { stanje.jacinaRada = i; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); }
      else { stanje.jacinaRada = 1; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); }
    }
    else // automatski rad
    {
      if ( parametar[sondaAmbijenta] ) // da li ima sonde ambijenta
      { // ima sonde
        stanje.jacinaRada = 0; // prelazi se u automatski rad
        // citanje zadate temperature
        i = (baferUART[72]-48)*10 + (baferUART[73]-48);
        // provera
        if ( (baferUART[71]==' ')&&(i>10)&&(i<parametar[maxTV]) )
        { // podatak o temperaturi je u redu
          parametarA[zadTV] = i;
        }
        // parametar o temperaturi nije u redu, zadata temperatura ostaje nepromenjena
        podesavanjeRada (); // novo podesavanje rada
        upisAparametra(pocetnaJacina); Delay_us(50); upisAparametra(zadTV);
        ispisRada (); // ispis rada
      }
      else
      { // nema sonde podesava se na P1
        stanje.jacinaRada = 1; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada ();
      }
    }
    saljiPodatke();
    return;
  }
  else { slanjeSMS ( GSMpodaci.brojMob, "Postovani korisnice poruka je primljena ali je malo nejasna. Molim Vas pokusajte ponovo." ); }
}
//==============================================================================