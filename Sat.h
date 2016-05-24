// program za upravljanje gorionikom na pelete
// deklaracija globalnih promenljivih i funkcija za rad sa RTC

#ifndef _Sat_h_
#define _Sat_h_

// RTC Definitions
#define RTC_ADDR  0x68

//==============================================================================
// konstante

// broj programa u jednom danu za hronotermostat
extern const brojProg;

//==============================================================================
// struktura za rad sa vremenom
typedef struct
        {
        unsigned char   ss ;    // seconds
        unsigned char   mn ;    // minutes
        unsigned char   hh ;    // hours
        unsigned char   md ;    // day in month, from 1 to 31
        unsigned char   wd ;    // day in week, monday=0, tuesday=1, .... sunday=6
        unsigned char   mo ;    // month number, from 1 to 12 (and not from 0 to 11 as with unix C time !)
        unsigned int    yy ;    // year Y2K compliant, from 1892 to 2038
        } TimeStruct ;

extern TimeStruct ts; // struktura za vreme koje se koristi kao informacij o vremenu

extern TimeStruct tsd; // struktura za vreme koja se koristi za podesavanje vremena

// struktura za rad sa hronotermostatom
typedef struct
        {
        long epoha; // epoha dogadjaja
        unsigned char kakavRad; // sta da se radi sa gorionikom
        unsigned int automatika; // ako je potrebno podesiti automatski rad kolika temperatura da bude
        } HronoStruct;

extern HronoStruct hronoProg[];

extern unsigned char pokHrono; // pokazivac na program hronotermostata

extern unsigned char provera1sek; // pomocna promenljiva za proveru da li je prosla prva sekunda minuta

//==============================================================================
// funkcije

// funkcija za inicijalizaciju mikrokontrolera za rad sa RTC
void initSat ();

// funkcija koja podesava vreme u RTC
void upisRTC (TimeStruct *ts);

// funkcija koja cita vreme iz RTC
TimeStruct citanjeRTC ();

// funkcija koja ispisuje vreme i datum na osnovnom prozoru
void ispisiDatumVreme ();

// funkcija koja ispisuje sate u polje za podesavanje
void ispisiSate ();

// funkcija koja ispisuje minute u polje za podesavanje
void ispisiMinute ();

// funkcija koja ispisuje dane u polje za podesavanje
void ispisiDane ();

// funkcija koja ispisuje mesece u polje za podesavanje
void ispisiMesece ();

// funkcija koja ispisuje godine u polje za podesavanje
void ispisiGodine ();

// funkcija koja konveruje bajt u string sa nulama
void ByteToStrWithZeros ( unsigned short input, char *output );

// funkcija koja upisuje u fajl programe hronotermostata
void upisivanjeHrono ();

// funkcija koja reseuje sve programe hronotermostata
void resetHrono ();

// funkcija koja proverava da li je potrebno aktivirati program hronotermostata
// i ispisuje datum i vreme na glavni displej
void proveraHrono ();

// funkcija koja ispisuje dane u polje za podesavanje
void ispisiHronoDane ();

// funkcija koja ispisuje programe u polje za podesavanje
void ispisiHronoProgram ();

// funkcija koja ispisuje sate u polje za podesavanje
void ispisiHronoSate ();

// funkcija koja ispisuje zadatak programa u polje za podesavanje
void ispisiHronoZadatak ();

// funkcija koja ispisuje zadatu temperaturu u polje za podesavanje
void ispisiHronoZadTemp ();

// funkcija koja epohe niza programa hronotermostata prilagodjava trenutnoj nedelji
void srediHrono ();

// funkcija vraca na koji dan pokazuje pokHrono
/*unsigned char vratiHronoDan ();*/

//==============================================================================
#endif