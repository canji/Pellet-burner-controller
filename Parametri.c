// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za rad sa parametrima

#include "Parametri.h"
#include "GNPfunkcije.h"
#include "Sat.h"
#include "DIulazi.h"
#include "Komunikacija.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"
#include "built_in.h"

//==============================================================================
// konstante

// broj parametara
const brojP = 43;

// broj automatskih parametara
const brojPA = 10;

// sifra za ulaz u podesavanje parametara
const sifraTacna = 8493;

// brojevi parametara
const jezik = 0;
const M2tM1 = 1;
const tM1prvoUbacivanje = 2;
const usijavanje = 3;
const tM1ubacivanje = 4;
const pM1ubacivanje = 5;
const SVubacivanje = 6;
const TDubacivanje = 7;
const ubacivanje = 8;
const tM1stabilizacija = 9;
const pM1stabilizacija = 10;
const SVstabilizacija = 11;
const stabilizacija = 12;
const tM1redovanP1 = 13;
const pM1redovanP1 = 14;
const SVredovanP1 = 15;
const tM1redovanP2 = 16;
const pM1redovanP2 = 17;
const SVredovanP2 = 18;
const tM1redovanP3 = 19;
const pM1redovanP3 = 20;
const SVredovanP3 = 21;
const tM1redovanP4 = 22;
const pM1redovanP4 = 23;
const SVredovanP4 = 24;
const tM1redovanP5 = 25;
const pM1redovanP5 = 26;
const SVredovanP5 = 27;
const tacka1 = 28;
const tacka2 = 29;
const tacka3 = 30;
const SVgasenje = 31;
const TDgasenje = 32;
const intervalDuvanja = 33;
const duzinaDuvanja = 34;
const SVduvanje = 35;
const intervalCiscenja = 36;
const duzinaCiscenja = 37;
const maxTD = 38;
const maxTV = 39;
const sondaAmbijenta = 40;
const sondaDimnjace = 41;
const vremeCekanje = 42;

// brojevi automatskih parametara
const Xminimum = 0; // konstanta za kalibraciju displeja
const Xmaksimum = 1; // konstanta za kalibraciju displeja
const Yminimum = 2; // konstanta za kalibraciju displeja
const Ymaksimum = 3; // konstanta za kalibraciju displeja
const pocetnaJacina = 4; // pocetna jacina rada
const zadTV = 5; // zadata temperatura ambijenta
const hronoAktivnost = 6; // da li je hronotermostat aktivan ili ne
const GSMaktivnost = 7; // da li je GSM aktivan ili ne
const pravilnoGasenje = 8; // da li je korisnik ugasio gorionik
const IPadresa = 9; // poslednji bajt IP adrese

//------------------------------------------------------------------------------
// globalne promenljive

// niz za parametre
unsigned int parametar[brojP];

// niz za automatske parametre
unsigned int parametarA[brojPA];

// pokazivaf za broj cifre unosa sifre i pokazivac na parametar prilikom promene parametra
unsigned short pokSh = 0;

// promenljiva za vrednos unosa za sifru
unsigned int sifraUnos = 0;

// promenljiva koja govori prekidnoj rutini da je testiranje cistaca u toku
unsigned short testCistaca = 0;

