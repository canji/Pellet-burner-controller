// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za komunikaciju putem ethernet i GSM

#ifndef _Komunikacija_h_
#define _Komunikacija_h_

//==============================================================================
// konstante

// AT komande
extern const char atc[]; // provera GSM modula
extern const char atExoOff[]; // exo iskljucen
extern const char formatTextSMS[]; // format za tekstualne poruke
extern const char resiveSMS[]; // citanje SMS poruke iz prve lokacije u inbox
extern const char erasingSMS[]; // brisanje svih SMS poruka u inbox

//==============================================================================
// struktura za rad sa GSMom
typedef struct GSMstruktura
{
  unsigned int vremeGSM; // promenljiva za merenje vrmena koja odredjuje kad se radi sam GSMom
  unsigned short greskaGSM; // brojac gresaka GSM modula, ako se pojavi odredjen broj uzastopnih gresaka alarmira se korisnik
  unsigned char brojMob[20]; // niz za smestanje broja mobilnog telefona
  unsigned short stanjeGSM; // promenljiva za stanje rada sa GSM modulom, sluzi za tok funkcije radGSM
  unsigned short ispravnostGSM; // 0 rad sa GSM je u redu, greska u radu sa GSM
  char tekstSMS[250]; // tekst SMS poruke
} GSMstatus;
//------------------------------------------------------------------------------
// globalne promenljive

// struktura za rad se GSMom
extern GSMstatus GSMpodaci;

// broj bajtova koje primi UART od GSM modula
extern unsigned short brojacPrijema;

// podaci primljeni od GSM modula putem UARTa
extern char baferUART[512];

// broj mobilnogtelefona za slanje alarmnih poruka
extern char alarmniBroj[20];

//==============================================================================
// funkcije

// funkcija za ispsis IP adrese
void ispisiIP ();

// funkcija za ispis broja mobilnog za alarmne poruke
void ispisiMob ();

// funkcija za inicijalizaciju GSM
void initGSM ();

// slanje AT komande
void saljiAT(const char *s);

// slanje AT komande iz RAM memorija
void saljiATram(const char *s);

// slanje SMS poruke
// vraca se 0 ako je SMS poslat, vrava se 1 ako SMS nije poslat
// sgrumenti su broj mobilnog na koji se salje poruka i tekst poruke
void slanjeSMS (char* brojZaSlanje, char* tekstSMS);

// funkcija za rad sa GSM modulom
void radGSM ();

/*// funkcija koja se poziva kad dodje do greske u radu GSM
void greskaGSM ();*/

// funkcija koja salje SMS poruku o stanju gorionika
void saljiPodatke();

// funkcija koja podesava alarmni broj za SMS poruke
void podesavanjeBroja();

// funkcija koja podesava rad gorionika na osnovu SMS poruke
void daljinskaKomanda();

//==============================================================================
#endif