// program za upravljanje gorionikom na pelete

#include "GNPfunkcije.h"
#include "Temperatura.h"
#include "Parametri.h"
#include "Sat.h"
#include "DIulazi.h"
#include "Komunikacija.h"
#include "MiniWebServer.h"
#include "__NetEthInternal_Stellaris.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// pinovi za funkcionisanje motora
// ovde se deinisu i inicijalizuju zato sto se samo koriste u prekidnoj rutini koja je ovde definisana

sbit M1 at GPIO_PORTE_DATA6_bit; // motor 1
sbit M2 at GPIO_PORTD_DATA2_bit; // motor 2
sbit CISTAC at GPIO_PORTE_DATA7_bit; // cistac
// PD6 je za spoljasnji prekid, prolazak kroz nulu
// PC6 PWM za okidanje triaka

unsigned short tok = 0; // promenljiva za tok glavnog programa
unsigned short pojacanoDuvanjeStaro = 0; // promenljiva koja pamti staru vrednost za pojacano duvanje
unsigned int podesavanjeRadaVreme = 0; // brojacka promenljiva uz pomoc koje se proverava kad je vreme za podesavanje rada

//==============================================================================
// prekidne rutine

// prekid tajmera 3 na svakih 0.1 sekundu,
// sluzi za inkrementiranje brojaca na svakih 100ms i paljenje i gasnje DC motora
// ova prekidna rutina je nizeg prioriteta
void prekidNa100ms() iv IVT_INT_TIMER3A ics ICS_OFF
{
  TIMER3_ICR.B0 = 1; // brisanje flega
  
  vremeRada++; // inkrementiranje brojaca za rad sa fazama rada
  DtasterOnOff++; // inkrementiranje promenljive za debaunsiranje tastera on off
  DtasterJacina++; // inkrementiranje promenljive za debaunsiranje tastera jacina
  Dtermostat++; // inkrementiranje promenljive za debaunsiranje termostata
  Dfotocelija++; // inkrementiranje promenljive za debaunsiranje fotocelije
  podesavanjeRadaVreme++; // inkrementiranje promenljive za proveru rada
  brojacTD++; // brojanje intervala od 10 sekundi kad se cita temperatura dima
  brojacTA++; // brojac intervala od 10 sekundi kad se cita DS18S20
  GSMpodaci.vremeGSM++; // brojac za rad sa GSM-om
  pomWebBrojac++; // brojac za mini web server
  if (pomWebBrojac==10) { pomWebBrojac = 0; Net_Ethernet_Intern_UserTimerSec++ ; }

  // provera da li treba da radi M1---------------------------------------------
  vremeDC++;  // inkrementiranje brojaca za rad sa motorima M1 i M2
  if ( vremeDC < tM1 ) { M1 = 1; }
  else { M1 = 0; }
  // provera da li treba da radi M2
  if ( vremeDC < (tM1+M2duzeM1) ) { M2 = 1; }
  else
  {
    M2 = 0;
  }
  // resetovanje brojaca desetinki
  if ( vremeDC >= (tM1+pM1) ) { vremeDC = 0; }
  //----------------------------------------------------------------------------
  
  // rad sa pojacanim duvanjem--------------------------------------------------
  // pojacano duvanje i ciscenje se vrsi samo u normalnom radu, modulaciji i max T
  if ( (stanje.fazaRada > 4)||(testCistaca) )
  {
    vremeDuvanje++;  // inkrementiranje brojaca za pojacano duvanje-------------
    if ( vremeDuvanje > parametar[intervalDuvanja] ) { stanje.pojacanoDuvanje = 1; }
    else { stanje.pojacanoDuvanje= 0; }
    if ( vremeDuvanje > parametar[intervalDuvanja]+parametar[duzinaDuvanja] )
    { stanje.pojacanoDuvanje = 0; vremeDuvanje = 0; }
    //--------------------------------------------------------------------------
    
    // provera da li treba zavrsiti testiranje
    if ( vremeCiscenje==0xFFFF ) { testCistaca = 0; vremeCiscenje = 0; }
    
    vremeCiscenje++; // inkrementiranje brojaca za rad sistema za ciscenje------
    // interval kad CISTAC radi
    if ( (vremeCiscenje > parametar[intervalCiscenja])&&(vremeCiscenje < parametar[intervalCiscenja]+parametar[duzinaCiscenja]) )
    { CISTAC = 1; }
    else { CISTAC = 0; }
    // interval kad se CISTAC gasi i kad se njegov brojac resetuje
    if ( vremeCiscenje > parametar[intervalCiscenja]+parametar[duzinaCiscenja] )
    { CISTAC = 0; vremeCiscenje = 0; }
  }
  else 
  { 
    vremeDuvanje = 0; vremeCiscenje = 0;
    if ( stanje.fazaRada == 1 ) // ako je faza gasenja CISTAC radi non stop
    {
      CISTAC = 1;
    }
    else { CISTAC = 0; } // ako je neka druga faza
  }
}

