// definicija globalnih promenljivih za rad sa sajtom

#include "sajt.h"

// promenljivi deo sajta, sluzi za prenos promenljivih

char promWeb0[31] = "<script language=\"javascript\">"; // prvi red koda oznacava pocetak java skripte
char promWeb1[29] = "var adresa=\"192.168.20.60\";"; // IP adresa, clanovi niza 23 i 24 i mozda 25 se menjaju
char promWeb2[16] = "var fazaRada=5;"; // faza rada, clan niza 13 se menja
char promWeb3[25] = "var temperaturaDima=134;"; // temperatura dima, clanovi niza 20, 21, 22 se menjaju
char promWeb4[24] = "var temperaturaVode=45;"; // temperatura vode, clanovi niza 20, 21 se menjaju
char promWeb5[18] = "var jacinaRada=0;"; // jacina rada, clan niza 15 se menja
char promWeb6[23] = "var zadataTempVode=25;"; // zadata temperatura, clanovi niza 19, 20 se menjaju
char promWebKraj[2] = "\n"; // krja za promenljive

// fiksno deo sajta se sastoji iz dva dela
// prvi deo sluzi za opis rada i ispis temperatura, drugi deo sluzi za upravljanje gorionikom

const code char *webSajt =
"switch(fazaRada)\
{\
case 0:{fazaRadaWeb=\"Gorionik ugasen\";break;}\
case 1:{fazaRadaWeb=\"Gasenje\"; break;}\
case 2:{fazaRadaWeb=\"Usijavanje grejaca\";break;}\
case 3:{fazaRadaWeb=\"Ubacivanje goriva\";break;}\
case 4:{fazaRadaWeb=\"Stabilizacija plamena\";break;}\
case 5:{fazaRadaWeb=\"Redovan rad\";break;}\
case 6:{fazaRadaWeb=\"Modulacija\";break;}\
case 7:{fazaRadaWeb=\"Maksimalna temperatura dimnjace\";break;}\
default:{fazaRadaWeb=\"Gorionik ugasen\";break;}\
}\
document.writeln(\"<meta http-equiv=\\\"refresh\\\" content=\\\"15;url=http://\",adresa,\"\\\">\");\
</script>\n\
<html>\
<head>\
<title>Gorionik na pelet</title>\
<body bgcolor=\"#FFFF00\" text=\"black\">\
</head>\
<body>\
<font face=\"Arial, Helvetica, sans-serif\" size=\"4\">\
<h1>GORIONIK NA PELET</h1>\
<table width=\"434\" height=\"72\" border=\"3\">\
<caption><font size=\"6\"><b> FAZA RADA: </b></font></caption>\
<td width=\"420\" height=\"62\" bgcolor=\"#00FF00\"><font size=\"6\"><b><script>document.write(fazaRadaWeb);</script></b></font></td>\
</table>\
<br>\
<table width=\"244\" height=\"56\" border=\"3\">\
<caption><font size=\"4\"><b> TEMPERATURE: </b></font></caption>\
<tr>\
<td width=\"112\" height=\"46\"><font size=\"4\"><b>DIMNJACE</b></font></td>\
<td width=\"112\" height=\"46\"><font size=\"4\"><b>AMBIJENTA</b></font></td>\
</tr>\
<tr>\
<td width=\"112\" height=\"46\" bgcolor=\"#00FF00\"><font size=\"4\"><b><script>document.write(temperaturaDima);</script> oC</b></font></td>\
<td width=\"112\" height=\"46\" bgcolor=\"#00FF00\"><font size=\"4\"><b><script>document.write(temperaturaVode);</script> oC</b></font></td>\
</tr>\
</table>\
<br>\n\
<script language=\"javascript\">\
if (fazaRada<2){document.write(\"<a href=\\\"http://\",adresa,\"/o\\\"><font size=\\\"6\\\">UKLJUCI GORIONIK</font></a><br><br>\");}\
else\
{\
document.write(\"<a href=\\\"http://\",adresa,\"/o\\\"><font size=\\\"6\\\">ISKLJUCI GORIONIK</font></a><br><br>\");\
if (jacinaRada){document.write(\"<a href=\\\"http://\",adresa,\"/a\\\">PREBACI NA AUTOMATSKI RAD</a><br><br>\");}\
else {document.write(\"<a href=\\\"http://\",adresa,\"/m\\\">PREBACI NA MANUELNI RAD</a><br><br>\");}\
}\n\
if (fazaRada>1)\
{\
if (jacinaRada)\
{\
document.write(\"<a href=\\\"http://\",adresa,\"/P1\\\">P1</a>\");\
if (jacinaRada==1){document.write(\"<<<<br>\");}\
else {document.write(\"<br>\");}\
document.write(\"<a href=\\\"http://\",adresa,\"/P2\\\">P2</a>\");\
if (jacinaRada==2){document.write(\"<<<<br>\");}\
else {document.write(\"<br>\");}\
document.write(\"<a href=\\\"http://\",adresa,\"/P3\\\">P3</a>\");\
if (jacinaRada==3) {document.write(\"<<<<br>\");}\
else {document.write(\"<br>\");}\
document.write(\"<a href=\\\"http://\",adresa,\"/P4\\\">P4</a>\");\
if (jacinaRada==4) {document.write(\"<<<<br>\");}\
else {document.write(\"<br>\");}\
document.write(\"<a href=\\\"http://\",adresa,\"/P5\\\">P5</a>\");\
if (jacinaRada==5) {document.write(\"<<<<br>\");}\
else {document.write(\"<br>\");}\
}\
else\
{\
document.write(\"ZADATA TEMPERATURA: \",zadataTempVode,\"oC<br><br>\");\
document.write(\"<form method=\\\"get\\\" action=\\\"http://\",adresa,\"/\\\" name=\\\"zadTemperatura\\\"> <input type=\\\"text\\\" name=\\\"t\\\" size=\\\"10\\\"> <button type=\\\"submint\\\">PODESI ZADATU TEMPERATURU</button></form>\");\
}\
}\
</script>\
</font>\
</body>\
</html>\
";