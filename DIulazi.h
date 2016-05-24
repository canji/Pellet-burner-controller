// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za debaunsiranje digitalnih ulaza

#ifndef _DIulazi_h_
#define _DIulazi_h_

//==============================================================================i

// konstante

extern const debaunKratko; // debaunsiranje 3 ms, kratko
extern const debaunDugacko; // debaunsiranje 2 s, dugacko

//------------------------------------------------------------------------------
// globalne promenljive

// brojaci desetinki za debaunsiranje
extern unsigned char DtasterOnOff; // debaunsiranje tastera on off
extern unsigned char DtasterJacina; // debaunsiranje tastera jacina
extern unsigned char Dtermostat; // debaunsiranje termostata
extern unsigned char Dfotocelija; // debaunsiranje fotocelije
extern unsigned char Dpresostat; // debaunsiranje termostata

// promenljive koje programu govore o stanju digitalnih ulaza
extern unsigned char stanjeTermostat; // 1 temperatura je presla zadatu, 0 nije
extern unsigned char stanjeFotocelija; // 1 ima plamena, 0 nema
extern unsigned char stanjePresostat; // 0 nema pritiska, 1 ima pritiska

//==============================================================================
// funkcije

// inicijalizacija digitalnih ulaza
void initDIulazi ();

// funkcija za proveru tastera
void tasteri ();

// funkcija za proveru digitalnih ulaza
void digitalniUlazi ();

//==============================================================================
#endif