//------------------------------------------------------------------------------
// prekidna rutina eksternog prekida na ivicu ulaznog signala, prolazak kroz nulu
void prolazKorzNulu() iv IVT_INT_GPIOD ics ICS_OFF
{
  PWM_CCP0_Stop ();  // prestanak slanja povorke impulsa triaku
  TIMER2_CTL.B0 = 1;  // startovanje tajmera 2
  GPIO_PORTD_ICR.B6 = 1;  // brisanje flega
}

//------------------------------------------------------------------------------
// prekidan rutina tajmera 2 kojeg startuje prolazak kroz nulu, ukljucenje triaka
void okidanjeTriaka() iv IVT_INT_TIMER2A ics ICS_OFF
{
  PWM_CCP0_Start ();  // slanje povorke impulsa triaku
  TIMER2_CTL.B0 = 0;  // zaustavljanje tajmera 2
  TIMER2_ICR.B0 = 1;  // brisanje flega
}
//------------------------------------------------------------------------------
// prekidna rutina prijema UART-a
void prijemUART() iv IVT_INT_UART0 ics ICS_OFF
{
  if ( UART0_MIS.B4 == 1 ) // da li je prekid od prijema
  {
    UART0_ICR.B4 = 1; // brisanje flega
    baferUART[brojacPrijema] = UART0_Read(); // citanje UARTa
    brojacPrijema++; // inkrementiranje brojaca
  }
}
//==============================================================================