//==============================================================================
// funkcija za upisivanje P parametra u eeprom
// argument je broj parametra koji se upisuje
void upisPparametra ( unsigned int brojParam )
{
  I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 2*brojParam, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // slanje adrese eeprom-a za upis
  I2C0_Write( Hi(parametar[brojParam]), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje viseg bajta parametra na nizu adresu
  I2C0_Write( Lo(parametar[brojParam]), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // slanje nizeg bajta parametra na visu adresu

  return;
}
//==============================================================================
// funkcija za upisivanje A parametra u eeprom
// argument je broj parametra koji se upisuje
void upisAparametra ( unsigned int brojParam )
{
  I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT); // adresiranje i obavestenje o upisu
  I2C0_Write( 120+2*brojParam, _I2C_MASTER_MODE_BURST_RECEIVE_START ); // slanje adrese eeprom-a za upis
  I2C0_Write( Hi(parametarA[brojParam]), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // slanje viseg bajta parametra na nizu adresu
  I2C0_Write( Lo(parametarA[brojParam]), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // slanje nizeg bajta parametra na visu adresu

  return;
}
//==============================================================================
// funkcija koja cita sve podatke iz eeprom-a
void citanjeEEPROM ()
{
  unsigned int i = 0; // brojacka promenljiva

  // citanje P parametara
  for ( i=0; i<brojP; i++)
  {
    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT);  // selektovanje eeprom prve stranice
    I2C0_Write( 2*i, _I2C_MASTER_MODE_SINGLE_SEND);       // adresiranje eeprom-a

    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_RECEIVE);  // set slave address, and read operation
    I2C0_Read( Hi(parametar[i]), _I2C_MASTER_MODE_BURST_RECEIVE_START ); // citanje viseg bajta parametra sa nize adresu
    I2C0_Read( Lo(parametar[i]), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // citanje nizeg bajta parametra sa vise adresu
    
    Delay_us(50);
  }
  proveraParametara (); // provera P parametara
  
  // citanje A parametara
  for ( i=0; i<brojPA; i++)
  {
    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT);  // selektovanje eeprom prve stranice
    I2C0_Write( 120+2*i, _I2C_MASTER_MODE_SINGLE_SEND);       // adresiranje eeprom-a

    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_RECEIVE);  // podesavanje za citanje
    I2C0_Read( Hi(parametarA[i]), _I2C_MASTER_MODE_BURST_RECEIVE_START ); // citanje viseg bajta parametra sa nize adresu
    I2C0_Read( Lo(parametarA[i]), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // citanje nizeg bajta parametra sa vise adresu

    if (parametarA[i]==0xFFFF) { parametarA[i] = 0; } // zastita od citanja prazne memorije sa svim jedinicama

    Delay_us(50);
  }
  
  // citanje alarmnog broja
  for ( i=0; i<20; i++ )
  {
    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT);  // selektovanje eeprom prve stranice
    I2C0_Write( 90+i, _I2C_MASTER_MODE_SINGLE_SEND);       // adresiranje eeprom-a

    I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_RECEIVE);  // podesavanje za citanje
    I2C0_Read( alarmniBroj[i], _I2C_MASTER_MODE_SINGLE_RECEIVE ); // citanje nizeg bajta parametra sa vise adresu

    if (alarmniBroj[i]==0xFF) { alarmniBroj[i] = 0; } // zastita od citanja prazne memorije sa svim jedinicama

    Delay_us(50);
  }
  
  // citanje programa hronotermostata
  for ( i=0; i<brojProg*7; i++ )
  {
    // da li se upisuje adresa na prve ili druge stranice eeprom-a
    if ( i < 15 ) { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_TRANSMIT); } // adresiranje i obavestenje o upisu adrese, prva stranica
    else { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_2, _I2C_DIR_MASTER_TRANSMIT); } // adresiranje i obavestenje o upisu adrese, druga stranica
  
    I2C0_Write( 151+i*7, _I2C_MASTER_MODE_SINGLE_SEND);       // adresiranje eeprom-a
    
    // da li se cita prva ili druga stranica eeprom-a
    if ( i < 15 ) { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_1, _I2C_DIR_MASTER_RECEIVE); } // adresiranje i obavestenje o upisu adrese, prva stranica
    else { I2C0_Master_Slave_Addr_Set(ADR_EEPROM_2, _I2C_DIR_MASTER_RECEIVE); } // adresiranje i obavestenje o upisu adrese, druga stranica
    
    I2C0_Read( Highest(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_START ); // citanje najviseg bajta epohe
    I2C0_Read( Higher(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // citanje visokog bajta epohe
    I2C0_Read( Hi(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // citanje viseg bajta epohe
    I2C0_Read( Lo(hronoProg[i].epoha), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // citanje izeg bajta epohe
    I2C0_Read( hronoProg[i].kakavRad, _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // citanje opisa rada
    I2C0_Read( Hi(hronoProg[i].automatika), _I2C_MASTER_MODE_BURST_RECEIVE_CONT ); // citanje viseg bajta zadate temperature
    I2C0_Read( Lo(hronoProg[i].automatika), _I2C_MASTER_MODE_BURST_RECEIVE_FINISH ); // citanje nizeg bajta zadate temperature
    
    // zastita od citanja prazne memorije sa svim jedinicama
    if (hronoProg[i].epoha==0xFFFFFFFF) { hronoProg[i].epoha = 0; }
    if (hronoProg[i].kakavRad==0xFF) { hronoProg[i].kakavRad = 0; }
    if (hronoProg[i].automatika==0xFFFF) { hronoProg[i].automatika = 0; }
    
    Delay_us(50);
  }
  
  return;
}
//==============================================================================
// funkcija za proveru parametara
// proverava da li su parametri u odgovarajucem opsegu i ako nisu postavlja ih u odgovarajuci opseg
void proveraParametara ()
{
   unsigned short i = 0; // brojacka promenljiva

  // prolazi se kroz sve parametre i proverava se da li su u odgovarajucem opsegu
  for (i=0; i<brojP; i++)
  {
    switch (i)
    {
      case 0: { if ( (parametar[i]<1)||(parametar[i]>5) ) { parametar[i] = 1; } break; }
      case 1: { if ( (parametar[i]<0)||(parametar[i]>300) ) { parametar[i] = 100; } break; }
      case 2: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 15; } break; }
      case 3: { if ( (parametar[i]<600)||(parametar[i]>12000) ) { parametar[i] = 2400; } break; }
      case 4: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 7; } break; }
      case 5: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 350; } break; }
      case 6: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 60; } break; }
      case 7: { if ( (parametar[i]<20)||(parametar[i]>100) ) { parametar[i] = 55; } break; }
      case 8: { if ( (parametar[i]<600)||(parametar[i]>12000) ) { parametar[i] = 3000; } break; }
      case 9: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 10; } break; }
      case 10: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 350; } break; }
      case 11: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 55; } break; }
      case 12: { if ( (parametar[i]<600)||(parametar[i]>12000) ) { parametar[i] = 600; } break; }
      case 13: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 5; } break; }
      case 14: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 400; } break; }
      case 15: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 52; } break; }
      case 16: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 10; } break; }
      case 17: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 400; } break; }
      case 18: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 56; } break; }
      case 19: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 15; } break; }
      case 20: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 400; } break; }
      case 21: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 60; } break; }
      case 22: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 20; } break; }
      case 23: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 400; } break; }
      case 24: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 75; } break; }
      case 25: { if ( (parametar[i]<5)||(parametar[i]>100) ) { parametar[i] = 25; } break; }
      case 26: { if ( (parametar[i]<10)||(parametar[i]>1200) ) { parametar[i] = 400; } break; }
      case 27: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 85; } break; }
      case 28: { if ( (parametar[i]<0)||(parametar[i]>10) ) { parametar[i] = 0; } break; }
      case 29: { if ( (parametar[i]<0)||(parametar[i]>10) ) { parametar[i] = 0; } break; }
      case 30: { if ( (parametar[i]<0)||(parametar[i]>10) ) { parametar[i] = 0; } break; }
      case 31: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 80; } break; }
      case 32: { if ( (parametar[i]<40)||(parametar[i]>100) ) { parametar[i] = 50; } break; }
      case 33: { if ( (parametar[i]<0)||(parametar[i]>36000) ) { parametar[i] = 6000; } break; }
      case 34: { if ( (parametar[i]<0)||(parametar[i]>600) ) { parametar[i] = 100; } break; }
      case 35: { if ( (parametar[i]<30)||(parametar[i]>99) ) { parametar[i] = 90; } break; }
      case 36: { if ( (parametar[i]<0)||(parametar[i]>36000) ) { parametar[i] = 600; } break; }
      case 37: { if ( (parametar[i]<0)||(parametar[i]>1200) ) { parametar[i] = 100; } break; }
      case 38: { if ( (parametar[i]<50)||(parametar[i]>400) ) { parametar[i] = 300; } break; }
      case 39: { if ( (parametar[i]<20)||(parametar[i]>100) ) { parametar[i] = 85; } break; }
      case 40: { if ( (parametar[i]<0)||(parametar[i]>2) ) { parametar[i] = 0; } break; }
      case 41: { if ( (parametar[i]<0)||(parametar[i]>1) ) { parametar[i] = 0; } break; }
      case 42: { if ( (parametar[i]<0)||(parametar[i]>36000) ) { parametar[i] = 0; } break; }
      default: { break; }
    } // kraj switch
  } // kraj for
} // kraj funkcije
//==============================================================================
// funkcija za obradu unosa sifre
// argument je cifra koja je uneta
void obradaSifre ( unsigned short cifraUnosa )
{
   switch ( pokSh )
   {
     case 0:
     {
       sifraUnos += cifraUnosa * 1000;
       pokSh++;
       sh1.Visible = 1;
       DrawCircle (&sh1);
       return;
     }
     case 1:
     {
       sifraUnos += cifraUnosa * 100;
       pokSh++;
       sh2.Visible = 1;
       DrawCircle (&sh2);
       return;
     }
     case 2:
     {
       sifraUnos += cifraUnosa * 10;
       pokSh++;
       sh3.Visible = 1;
       DrawCircle (&sh3);
       return;
     }
     case 3:
     {
       sifraUnos += cifraUnosa;
       pokSh = 0;
       sh4.Visible = 1;
       DrawCircle (&sh4);
       Delay_ms (500);
       // provera da li je sifra dobra
       if ( sifraUnos == sifraTacna )
       { // ulaz u promenu parametara
         pokSh = 1;  // krece se od parametra 1
         paramTest.Visible = 1; paramTest.Active = 1; // taster za test se vidi kod prvog parametra
         DrawScreen(32772);
         ispisBrojaParam (pokSh);
         ispisVrednostiParam (pokSh);
       }
       else // sifra nije dobra, sve ispocetka
       {
         parametrija ();
       }
       return;
     }
     default:
     {
       parametrija ();
     }
   }
}
//==============================================================================
// resetovanje parametara
void resetP ()
{
  parametar[0] = 1; upisPparametra(0); Delay_us(50);
  parametar[1] = 50; upisPparametra(1); Delay_us(50);
  parametar[2] = 15; upisPparametra(2); Delay_us(50);
  parametar[3] = 200; upisPparametra(3); Delay_us(50);
  parametar[4] = 7; upisPparametra(4); Delay_us(50);
  parametar[5] = 150; upisPparametra(5); Delay_us(50);
  parametar[6] = 70; upisPparametra(6); Delay_us(50);
  parametar[7] = 55; upisPparametra(7); Delay_us(50);
  parametar[8] = 600; upisPparametra(8); Delay_us(50);
  parametar[9] = 10; upisPparametra(9); Delay_us(50);
  parametar[10] = 150; upisPparametra(10); Delay_us(50);
  parametar[11] = 46; upisPparametra(11); Delay_us(50);
  parametar[12] = 300; upisPparametra(12); Delay_us(50);
  parametar[13] = 5; upisPparametra(13); Delay_us(50);
  parametar[14] = 150; upisPparametra(14); Delay_us(50);
  parametar[15] = 45; upisPparametra(15); Delay_us(50);
  parametar[16] = 10; upisPparametra(16); Delay_us(50);
  parametar[17] = 150; upisPparametra(17); Delay_us(50);
  parametar[18] = 55; upisPparametra(18); Delay_us(50);
  parametar[19] = 15; upisPparametra(19); Delay_us(50);
  parametar[20] = 150; upisPparametra(20); Delay_us(50);
  parametar[21] = 65; upisPparametra(21); Delay_us(50);
  parametar[22] = 20; upisPparametra(22); Delay_us(50);
  parametar[23] = 150; upisPparametra(23); Delay_us(50);
  parametar[24] = 75; upisPparametra(24); Delay_us(50);
  parametar[25] = 25; upisPparametra(25); Delay_us(50);
  parametar[26] = 150; upisPparametra(26); Delay_us(50);
  parametar[27] = 85; upisPparametra(27); Delay_us(50);
  parametar[28] = 0; upisPparametra(28); Delay_us(50);
  parametar[29] = 0; upisPparametra(29); Delay_us(50);
  parametar[30] = 0; upisPparametra(30); Delay_us(50);
  parametar[31] = 80; upisPparametra(31); Delay_us(50);
  parametar[32] = 50; upisPparametra(32); Delay_us(50);
  parametar[33] = 600; upisPparametra(33); Delay_us(50);
  parametar[34] = 200; upisPparametra(34); Delay_us(50);
  parametar[35] = 95; upisPparametra(35); Delay_us(50);
  parametar[36] = 0; upisPparametra(36); Delay_us(50);
  parametar[37] = 0; upisPparametra(37); Delay_us(50);
  parametar[38] = 300; upisPparametra(38); Delay_us(50);
  parametar[39] = 85; upisPparametra(39); Delay_us(50);
  parametar[40] = 0; upisPparametra(40); Delay_us(50);
  parametar[41] = 0; upisPparametra(41); Delay_us(50);
  parametar[42] = 0; upisPparametra(42); Delay_us(50);
}
//==============================================================================
// resetovanje automatskih parametara
void resetPA ()
{
  parametarA[0] = 0; upisAparametra(0); Delay_us(50);
  parametarA[1] = 0; upisAparametra(1); Delay_us(50);
  parametarA[2] = 0; upisAparametra(2); Delay_us(50);
  parametarA[3] = 0; upisAparametra(3); Delay_us(50);
  parametarA[4] = 1; upisAparametra(4); Delay_us(50);
  parametarA[5] = 26; upisAparametra(5); Delay_us(50);
  parametarA[6] = 0; upisAparametra(6); Delay_us(50);
  parametarA[7] = 0; upisAparametra(7); Delay_us(50);
  parametarA[8] = 1; upisAparametra(8); Delay_us(50);
  parametarA[9] = 0; upisAparametra(9); Delay_us(50);
}
//==============================================================================
// ispisivanje broja parametra prilikom promene parametara
void ispisBrojaParam ( unsigned short brojParamIspis )
{
  char txt[4]; // pomocna promenljiva
  
  // brisanje stare vrednosti
  DrawBox(&boksBrojParam);
  ByteToStr (brojParamIspis, txt);
  brojParam_Caption[0] = txt[1];
  brojParam_Caption[1] = txt[2];
  brojParam_Caption[2] = '\0';
  DrawLabel (&brojParam); // ispsi broja parametra
}
//==============================================================================
// ispisivanje vrednosti parametara prilikom promene parametara
void ispisVrednostiParam ( unsigned short brojParamIspis )
{
  // brisanje stare vrednosti
  DrawBox(&boksVrednostParam);
  
  switch (brojParamIspis)
  {
    case 0: { formatParam(0,6); break; }
    case 1: { formatParam(1,0); break; }
    case 2: { formatParam(2,1); break; }
    case 3: { formatParam(3,2); break; }
    case 4: { formatParam(4,1); break; }
    case 5: { formatParam(5,0); break; }
    case 6: { formatParam(6,3); break; }
    case 7: { formatParam(7,4); break; }
    case 8: { formatParam(8,2); break; }
    case 9: { formatParam(9,1); break; }
    case 10: { formatParam(10,0); break; }
    case 11: { formatParam(11,3); break; }
    case 12: { formatParam(12,2); break; }
    case 13: { formatParam(13,1); break; }
    case 14: { formatParam(14,0); break; }
    case 15: { formatParam(15,3); break; }
    case 16: { formatParam(16,1); break; }
    case 17: { formatParam(17,0); break; }
    case 18: { formatParam(18,3); break; }
    case 19: { formatParam(19,1); break; }
    case 20: { formatParam(20,0); break; }
    case 21: { formatParam(21,3); break; }
    case 22: { formatParam(22,1); break; }
    case 23: { formatParam(23,0); break; }
    case 24: { formatParam(24,3); break; }
    case 25: { formatParam(25,1); break; }
    case 26: { formatParam(26,0); break; }
    case 27: { formatParam(27,3); break; }
    case 28: { formatParam(28,4); break; }
    case 29: { formatParam(29,4); break; }
    case 30: { formatParam(30,4); break; }
    case 31: { formatParam(31,3); break; }
    case 32: { formatParam(32,4); break; }
    case 33: { formatParam(33,2); break; }
    case 34: { formatParam(34,0); break; }
    case 35: { formatParam(35,3); break; }
    case 36: { formatParam(36,2); break; }
    case 37: { formatParam(37,0); break; }
    case 38: { formatParam(38,4); break; }
    case 39: { formatParam(39,4); break; }
    case 40: { formatParam(40,6); break; }
    case 41: { formatParam(41,5); break; }
    case 42: { formatParam(42,2); break; }
    default: { break; }
  } // krja switch
  DrawLabel(&paramVrednost);
}
//==============================================================================
// funkcija za formatiranje unsigned int vrednosti parametra u vrednost za korisnika
// argumet tipF je tip formatiranja unsigned int:
// 0 - sekunde, 1 - sekunde0.1, 2 - minuti, 3 - %, 4 - celzijusi, 5 - da ne, 6 - n d s
void formatParam ( unsigned short brojParamIspis, unsigned short tipF )
{
  char txt[6]; // pomocna promenljiva

  switch (tipF) // izbor tipa formatiranja
  {
    case 0:
    {
      WordToStr (parametar[brojParamIspis], txt);
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = txt[0];
      paramVrednost_Caption[2] = txt[1];
      paramVrednost_Caption[3] = txt[2];
      paramVrednost_Caption[4] = txt[3];
      paramVrednost_Caption[5] = ' ';
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = 's';
      paramVrednost_Caption[8] = 'e';
      paramVrednost_Caption[9] = 'c';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 1:
    {
      WordToStr (parametar[brojParamIspis], txt);
      paramVrednost_Caption[0] = txt[0];
      paramVrednost_Caption[1] = txt[1];
      paramVrednost_Caption[2] = txt[2];
      paramVrednost_Caption[3] = txt[3];
      paramVrednost_Caption[4] = '.';
      paramVrednost_Caption[5] = txt[4];
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = 's';
      paramVrednost_Caption[8] = 'e';
      paramVrednost_Caption[9] = 'c';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 2:
    {
      WordToStr ( (parametar[brojParamIspis]/600), txt);
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = txt[0];
      paramVrednost_Caption[2] = txt[1];
      paramVrednost_Caption[3] = txt[2];
      paramVrednost_Caption[4] = txt[3];
      paramVrednost_Caption[5] = txt[4];
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = 'm';
      paramVrednost_Caption[8] = 'i';
      paramVrednost_Caption[9] = 'n';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 3:
    {
      WordToStr (parametar[brojParamIspis], txt);
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = txt[0];
      paramVrednost_Caption[2] = txt[1];
      paramVrednost_Caption[3] = txt[2];
      paramVrednost_Caption[4] = txt[3];
      paramVrednost_Caption[5] = txt[4];
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = ' ';
      paramVrednost_Caption[8] = '%';
      paramVrednost_Caption[9] = ' ';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 4:
    {
      WordToStr (parametar[brojParamIspis], txt);
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = txt[0];
      paramVrednost_Caption[2] = txt[1];
      paramVrednost_Caption[3] = txt[2];
      paramVrednost_Caption[4] = txt[3];
      paramVrednost_Caption[5] = txt[4];
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = 176;
      paramVrednost_Caption[8] = 'C';
      paramVrednost_Caption[9] = ' ';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 5:
    {
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = ' ';
      paramVrednost_Caption[2] = ' ';
      paramVrednost_Caption[3] = ' ';
      if (parametar[brojParamIspis]) { paramVrednost_Caption[4] = 'y'; }
      else { paramVrednost_Caption[4] = 'n'; }
      paramVrednost_Caption[5] = ' ';
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = ' ';
      paramVrednost_Caption[8] = ' ';
      paramVrednost_Caption[9] = ' ';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    case 6:
    {
      paramVrednost_Caption[0] = ' ';
      paramVrednost_Caption[1] = ' ';
      paramVrednost_Caption[2] = ' ';
      paramVrednost_Caption[3] = ' ';
      if ( parametar[brojParamIspis] == 0 ) { paramVrednost_Caption[4] = 'n'; }
      if ( parametar[brojParamIspis] == 1 ) { paramVrednost_Caption[4] = 'd'; }
      if ( parametar[brojParamIspis] == 2 ) { paramVrednost_Caption[4] = 's'; }
      paramVrednost_Caption[5] = ' ';
      paramVrednost_Caption[6] = ' ';
      paramVrednost_Caption[7] = ' ';
      paramVrednost_Caption[8] = ' ';
      paramVrednost_Caption[9] = ' ';
      paramVrednost_Caption[10] = '\0';
      break;
    }
    default: { break; }
  } // kraj switch
}
//==============================================================================
// funkcija za inkremntiranje ili dekremntiranje vrednosti parametara
// argumenti:
// brojParamIspis - broj parametra
// plusILIminus - inkrementiranje ili dekrementiranje, 1++, 0--
// korak - za koliko se povecava ili smanjuje parametar
// donjiOpseg - minimalna vrednost parametra
// gornji opseg - maksimalna vrednpost parametra
void plusMinusParam ( unsigned short brojParamIspis, unsigned short plusILIminus, unsigned int korak, unsigned int donjiOpseg, unsigned int gornjiOpseg )
{
  if ( plusILIminus ) // da li se sabira ili oduzima
  { // sabira se
    if ( parametar[brojParamIspis] == gornjiOpseg ) { return; } // krja opsega povecanja
    parametar[brojParamIspis] += korak;
  }
  else // oduzima se
  {
    if ( parametar[brojParamIspis] == donjiOpseg ) { return; } // krja opsega povecanja
    parametar[brojParamIspis] -= korak;
  }
}
//==============================================================================