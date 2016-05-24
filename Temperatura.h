// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za merenje temperature

#ifndef _Temperatura_h_
#define _Temperatura_h_

//==============================================================================
// globalne promenljive

extern unsigned int Tdimnjace; // temperatura dimnjace
//extern double sumaTdimnjace; // suma odbiraka temperature dimnjce

extern unsigned int brojacTD; // brojac intervala od 10 sekundi kada se citaju odbirci temperature dima
extern unsigned int brojacTA; // brojac intervala od 10 sekundi kada se citaju odbirci temperature ambijenta
extern unsigned int brojacTV; // brojac odbiraka temperature vode

extern unsigned int Tambijenta; // temperatura ambijenta
extern double sumaTvode; // suma odbiraka temperature ambijenta


//==============================================================================
// funkcije

// funkcija za inicijalizaciju mikrokontrolera za rad sa senzorima temperature
void initTemperatura ();

// polinomska funkcija za karakteristiku J termopara
// pretvara Temperaturu u milivolte
double TfmV ( double x );

// polinomska funkcija za karakteristiku J termopara
// pretvara milivolte u temperaturu
double mVfT ( double x );

// skupljanje odbiraka, njihovo usrednjavanje i racunanje temperature dimnjace
void temperaturaD ();

// skupljanje odbiraka, njihovo usrednjavanje i racunanje temperature ambijenta
void temperaturaA ();

//==============================================================================
#endif