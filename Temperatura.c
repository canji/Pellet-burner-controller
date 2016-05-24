// program za upravljanje gorionikom na pelete
// definicija globalnih promenljivih i funkcija za merenje temperature

#include "Temperatura.h"
#include "Parametri.h"
#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"

//==============================================================================
// pinovi

sbit TPCS at GPIO_PORTA_DATA6_bit; // chip select za termopar

//==============================================================================
// globalne promenljive

unsigned int Tdimnjace = 0; // temperatura dimnjace
//unsigned int sumaTdimnjace = 0; // prethodni odbirak temperature dima
unsigned short greskaTdimnjace = 0; // suma gresaka temperature dima

unsigned int brojacTD = 0; // brojac intervala od 10 sekundi kada se citaju odbirci temperature dima
unsigned int brojacTA = 0; // brojac intervala od 10 sekundi kada se citaju odbirci temperature ambijenta
unsigned int brojacTV = 0; // brojac odbiraka temperature vode

unsigned int Tambijenta = 0; // temperatura ambijenta
double sumaTvode = 0; // suma odbiraka temperature ambijenta
unsigned short greskaTambijenta = 0; // suma gresaka temperature ambijenta

//==============================================================================
// funkcija za inicijalizaciju mikrokontrolera za rad sa senzorima temperature
void initTemperatura ()
{
  // inicijalizacija za termopar MAX
  GPIO_Digital_Output (&GPIO_PORTA, _GPIO_PINMASK_6); TPCS = 1; // TPCS je izlazni i cip nije selektovan
  // inicijalizacija SPI
  SPI0_Init_Advanced(1000000, _SPI_MASTER, _SPI_16_BIT, &_GPIO_MODULE_SPI0_A245);

  // inicijalizacija pina za one wire
  GPIO_Unlock(_GPIO_COMMIT_PIN_B7);
  GPIO_Digital_Input(&GPIO_PORTB,_GPIO_PINMASK_7);
  
  // inicijalizacija za pt100
  ADC_Set_Input_Channel( _ADC_CHANNEL_8 ); // podesavalje analognog ulaza pin PE3
  ADC1_Init(); // ADC inicijalizacija
}
//==============================================================================
// skupljanje odbiraka, njihovo usrednjavanje i racunanje temperature dimnjace
void temperaturaD ()
{
  const brojOdbiraka = 100; // interval u desetinkama kad se cita temperatura

  unsigned int odbirak = 0; // racunanje odbirka temperature

  // provera da li se koriste sonde za merenje temperature dimnjace
  if ( !parametar[sondaDimnjace] ) { Tdimnjace = 0; return; }

  // provera da li je vreme da se cita temperatura
  if ( brojacTD > brojOdbiraka ) { brojacTD = 0; }
  else { return; }

  // provera da li je senzor neispravan
  if ( greskaTdimnjace > 4 ) { parametar[sondaDimnjace] = 0; greskaTdimnjace = 0; return; } // neispravan senzor i vise se ne koristi

  // citanje temperature
  TPCS = 0; // selktovanje TP
  Delay_us(1);
  odbirak = SPI_Read(0);
  TPCS = 1; // deselektovanje TP

  if ( odbirak & 0x0004 ) { greskaTdimnjace++; return; } // nema senzora

  odbirak >>= 3; // izbacivanje prva tri bita
  odbirak &= 0x0FFF; // informacija je 12-bitna, tako da su najvisa 4 bita nule
  odbirak /= 4; // skaliranje 12-bitne vrednosti na interval 0 do 1024

  if ( (odbirak > parametar[maxTD]+5) || (odbirak == 0) ) { greskaTdimnjace++; return; } // ako je dosla smetnja vrednost bude prevelika
  if ( (Tdimnjace != 0) && ( ((int)(Tdimnjace-odbirak)>50) || ((int)(odbirak-Tdimnjace)>50) ) ) { greskaTdimnjace++; return; } // ako je dosla smetnja i razlika dva uzastopna odbirka bude prevelika

  greskaTdimnjace = 0; // nema neispravnosti

  Tdimnjace = odbirak;
  
  ispisTemperature (Tdimnjace, &TD, TD_Caption, &boksTdima);
}
//==============================================================================
// skupljanje odbiraka, njihovo usrednjavanje i racunanje temperature ambijenta
void temperaturaA ()
{
  const brojOdbiraka = 1000; // broj odbiraka temperatura
  const intervalDesetinke = 50; // svakih 5 sekundi se cita DS18S20
  const otpornost = 4660; // otpornost naponskog razdelnika za ntc
  const R25 = 10000; // otpornost na 25 stepeni celzijusa
  const koeficijentB = 3525; // B koeficijenat
  double naponNapajanja = 3.3; // napon napajanja 3.3V
  
  double naponUlaz = 0; // ulazni napon u voltima
  double Rntc = 0; // otpornost ntc
  unsigned int tempOdbirak = 0; // odbirak za temperaturu
  double tempOdbirak2 = 0; // odbirak za temperaturu

  // provera da li se koristi sonda za merenje temperature ambijenta
  if ( !parametar[sondaAmbijenta] ) { Tambijenta = 0; return; }

  // provera da li je senzor neispravan
  if ( greskaTambijenta > 4 ) { parametar[sondaAmbijenta] = 0; greskaTambijenta = 0; return; } // neispravan senzor i vise se ne koristi

  if ( parametar[sondaAmbijenta] == 1 ) // ako se koristi DS18S20
  {
    // provera da li je vreme da se cita temperatura
    if ( brojacTA > intervalDesetinke ) { brojacTA = 0; }
    else { return; }
  
    // racunanje odbirka temperature ambijenta i njegovo ubacivanje u sumu
    Ow_Reset(&GPIO_PORTB_DATA, 7);                         // Onewire reset signal
    Ow_Write(&GPIO_PORTB_DATA, 7, 0xCC);                   // Issue command SKIP_ROM
    Ow_Write(&GPIO_PORTB_DATA, 7, 0x44);                   // Issue command CONVERT_T

    Ow_Reset(&GPIO_PORTB_DATA, 7);
    Ow_Write(&GPIO_PORTB_DATA, 7, 0xCC);                   // Issue command SKIP_ROM
    Ow_Write(&GPIO_PORTB_DATA, 7, 0xBE);                    // Issue command READ_SCRATCHPAD

    tempOdbirak =  Ow_Read(&GPIO_PORTB_DATA, 7);
    tempOdbirak = (Ow_Read(&GPIO_PORTB_DATA, 7) << 8) + tempOdbirak;
    
    tempOdbirak = tempOdbirak / 2;
    
    if ( (tempOdbirak > parametar[maxTV]+5) || (tempOdbirak == 0) ) { greskaTambijenta++; return; } // ako je dosla smetnja vrednost bude prevelika
    if ( (Tambijenta != 0) && ( ((int)(Tambijenta-tempOdbirak)>60) || ((int)(tempOdbirak-Tambijenta)>60) ) ) { greskaTambijenta++; return; } // ako je dosla smetnja i razlika dva uzastopna odbirka bude prevelika
  
    greskaTambijenta = 0; // nema neispravnosti

    Tambijenta = tempOdbirak;
    
    ispisTemperature (Tambijenta, &TA, TA_Caption, &boksTambijenta);
    
    return;
  }

  if ( parametar[sondaAmbijenta] == 2 ) // ako se koristi ntc 10K
  {
    naponUlaz = 0.00293255 * ADC1_Get_Sample(8); // citanje ulaznog napona

    if ( naponUlaz > 2.9 ) { greskaTambijenta++; return; } // ako je dosla smetnja vrednost bude prevelika

    Rntc = (naponUlaz*otpornost)/(naponNapajanja-naponUlaz); // racunanje ntc otpora

    tempOdbirak2 = 1 / ( ( log(Rntc/R25) / koeficijentB  ) + (1/298.15) ); // temperatura u kelvinima
    
    tempOdbirak2 -= 273.15; // konvertovanje u celzijuse   0

    greskaTambijenta = 0; // nema neispravnosti

    sumaTvode += tempOdbirak2;

    brojacTV++; // inkrementiranje brojaca

    if ( brojacTV == brojOdbiraka )
    {
      // racunanje temperature ambijenta
      Tambijenta = ((unsigned int)( sumaTvode / brojacTV ));
      // resetovanje brojaca i sume
      sumaTvode = 0;
      brojacTV = 0;
      // ispisivanje
      ispisTemperature (Tambijenta, &TA, TA_Caption, &boksTambijenta);
    }
  }
}
//==============================================================================