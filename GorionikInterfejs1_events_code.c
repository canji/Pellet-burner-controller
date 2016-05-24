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

//--------------------- User code ---------------------//
//==============================================================================
// ispisivanje temperature
// temp je temperatura koja se ispisuje, labela je labela koja se ispisuje
// labelaCaption je string labele koja se ispisuje, boksZaBrisanje je Box koji brise prethodno ispisanu vrednost
void ispisTemperature (unsigned int temp, TLabel *labela, char* labelaCaption, TBox *boksZaBrisanje )
{
  char txt[6]; // pomocna promenljiva

  // ispis se ne vrsi ako je u toku rad sa menijem
  if ( stanje.displej ) { return; }

  DrawBox (boksZaBrisanje);

  // ako se ne koristi sonda za merenje temperature ispisuju se --
  if ( temp )
  {  // formatiranje stringa
    WordToStr (temp, txt);
    labelaCaption[0] = txt[1];
    labelaCaption[1] = txt[2];
    labelaCaption[2] = txt[3];
    labelaCaption[3] = txt[4];
    labelaCaption[4] = ' ';
    labelaCaption[5] = 176;
    labelaCaption[6] = 'C';
    labelaCaption[7] = '\0';
  }
  else
  {
    labelaCaption[0] = ' ';
    labelaCaption[1] = ' ';
    labelaCaption[2] = '-';
    labelaCaption[3] = ' ';
    labelaCaption[4] = '-';
    labelaCaption[5] = ' ';
    labelaCaption[6] = ' ';
    labelaCaption[7] = '\0';
  }
  DrawLabel (labela); // ispisivanje temperature
}
//==============================================================================

//----------------- End of User code ------------------//

// Event Handlers

// pritisnut taster ON OFF
void paliGasi ()
{
  if ( stanje.fazaRada < 2 ) // gorionik ugasen ili gasenje, prelazi se u usijavanje
  {
    parametarA[pravilnoGasenje] = 0; // korisnik je upalio gorionik
    upisAparametra(pravilnoGasenje);
    if (stanje.cekanje) // ako je cekanje aktivno, cekanje se deaktivira, a faza rada ostaje ista
    {
      stanje.cekanje = 0; vremeRada = 0; podesavanjeRada (); ispisRada ();
    }
    else
    {
      stanje.fazaRada = 2; vremeRada = 0; podesavanjeRada (); ispisRada ();
    }
  }
  else // ako je neka druga faza prelazi se u gasenje
  {
    stanje.fazaRada = 1; vremeRada = 0; podesavanjeRada (); ispisRada ();
    parametarA[pravilnoGasenje] = 1; // korisnik je ugasio gorionik
    upisAparametra(pravilnoGasenje);
  }
  return;
}

// pritisnut taster meni
void ulazMeni ()
{
  stanje.displej = 1;
  DrawScreen(32769);
}

// prtisnut P1
void klikP1 ()
{
  stanje.jacinaRada = 1; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); // jacina rada 1
}

// prtisnut P2
void klikP2 ()
{
  stanje.jacinaRada = 2; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); // jacina rada 2
}

// prtisnut P3
void klikP3 ()
{
  stanje.jacinaRada = 3; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); // jacina rada 3
}

// prtisnut P4
void klikP4 ()
{
  stanje.jacinaRada = 4; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); // jacina rada 4
}

// prtisnut P5
void klikP5 ()
{
  stanje.jacinaRada = 5; podesavanjeRada (); upisAparametra(pocetnaJacina); ispisRada (); // jacina rada 5
}

// pritisnut -
void klikMinus ()
{
  if ( parametarA[zadTV] == 0 ) { return; } // ne sme da ide manje od 0
  else
  {
    parametarA[zadTV]--; // dekrementiranje
    ispisTemperature (parametarA[zadTV], &ZTA, ZTA_Caption, &boksTzad); // ispisivanje temperature
    upisAparametra(zadTV);
  }
  Delay_ms (100);
}

// pritisnut +
void klikPlus ()
{
  if ( parametarA[zadTV] == parametar[maxTV] ) { return; } // ne sme da ide vise od maxTV
  else
  {
    parametarA[zadTV]++; // inkrementiranje
    ispisTemperature (parametarA[zadTV], &ZTA, ZTA_Caption, &boksTzad); // ispisivanje temperature
    upisAparametra(zadTV);
  }
  Delay_ms (100);
}

