// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za rad sa RTC

#include "Sat.h"
#include "DIulazi.h"
#include "Parametri.h"
#include "GNPfunkcije.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"
#include "built_in.h"

//==============================================================================
// konstante

// broj programa u jednom danu za hronotermostat
const brojProg = 6;

//==============================================================================
// globalne promenljive

TimeStruct ts; // struktura za vreme koje se koristi kao informacija o vremenu

TimeStruct tsd; // struktura za vreme koja se koristi za podesavanje vremena

// niz za smestanje struktura sa podacima o programima hronotermostata
HronoStruct hronoProg[7*brojProg];

unsigned char pokHrono = 0; // pokazivac na program hronotermostata

unsigned char provera1sek = 0; // pomocna promenljiva za proveru da li je prosla prva sekunda minuta

//==============================================================================
// funkcija za inicijalizaciju mikrokontrolera za rad sa RTC
void initSat ()
{
  I2C0_Init (); // inicijalizacija RTC
  
  /*// sabansko resenje blokiranja sata
  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 0, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // pocetak upisivanja je na aredsi 0
  I2C0_Write( 0x0, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // startovanje brojaca i upisivanje 0 sekundi*/
  
  /*// provera RTC, ako nesto nije u redu program ce blokirati prilikom citanja regostara
  // ovo treba da spreci blokiranje programa i da startuje RTC ako nesto nije u redu sa baterijom
  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 3, _I2C_MASTER_MODE_SINGLE_RECEIVE ); // pocetak upisivanja je na aredsi 3, citanje dana u nedelji

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_RECEIVE); // adresiranje i obavestenje o citanju

  // ako nije u redu sat se sam podesava na 1. januar 2000.
  if ( I2C0_MCS.B6 == 1 ) // bus busy
  { tsd.ss = 0; tsd.mn = 0; tsd.hh = 0; tsd.md = 1; tsd.wd = 0; tsd.mo = 1; tsd.yy = 2000; upisRTC (&tsd); return; } // podesavanje sata na 1. januar 2000.

  I2C0_MCS = 0x7; // cita se jedan bajt

  if ( I2C0_MCS.B0 == 1 ) // controller busy
  { tsd.ss = 0; tsd.mn = 0; tsd.hh = 0; tsd.md = 1; tsd.wd = 0; tsd.mo = 1; tsd.yy = 2000; upisRTC (&tsd); return; } // podesavanje sata na 1. januar 2000.

  if ( I2C0_MCS.B1 == 1 ) // error
  { tsd.ss = 0; tsd.mn = 0; tsd.hh = 0; tsd.md = 1; tsd.wd = 0; tsd.mo = 1; tsd.yy = 2000; upisRTC (&tsd); return; } // podesavanje sata na 1. januar 2000.

  if ( I2C0_MDR == 0 ) // nista nije stiglo
  { tsd.ss = 0; tsd.mn = 0; tsd.hh = 0; tsd.md = 1; tsd.wd = 0; tsd.mo = 1; tsd.yy = 2000; upisRTC (&tsd); return; } // podesavanje sata na 1. januar 2000.*/
}
//==============================================================================
// funkcija koja podesava vreme u RTC
void upisRTC (TimeStruct *ts)
{
  short min;
  short h;
  short week_day = 0;
  short day;
  short mon;
  short year;
  long epoha = 0;
  
  epoha = Time_dateToEpoch(ts); // ovo sluzi za racunanje dana u nedelji
  Time_epochToDate(epoha, ts);

  // konvertovanje BCD kod
  min = Dec2Bcd (ts->mn);
  h = Dec2Bcd (ts->hh);
  week_day = (ts->wd+1);
  day = Dec2Bcd (ts->md);
  mon = Dec2Bcd (ts->mo);
  year = Dec2Bcd ((short)(ts->yy-2000));

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 0, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // pocetak upisivanja je na aredsi 0
  I2C0_Write( 0x80, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // zaustavljanje brojaca i upisivanje 0 sekundi
  I2C0_Write( min, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // upisivanje minuta
  I2C0_Write( h, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // upisivanje sati
  I2C0_Write( week_day, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // upisivanje dana u nedelji
  I2C0_Write( day, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // upisivanje dana u mesecu
  I2C0_Write( mon, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // upisivanje meseca
  I2C0_Write( year, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // upisivanje godine

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 0, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // pocetak upisivanja je na aredsi 0
  I2C0_Write( 0x0, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // startovanje brojaca i upisivanje 0 sekundi
  
  return;
}
//==============================================================================
// funkcija koja cita vreme iz RTC
TimeStruct citanjeRTC ()
{
  char buffer[7];
  TimeStruct tdd;

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 0, _I2C_MASTER_MODE_SINGLE_RECEIVE ); // pocetak upisivanja je na aredsi 0

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_RECEIVE); // adresiranje i obavestenje o citanju
  I2C0_Read( &buffer[0], _I2C_MASTER_MODE_BURST_RECEIVE_START ); // citanje sekundi
  I2C0_Read( &buffer[1], _I2C_MASTER_MODE_BURST_RECEIVE_CONT );  // citanje minuta
  I2C0_Read( &buffer[2], _I2C_MASTER_MODE_BURST_RECEIVE_CONT );  // citanje sati
  I2C0_Read( &buffer[3], _I2C_MASTER_MODE_BURST_RECEIVE_CONT );  // citanje dan u nedelji
  I2C0_Read( &buffer[4], _I2C_MASTER_MODE_BURST_RECEIVE_CONT );  // citanje dan u mesecu
  I2C0_Read( &buffer[5], _I2C_MASTER_MODE_BURST_RECEIVE_CONT );  // citanje mesec
  I2C0_Read( &buffer[6], _I2C_MASTER_MODE_BURST_RECEIVE_FINISH );// citanje godina

  // transform time
  tdd.ss = ((buffer[0] & 0x70) >> 4)*10 + (buffer[0] & 0x0F);
  tdd.mn = ((buffer[1] & 0xF0) >> 4)*10 + (buffer[1] & 0x0F);
  tdd.hh  = ((buffer[2] & 0x30) >> 4)*10 + (buffer[2] & 0x0F);
  tdd.md = ((buffer[4] & 0xF0) >> 4)*10 + (buffer[4] & 0x0F);
  tdd.mo  = ((buffer[5] & 0x10) >> 4)*10 + (buffer[5] & 0x0F);
  tdd.yy = ( (int)( ((buffer[6] & 0xF0)>>4)*10 + (buffer[6] & 0x0F) ) ) + 2000;
  tdd.wd = buffer[3]-1;

  return tdd;
}
//==============================================================================
// funkcija koja ispisuje vreme i datum na osnovnom prozoru
void ispisiDatumVreme ()
{
  char txt[4]; // pomocna promenljiva
  
  if ( stanje.displej ) { return; }  // ispis se ne vrsi ako je u toku rad sa menijem

  ts = citanjeRTC (); // citanje vremena

  ByteToStrWithZeros (ts.md, txt); // priprema za ispis dana u mesecu
  vreme_Caption[0] = txt[1];
  vreme_Caption[1] = txt[2];
  vreme_Caption[2] = '-';

  switch(ts.mo) // priprema za ispis meseci
  {
    case 1: { vreme_Caption[3] = 'J'; vreme_Caption[4] = 'A'; vreme_Caption[5] = 'N'; break; }
    case 2: { vreme_Caption[3] = 'F'; vreme_Caption[4] = 'E'; vreme_Caption[5] = 'B'; break; }
    case 3: { vreme_Caption[3] = 'M'; vreme_Caption[4] = 'A'; vreme_Caption[5] = 'R'; break; }
    case 4: { vreme_Caption[3] = 'A'; vreme_Caption[4] = 'P'; vreme_Caption[5] = 'R'; break; }
    case 5: { vreme_Caption[3] = 'M'; vreme_Caption[4] = 'A'; vreme_Caption[5] = 'J'; break; }
    case 6: { vreme_Caption[3] = 'J'; vreme_Caption[4] = 'U'; vreme_Caption[5] = 'N'; break; }
    case 7: { vreme_Caption[3] = 'J'; vreme_Caption[4] = 'U'; vreme_Caption[5] = 'L'; break; }
    case 8: { vreme_Caption[3] = 'A'; vreme_Caption[4] = 'U'; vreme_Caption[5] = 'G'; break; }
    case 9: { vreme_Caption[3] = 'S'; vreme_Caption[4] = 'E'; vreme_Caption[5] = 'P'; break; }
    case 10: { vreme_Caption[3] = 'O'; vreme_Caption[4] = 'K'; vreme_Caption[5] = 'T'; break; }
    case 11: { vreme_Caption[3] = 'N'; vreme_Caption[4] = 'O'; vreme_Caption[5] = 'V'; break; }
    case 12: { vreme_Caption[3] = 'D'; vreme_Caption[4] = 'E'; vreme_Caption[5] = 'C'; break; }
    default: { ts.mo=1; vreme_Caption[3] = 'J'; vreme_Caption[4] = 'A'; vreme_Caption[5] = 'N'; break; }
  }
  vreme_Caption[6] = '-';
  
  vreme_Caption[7] = '2'; // priprema za ispis godine
  vreme_Caption[8] = '0';
  ByteToStrWithZeros (ts.yy-2000, txt);
  vreme_Caption[9] = txt[1];
  vreme_Caption[10] = txt[2];
  vreme_Caption[11] = '.';
  vreme_Caption[12] = ' ';
  vreme_Caption[13] = ' ';
  
  ByteToStrWithZeros (ts.hh, txt); // priprema za ispis sati
  vreme_Caption[14] = txt[1];
  vreme_Caption[15] = txt[2];
  vreme_Caption[16] = ':';
  
  ByteToStrWithZeros (ts.mn, txt); // priprema za ispis minuta
  vreme_Caption[17] = txt[1];
  vreme_Caption[18] = txt[2];
  vreme_Caption[19] = ' ';
  vreme_Caption[20] = '\0';
  
  DrawCBox (&sat); // brisanje stare vrednosti
  DrawLabel (&vreme); // ispis nove vrednosti
}
//==============================================================================
// funkcija koja ispisuje sate u polje za podesavanje
void ispisiSate ()
{
  char txt[4]; // pomocna promenljiva
  
  ByteToStrWithZeros (tsd.hh, txt); // priprema za ispis sati
  labelSati_Caption[0] = txt[1];
  labelSati_Caption[1] = txt[2];
  labelSati_Caption[2] = '\0';
  
  DrawBox (&boksSati); // ispisivanje
  DrawLabel (&labelSati);
}
//==============================================================================
// funkcija koja ispisuje minute u polje za podesavanje
void ispisiMinute ()
{
  char txt[4]; // pomocna promenljiva

  ByteToStrWithZeros (tsd.mn, txt); // priprema za ispis minuta
  labelMinuti_Caption[0] = txt[1];
  labelMinuti_Caption[1] = txt[2];
  labelMinuti_Caption[2] = '\0';

  DrawBox (&boksMinuti); // ispisivanje
  DrawLabel (&labelMinuti);
}
//==============================================================================
// funkcija koja ispisuje dane u polje za podesavanje
void ispisiDane ()
{
  char txt[4]; // pomocna promenljiva

  ByteToStrWithZeros (tsd.md, txt); // priprema za ispis dana u mesecu
  labelDani_Caption[0] = txt[1];
  labelDani_Caption[1] = txt[2];
  labelDani_Caption[2] = '\0';

  DrawBox (&boksDani); // ispisivanje
  DrawLabel (&labelDani);
}
//==============================================================================
// funkcija koja ispisuje mesece u polje za podesavanje
void ispisiMesece ()
{
  switch(tsd.mo) // priprema za ispis meseci
  {
    case 1: { labelMesec_Caption[0] = 'J'; labelMesec_Caption[1] = 'A'; labelMesec_Caption[2] = 'N'; break; }
    case 2: { labelMesec_Caption[0] = 'F'; labelMesec_Caption[1] = 'E'; labelMesec_Caption[2] = 'B'; break; }
    case 3: { labelMesec_Caption[0] = 'M'; labelMesec_Caption[1] = 'A'; labelMesec_Caption[2] = 'R'; break; }
    case 4: { labelMesec_Caption[0] = 'A'; labelMesec_Caption[1] = 'P'; labelMesec_Caption[2] = 'R'; break; }
    case 5: { labelMesec_Caption[0] = 'M'; labelMesec_Caption[1] = 'A'; labelMesec_Caption[2] = 'J'; break; }
    case 6: { labelMesec_Caption[0] = 'J'; labelMesec_Caption[1] = 'U'; labelMesec_Caption[2] = 'N'; break; }
    case 7: { labelMesec_Caption[0] = 'J'; labelMesec_Caption[1] = 'U'; labelMesec_Caption[2] = 'L'; break; }
    case 8: { labelMesec_Caption[0] = 'A'; labelMesec_Caption[1] = 'U'; labelMesec_Caption[2] = 'G'; break; }
    case 9: { labelMesec_Caption[0] = 'S'; labelMesec_Caption[1] = 'E'; labelMesec_Caption[2] = 'P'; break; }
    case 10: { labelMesec_Caption[0] = 'O'; labelMesec_Caption[1] = 'K'; labelMesec_Caption[2] = 'T'; break; }
    case 11: { labelMesec_Caption[0] = 'N'; labelMesec_Caption[1] = 'O'; labelMesec_Caption[2] = 'V'; break; }
    case 12: { labelMesec_Caption[0] = 'D'; labelMesec_Caption[1] = 'E'; labelMesec_Caption[2] = 'C'; break; }
    default: { ts.mo=1; labelMesec_Caption[0] = 'J'; labelMesec_Caption[1] = 'A'; labelMesec_Caption[2] = 'N'; break; }
  }
  labelMesec_Caption[3] = '\0';
  DrawBox (&boksMesec); // ispisivanje
  DrawLabel (&labelMesec);
}
//==============================================================================
// funkcija koja ispisuje godine u polje za podesavanje
void ispisiGodine ()
{
  char txt[4]; // pomocna promenljiva

  ByteToStrWithZeros (tsd.yy-2000, txt);
  labelGodina_Caption[0] = txt[1];
  labelGodina_Caption[1] = txt[2];
  labelGodina_Caption[2] = '\0';

  DrawBox (&boksGodina); // ispisivanje
  DrawLabel (&labelGodina);
}
//==============================================================================
// funkcija koja konveruje bajt u string sa nulama
void ByteToStrWithZeros ( unsigned short input, char *output )
{
  char txt[4]; // pomocna promenljiva
  
  ByteToStr ( input, txt );

  if ( txt[0] == ' ' ) { output[0] = '0'; }
  else { output[0] = txt[0]; }

  if ( txt[1] == ' ' ) { output[1] = '0'; }
  else { output[1] = txt[1]; }
  
  if ( txt[2] == ' ' ) { output[2] = '0'; }
  else { output[2] = txt[2]; }

  output[3] = '\0';
}
//==============================================================================
// funkcija koja upisuje u fajl programe hronotermostata
void upisivanjeHrono ()
{
  unsigned short i = 0; // brojacka promenljiva
  unsigned short j = 0; // brojacka promenljiva

  for (i=0; i<brojProg*7; i++)
  {
    // da li se upisuje na,prvu ili drugu stranicu eeprom-a
    if ( i < 15 ) { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT); } // adresiranje i obavestenje o upisu, prva stranica
    else { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_2, _I2C_DIR_MASTER_TRANSMIT); } // adresiranje i obavestenje o upisu, druga stranica
    
    I2C0_Write( 151+i*7, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // slanje adrese eeprom-a za upis
    I2C0_Write( Highest(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje najviseg bajta epohe
    I2C0_Write( Higher(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje visokog bajta epohe
    I2C0_Write( Hi(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje viseg bajta epohe
    I2C0_Write( Lo(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje nizeg bajta epohe
    I2C0_Write( hronoProg[i].kakavRad, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje opisa rada
    I2C0_Write( Hi(hronoProg[i].automatika), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje viseg bajta zadate temperature
    I2C0_Write( Lo(hronoProg[i].automatika), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // slanje nizeg bajta zadate temperature
    
    Delay_us(200);
  }
  
  return;
}
//==============================================================================
// funkcija koja reseuje sve programe hronotermostata
 void resetHrono ()
{
  unsigned char i = 0; // pomocna promenljiva
  
  // resetovanje svih programa tako sto polje kakavRad svugde bude 0, to znaci da se nista ne radi
  for (i=0; i<brojProg*7; i++) { hronoProg[i].epoha = 0; hronoProg[i].kakavRad = 0; hronoProg[i].automatika = 0; }
  upisivanjeHrono(); // upisivanje u memoriju
}
//==============================================================================
// funkcija koja proverava da li je potrebno aktivirati program hronotermostata
// i ispisuje datum i vreme na glavni displej
void proveraHrono ()
{
  unsigned short i = 0; // brojacka promenljiva
  long epoha = 0; // promenljiva za trenutnu epohu
  
  ts = citanjeRTC (); // citanje vremena
  
  // rad sa vremenom se vrsi svaki minut
  if ( (ts.ss>=1)&&provera1sek ) // provera da li je prva sekunda minuta
  { 
    provera1sek = 0; 
    ispisiDatumVreme (); // ispisivanje datuma i vremena na glavni displej
    
    if ( parametarA[hronoAktivnost] ) // provera da li se radi sa hronotermostatom
    {
      epoha = Time_dateToEpoch(&ts); // trenutna epoha
      for (i=brojProg*ts.wd; i<brojProg*ts.wd+brojProg; i++) // proveravaju se samo programi za trenuti dan
      {
        if ( hronoProg[i].kakavRad ) // da li je trenutni program aktivan
        {
          if ( (hronoProg[i].epoha < epoha)&&(hronoProg[i].epoha+20 > epoha ) ) // da li je doslo vreme da se aktivira program
          {
            switch (hronoProg[i].kakavRad) // sta je potrebno aktivirati
            {
              case 1: // podesiti na P1
              {
                if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                stanje.jacinaRada = 1; // podesiti na P1
                vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                parametarA[pravilnoGasenje] = 0; upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina); // korisnik je upalio gorionik
                break;
              }
              case 2: // podesiti na P2
              {
                if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                stanje.jacinaRada = 2; // podesiti na P2
                vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                parametarA[pravilnoGasenje] = 0; upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina); // korisnik je upalio gorionik
                break;
              }
              case 3: // podesiti na P3
              {
                if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                stanje.jacinaRada = 3; // podesiti na P3
                vremeRada = 0; podesavanjeRada (); ispisRada ();// podesavanje rada
                parametarA[pravilnoGasenje] = 0; upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina); // korisnik je upalio gorionik
                break;
              }
              case 4: // podesiti na P4
              {
                if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                stanje.jacinaRada = 4; // podesiti na P4
                vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                parametarA[pravilnoGasenje] = 0; upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina); // korisnik je upalio gorionik
                break;
              }
              case 5: // podesiti na P5
              {
                if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                stanje.jacinaRada = 5; // podesiti na P5
                vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                parametarA[pravilnoGasenje] = 0; upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina); // korisnik je upalio gorionik
                break;
              }
              case 6: // podesiti na automatski rad
              {
                if ( parametar[sondaAmbijenta] ) // moze automatski rad samo ako ima sonda ambijenta
                {
                  if (stanje.fazaRada<2) { stanje.fazaRada = 2; } // ako je gorionik ugasen upaliti ga
                  stanje.jacinaRada = 0; // podesiti na automatsko
                  parametarA[zadTV] = hronoProg[i].automatika;
                  vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                  parametarA[pravilnoGasenje] = 0; // korisnik je upalio gorionik
                  upisAparametra(pravilnoGasenje); Delay_us(50); upisAparametra(pocetnaJacina);  Delay_us(50); upisAparametra(zadTV);
                  break;
                }
              }
              case 7: // podesiti na gasenje
              {
                if (stanje.fazaRada>1) { stanje.fazaRada = 1; } // ako je gorionik upaljen, gasi ga
                vremeRada = 0; podesavanjeRada (); ispisRada (); // podesavanje rada
                parametarA[pravilnoGasenje] = 1; upisAparametra(pravilnoGasenje);// korisnik je ugasio gorionik
                break;
              }
              default: { break; }
            } // end switch
            //******************************************************************
            hronoProg[i].epoha += 604800; // uradjeni dogadjaj se pomera za nedelju dana
            //******************************************************************
          } // end if da li je doslo vreme da se aktivira program
        } // if da li je trenutni program aktivan
      } // end for provera programa hronotermostata
    } // end if rad sa hronotermostatom
  } // end if prva sekunda minuta
  else { if (ts.ss<1) { provera1sek = 1; }}
  
  // sredjivanje niza programa hronotermostata ponedeljkom u ponoc, nova nedelja
  if ( (ts.wd==0)&&(ts.hh==0)&&(ts.mn)&&(ts.ss<20) ) { srediHrono (); }
}
//==============================================================================
// funkcija koja ispisuje dane u polje za podesavanje
void ispisiHronoDane ()
{
  Time_epochToDate(hronoProg[pokHrono].epoha, &ts); // epohu pretvara u strukturu vreme
  
  switch(ts.wd) // priprema za dana
  {
    case 0: { labelHronoDan_Caption[0] = 'P'; labelHronoDan_Caption[1] = 'O'; labelHronoDan_Caption[2] = 'N'; break; }
    case 1: { labelHronoDan_Caption[0] = 'U'; labelHronoDan_Caption[1] = 'T'; labelHronoDan_Caption[2] = 'O'; break; }
    case 2: { labelHronoDan_Caption[0] = 'S'; labelHronoDan_Caption[1] = 'R'; labelHronoDan_Caption[2] = 'E'; break; }
    case 3: { labelHronoDan_Caption[0] = 'C'; labelHronoDan_Caption[1] = 'E'; labelHronoDan_Caption[2] = 'T'; break; }
    case 4: { labelHronoDan_Caption[0] = 'P'; labelHronoDan_Caption[1] = 'E'; labelHronoDan_Caption[2] = 'T'; break; }
    case 5: { labelHronoDan_Caption[0] = 'S'; labelHronoDan_Caption[1] = 'U'; labelHronoDan_Caption[2] = 'B'; break; }
    case 6: { labelHronoDan_Caption[0] = 'N'; labelHronoDan_Caption[1] = 'E'; labelHronoDan_Caption[2] = 'D'; break; }
    default: { ts.wd=1; labelHronoDan_Caption[0] = 'P'; labelHronoDan_Caption[1] = 'O'; labelHronoDan_Caption[2] = 'N'; break; }
  }
  labelHronoDan_Caption[3] = '\0';
  DrawBox(&boksDanHrono); // brisanje stare vrednosti
  DrawLabel(&labelHronoDan); // ispis labele
}
//==============================================================================
// funkcija koja ispisuje programe u polje za podesavanje
void ispisiHronoProgram ()
{
  labelHronoProg_Caption[0] = 48 + ( (pokHrono+1) % brojProg ); // racunanje hrono programa
  if ( labelHronoProg_Caption[0] == 48 ) { labelHronoProg_Caption[0] = 54; } // ako je pokHrono+1 broj deljiv sa 6 onda treba da pise 6
  labelHronoProg_Caption[1] = '\0';
  DrawBox(&boksProgHrono); // brisanje stare vrednosti
  DrawLabel(&labelHronoProg); // ispis labele
}
//==============================================================================
// funkcija koja ispisuje sate u polje za podesavanje
void ispisiHronoSate ()
{
  char txt[4]; // pomocna promenljiva

  Time_epochToDate(hronoProg[pokHrono].epoha, &ts); // epohu pretvara u strukturu vreme

  ByteToStrWithZeros (ts.hh, txt); // priprema za ispis sati
  labelHronoSat_Caption[0] = txt[1];
  labelHronoSat_Caption[1] = txt[2];
  labelHronoSat_Caption[2] = ':';
  ByteToStrWithZeros (ts.mn, txt); // priprema za ispis minuta
  labelHronoSat_Caption[3] = txt[1];
  labelHronoSat_Caption[4] = txt[2];
  labelHronoSat_Caption[5] = '\0';
  DrawBox(&boksSatHrono); // brisanje stare vrednosti
  DrawLabel(&labelHronoSat); // ispis labele
}
//==============================================================================
// funkcija koja ispisuje zadatak programa u polje za podesavanje
void ispisiHronoZadatak ()
{
  switch ( hronoProg[pokHrono].kakavRad )
  {
    case 0: { lablHronoPoso_Caption[0]='-'; lablHronoPoso_Caption[1]='-'; lablHronoPoso_Caption[2]='-'; lablHronoPoso_Caption[3]='-'; break; }
    case 1: { lablHronoPoso_Caption[0]=' '; lablHronoPoso_Caption[1]='P'; lablHronoPoso_Caption[2]='1'; lablHronoPoso_Caption[3]=' '; break; }
    case 2: { lablHronoPoso_Caption[0]=' '; lablHronoPoso_Caption[1]='P'; lablHronoPoso_Caption[2]='2'; lablHronoPoso_Caption[3]=' '; break; }
    case 3: { lablHronoPoso_Caption[0]=' '; lablHronoPoso_Caption[1]='P'; lablHronoPoso_Caption[2]='3'; lablHronoPoso_Caption[3]=' '; break; }
    case 4: { lablHronoPoso_Caption[0]=' '; lablHronoPoso_Caption[1]='P'; lablHronoPoso_Caption[2]='4'; lablHronoPoso_Caption[3]=' '; break; }
    case 5: { lablHronoPoso_Caption[0]=' '; lablHronoPoso_Caption[1]='P'; lablHronoPoso_Caption[2]='5'; lablHronoPoso_Caption[3]=' '; break; }
    case 6: { lablHronoPoso_Caption[0]='A'; lablHronoPoso_Caption[1]='U'; lablHronoPoso_Caption[2]='T'; lablHronoPoso_Caption[3]='O'; break; }
    case 7: { lablHronoPoso_Caption[0]='G'; lablHronoPoso_Caption[1]='A'; lablHronoPoso_Caption[2]='S'; lablHronoPoso_Caption[3]='I'; break; }
    default: { hronoProg[pokHrono].kakavRad = 0; lablHronoPoso_Caption[0]='-'; lablHronoPoso_Caption[1]='-'; lablHronoPoso_Caption[2]='-'; lablHronoPoso_Caption[3]='-'; break; }
  }
  lablHronoPoso_Caption[4] = '\0';
  DrawBox(&boksPosoHrono); // brisanje stare vrednosti
  DrawLabel(&lablHronoPoso); // ispis labele
}
//==============================================================================
// funkcija koja ispisuje zadatu temperaturu u polje za podesavanje
void ispisiHronoZadTemp ()
{
  char txt[6]; // pomocna promenljiva
  
  DrawBox (&boksZadTAhrono);
  if ( hronoProg[pokHrono].automatika )
  {
    // dovodjenje hrono zadate temperature u odgovarajuci opseg
    if ( hronoProg[pokHrono].automatika > parametar[maxTV] ) { hronoProg[pokHrono].automatika = 1; }
    
    // formatiranje stringa
    WordToStr (hronoProg[pokHrono].automatika, txt);
    labelHronoZadTA_Caption[0] = txt[1];
    labelHronoZadTA_Caption[1] = txt[2];
    labelHronoZadTA_Caption[2] = txt[3];
    labelHronoZadTA_Caption[3] = txt[4];
    labelHronoZadTA_Caption[4] = ' ';
    labelHronoZadTA_Caption[5] = 176;
    labelHronoZadTA_Caption[6] = 'C';
    labelHronoZadTA_Caption[7] = '\0';
  }
  else
  {
    labelHronoZadTA_Caption[0] = ' ';
    labelHronoZadTA_Caption[1] = ' ';
    labelHronoZadTA_Caption[2] = '-';
    labelHronoZadTA_Caption[3] = ' ';
    labelHronoZadTA_Caption[4] = '-';
    labelHronoZadTA_Caption[5] = ' ';
    labelHronoZadTA_Caption[6] = ' ';
    labelHronoZadTA_Caption[7] = '\0';
  }
  DrawLabel (&labelHronoZadTA); // ispisivanje temperature
}
//==============================================================================
/*// funkcija za citanje sekundi iz RTC
unsigned char citajRTCsekunde ()
{
  char sek;

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 0, _I2C_MASTER_MODE_SINGLE_RECEIVE ); // pocetak upisivanja je na aredsi 0

  I2C0_Master_Slave_Addr_Set(RTC_ADDR, _I2C_DIR_MASTER_RECEIVE); // adresiranje i obavestenje o citanju
  I2C0_Read( &sek, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // citanje sekundi

  return ((sek & 0x70) >> 4)*10 + (sek & 0x0F);
}*/
//==============================================================================
// funkcija koja epohe niza programa hronotermostata prilagodjava trenutnoj nedelji
void srediHrono ()
{
  long epoha1; // epoha pomedeljka ove nedelje u ponoc
  unsigned short i = 0; // brojacka promenljiva

  ts = citanjeRTC (); // citanje trenutnog vremena
  
  ts.ss = 0; // podesavanje vremena na ponoc danas
  ts.mn = 0;
  ts.hh = 0;
  
  epoha1 = Time_dateToEpoch(&ts) - ((long)(ts.wd))*86400; // epoha ponedeljka ove nedelje u ponoc

  for (i=0; i<brojProg; i++) // sredjivanje ponedeljka
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1;
    }
  }

  for (i=brojProg; i<brojProg*2; i++) // srednjivanje utorka
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*1 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*1;
    }
  }
  
  for (i=brojProg*2; i<brojProg*3; i++) // sredjivanje srede
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*2 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*2;
    }
  }
  
  for (i=brojProg*3; i<brojProg*4; i++) // sredjivanje cetvrtka
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*3 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*3;
    }
  }
  
  for (i=brojProg*4; i<brojProg*5; i++) // sredjivanje petka
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*4 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*4;
    }
  }
  
  for (i=brojProg*5; i<brojProg*6; i++) // sredjivanje subote
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*5 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*5;
    }
  }
  
  for (i=brojProg*6; i<brojProg*7; i++) // sredjivanje nedelje
  {
    if ( hronoProg[i].kakavRad )
    {
      Time_epochToDate(hronoProg[i].epoha, &ts); // stara epoha u vreme
      hronoProg[i].epoha = epoha1 + 86400*6 + ((long)(ts.hh))*3600 + ts.mn*60; // na ponedeljak u ponoc se dodaje dan u nedelji i vreme
    }
    else
    {
      hronoProg[i].epoha = epoha1 + 86400*6;
    }
  }
  upisivanjeHrono(); // upisivanje u memoriju
}
//==============================================================================