// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za osnovni rad gorionika

#ifndef _GNPfunkcije_h_
#define _GNPfunkcije_h_

//==============================================================================i

// konstante

extern const Tm; // poluperioda mreznog napona u milisekundama

//------------------------------------------------------------------------------
// globalne promenljive

// struktura za stanje gorionika
// displej je 0 kad se prikazuje rad, a 1 kad se prikazuje meni
// jacinaRada je 0 kad je rad automatski, a 1, 2, 3, 4 ili 5 je kad je rad manuelan
// fazaRada odredjuje fazu rada gorionika
// pojacano duvanje odredjuje da li je pojacano duvanje u toku
// cekanje odredjuje da li je cekanje u toku
struct st 
{ unsigned short displej; unsigned short jacinaRada; 
  unsigned short fazaRada; unsigned short pojacanoDuvanje; unsigned short cekanje; };

typedef struct st stanjeGorionika;

extern stanjeGorionika stanje;

// promenljive za rad DC motora
extern unsigned int tM1; // duzina rada motora M1 u desetinkama
extern unsigned int pM1; // pauza u radu motora M1 u desetinkama
extern unsigned int M2duzeM1; // koliko M2 radi duze od M1 u desetinkama

// brojaci desetinki
extern unsigned int vremeDC; // brojac za rad motora M1 i M2
extern unsigned int vremeRada; // brojac za rad sa fazama rada
extern unsigned int vremeDuvanje; // brojac za pojacano duvanje
extern unsigned int vremeCiscenje; // brojac za upravljanje sistemom za ciscenje

//==============================================================================
// funkcije

// funkcija za inicijalizaciju mikrokontrolera za osnovnu funkciju gorionika
void initOsnovno ();

// funkcija za podesavanje snage AC motora, motor za ubacivanje vazduha
// argunemt je snaga motora u procentima
void ACmotor (unsigned int ac);

// funkcija koja podesava parametra rada na osnovu stanja
 void podesavanjeRada ();
 
 // funkcija koja menja stanja na osnovu vremena i temperatura
void promenaStanja ();

// funkcija koja ispisuje fazu rada na displeju
void ispisRada ();

// funkcija koja gasi gorionik i javlja da je doslo do greske
void greska (unsigned short greh);

//==============================================================================
#endif