void radAuto ()
{
  if ( !parametar[sondaAmbijenta] ) { return; } // nema automtskog rada ako se ne koristi sonda za merenje temperature ambijenta

  stanje.displej = 0; // vise se ne radi sa menijima
  stanje.jacinaRada = 0; // prelazi se u automatski rad
  podesavanjeRada (); // novo podesavanje rada
  upisAparametra(pocetnaJacina);
  ispisRada (); // ispis rada

}

void radManf ()
{
  stanje.displej = 0; // vise se ne radi sa menijima
  stanje.jacinaRada = 1; // prelazi se u manuellni rad P1
  podesavanjeRada (); // novo podesavanje rada
  upisAparametra(pocetnaJacina);
  ispisRada (); // ispis rada
}

void hrono ()
{
  pokHrono = 0;
  if ( hronoProg[pokHrono].kakavRad == 6 )
  { // vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 1; tHronoZadTAMinus.Active = 1;
    tHronoZadTAPlus.Visible = 1; tHronoZadTAPlus.Active = 1;
    boksZadTAhrono.Visible = 1; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 1; labelHronoZadTA.Active = 0;
  }
  else
  { // ne vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 0; tHronoZadTAMinus.Active = 0;
    tHronoZadTAPlus.Visible = 0; tHronoZadTAPlus.Active = 0;
    boksZadTAhrono.Visible = 0; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 0; labelHronoZadTA.Active = 0;
  }
  if ( parametarA[hronoAktivnost] ) // ako je hronotermostat aktivan
  {
    hronoPok.Color = 0x07E0; // zeleno
    thronoOnOff.Gradient_End_Color = 0xF800; // crveno
    thronoOnOff_Caption[0] = 'O';
    thronoOnOff_Caption[1] = 'F';
    thronoOnOff_Caption[2] = 'F';
    thronoOnOff_Caption[3] = '\0';
  }
  else // ako nije aktivan
  {
    hronoPok.Color = 0xF800; // crveno
    thronoOnOff.Gradient_End_Color = 0x07E0; // zeleno
    thronoOnOff_Caption[0] = 'O';
    thronoOnOff_Caption[1] = 'N';
    thronoOnOff_Caption[2] = ' ';
    thronoOnOff_Caption[3] = '\0';
  }
  DrawScreen (32776);
  ispisiHronoDane ();
  ispisiHronoProgram ();
  ispisiHronoSate ();
  ispisiHronoZadatak ();
  ispisiHronoZadTemp ();
}

void cifra1 ()
{
  obradaSifre (1);
}

void cifra2 ()
{
  obradaSifre (2);
}

void cifra3 ()
{
  obradaSifre (3);
}

void cifra4 ()
{
  obradaSifre (4);
}

void cifra5 ()
{
  obradaSifre (5);
}

void cifra6 ()
{
  obradaSifre (6);
}

void cifra7 ()
{
  obradaSifre (7);
}

void cifra8 ()
{
  obradaSifre (8);
}

void cifra9 ()
{
  obradaSifre (9);
}

