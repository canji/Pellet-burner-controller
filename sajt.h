// deklaracija globalnih promenljivih za rad sa sajtom

#ifndef _sajt_h_
#define _sajt_h_

// promenljivi deo sajta, sluzi za prenos promenljivih

extern char promWeb0[31]; // prvi red koda oznacava pocetak java skripte
extern char promWeb1[29]; // IP adresa, clanovi niza 24 i 25 se menjaju
extern char promWeb2[16]; // faza rada, clan niza 13 se menja
extern char promWeb3[25]; // temperatura dima, clanovi niza 20, 21, 22 se menjaju
extern char promWeb4[24]; // temperatura vode, clanovi niza 20, 21 se menjaju
extern char promWeb5[18]; // jacina rada, clan niza 15 se menja
extern char promWeb6[23]; // zadata temperatura, clanovi niza 19, 20 se menjaju
extern char promWebKraj[2]; // krja za promenljive

// fiksno deo sajta se sastoji iz dva dela
// prvi deo sluzi za opis rada i ispis temperatura, drugi deo sluzi za upravljanje gorionikom

extern const code char *webSajt;

#endif