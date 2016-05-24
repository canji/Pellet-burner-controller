// program za upravljanje gorionikom na pelete
// definicije globalnih promenljivih i funkcija za debaunsiranje digitalnih ulaza

#include "GNPfunkcije.h"
#include "Parametri.h"
#include "Temperatura.h"
#include "Sat.h"
#include "DIulazi.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// pinovi

sbit TERMOSTAT at GPIO_PORTD_DATA7_bit; // spoljni TERMOSTAT
// sbit FOTOCELIJA at GPIO_PORTE_DATA3_bit; // fotocelija za sad ce biti za NTC sondu
sbit TASTERONOFF at GPIO_PORTH_DATA3_bit; // taster za on-off
// bice trenutno fotocelija
sbit TASTERJACINA at GPIO_PORTH_DATA2_bit; // taster za promenu jacine rada
sbit FOTOCELIJA at GPIO_PORTH_DATA1_bit; // FOTOCELIJA,d je na PH1, a ne na PE3, zauzima mesto PRESOOSTATA

//------------------------------------------------------------------------------
// konstante

const debaunKratko = 3; // debaunsiranje 3 ms, kratko
const debaunDugacko = 20; // debaunsiranje 2 s, dugacko

//------------------------------------------------------------------------------
// globalne promenljive

// brojaci desetinki za debaunsiranje
unsigned char DtasterOnOff = 0; // debaunsiranje tastera on off
unsigned char DtasterJacina = 0; // debaunsiranje tastera jacina
unsigned char Dtermostat = 0; // debaunsiranje termostata
unsigned char Dfotocelija = 0; // debaunsiranje fotocelije

// promenljive koje programu govore o stanju digitalnih ulaza
unsigned char stanjeTermostat = 0; // 1 temperatura je presla zadatu, 0 nije
unsigned char stanjeFotocelija = 0; // 1 ima plamena, 0 nema

//==============================================================================
// inicijalizacija digitalnih ulaza
void initDIulazi ()
{
   // inicijalizacija pina za spoljni termostat
  GPIO_Digital_Input (&GPIO_PORTD, _GPIO_PINMASK_7); // TERMOSTAT je ulazni

  // inicijalizacija pina za fotoceliju
  GPIO_Digital_Input (&GPIO_PORTH, _GPIO_PINMASK_1); // FOTOCELIJA je ulazni, sad je na PH1, a ne na PE3

  // inicijalizacina pinova za tastere
  GPIO_Digital_Input (&GPIO_PORTH, _GPIO_PINMASK_3); // TASTERONOFF je ulazni
  GPIO_Digital_Input (&GPIO_PORTH, _GPIO_PINMASK_2); // TASTERJACINA je ulazni
}
//==============================================================================
// funkcija za proveru tastera
void tasteri ()
{
  // provera da li je pritisnut taster on off dovoljno dugo
  if ( (TASTERONOFF==1)&&(DtasterOnOff>=debaunKratko) )
  {
    paliGasi ();
    while (TASTERONOFF);  // ceka se da se pusti taster
  }

  // resetovanje promenljive za debaunsiranje tastera on off
  if ( TASTERONOFF==0 ) { DtasterOnOff = 0; }

  // provera da li je pritisnut taster jacina dovoljno dugo
  if ( (TASTERJACINA==1)&&(DtasterJacina>=debaunKratko) )
  {
    if ( (stanje.fazaRada>1)&&(stanje.jacinaRada) ) // taster funkcionise samo kad je gorionik ukljucen i kad je rad manuelan
    {
      if ( stanje.jacinaRada == 5 ) { stanje.jacinaRada = 1; podesavanjeRada (); ispisRada (); } // pose P5 se prebacuje na P1
      else { stanje.jacinaRada++; podesavanjeRada (); ispisRada (); }
    }
    while (TASTERJACINA);  // ceka se da se pusti taster
  }

  // resetovanje promenljive za debaunsiranje tastera jacina
  if ( TASTERJACINA==0 ) { DtasterJacina = 0; }
}
//==============================================================================
// funkcija za proveru digitalnih ulaza
void digitalniUlazi ()
{
  // proveta tastera
  tasteri ();
  
  // provera termostata
  if ( stanjeTermostat ) // u kom je stanju termosta
  { // TERMOSTAT je otvoren sto znaci da je temperatura prevelika
    if ( (TERMOSTAT==0)&&(Dtermostat>debaunDugacko) ) 
    { stanjeTermostat = 0; } // temperatura je pala i TERMOSTAT je zatvorio
    if ( TERMOSTAT==1 ) { Dtermostat = 0; } // TERMOSTAT je jos otvoren
  }
  else
  {  // TERMOSTAT je zatvoren sto zanci da temperatura nije prevelika
    if ( (TERMOSTAT=1)&&(Dtermostat>debaunDugacko) )
    { stanjeTermostat = 1; } // temperatura je porasla i TERMOSTAT je otvorio
    if ( TERMOSTAT==0 ) { Dtermostat = 0; } // TERMOSTAT je jos zatvoren
  }

  // provera termostata
  if ( stanjeFotocelija ) // u kom je stanju fotovelija
  { // FOTOCELIJA je otvoren sto znaci da je ima plamena
    if ( (FOTOCELIJA==0)&&(Dfotocelija>debaunDugacko) )
    { stanjeFotocelija = 0; } // nema plamena
    if ( FOTOCELIJA==1 ) { Dfotocelija = 0; } // jos ima plamena
  }
  else
  {  // FOTOCELIJA je nula sto znaci da nema plamena
    if ( (FOTOCELIJA=1)&&(Dfotocelija>debaunDugacko) )
    { stanjeFotocelija = 1; } // pojavio se palmen
    if ( FOTOCELIJA==0 ) { Dfotocelija = 0; } // jos nema plamena
  }
}
//==============================================================================