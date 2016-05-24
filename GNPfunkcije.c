// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za osnovni rad gorionika

#include "GNPfunkcije.h"
#include "Parametri.h"
#include "Temperatura.h"
#include "Sat.h"
#include "DIulazi.h"
#include "Komunikacija.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// pinovi

sbit G at GPIO_PORTE_DATA5_bit; // grejac

//------------------------------------------------------------------------------
// konstante

const Tm = 10; // poluperioda mreznog napona u milisekundama

//------------------------------------------------------------------------------
// globalne promenljive

// struktura za stanje gorionika
// displej je 0 kad se prikazuje rad, a 1 kad se prikazuje meni
// jacinaRada je 0 kad je rad automatski, a 1, 2, 3, 4 ili 5 je kad je rad manuelan
// fazaRada odredjuje fazu rada gorionika
// pojacano duvanje odredjuje da li je pojacano duvanje u toku
stanjeGorionika stanje;

// promenljive za rad DC motora
unsigned int tM1 = 0; // duzina rada motora M1 u desetinkama
unsigned int pM1 = 0; // pauza u radu motora M1 u desetinkama
unsigned int M2duzeM1 = 0; // koliko M2 radi duze od M1 u desetinkama

// brojaci desetinki
unsigned int vremeDC = 0; // brojac za rad motora M1 i M2
unsigned int vremeRada = 0; // brojac za rad sa fazama rada
unsigned int vremeDuvanje = 0; // brojac za pojacano duvanje
unsigned int vremeCiscenje = 0; // brojac za upravljanje sistemom za ciscenje

