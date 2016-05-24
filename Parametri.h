// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za rad sa parametrima

#ifndef _Parametri_h_
#define _Parametri_h_

//==============================================================================
// konstante

// adresa eeprom memorije prva strana
#define ADR_EEPROM_1 0x56

// adresa eeprom memorije druga strana
#define ADR_EEPROM_2 0x57

// broj parametara
extern const brojP;

// broj automatskih parametara
extern const brojPA;

// sifra za ulaz u podesavanje parametara
extern const sifraTacna;

// brojevi parametara
extern const jezik;
extern const M2tM1;
extern const tM1prvoUbacivanje;
extern const usijavanje;
extern const tM1ubacivanje;
extern const pM1ubacivanje;
extern const SVubacivanje;
extern const TDubacivanje;
extern const ubacivanje;
extern const tM1stabilizacija;
extern const pM1stabilizacija;
extern const SVstabilizacija;
extern const stabilizacija;
extern const tM1redovanP1;
extern const pM1redovanP1;
extern const SVredovanP1;
extern const tM1redovanP2;
extern const pM1redovanP2;
extern const SVredovanP2;
extern const tM1redovanP3;
extern const pM1redovanP3;
extern const SVredovanP3;
extern const tM1redovanP4;
extern const pM1redovanP4;
extern const SVredovanP4;
extern const tM1redovanP5;
extern const pM1redovanP5;
extern const SVredovanP5;
extern const tacka1;
extern const tacka2;
extern const tacka3;
extern const SVgasenje;
extern const TDgasenje;
extern const intervalDuvanja;
extern const duzinaDuvanja;
extern const SVduvanje;
extern const intervalCiscenja;
extern const duzinaCiscenja;
extern const maxTD;
extern const maxTV;
extern const sondaAmbijenta;
extern const sondaDimnjace;
extern const vremeCekanje;

// brojevi automatskih parametara
extern const Xminimum; // konstanta za kalibraciju displeja
extern const Xmaksimum; // konstanta za kalibraciju displeja
extern const Yminimum; // konstanta za kalibraciju displeja
extern const Ymaksimum; // konstanta za kalibraciju displeja
extern const pocetnaJacina; // pocetna jacina rada
extern const zadTV; // zadata temperatura ambijenta
extern const hronoAktivnost; // da li je hronotermostat aktivan ili ne
extern const GSMaktivnost; // da li je GSM aktivan ili ne
extern const pravilnoGasenje; // da li je korisnik ugasio gorionik
extern const IPadresa; // poslednji bajt IP adrese
//------------------------------------------------------------------------------
// globalne promenljive

// niz za parametre
extern unsigned int parametar[];

// niz za automatske parametre
extern unsigned int parametarA[];

// pokazivaf za broj cifre unosa sifre i pokazivac na parametar prilikom promene parametra
extern unsigned short pokSh;

// promenljiva za vrednos unosa za sifru
extern unsigned int sifraUnos;

// promenljiva koja govori prekidnoj rutini da je testiranje cistaca u toku
extern unsigned short testCistaca;

//==============================================================================
// funkcije

// funkcija za upisivanje P parametra u eeprom
// argument je broj parametra koji se upisuje
void upisPparametra ( unsigned int brojParam );

// funkcija za upisivanje A parametra u eeprom
// argument je broj parametra koji se upisuje
void upisAparametra ( unsigned int brojParam );

// funkcija koja cita sve podatke iz eeprom-a
void citanjeEEPROM ();

// funkcija za proveru parametara
// proverava da li su parametri u odgovarajucem opsegu i ako nisu postavlja ih u odgovarajuci opseg
void proveraParametara ();

// funkcija za obradu unosa sifre
// argument je cifra koja je uneta
void obradaSifre ( unsigned short cifraUnosa );

// resetovanje parametara
void resetP ();

// resetovanje automatskih parametara
void resetPA ();

// ispisivanje broja parametra prilikom promene parametara
void ispisBrojaParam ( unsigned short brojParamIspis );

// ispisivanje vrednosti parametara prilikom promene parametara
void ispisVrednostiParam ( unsigned short brojParamIspis );

// funkcija za formatiranje unsigned int vrednosti parametra u vrednost za korisnika
// argumet tipF je tip formatiranja unsigned int:
// 0 - sekunde, 1 - sekunde0.1, 2 - minuti, 3 - %, 4 - celzijusi, 5 - da ne
void formatParam ( unsigned short brojParamIspis, unsigned short tipF );

// funkcija za inkremntiranje ili dekremntiranje vrednosti parametara
// argumenti:
// brojParamIspis - broj parametra
// plusILIminus - inkrementiranje ili dekrementiranje, 1++, 0--
// korak - za koliko se povecava ili smanjuje parametar
// donjiOpseg - minimalna vrednost parametra
// gornji opseg - maksimalna vrednpost parametra
void plusMinusParam ( unsigned short brojParamIspis, unsigned short plusILIminus, unsigned int korak, unsigned int donjiOpseg, unsigned int gornjiOpseg );

//==============================================================================
#endif