void nazad1 ()
{
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void nazad2 ()
{
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void nazad3 ()
{
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void kalibrisi ()
{
  TFT_Fill_Screen(0);
  Calibrate();
  TFT_Fill_Screen(0);
  TP_TFT_Get_Calibration_Consts( parametarA[Xminimum], parametarA[Xmaksimum], parametarA[Yminimum], parametarA[Ymaksimum]);
  upisAparametra(Xminimum); Delay_us(50); upisAparametra(Xmaksimum); Delay_us(50);
  upisAparametra(Yminimum); Delay_us(50); upisAparametra(Ymaksimum); Delay_us(50);
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void parametrija ()
{
  pokSh = 0; // pokazvaf unosa sifre pokazuje na prvu cifru
  sifraUnos = 0; // vrednost unossene sifre je 0
  // ne vide se kruzici za cifre
  sh1.Visible = 0; sh1.Active = 0;
  sh2.Visible = 0; sh2.Active = 0;
  sh3.Visible = 0; sh3.Active = 0;
  sh4.Visible = 0; sh4.Active = 0;
  DrawScreen (32771);
}

void podUlaz ()
{
  DrawScreen (32770);
}

void klikMinusP()
{
  switch (pokSh)
  {
//    case 0: { plusMinusParam(0,0,); break; }
    case 1: { plusMinusParam(1,0,10,0,300); break; }
    case 2: { plusMinusParam(2,0,1,5,100); break; }
    case 3: { plusMinusParam(3,0,600,600,12000); break; }
    case 4: { plusMinusParam(4,0,1,5,100); break; }
    case 5: { plusMinusParam(5,0,10,10,1200); break; }
    case 6: { plusMinusParam(6,0,1,30,99); break; }
    case 7: { plusMinusParam(7,0,1,20,100); break; }
    case 8: { plusMinusParam(8,0,600,600,12000); break; }
    case 9: { plusMinusParam(9,0,1,5,100); break; }
    case 10: { plusMinusParam(10,0,10,10,1200); break; }
    case 11: { plusMinusParam(11,0,1,30,99); break; }
    case 12: { plusMinusParam(12,0,600,600,12000); break; }
    case 13: { plusMinusParam(13,0,1,5,100); break; }
    case 14: { plusMinusParam(14,0,10,10,1200); break; }
    case 15: { plusMinusParam(15,0,1,30,99); break; }
    case 16: { plusMinusParam(16,0,1,5,100); break; }
    case 17: { plusMinusParam(17,0,10,10,1200); break; }
    case 18: { plusMinusParam(18,0,1,30,99); break; }
    case 19: { plusMinusParam(19,0,1,5,100); break; }
    case 20: { plusMinusParam(20,0,10,10,1200); break; }
    case 21: { plusMinusParam(21,0,1,30,99); break; }
    case 22: { plusMinusParam(22,0,1,5,100); break; }
    case 23: { plusMinusParam(23,0,10,10,1200); break; }
    case 24: { plusMinusParam(24,0,1,30,99); break; }
    case 25: { plusMinusParam(25,0,1,5,100); break; }
    case 26: { plusMinusParam(26,0,10,10,1200); break; }
    case 27: { plusMinusParam(27,0,1,30,99); break; }
    case 28: { plusMinusParam(28,0,1,0,10); break; }
    case 29: { plusMinusParam(29,0,1,0,10); break; }
    case 30: { plusMinusParam(30,0,1,0,10); break; }
    case 31: { plusMinusParam(31,0,1,30,99); break; }
    case 32: { plusMinusParam(32,0,1,40,100); break; }
    case 33: { plusMinusParam(33,0,600,0,36000); break; }
    case 34: { plusMinusParam(34,0,10,0,600); break; }
    case 35: { plusMinusParam(35,0,1,30,90); break; }
    case 36: { plusMinusParam(36,0,600,0,36000); break; }
    case 37: { plusMinusParam(37,0,10,0,1200); break; }
    case 38: { plusMinusParam(38,0,1,50,400); break; }
    case 39: { plusMinusParam(39,0,1,20,100); break; }
    case 40: { plusMinusParam(40,0,1,0,2); break; }
    case 41: { plusMinusParam(41,0,1,0,1); break; }
    case 42: { plusMinusParam(42,0,600,0,36000); break; }
    default: { break; }
  }
  ispisVrednostiParam (pokSh);
  Delay_ms (100);
}

void klikPlusP()
{
switch (pokSh)
  {
//    case 0: { plusMinusParam(0,0,); break; }
    case 1: { plusMinusParam(1,1,10,0,300); break; }
    case 2: { plusMinusParam(2,1,1,5,100); break; }
    case 3: { plusMinusParam(3,1,600,600,12000); break; }
    case 4: { plusMinusParam(4,1,1,5,100); break; }
    case 5: { plusMinusParam(5,1,10,10,1200); break; }
    case 6: { plusMinusParam(6,1,1,30,99); break; }
    case 7: { plusMinusParam(7,1,1,20,100); break; }
    case 8: { plusMinusParam(8,1,600,600,12000); break; }
    case 9: { plusMinusParam(9,1,1,5,100); break; }
    case 10: { plusMinusParam(10,1,10,10,1200); break; }
    case 11: { plusMinusParam(11,1,1,30,99); break; }
    case 12: { plusMinusParam(12,1,600,600,12000); break; }
    case 13: { plusMinusParam(13,1,1,5,100); break; }
    case 14: { plusMinusParam(14,1,10,10,1200); break; }
    case 15: { plusMinusParam(15,1,1,30,99); break; }
    case 16: { plusMinusParam(16,1,1,5,100); break; }
    case 17: { plusMinusParam(17,1,10,10,1200); break; }
    case 18: { plusMinusParam(18,1,1,30,99); break; }
    case 19: { plusMinusParam(19,1,1,5,100); break; }
    case 20: { plusMinusParam(20,1,10,10,1200); break; }
    case 21: { plusMinusParam(21,1,1,30,99); break; }
    case 22: { plusMinusParam(22,1,1,5,100); break; }
    case 23: { plusMinusParam(23,1,10,10,1200); break; }
    case 24: { plusMinusParam(24,1,1,30,99); break; }
    case 25: { plusMinusParam(25,1,1,5,100); break; }
    case 26: { plusMinusParam(26,1,10,10,1200); break; }
    case 27: { plusMinusParam(27,1,1,30,99); break; }
    case 28: { plusMinusParam(28,1,1,0,10); break; }
    case 29: { plusMinusParam(29,1,1,0,10); break; }
    case 30: { plusMinusParam(30,1,1,0,10); break; }
    case 31: { plusMinusParam(31,1,1,30,99); break; }
    case 32: { plusMinusParam(32,1,1,40,100); break; }
    case 33: { plusMinusParam(33,1,600,0,36000); break; }
    case 34: { plusMinusParam(34,1,10,0,600); break; }
    case 35: { plusMinusParam(35,1,1,30,90); break; }
    case 36: { plusMinusParam(36,1,600,0,36000); break; }
    case 37: { plusMinusParam(37,1,10,0,1200); break; }
    case 38: { plusMinusParam(38,1,1,50,400); break; }
    case 39: { plusMinusParam(39,1,1,20,100); break; }
    case 40: { plusMinusParam(40,1,1,0,2); break; }
    case 41: { plusMinusParam(41,1,1,0,1); break; }
    case 42: { plusMinusParam(42,1,600,0,36000); break; }
    default: { break; }
  }
  ispisVrednostiParam (pokSh);
  Delay_ms (100);
}

void paramNatrag()
{
  if ( pokSh == 1) { pokSh = brojP-1; }
  else { pokSh--; }
  // crta se taster za testiranje
  if ( (pokSh==M2tM1)||(pokSh==tM1ubacivanje)||(pokSh==SVubacivanje)||(pokSh==duzinaCiscenja) )
  { paramTest.Visible = 1; paramTest.Active = 1; DrawScreen(32772); }
  // brise se tatster za tesstiranje
  if ( (pokSh==brojP-1)||(pokSh==tM1ubacivanje-1)||(pokSh==SVubacivanje-1)||(pokSh==duzinaCiscenja-1) )
  { paramTest.Visible = 0; paramTest.Active = 0; DrawScreen(32772); }
  ispisBrojaParam (pokSh);
  ispisVrednostiParam (pokSh);
  Delay_ms (100);
}

void paramDalje()
{
  if ( pokSh == brojP-1 ) { pokSh = 1; }
  else { pokSh++; }
  // crta se taster za testiranje
  if ( (pokSh==M2tM1)||(pokSh==tM1ubacivanje)||(pokSh==SVubacivanje)||(pokSh==duzinaCiscenja) )
  { paramTest.Visible = 1; paramTest.Active = 1; DrawScreen(32772); }
  // brise se tatster za tesstiranje
  if ( (pokSh==M2tM1+1)||(pokSh==tM1ubacivanje+1)||(pokSh==SVubacivanje+1)||(pokSh==duzinaCiscenja+1) )
  { paramTest.Visible = 0; paramTest.Active = 0; DrawScreen(32772); }
  ispisBrojaParam (pokSh);
  ispisVrednostiParam (pokSh);
  Delay_ms (100);
}

void nazadP()
{
  unsigned short i = 0;
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
  // upisivanje u EEPROM svih P parametra
  for ( i=0; i<brojP; i++ ) { upisPparametra(i); Delay_us(50); }
}

void paramTestiranje() // testiranje sistema za ciscenje
{
  unsigned int cuvanjeIntervala = 0; // promenljiva za cuvanje parametra o intervalu ciscenja

  if (stanje.fazaRada) { return; } // testiranje moze samo kad je gorionik ugasen

  switch (pokSh)
  {
    case M2tM1: // testiranje M1
    {
      if (tM1==0xFFFF) { podesavanjeRada (); }
      else { tM1 = 0xFFFF; M2duzeM1 = 0x1; }
      break;
    }
    case tM1ubacivanje: // testiranje M2
    {
      if (M2duzeM1==0xFFFF) { podesavanjeRada (); }
      else { tM1 = 0; M2duzeM1 = 0xFFFF;; }
      break;
    }
    case SVubacivanje: // testiranje ventilatora
    {
      if (TIMER2_TAILR==800000) { ACmotor (parametar[SVubacivanje]); }
      else { podesavanjeRada (); }
      break;
    }
    case duzinaCiscenja:
    {
      // ako je testiranje u toku testiranje se prekida
      if (testCistaca) 
      {
        vremeCiscenje = 0xFFFF; // govori prekidnoj rutini da je kraj testa
      }
      else
      {
        testCistaca = 1; // pocinje testiranje
        vremeCiscenje = parametar[intervalCiscenja]; // ukljucivanje cistaca
      }
      break;
    }
    default: { break; }
  }
}

void popraviti()
{
  // resetovanje fajlova na default vrednosti
  resetP ();
  resetPA ();
  resetHrono ();
}

void mesecPlus()
{
  if ( tsd.mo == 12 ) { tsd.mo = 1; } // nema vise od 12 meseci
  else { tsd.mo++; }
  ispisiMesece ();
  Delay_ms (100);
}

void mesecMinus()
{
  if ( tsd.mo == 1 ) { tsd.mo = 12; } // nema nulti mesec
  else { tsd.mo--; }
  ispisiMesece ();
  Delay_ms (100);
}

void danPlus()
{
  if ( (tsd.mo==2)&&(tsd.yy%4==0) ) // februar prestupne godine
  {
    if ( tsd.md == 29 ) { tsd.md = 1; }
    else { tsd.md++; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( tsd.mo==2 ) // februar neprestupne godine
  {
    if ( tsd.md == 28 ) {tsd.md = 1; }
    else { tsd.md++; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( (tsd.mo==4)||(tsd.mo==6)||(tsd.mo==9)||(tsd.mo==11) ) // meseci sa 30 dana
  {
    if ( tsd.md == 30 ) { tsd.md = 1; }
    else { tsd.md++; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( tsd.md == 31 ) { tsd.md = 1; } // meseci sa 31 dan
  else { tsd.md++; }
  ispisiDane ();
  Delay_ms (100);
  return;
}

void danMinus()
{
  if ( (tsd.mo==2)&&(tsd.yy%4==0) ) // februar prestupne godine
  {
    if ( tsd.md == 1 ) { tsd.md = 29; }
    else { tsd.md--; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( tsd.mo==2 ) // februar neprestupne godine
  {
    if ( tsd.md == 1 ) {tsd.md = 28; }
    else { tsd.md--; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( (tsd.mo==4)||(tsd.mo==6)||(tsd.mo==9)||(tsd.mo==11) ) // meseci sa 30 dana
  {
    if ( tsd.md == 1 ) { tsd.md = 30; }
    else { tsd.md--; }
    ispisiDane ();
    Delay_ms (100);
    return;
  }

  if ( tsd.md == 1 ) { tsd.md = 31; } // meseci sa 31 dan
  else { tsd.md--; }
  ispisiDane ();
  Delay_ms (100);
  return;
}

void satPlus()
{
  if ( tsd.hh == 23 ) { tsd.hh = 0; }
  else { tsd.hh++; }
  ispisiSate ();
  Delay_ms (100);
}

void satMinus()
{
  if ( tsd.hh == 0 ) { tsd.hh = 23; }
  else { tsd.hh--; }
  ispisiSate ();
  Delay_ms (100);
}

void minutiPlus ()
{
  if ( tsd.mn == 59 ) { tsd.mn = 0; }
  else { tsd.mn++; }
  ispisiMinute ();
  Delay_ms (100);
}

void minutiMinus ()
{
  if ( tsd.mn == 0 ) { tsd.mn = 59; }
  else { tsd.mn--; }
  ispisiMinute ();
  Delay_ms (100);
}

void godinaPlus ()
{
  if ( tsd.yy == 99 ) { tsd.yy = 0; }
  else { tsd.yy++; }
  ispisiGodine ();
  Delay_ms (100);
}

void godinaMinus ()
{
  if ( tsd.yy == 0 ) { tsd.yy = 99; }
  else { tsd.yy--; }
  ispisiGodine ();
  Delay_ms (100);
}

void podesiDiV()
{
  tsd = citanjeRTC (); // citanje vremena
  DrawScreen (32774); // crtanje ekrana i ispis vrednosti
  ispisiSate ();
  ispisiMinute ();
  ispisiDane ();
  ispisiMesece ();
  ispisiGodine ();
}

void nazadSat ()
{
  upisRTC (&tsd);
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void hronoProgPlus()  // plus programa, ide od 1 do 6
{
  while (1)
  {
    pokHrono++;
    if ( pokHrono > brojProg*7-1 ) { pokHrono = 0; break; } // kad dodje do kraja vraca se na pocetak niza programa hronotermostata
    if ( (pokHrono%6==0)||(hronoProg[pokHrono-1].kakavRad)||(hronoProg[pokHrono].kakavRad) ) { break; } // pokazivac se povecava dokle god se ne stigne do prvog programa u danu ili je prethodni program aktivan
  }
  if ( hronoProg[pokHrono].kakavRad == 6 )
  { // vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 1; tHronoZadTAMinus.Active = 1;
    tHronoZadTAPlus.Visible = 1; tHronoZadTAPlus.Active = 1;
    boksZadTAhrono.Visible = 1; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 1; labelHronoZadTA.Active = 0;
  }
  else
  { // ne vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 0; tHronoZadTAMinus.Active = 0;
    tHronoZadTAPlus.Visible = 0; tHronoZadTAPlus.Active = 0;
    boksZadTAhrono.Visible = 0; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 0; labelHronoZadTA.Active = 0;
  }
  DrawScreen (32776);
  ispisiHronoDane ();
  ispisiHronoProgram ();
  ispisiHronoSate ();
  ispisiHronoZadTemp ();
  ispisiHronoZadatak ();
  ispisiHronoZadTemp ();
  Delay_ms (100);
}

void hronoProgMinus()
{
  while (1)
  {
    pokHrono--;
    if ( pokHrono > brojProg*7-1 ) { pokHrono = brojProg*7-1; } // kad dodje do pocetka vraca se na kraj niza programa hronotermostata
    if ( (pokHrono%6==0)||(hronoProg[pokHrono-1].kakavRad)||(hronoProg[pokHrono].kakavRad) ) { break; } // pokazivac se smanjuje dokle god se ne stigne do prvog programa u danu ili je prethodni program aktivan
  }
  if ( hronoProg[pokHrono].kakavRad == 6 )
  { // vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 1; tHronoZadTAMinus.Active = 1;
    tHronoZadTAPlus.Visible = 1; tHronoZadTAPlus.Active = 1;
    boksZadTAhrono.Visible = 1; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 1; labelHronoZadTA.Active = 0;
  }
  else
  { // ne vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 0; tHronoZadTAMinus.Active = 0;
    tHronoZadTAPlus.Visible = 0; tHronoZadTAPlus.Active = 0;
    boksZadTAhrono.Visible = 0; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 0; labelHronoZadTA.Active = 0;
  }
  DrawScreen (32776);
  ispisiHronoDane ();
  ispisiHronoProgram ();
  ispisiHronoSate ();
  ispisiHronoZadatak ();
  ispisiHronoZadTemp ();
  Delay_ms (100);
}

void hronoSatPlus()
{
  Time_epochToDate(hronoProg[pokHrono].epoha, &ts); // epohu pretvara u strukturu vreme

  if ( (ts.hh==23)&&(ts.mn==45) ) { hronoProg[pokHrono].epoha -= 85500; }  // sat se vraca na 00:00, tj. minus 85500 sek
  else { hronoProg[pokHrono].epoha += 900; } // povecanje 15 min, tj. 900 sek
  ispisiHronoSate ();
  Delay_ms (100);
}

void hronoSatMinus()
{
  Time_epochToDate(hronoProg[pokHrono].epoha, &ts); // epohu pretvara u strukturu vreme

  if ( (ts.hh==0)&&(ts.mn==0) ) { hronoProg[pokHrono].epoha += 85500; }  // sat tera na 23:45
  else { hronoProg[pokHrono].epoha -= 900; } // smanjuje 15 min, tj. 900 sek
  ispisiHronoSate ();
  Delay_ms (100);
}

void hronoPosoMinus()
{
  if ( hronoProg[pokHrono].kakavRad == 0 ) { hronoProg[pokHrono].kakavRad = 7; }
  else { hronoProg[pokHrono].kakavRad--; }
  if ( hronoProg[pokHrono].kakavRad == 6 ) // menja se temperatura
  { // vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 1; tHronoZadTAMinus.Active = 1;
    tHronoZadTAPlus.Visible = 1; tHronoZadTAPlus.Active = 1;
    boksZadTAhrono.Visible = 1; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 1; labelHronoZadTA.Active = 0;
    DrawScreen (32776);
    ispisiHronoDane ();
    ispisiHronoProgram ();
    ispisiHronoSate ();
    ispisiHronoZadTemp ();
  }
  if ( hronoProg[pokHrono].kakavRad == 5 ) // ne menja se temperatura
  { // ne vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 0; tHronoZadTAMinus.Active = 0;
    tHronoZadTAPlus.Visible = 0; tHronoZadTAPlus.Active = 0;
    boksZadTAhrono.Visible = 0; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 0; labelHronoZadTA.Active = 0;
    DrawScreen (32776);
    ispisiHronoDane ();
    ispisiHronoProgram ();
    ispisiHronoSate ();
    ispisiHronoZadTemp ();
  }
  ispisiHronoZadatak ();
}

void hronoPosoPlus()
{
  if ( hronoProg[pokHrono].kakavRad == 7 ) { hronoProg[pokHrono].kakavRad = 0; }
  else { hronoProg[pokHrono].kakavRad++; }
  if ( hronoProg[pokHrono].kakavRad == 6 ) // menja se temperatura
  { // vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 1; tHronoZadTAMinus.Active = 1;
    tHronoZadTAPlus.Visible = 1; tHronoZadTAPlus.Active = 1;
    boksZadTAhrono.Visible = 1; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 1; labelHronoZadTA.Active = 0;
    DrawScreen (32776);
    ispisiHronoDane ();
    ispisiHronoProgram ();
    ispisiHronoSate ();
    ispisiHronoZadTemp ();
  }
  if ( hronoProg[pokHrono].kakavRad == 7 ) // ne menja se temperatura
  { // ne vidi se polje za menjanje zadate temperature
    tHronoZadTAMinus.Visible = 0; tHronoZadTAMinus.Active = 0;
    tHronoZadTAPlus.Visible = 0; tHronoZadTAPlus.Active = 0;
    boksZadTAhrono.Visible = 0; boksZadTAhrono.Active = 0;
    labelHronoZadTA.Visible = 0; labelHronoZadTA.Active = 0;
    DrawScreen (32776);
    ispisiHronoDane ();
    ispisiHronoProgram ();
    ispisiHronoSate ();
    ispisiHronoZadTemp ();
  }
  ispisiHronoZadatak ();
}

void hronoZadTAMinus()
{
  if ( hronoProg[pokHrono].automatika == 0 ) { return; } // ne sme da ide manje od 0
  else
  {
    hronoProg[pokHrono].automatika--; // dekrementiranje
    ispisiHronoZadTemp ();
  }
  Delay_ms (100);
}

void hronoZadTAPlus()
{
  if ( hronoProg[pokHrono].automatika == parametar[maxTV] ) { return; } // ne sme da ide manje od 0
  else
  {
    hronoProg[pokHrono].automatika++; // inkrementiranje
    ispisiHronoZadTemp ();
  }
  Delay_ms (100);
}

void nazadHrono ()
{
  srediHrono ();
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void hronoOnOff ()
{
  parametarA[hronoAktivnost] = !parametarA[hronoAktivnost]; // pali ili gasi
  upisAparametra(hronoAktivnost); // upisivanje u memoriju
  if ( parametarA[hronoAktivnost] ) // ako je hronotermostat aktivan
  {
    hronoPok.Color = 0x07E0; // zeleno
    thronoOnOff.Gradient_End_Color = 0xF800; // crveno
    thronoOnOff_Caption[0] = 'O';
    thronoOnOff_Caption[1] = 'F';
    thronoOnOff_Caption[2] = 'F';
    thronoOnOff_Caption[3] = '\0';
  }
  else // ako nije aktivan
  {
    hronoPok.Color = 0xF800; // crveno
    thronoOnOff.Gradient_End_Color = 0x07E0; // zeleno
    thronoOnOff_Caption[0] = 'O';
    thronoOnOff_Caption[1] = 'N';
    thronoOnOff_Caption[2] = ' ';
    thronoOnOff_Caption[3] = '\0';
  }
  DrawScreen (32776);
  ispisiHronoDane ();
  ispisiHronoProgram ();
  ispisiHronoSate ();
  ispisiHronoZadatak ();
  ispisiHronoZadTemp ();
}

void paramTestPustanje () { } // izbrisati
void paramTestDrzanje () { } // izbrisati

void fKomunikacija ()
{
  if ( parametarA[GSMaktivnost] ) // ako je GSM aktivan
  {
    GSMpok.Color = 0x07E0; // zeleno
    tGSMonOff.Gradient_End_Color = 0xF800; // crveno
    tGSMonOff_Caption[0] = 'O';
    tGSMonOff_Caption[1] = 'F';
    tGSMonOff_Caption[2] = 'F';
    tGSMonOff_Caption[3] = '\0';
  }
  else // ako nije aktivan
  {
    GSMpok.Color = 0xF800; // crveno
    tGSMonOff.Gradient_End_Color = 0x07E0; // zeleno
    tGSMonOff_Caption[0] = 'O';
    tGSMonOff_Caption[1] = 'N';
    tGSMonOff_Caption[2] = ' ';
    tGSMonOff_Caption[3] = '\0';
  }
  DrawScreen (32777);
  ispisiIP ();
  ispisiMob ();
}

void nazadKom ()
{
  initMiniWebServer();
  upisAparametra(IPadresa);
  stanje.displej = 0; // vise se ne radi sa menijima
  ispisRada (); // ispis rada
}

void IPminus ()
{
  if ( parametarA[IPadresa] == 0 ) { parametarA[IPadresa] = 255; }
  else { parametarA[IPadresa]--; }
  ispisiIP ();
  Delay_ms (100);
}

void IPplus ()
{
  if ( parametarA[IPadresa] == 255 ) { parametarA[IPadresa] = 0; }
  else { parametarA[IPadresa]++; }
  ispisiIP ();
  Delay_ms (100);
}

void GSMonOff ()
{
  parametarA[GSMaktivnost] = !parametarA[GSMaktivnost]; // pali ili gasi
  upisAparametra(GSMaktivnost);
  if ( parametarA[GSMaktivnost] ) // ako je GSM aktivan
  {
    GSMpok.Color = 0x07E0; // zeleno
    tGSMonOff.Gradient_End_Color = 0xF800; // crveno
    tGSMonOff_Caption[0] = 'O';
    tGSMonOff_Caption[1] = 'F';
    tGSMonOff_Caption[2] = 'F';
    tGSMonOff_Caption[3] = '\0';
    // ispisivanje poruke da se ceka inicijalizacija
    strcpy( labelBorjMob_Caption, "Sacekati!" );
    bokBrojMob.Color = 0xFFFF; // boks je bele boje
    DrawBox(&bokBrojMob); // brisanje stare vrednosti
    DrawLabel (&labelBorjMob);
    initGSM(); // inicijalizacija GSM moula
    ispisiMob(); // kraj inicijalizacije, ispisivanje broja mobilnog
  }
  else // ako nije aktivan
  {
    GSMpok.Color = 0xF800; // crveno
    tGSMonOff.Gradient_End_Color = 0x07E0; // zeleno
    tGSMonOff_Caption[0] = 'O';
    tGSMonOff_Caption[1] = 'N';
    tGSMonOff_Caption[2] = ' ';
    tGSMonOff_Caption[3] = '\0';
  }
  DrawScreen (32777);
}