//==============================================================================
// funkcija za inicijalizaciju mikrokontrolera za osnovnu funkciju gorionika
void initOsnovno ()
{
  unsigned int prom = 0; // pomocna promenljiva

  // inicijalizacija pina za grejac---------------------------------------------
  GPIO_Digital_Output (&GPIO_PORTE, _GPIO_PINMASK_5); G = 0; // G izalzni
  
  // PWM na pinu PC6
  prom = PWM_CCP0_Init (12500, &_GPIO_MODULE_CCP0_C6); // podesavanje periode
  PWM_CCP0_Set_Duty (4000, _PWM_INVERTED_DISABLE); // podesavanje duty
  PWM_CCP0_Stop ();

  // spoljasnji prekid na pinu PD6----------------------------------------------
  GPIO_PORTD_AFSEL6_bit = 0; // pin nije analogni
  GPIO_PORTD_DIR6_bit = 0; // pin je ulazni
  GPIO_PORTD_ODR.B6 = 0; // nije open drain
  GPIO_PORTD_DEN6_bit = 1; // pin je digitalni
  GPIO_PORTD_PUR.B6 = 0; // nema pullup
  GPIO_PORTD_PDR.B6 = 0; // nema pulldown

  GPIO_PORTD_IS.B6 = 0; // prekid je na ivicu
  GPIO_PORTD_IBE.B6 = 1; // prekid je na obe ivice
  GPIO_PORTD_IEV.B6 = 0; // ovo bi trebalo da nema veze
  GPIO_PORTD_IM.B6 = 0; // maskira se prekid
  GPIO_PORTD_IM.B6 = 1; // demaskira se prekid

  NVIC_PRI0 = 0; // najveci prioritet 0
  NVIC_IntDisable(IVT_INT_GPIOD); // zabrana prekida porta D
  //----------------------------------------------------------------------------
  // tajmeri

  // tajmer 3 za interval od 0.1s-----------------------------------------------
  SYSCTL_RCGC1.B19 = 1; // davanje takata tajmeru 3
  Delay_ms (1);
  TIMER3_CTL = 0; // tajmer je ugasen
  TIMER3_CTL.B0 = 0;
  TIMER3_CFG = 0; // 32-bitni tajmer
  TIMER3_TAMR = 0x2; // periodicni mod
  TIMER3_TAILR = Get_Fosc_kHz () * 100;  // podesavanje duzine brojanja
  NVIC_IntEnable(IVT_INT_TIMER3A); // dozvoljavanje prekida tajmera 3
  NVIC_PRI8 = 0x60000000; // prioritet 3
  TIMER3_IMR.B0 = 1; // dozvoljavanje timeout prekida tajmera 3
  TIMER3_CTL.B0 = 1; // ukljucivanje tajmera

  // tajmer 2 za okidanje triaka------------------------------------------------
  SYSCTL_RCGC1.B18 = 1; // davanje takata tajmeru 2
  Delay_ms (1);
  TIMER2_CTL = 0; // tajmer je ugasen
  TIMER2_CTL.B0 = 0;
  TIMER2_CFG = 0; // 32-bitni tajmer
  TIMER2_TAMR = 0x1; // one-shot mod
  NVIC_IntEnable(IVT_INT_TIMER2A); // dozvoljavanje prekida tajmera 2
  NVIC_PRI5 = 0; // najveci prioritet 0
  TIMER2_IMR.B0 = 1; // dozvoljavanje timeout prekida tajmera 2
  
  // U PROGRAMU SE MORAJU DOZVOLITI PREKIDI    EnableInterrupts();
  DisableInterrupts();
  //----------------------------------------------------------------------------
}
//==============================================================================
// funkcija za podesavanje snage AC motora, motor za ubacivanje vazduha
// argunemt je snaga motora u procentima
void ACmotor (unsigned int ac)
{
  // tababela koja zeljeni procenat snage pretvara u normalizovani ugao neprovodjenja triaka
  const double snagaUugao [100] =
  {
  /* 0 -  9 */ 1,      0.8840, 0.8531, 0.8310, 0.8132, 0.7980, 0.7846, 0.7724, 0.7612, 0.7508,
  /*10 - 19 */ 0.7411, 0.7319, 0.7231, 0.7147, 0.7067, 0.6989, 0.6915, 0.6842, 0.6772, 0.6703,
  /*20 - 29 */ 0.6637, 0.6571, 0.6508, 0.6445, 0.6384, 0.6324, 0.6264, 06206., 0.6148, 0.6092,
  /*30 - 39 */ 0.6036, 0.5981, 0.5926, 0.5872, 0.5818, 0.5765, 0.5712, 0.5659, 0.5608, 0.5556,
  /*40 - 49 */ 0.5504, 0.5453, 0.5402, 0.5352, 0.5301, 0.5251, 0.5201, 0.5150, 0.5100, 0.5050,
  /*50 - 59 */ 0.5000, 0.4950, 0.4900, 0.4850, 0.4800, 0.4749, 0.4699, 0.4649, 0.4598, 0.4547,
  /*60 - 69 */ 0.4496, 0.4445, 0.4392, 0.4341, 0.4288, 0.4236, 0.4182, 0.4129, 0.4075, 0.4019,
  /*70 - 79 */ 0.3964, 0.3908, 0.3852, 0.3794, 0.3736, 0.3676, 0.3616, 0.3555, 0.3492, 0.3428,
  /*80 - 89 */ 0.3363, 0.3297, 0.3228, 0.3157, 0.3085, 0.3011, 0.2933, 0.2853, 0.2769, 0.2681,
  /*90 - 99 */ 0.2589, 0.2492, 0.2388, 0.2276, 0.2154, 0.2020, 0.1868, 0.1689, 0.1469, 0.1161
  };

  TIMER2_TAILR = Get_Fosc_kHz () * snagaUugao[ac] * Tm;  // podesavanje duzine brojanja
 
  // da li je motor ugasen
  if ( ac == 0 ) { NVIC_IntDisable(IVT_INT_GPIOD); PWM_CCP0_Stop (); }
  else { NVIC_IntEnable(IVT_INT_GPIOD); } // dozvoljavanje prekida porta D
}
//==============================================================================
// funkcija koja podesava parametra rada na osnovu stanja
 void podesavanjeRada ()
{
  switch ( stanje.fazaRada )
  {
    case 0: // gorionik ugasen
    {
      vremeDC = 0;
      tM1 = 0;
      M2duzeM1 = 0;
      pM1 = 0;
      ACmotor (0); // AC motor ne radi u ovom stanju
      G = 0; // grejac je iskljucen
      return;
    }
    case 1: // gasenje
    {
      vremeDC = 0;
      tM1 = 0;
      M2duzeM1 = 0;
      pM1 = 0;
      ACmotor (parametar[SVgasenje]); // AC motor radi
      G = 0; // grejac je iskljucen
      return;
    }
    case 2: // usijavanje
    {
      // prvi deo ubacivanja se cisti gorionik duvanjem, a u drugom ide klasicno usijavanje
      if ( vremeRada > 300 )
      { // klasicno ubacivanje
        tM1 = parametar[tM1prvoUbacivanje]; // DC motori
        M2duzeM1 = parametar[M2tM1];
        pM1 = parametar[usijavanje]+3;
        ACmotor (0); // AC motor ne radi u ovom stanju
      }
      else // ciscenje
      {
        vremeDC = 0;
        tM1 = 0;
        M2duzeM1 = 0;
        pM1 = 0;
        ACmotor (parametar[SVduvanje]);
      }
      G = 1; // grejac radi u ovom stanju
      return;
    }
    case 3: // ubacivanje
    {
      tM1 = parametar[tM1ubacivanje]; // DC motori
      M2duzeM1 = parametar[M2tM1];
      pM1 = parametar[pM1ubacivanje];
      ACmotor (parametar[SVubacivanje]); // AC motor radi u ovom stanju
      G = 1; // grejac radi u ovom stanju
      return;
    }
    case 4: // stabilizacija
    {
      tM1 = parametar[tM1stabilizacija]; // DC motori
      M2duzeM1 = parametar[M2tM1];
      pM1 = parametar[pM1stabilizacija];
      ACmotor (parametar[SVstabilizacija]); // AC motor ne radi u ovom stanju
      G = 0; // grejac je iskljucen
      return;
    }
    case 5: // redovan rad
    {
      G = 0; // grejac je iskljucen
      M2duzeM1 = parametar[M2tM1]; // razlika rada M1 i M2 je uvek ista
      switch (stanje.jacinaRada) // podesavanje jacine rada
      {
        case 0: // automatski rad
        {
          // ako je razlika izmedju zadate temperature i temperature ambijenta veca od 5
          if ( parametarA[zadTV]-Tambijenta > parametar[tacka1] )
          { // podesi P5
            tM1 = parametar[tM1redovanP5];
            pM1 = parametar[pM1redovanP5];
            if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
            else { ACmotor (parametar[SVredovanP5]); }
            break;
          }
          // ako je razlika izmedju zadate temperature i temperature ambijenta veca od 3, a manja od 5
          if ( parametarA[zadTV]-Tambijenta > parametar[tacka2] )
          { // podesi P4
            tM1 = parametar[tM1redovanP4];
            pM1 = parametar[pM1redovanP4];
            if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
            else { ACmotor (parametar[SVredovanP4]); }
            break;
          }
          // ako je razlika izmedju zadate temperature i temperature ambijenta veca od 1, a manja od 3
          if ( parametarA[zadTV]-Tambijenta > parametar[tacka3] )
          { // podesi P3
            tM1 = parametar[tM1redovanP3];
            pM1 = parametar[pM1redovanP3];
            if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
            else { ACmotor (parametar[SVredovanP3]); }
            break;
          }
          // ako je razlika izmedju zadate temperature i temperature ambijenta veca od 0, a manja od 1
          if ( parametarA[zadTV]-Tambijenta >= 0 )
          { // podesi P2
            tM1 = parametar[tM1redovanP2];
            pM1 = parametar[pM1redovanP2];
            if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
            else { ACmotor (parametar[SVredovanP2]); }
            break;
          }
        }
        case 1: // P1
        {
          tM1 = parametar[tM1redovanP1];
          pM1 = parametar[pM1redovanP1];
          if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
          else { ACmotor (parametar[SVredovanP1]); }
          break;
        }
        case 2: // P2
        {
          tM1 = parametar[tM1redovanP2];
          pM1 = parametar[pM1redovanP2];
          if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
          else { ACmotor (parametar[SVredovanP2]); }
          break;
        }
        case 3: // P3
        {
          tM1 = parametar[tM1redovanP3];
          pM1 = parametar[pM1redovanP3];
          if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
          else { ACmotor (parametar[SVredovanP3]); }
          break;
        }
        case 4: // P4
        {
          tM1 = parametar[tM1redovanP4];
          pM1 = parametar[pM1redovanP4];
          if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
          else { ACmotor (parametar[SVredovanP4]); }
          break;
        }
        case 5: // P5
        {
          tM1 = parametar[tM1redovanP5];
          pM1 = parametar[pM1redovanP5];
          if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
          else { ACmotor (parametar[SVredovanP5]); }
          break;
        }
        default:
        {
          stanje.jacinaRada = 1;
          break;
        }
      }
      return;
    }
    case 6: // modulacija
    {
      tM1 = parametar[tM1redovanP1];
      M2duzeM1 = parametar[M2tM1];
      pM1 = parametar[pM1redovanP1];
      if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
      else { ACmotor (parametar[SVredovanP1]); }
      G = 0; // grejac je iskljucen
      return;
    }
    case 7: // maxT
    {
      tM1 = parametar[tM1redovanP1];
      M2duzeM1 = parametar[M2tM1];
      pM1 = parametar[pM1redovanP1];
      if ( stanje.pojacanoDuvanje == 1 ) { ACmotor (parametar[SVduvanje]); } // da li je u toku ciscenje
      else { ACmotor (parametar[SVredovanP1]); }
      G = 0; // grejac je iskljucen
      return;
    }
    default:
    {
      stanje.fazaRada = 0;
      return;
    }
  }
}
//==============================================================================
// funkcija koja menja stanja na osnovu vremena i temperatura
void promenaStanja ()
{
  switch ( stanje.fazaRada )
  {
    case 0: // gorionik ugasen
    {
      // prelazak u usijacanje ako se pritisne taster ON, hronotermostat, internet ili GSM
      
      // IZLAZ IZ CEKANJA
      // izlaz iz cekanja zavisi da li je automatski ili manuelni rezim
      if (stanje.jacinaRada)
      { // manueleni rezim
        if ( (Tambijenta < parametar[maxTV] - 5)&&(!stanjeTermostat)&&(stanje.cekanje) ) // ako temperatura ambijenta padne za 5 ispod maksimalne izlazi se iz cekanja
        { stanje.cekanje = 0; stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      else
      { // automatski rezim
        if ( (Tambijenta <= parametarA[zadTV])&&(!stanjeTermostat)&&(stanje.cekanje) ) // temperatura ambijenta padne ispod zadate temperature izlazi se iz cekanja
        { stanje.cekanje = 0; stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      
      return;
    }
    case 1: // gasenje
    {
      // pritiskom na taster ON, hronotermostat, internet ili GSM gorionik se ponovo pali

      // ako temperatura dima padne ispod temperature za gasenje gasi se gorionik
      if ( (Tdimnjace < parametar[TDgasenje])&&(!stanjeFotocelija) )
      { stanje.fazaRada = 0; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      // IZLAZ IZ CEKANJA
      // izlaz iz cekanja zavisi da li je automatski ili manuelni rezim
      if (stanje.jacinaRada)
      { // manueleni rezim
        if ( (Tambijenta < parametar[maxTV] - 5)&&(!stanjeTermostat)&&(stanje.cekanje) ) // ako temperatura ambijenta padne za 5 ispod maksimalne izlazi se iz cekanja
        { stanje.cekanje = 0; stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      else
      { // automatski rezim
        if ( (Tambijenta <= parametarA[zadTV])&&(!stanjeTermostat)&&(stanje.cekanje) ) // temperatura ambijenta padne ispod zadate temperature izlazi se iz cekanja
        { stanje.cekanje = 0; stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      
      return;
    }
    case 2: // usijavanje
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // prvi deo usijavanja traje 30 sekundi i tada ventilator cisti gorionik, kad prodje taj trenutak poziva se podesavanje rada
      if ( vremeRada > 300 ) { podesavanjeRada (); }

      // ako je temperatura dima veca od temperature za ulazak u stabilizaciju prelazi se u stabilizaciju
      if ( (Tdimnjace > parametar[TDubacivanje])||(stanjeFotocelija) )
      { stanje.fazaRada = 4; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      // kad prodje odredjeno vreme prelazi se u fazu ubacivanje
      if ( vremeRada > parametar[usijavanje] )
      { stanje.fazaRada = 3; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      return;
    }
    case 3: // ubacivanje
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // ako je temperatura dima veca od temperature za ulazak u stabilizaciju prelazi se u stabilizaciju
      if ( (Tdimnjace > parametar[TDubacivanje])||(stanjeFotocelija) )
      { stanje.fazaRada = 4; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      // ako temperatura ne poraste za odredjeno vreme javlja se greska
      if ( vremeRada > parametar[ubacivanje] ) { greska (0); }

      return;
    }
    case 4: // stabilizacija
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // kad prodje odredjeno vreme prelazi se u normalan rad
      if ( vremeRada > parametar[stabilizacija] )
      { stanje.fazaRada = 5; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      return;
    }
    case 5:  // redovan rad
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // prelazak u modlulaciju zavisi da li je automatski ili manuelni rezim
      if (stanje.jacinaRada)
      { // manueleni rezim
        if ( (Tambijenta > parametar[maxTV])||(stanjeTermostat) ) // ako temperatura ambijenta predje maksimalnu vrednost prelazi se u modulaciju
        { stanje.fazaRada = 6; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      else
      { // automatski rezim
        if ( (Tambijenta > parametarA[zadTV])||(stanjeTermostat) ) // temperatura ambijenta predje zadatu temperaturu prelazi se u modulaciju
        { stanje.fazaRada = 6; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      
      // ako temperatura dimnjace predje maksimalnu vrednost prelazi se u maxT rezim
      if ( Tdimnjace > parametar[maxTD] )
      { stanje.fazaRada = 7; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      // ako temperatura dimnjace padne za vreme normalnog rada, gorionik se gasi
      if ( (!stanjeFotocelija)&&(Tdimnjace < parametar[TDubacivanje]) ) { greska (1); }
      
      return;
    }
    case 6: // modulacija
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // izlaz iz modulacije zavisi da li je automatski ili manuelni rezim
      if (stanje.jacinaRada)
      { // manueleni rezim
        if ( (Tambijenta < parametar[maxTV] - 3)&&(!stanjeTermostat) ) // ako temperatura ambijenta padne za 5 ispod maksimalne vraca se u normalan rad
        { stanje.fazaRada = 5; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
      else
      { // automatski rezim
        if ( (Tambijenta <= parametarA[zadTV])&&(!stanjeTermostat) ) // temperatura ambijenta padne ispod zadate temperature izlazi se iz modulacije
        { stanje.fazaRada = 5; vremeRada = 0; podesavanjeRada (); ispisRada (); }
      }
       // JEDINI ULAZ U CEKANJE
      // ako se odredjeno vreme ostane u modulaciji i cekanje je aktivno sistem prelazi u cekanje
      if ( (parametar[vremeCekanje])&&(parametar[vremeCekanje]<vremeRada) )
      { stanje.cekanje = 1; stanje.fazaRada = 1; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      // ako temperatura dimnjace padne za vreme normalnog rada, gorionik se gasi
      if ( (!stanjeFotocelija)&&(Tdimnjace < parametar[TDubacivanje]) ) { greska (1); }

      return;
    }
    case 7: // maxT
    {
      // pritiskom na taster OFF, hronotermostat, internet ili GSM prelazi se u fazu gasenja

      // ako temperatura dima padne za 10 ispod maksimalne vraca se u normalan rad
      if ( Tdimnjace < parametar[maxTD] - 10 )
      { stanje.fazaRada = 5; vremeRada = 0; podesavanjeRada (); ispisRada (); }

      return;
    }
    default:
    {
      stanje.fazaRada = 1;
      return;
    }
  }
}
//==============================================================================
// funkcija koja ispisuje fazu rada na displeju
void ispisRada ()
{
  // ispis se ne vrsi ako je u toku rad sa menijem
  if ( stanje.displej ) { return; }

  // inicijalizacija objekata u zavisnosti od stanja i tipa rada
  
  //objekti koji se ne menjaju--------------------------------------------------
  // taster meni, polje sat, polje displej, oznaka za temperaturu dima, oznaka za temperaturu ambijenta
  
  // uvek vidljivi objekti su labela za ispis faze i taster za paljenje i gasenje
  fazaRada.Visible = 1; fazaRada.Active = 1; onOff.Visible = 1; onOff.Active = 1;
  
  // svi objekte koje menja ova funkcija se postavljaju da budu nevidljivi i neaktivni
  tasterP1.Visible = 0; tasterP1.Active = 0;
  tasterP2.Visible = 0; tasterP2.Active = 0;
  tasterP3.Visible = 0; tasterP3.Active = 0;
  tasterP4.Visible = 0; tasterP4.Active = 0;
  tasterP5.Visible = 0; tasterP5.Active = 0;
  krugP1.Visible = 0; krugP1.Active = 0;
  krugP2.Visible = 0; krugP2.Active = 0;
  krugP3.Visible = 0; krugP3.Active = 0;
  krugP4.Visible = 0; krugP4.Active = 0;
  krugP5.Visible = 0; krugP5.Active = 0;
  minus.Visible = 0; minus.Active = 0;
  plus.Visible = 0; plus.Active = 0;
  zadataT.Visible = 0; zadataT.Active = 0;
  ZTA.Visible = 0; ZTA.Active = 0;
  boksTzad.Visible = 0; boksTzad.Active = 0;
  
  // podesavanje labele o fazi rada i tastera ONOFF u zavisnosti od stanja i cekanja
  switch ( stanje.fazaRada )
  {
    case 0: // gorionik ugasen
    {
      if (stanje.cekanje)
      {
        strcpy (fazaRada_Caption, "CEKANJE");
        strcpy (onOff_Caption, "OFF");
      }
      else
      {
        strcpy (fazaRada_Caption, "GORIONIK   UGASEN");
        strcpy (onOff_Caption, "ON");
      }
      break;
    }
    case 1: // gasenje
    {
      if (stanje.cekanje)
      {
        strcpy (fazaRada_Caption, "CEKANJE");
        strcpy (onOff_Caption, "OFF");
      }
      else
      {
        strcpy (fazaRada_Caption, "GASENJE");
        strcpy (onOff_Caption, "ON");
      }
      break;
    }
    case 2: // usijavanje
    {
      strcpy (fazaRada_Caption, "USIJAVANJE");
      strcpy (onOff_Caption, "OFF");
      break;
    }
    case 3: // ubacivanje
    {
      strcpy (fazaRada_Caption, "UBACIVANJE");
      /*if ( parametar[sondaDimnjace] ) // da li se koristi sonda dimnjace
      { strcpy (onOff_Caption, "OFF"); }
      else { strcpy (onOff_Caption, ">>"); }*/
      strcpy (onOff_Caption, "OFF");
      break;
    }
    case 4: // stabilizacija
    {
      strcpy (fazaRada_Caption, "STABILIZACIJA");
      strcpy (onOff_Caption, "OFF");
      break;
    }
    case 5: // redovan rad
    {
      strcpy (fazaRada_Caption, "REDOVAN   RAD");
      strcpy (onOff_Caption, "OFF");
      break;
    }
    case 6: // modulacija
    {
      strcpy (fazaRada_Caption, "MODULACIJA");
      strcpy (onOff_Caption, "OFF");
      break;
    }
    case 7: // maxT
    {
      strcpy (fazaRada_Caption, "MAX   TEMP");
      strcpy (onOff_Caption, "OFF");
      break;
    }
    default:
    {
      strcpy (fazaRada_Caption, "DISPLAY   ERROR");
      strcpy (onOff_Caption, "OFF");
      break;
    }
  }

  // podesavanje tastera displeja na osnovu jacine rad
  if ( stanje.fazaRada > 1 ) // ovi tasteri se ne pojavljuju u gasenju i ugasenom stanju
  {
    if ( stanje.jacinaRada ) // ako je manuelan rad
    {
      tasterP1.Visible = 1; tasterP1.Active = 1;
      tasterP2.Visible = 1; tasterP2.Active = 1;
      tasterP3.Visible = 1; tasterP3.Active = 1;
      tasterP4.Visible = 1; tasterP4.Active = 1;
      tasterP5.Visible = 1; tasterP5.Active = 1;
      switch ( stanje.jacinaRada ) // podesavanje kruzica
      {
        case 0: { break; } // automatski rad
        case 1: { krugP1.Visible = 1; krugP1.Active = 0; break; }
        case 2: { krugP2.Visible = 1; krugP2.Active = 0; break; }
        case 3: { krugP3.Visible = 1; krugP3.Active = 0; break; }
        case 4: { krugP4.Visible = 1; krugP4.Active = 0; break; }
        case 5: { krugP5.Visible = 1; krugP5.Active = 0; break; }
        default: { break; }
      } // switch
    } // if da li je automatski ili manuelan rad
    else // automatski rad
    {
      minus.Visible = 1; minus.Active = 1;
      plus.Visible = 1; plus.Active = 1;
      zadataT.Visible = 1; zadataT.Active = 0;
      ZTA.Visible = 1; ZTA.Active = 0;
      boksTzad.Visible = 1; boksTzad.Active = 0;
    } // if automatski ili manuelan rad
  } // if da li je ugasen ili gasenje

  DrawScreen(32768); // crtanje ekerana
  ispisiDatumVreme (); // ispsis datuma i vremena
  ispisTemperature (Tdimnjace, &TD, TD_Caption, &boksTdima); // ispsi temperature dima
  ispisTemperature (Tambijenta, &TA, TA_Caption, &boksTambijenta); // ispsi temperature ambijenta
  if ( !stanje.jacinaRada ) { ispisTemperature (parametarA[zadTV], &ZTA, ZTA_Caption, &boksTzad); } // ispsi zadate temperature ambijenta
}
//==============================================================================
// funkcija koja gasi gorionik i javlja da je doslo do greske
void greska (unsigned short greh)
{
  // ako je gorionik ukljucek, gorionik se gasi
  if ( stanje.fazaRada > 1) { stanje.fazaRada = 1; podesavanjeRada (); }
  
  stanje.displej = 1; // displej se ponasa kao da je rad se menijima
  
  // taster za popravku se pojavljuje samo kad je potrebno resetovati parametre
  if ( greh==2 ) { popravkaButon.Visible = 1; popravkaButon.Active = 1; }
  else { popravkaButon.Visible = 0; popravkaButon.Active = 0; }
   
  // podesavanje isisa u yavisnosti od greske
  LabelGR1_Caption[0] = '\0';
  strcat (LabelGR1_Caption, "  GRESKA !!!   ");

  switch ( greh )
  {
    case 0: // ne moze da se podigne temperatura radi ulaza u stabilizaciju
    {
      LabelGR2_Caption[0] = '\0';
      LabelGR3_Caption[0] = '\0';
      LabelGR4_Caption[0] = '\0';
      LabelGR5_Caption[0] = '\0';
      LabelGR6_Caption[0] = '\0';
      LabelGR7_Caption[0] = '\0';
      strcat (LabelGR2_Caption, "Problem  sa");
      strcat (LabelGR3_Caption, "paljenjem!");
      strcat (LabelGR4_Caption, "Proveriti sondu");
      strcat (LabelGR5_Caption, "u  dimnjaci  i");
      strcat (LabelGR6_Caption, "grejac.");
      strcat (LabelGR7_Caption, "Resetovati!");
      // slanje SMS poruka ako je GSM u funkciji
      if (parametarA[GSMaktivnost]) { slanjeSMS( alarmniBroj, "Gorionik nece da se upali." ); }
      break;
    }
    case 1: // gorionik se sam ugasio u redovnom radu
    {
      LabelGR2_Caption[0] = '\0';
      LabelGR3_Caption[0] = '\0';
      LabelGR4_Caption[0] = '\0';
      LabelGR5_Caption[0] = '\0';
      LabelGR6_Caption[0] = '\0';
      LabelGR7_Caption[0] = '\0';
      strcat (LabelGR2_Caption, "Nezenjeno");
      strcat (LabelGR3_Caption, "gasenje!");
      strcat (LabelGR4_Caption, "Proveriti sondu");
      strcat (LabelGR5_Caption, "u  dimnjaci  i");
      strcat (LabelGR6_Caption, "grejac.");
      strcat (LabelGR7_Caption, "Resetovati!");
      // slanje SMS poruka ako je GSM u funkciji
      if (parametarA[GSMaktivnost]) { slanjeSMS( alarmniBroj, "Nezenjeno gasenje gorionika." ); }
      break;
    }
    case 2: // nisu u redu fajlovi
    {
      LabelGR2_Caption[0] = '\0';
      LabelGR3_Caption[0] = '\0';
      LabelGR4_Caption[0] = '\0';
      LabelGR5_Caption[0] = '\0';
      LabelGR6_Caption[0] = '\0';
      LabelGR7_Caption[0] = '\0';
      strcat (LabelGR2_Caption, "Problem  sa");
      strcat (LabelGR3_Caption, "fajlovima!");
      strcat (LabelGR4_Caption, "Pritiskom  na");
      strcat (LabelGR5_Caption, "taster  ce  se");
      strcat (LabelGR6_Caption, "obnoviti");
      strcat (LabelGR7_Caption, "faljovi.");
      break;
    }
    case 3: // nije u redu fajl sa automatskim parametrima
    {
      LabelGR2_Caption[0] = '\0';
      LabelGR3_Caption[0] = '\0';
      LabelGR4_Caption[0] = '\0';
      LabelGR5_Caption[0] = '\0';
      LabelGR6_Caption[0] = '\0';
      LabelGR7_Caption[0] = '\0';
      strcat (LabelGR2_Caption, "Problem sa fajlovima!");
      strcat (LabelGR3_Caption, "Pritiskom na tester");
      strcat (LabelGR4_Caption, "ce se resetovati svi");
      strcat (LabelGR5_Caption, "parametri. Potom");
      strcat (LabelGR6_Caption, "resetovati uredjaj.");
      DrawLabel (&LabelGR2);
      DrawLabel (&LabelGR3);
      DrawLabel (&LabelGR4);
      DrawLabel (&LabelGR5);
      DrawLabel (&LabelGR6);
      DrawLabel (&LabelGR7);
      break;
    }
    case 4: // nije u redu fajl sa programima za hronotermostat
    {
      LabelGR2_Caption[0] = '\0';
      LabelGR3_Caption[0] = '\0';
      LabelGR4_Caption[0] = '\0';
      LabelGR5_Caption[0] = '\0';
      LabelGR6_Caption[0] = '\0';
      LabelGR7_Caption[0] = '\0';
      strcat (LabelGR2_Caption, "Problem sa fajlovima!");
      strcat (LabelGR3_Caption, "Pritiskom na tester");
      strcat (LabelGR4_Caption, "ce se resetovati svi");
      strcat (LabelGR5_Caption, "parametri. Potom");
      strcat (LabelGR6_Caption, "resetovati uredjaj.");
      DrawLabel (&LabelGR2);
      DrawLabel (&LabelGR3);
      DrawLabel (&LabelGR4);
      DrawLabel (&LabelGR5);
      DrawLabel (&LabelGR6);
      DrawLabel (&LabelGR7);
      break;
    }
    default:
    {
      break;
    }
  }

  DrawScreen (32773);

  // ceka se na pritisak tastera ili reset
  while (1)
  {
    Check_TP();
    radGSM();
    WATCHDOG0_LOAD = Get_Fosc_kHz()*10000; // WATCHDOG, podesavanje vrednosti brojanja na 10 sekundi
  }
}
//==============================================================================