void main() 
{
  //----------------------------------------------------------------------------
  // inicijalizacija za pinove DC motora
  GPIO_Digital_Output (&GPIO_PORTE, _GPIO_PINMASK_6); M1 = 0; // M1 izalzni
  GPIO_Digital_Output (&GPIO_PORTD, _GPIO_PINMASK_2); M2 = 0; // M2 izlazni
  GPIO_Digital_Output (&GPIO_PORTE, _GPIO_PINMASK_7); CISTAC = 0; // CISTAC je izalzni
  
  // pozivanje funkcija za inicijalizaciju
  initOsnovno ();
  initTemperatura ();
  initSat ();
  initDIulazi ();
  //----------------------------------------------------------------------------
  // inicijalizacija stanja
  stanje.displej = 0;  // prikazuje se prikaz rada
  stanje.jacinaRada = 1;  // manuelan rad najmanje jacine
  stanje.fazaRada = 0;  // gorionik je ugasen
  stanje.pojacanoDuvanje = 0; // pojacano duvanje nije u toku
  stanje.cekanje = 0; // cekanje nije aktivno
  //----------------------------------------------------------------------------
  // inicijalizacija i prvi ispis displeja
  stanje.displej = 1; // prikaz menija da se ne bi ispisivale temperature prilikom inicijalizacije
  Start_TP(); // inicijalizacija displeja
  //----------------------------------------------------------------------------
  // citanje parametara i inicijalizacija rada na osnovu njih

  citanjeEEPROM(); // citanje eeprom memorije

  stanje.jacinaRada = parametarA[pocetnaJacina]; // pocetna jacina rada
  if ( (parametar[sondaAmbijenta]==0)&&(stanje.jacinaRada==0) ) { stanje.jacinaRada = 1; } // ne moze automatski rad ako nema sonde ambijenta

  if ( parametarA[pravilnoGasenje] ) // da li je gorionik ugasio korisnik
  { stanje.fazaRada = 0; } // korisnik je ugasio gorionik
  else { stanje.fazaRada = 2; } // ako korisnik nije ugasio gorionik on se sam pali
  //----------------------------------------------------------------------------
  // kalibtacija displeja ako je potrebna
  if ( (parametarA[Xminimum]==0)||(parametarA[Yminimum]==0)||(parametarA[Xmaksimum]==0)||(parametarA[Ymaksimum]==0) )
  { // potrebna je kalibracija
    Delay_ms(1000);
    TFT_Fill_Screen(0);
    Calibrate();
    TFT_Fill_Screen(0);
    TP_TFT_Get_Calibration_Consts( &parametarA[Xminimum], &parametarA[Xmaksimum], &parametarA[Yminimum], &parametarA[Ymaksimum]);
    // upisivanje podataka o kalibraciji u memoriju
    upisAparametra(Xminimum); Delay_us(50);
    upisAparametra(Xmaksimum); Delay_us(50);
    upisAparametra(Yminimum); Delay_us(50);
    upisAparametra(Ymaksimum); Delay_us(50);
  }
  else // podaci za kalibraciju uzimaju se iz parametara
  { TP_TFT_Set_Calibration_Consts( parametarA[Xminimum], parametarA[Xmaksimum], parametarA[Yminimum], parametarA[Ymaksimum]); }
  //----------------------------------------------------------------------------
  EnableInterrupts(); // dozvoljavanje prekida
  //----------------------------------------------------------------------------
  // podesavanje wtchdog tajmera
  SYSCTL_RCGC0_WDT0_bit = 1; // davanje takta tajmeru
  WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  WDT_CTL_RESEN_bit = 1;                // Enable the watchdog timer reset
  WDT_CTL_INTEN_bit = 1;                // Enable watchdog timer
  //----------------------------------------------------------------------------
  // inicijalizacija komunikacije
  initGSM(); // inicijalizacija GSM-a
  initMiniWebServer(); // inicijalizacija min web servera
  //----------------------------------------------------------------------------
  // prvo citanje temperatura
  Tdimnjace = 0;
  Tambijenta = 0;

  while ( (Tdimnjace == 0)&&(parametar[sondaDimnjace]) ) { temperaturaD (); } // prvo citanje temperature dima

  while ( (Tambijenta == 0)&&(parametar[sondaAmbijenta]) ) { temperaturaA (); } // prvo citanje temperature ambijenta

  //----------------------------------------------------------------------------
  stanje.displej = 0; // ispis rada
  ispisRada (); // ispis rada
  
  tok = 0; // inicijalizacija promenljive za tok glavnog programa
  pojacanoDuvanjeStaro = stanje.pojacanoDuvanje; // promenljiva za rad pojacanog duvanja

  // beskonacna petlja
  while (1)
  {
    switch (tok)
    {
      case 0: // podesavanje rada u trenucima kad treba aktivirati ili deaktivirai pojacano duvanje
      { 
        if ( pojacanoDuvanjeStaro != stanje.pojacanoDuvanje ) 
        { pojacanoDuvanjeStaro = stanje.pojacanoDuvanje; podesavanjeRada (); } 
        tok = 1; break;
      }
      case 1: { promenaStanja (); tok = 2; break; } // promena stanja
      case 2: { temperaturaD (); tok = 3; break; } // rad sa temperaturom dimnjace
      case 3: { temperaturaA (); tok = 4; break; } // rad sa temperaturom ambijenta
      case 4: { proveraHrono (); tok = 5; break; } // rad sa hronotermostatom
      case 5: { radGSM(); tok = 6; break; }
      case 6: { if ( podesavanjeRadaVreme > 599 ) { podesavanjeRadaVreme = 0; podesavanjeRada (); } tok = 0; break; } // podesavanje rada svaki minut
      default: { tok = 0; break; }
    }
    digitalniUlazi ();
    Check_TP(); // rad sa displejom
    Net_Ethernet_Intern_doPacket(); // mini web server

    WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  }
}