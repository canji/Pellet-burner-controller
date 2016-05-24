#include "GorionikInterfejs1_objects.h"
#include "GorionikInterfejs1_resources.h"
#include "built_in.h"


// TFT module connections
char TFT_DataPort at GPIO_PORTJ_DATA;
sbit TFT_RST at GPIO_PORTH_DATA5_bit;
sbit TFT_RS at GPIO_PORTG_DATA7_bit;
sbit TFT_CS at GPIO_PORTH_DATA6_bit;
sbit TFT_RD at GPIO_PORTC_DATA5_bit;
sbit TFT_WR at GPIO_PORTH_DATA4_bit;
sbit TFT_BLED at GPIO_PORTA_DATA3_bit;
char TFT_DataPort_Direction at GPIO_PORTJ_DIR;
sbit TFT_RST_Direction at GPIO_PORTH_DIR5_bit;
sbit TFT_RS_Direction at GPIO_PORTG_DIR7_bit;
sbit TFT_CS_Direction at GPIO_PORTH_DIR6_bit;
sbit TFT_RD_Direction at GPIO_PORTC_DIR5_bit;
sbit TFT_WR_Direction at GPIO_PORTH_DIR4_bit;
sbit TFT_BLED_Direction at GPIO_PORTA_DIR3_bit;
// End TFT module connections

// Touch Panel module connections
sbit Drivex_Left at GPIO_PORTB_DATA4_bit;
sbit Drivex_Right at GPIO_PORTE_DATA0_bit;
sbit Drivey_Up at GPIO_PORTE_DATA1_bit;
sbit Drivey_Down at GPIO_PORTB_DATA5_bit;
sbit Drivex_Left_Direction at GPIO_PORTB_DIR4_bit;
sbit Drivex_Right_Direction at GPIO_PORTE_DIR0_bit;
sbit Drivey_Up_Direction at GPIO_PORTE_DIR1_bit;
sbit Drivey_Down_Direction at GPIO_PORTB_DIR5_bit;
// End Touch Panel module connections

// Global variables
unsigned int Xcoord, Ycoord;
const ADC_THRESHOLD = 750;
char PenDown;
typedef unsigned long TPointer;
TPointer PressedObject;
int PressedObjectType;
unsigned int caption_length, caption_height;
unsigned int display_width, display_height;

int _object_count;
unsigned short object_pressed;
TButton_Round *local_round_button;
TButton_Round *exec_round_button;
int round_button_order;
TCButton_Round *local_round_cbutton;
TCButton_Round *exec_round_cbutton;
int round_cbutton_order;
TLabel *local_label;
TLabel *exec_label;
int label_order;
TCLabel *local_clabel;
TCLabel *exec_clabel;
int clabel_order;
TCircle *local_circle;
TCircle *exec_circle;
int circle_order;
TCircleButton *local_circle_button;
TCircleButton *exec_circle_button;
int circle_button_order;
TBox *local_box;
TBox *exec_box;
int box_order;
TCBox *local_cbox;
TCBox *exec_cbox;
int cbox_order;


void Init_ADC() {
  ADC_Set_Input_Channel(_ADC_CHANNEL_10 | _ADC_CHANNEL_11);
  ADC0_Init_Advanced(_ADC_EXTERNAL_REF);
}
static void InitializeTouchPanel() {
  Init_ADC();
  TFT_Init_ILI9341_8bit(320, 240);

  TP_TFT_Init(320, 240, 11, 10);                                  // Initialize touch panel
  TP_TFT_Set_ADC_Threshold(ADC_THRESHOLD);                              // Set touch panel ADC threshold

  PenDown = 0;
  PressedObject = 0;
  PressedObjectType = -1;
}

void Calibrate() {
  TFT_Set_Pen(CL_WHITE, 3);
  TFT_Set_Font(TFT_defaultFont, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("Touch selected corners for calibration", 50, 80);
  TFT_Line(315, 239, 319, 239);
  TFT_Line(309, 229, 319, 239);
  TFT_Line(319, 234, 319, 239);
  TFT_Write_Text("first here",235,220);

  TP_TFT_Calibrate_Min();                      // Calibration of bottom left corner
  Delay_ms(500);

  TFT_Set_Pen(CL_BLACK, 3);
  TFT_Set_Font(TFT_defaultFont, CL_BLACK, FO_HORIZONTAL);
  TFT_Line(315, 239, 319, 239);
  TFT_Line(309, 229, 319, 239);
  TFT_Line(319, 234, 319, 239);
  TFT_Write_Text("first here",235,220);

  TFT_Set_Pen(CL_WHITE, 3);
  TFT_Set_Font(TFT_defaultFont, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("now here ", 20, 5);
  TFT_Line(0, 0, 5, 0);
  TFT_Line(0, 0, 0, 5);
  TFT_Line(0, 0, 10, 10);

  TP_TFT_Calibrate_Max();                      // Calibration of bottom left corner
  Delay_ms(500);
}


/////////////////////////
 TCScreen * const code CScreens[10] =
         {
                  &rad                  ,//   radScreenID = 32768
                  &izbormeni            ,//   izbormeniScreenID = 32769
                  &podesavaj            ,//   podesavajScreenID = 32770
                  &lozinka              ,//   lozinkaScreenID = 32771
                  &podesavaParam        ,//   podesavaParamScreenID = 32772
                  &upuzorenje           ,//   upuzorenjeScreenID = 32773
                  &podesavanjeSat       ,//   podesavanjeSatScreenID = 32774
                  &pozdrav              ,//   pozdravScreenID = 32775
                  &podesavanjHrono      ,//   podesavanjHronoScreenID = 32776
                  &podesavajKom         //   podesavajKomScreenID = 32777
         };

  const radScreenID    = 32768;
  const izbormeniScreenID = 32769;
  const podesavajScreenID = 32770;
  const lozinkaScreenID = 32771;
  const podesavaParamScreenID = 32772;
  const upuzorenjeScreenID = 32773;
  const podesavanjeSatScreenID = 32774;
  const pozdravScreenID = 32775;
  const podesavanjHronoScreenID = 32776;
  const podesavajKomScreenID = 32777;

  unsigned int CurrentScreenID;

  TCScreen                rad=
         {
         0xFFF0                ,//  rad.Color
         320                   ,//  rad.Width                     
         240                   ,//  rad.Height                    
         27                    ,//  rad.ObjectsCount             
         5                     ,//  rad.Buttons_RoundCount              
         Screen1_Buttons_Round ,//  rad.Buttons_Round
         1                     ,//  rad.CButtons_RoundCount              
         Screen1_CButtons_Round,//  rad.CButtons_Round
         6                     ,//  rad.LabelsCount              
         Screen1_Labels        ,//  rad.Labels
         2                     ,//  rad.CLabelsCount              
         Screen1_CLabels       ,//  rad.CLabels
         5                     ,//  rad.CirclesCount              
         Screen1_Circles       ,//  rad.Circles
         3                     ,//  rad.CircleButtonsCount              
         Screen1_CircleButtons ,//  rad.CircleButtons
         3                     ,//  rad.BoxesCount              
         Screen1_Boxes         ,//  rad.Boxes
         2                     ,//  rad.CBoxesCount              
         Screen1_CBoxes        //  rad.CBoxes
         };

  TBox                   boksTzad;
  TBox                   boksTambijenta;
  TBox                   boksTdima;
  TCBox                displej = 
         {
         32768                 , //   displej.OwnerScreenID
         3                     , //   displej.Order           
         0                     , //   displej.Left           
         48                    , //   displej.Top             
         320                   , //   displej.Width           
         50                    , //   displej.Height          
         1                     , //   displej.Pen_Width       
         0x0000                , //   displej.Pen_Color       
         1                     , //   displej.Visible         
         1                     , //   displej.Active          
         1                     , //   displej.Transparent     
         0                     , //   displej.Gradient        
         0                     , //   displej.Gradient_Orientation    
         0xFFFF                , //   displej.Gradient_Start_Color    
         0xC618                , //   displej.Gradient_End_Color      
         0x07E8                , //   displej.Color           
         0                     , //   displej.PressColEnabled 
         0x8410                , //   displej.Press_Color     
         0                     , //   displej.OnUpPtr
         0                     , //   displej.OnDownPtr
         0                     , //   displej.OnClickPtr
         0                       //   displej.OnPressPtr
         };
  TCBox                sat = 
         {
         32768                 , //   sat.OwnerScreenID
         4                     , //   sat.Order           
         154                   , //   sat.Left           
         5                     , //   sat.Top             
         162                   , //   sat.Width           
         36                    , //   sat.Height          
         1                     , //   sat.Pen_Width       
         0x0000                , //   sat.Pen_Color       
         1                     , //   sat.Visible         
         1                     , //   sat.Active          
         1                     , //   sat.Transparent     
         0                     , //   sat.Gradient        
         0                     , //   sat.Gradient_Orientation    
         0xFFFF                , //   sat.Gradient_Start_Color    
         0xC618                , //   sat.Gradient_End_Color      
         0x001F                , //   sat.Color           
         0                     , //   sat.PressColEnabled 
         0x8410                , //   sat.Press_Color     
         0                     , //   sat.OnUpPtr
         0                     , //   sat.OnDownPtr
         0                     , //   sat.OnClickPtr
         0                       //   sat.OnPressPtr
         };
  TCButton_Round          meni = 
         {
         32768                 , //   meni.OwnerScreenID
         5                     , //   meni.Order           
         3                     , //   meni.Left            
         4                     , //   meni.Top              
         146                   , //   meni.Width            
         39                    , //   meni.Height        
         1                     , //   meni.Pen_Width          
         0x0000                , //   meni.Pen_Color       
         1                     , //   meni.Visible         
         1                     , //   meni.Active           
         1                     , //   meni.Transparent     
         &meni_Caption         , //   meni.Caption        
         _taCenter             , //   meni.TextAlign        
         &Arial16x19_Bold      , //   meni.FontName        
         0x0000                , //
         1                     , //   meni.Gradient        
         0                     , //   meni.Gradient_Orientation    
         0xFFFF                , //   meni.Gradient_Start_Color    
         0xC618                , //  meni.Gradient_Start_Color    
         0xC618                , //   meni.Color           
         3                     , //   meni.CornerRadius           
         1                     , //   meni.PressColEnabled 
         0x8410                , //   meni.Press_Color     
         0                     , //   meni.OnUpPtr
         0                     , //   meni.OnDownPtr
         ulazMeni              , //   meni.OnClickPtr
         0                       //   meni.OnPressPtr
         };
const char meni_Caption[5] = "MENI";

  TLabel                 fazaRada;
char fazaRada_Caption[21] = "GORIONIK UGASEN";

  TLabel                 vreme;
char vreme_Caption[21] = "1-JAN-1970.  00:00";

  TCircleButton          minus;
char minus_Caption[2] = "-";

  TCircleButton          plus;
char plus_Caption[2] = "+";

  TLabel                 zadataT;
char zadataT_Caption[10] = "ZADATA T:";

  TCLabel          TdimaD = 
         {
         32768                 , //   TdimaD.OwnerScreenID
         11                    , //   TdimaD.Order          
         3                     , //   TdimaD.Left           
         174                   , //   TdimaD.Top             
         67                    , //   TdimaD.Width         
         20                    , //   TdimaD.Height       
         &TdimaD_Caption       , //   TdimaD.Caption        
         &Arial16x19_Bold      , //   TdimaD.FontName       
         0x0000                , //   TdimaD.Font_Color     
         1                     , //   TdimaD.Visible        
         1                     , //   TdimaD.Active        
         0                     , //   TdimaD.OnUpPtr
         0                     , //   TdimaD.OnDownPtr
         0                     , //   TdimaD.OnClickPtr
         0                       //   TdimaD.OnPressPtr
         };
const char TdimaD_Caption[9] = "T  DIMA:";

  TCLabel          TambijentaD = 
         {
         32768                 , //   TambijentaD.OwnerScreenID
         12                    , //   TambijentaD.Order          
         97                    , //   TambijentaD.Left           
         174                   , //   TambijentaD.Top             
         121                   , //   TambijentaD.Width         
         20                    , //   TambijentaD.Height       
         &TambijentaD_Caption  , //   TambijentaD.Caption        
         &Arial16x19_Bold      , //   TambijentaD.FontName       
         0x0000                , //   TambijentaD.Font_Color     
         1                     , //   TambijentaD.Visible        
         1                     , //   TambijentaD.Active        
         0                     , //   TambijentaD.OnUpPtr
         0                     , //   TambijentaD.OnDownPtr
         0                     , //   TambijentaD.OnClickPtr
         0                       //   TambijentaD.OnPressPtr
         };
const char TambijentaD_Caption[14] = "T  AMBIJENTA:";

  TCircleButton          onOff;
char onOff_Caption[4] = "ON";

  TButton_Round          tasterP1;
char tasterP1_Caption[3] = "P1";

  TButton_Round          tasterP2;
char tasterP2_Caption[3] = "P2";

  TButton_Round          tasterP3;
char tasterP3_Caption[3] = "P3";

  TButton_Round          tasterP4;
char tasterP4_Caption[3] = "P4";

  TButton_Round          tasterP5;
char tasterP5_Caption[3] = "P5";

  TCircle                krugP1;
  TCircle                krugP2;
  TCircle                krugP3;
  TCircle                krugP4;
  TCircle                krugP5;
  TLabel                 TD;
char TD_Caption[8] = "180 0C";

  TLabel                 TA;
char TA_Caption[8] = "80 0C";

  TLabel                 ZTA;
char ZTA_Caption[8] = "80 0C";

  TButton_Round          * const code Screen1_Buttons_Round[5]=
         {
         &tasterP1,            
         &tasterP2,            
         &tasterP3,            
         &tasterP4,            
         &tasterP5             
         };
  TCButton_Round          * const code Screen1_CButtons_Round[1]=
         {
         &meni                 
         };
  TLabel                 * const code Screen1_Labels[6]=
         {
         &fazaRada,            
         &vreme,               
         &zadataT,             
         &TD,                  
         &TA,                  
         &ZTA                  
         };
  TCLabel                 * const code Screen1_CLabels[2]=
         {
         &TdimaD,              
         &TambijentaD          
         };
  TCircle                * const code Screen1_Circles[5]=
         {
         &krugP1,              
         &krugP2,              
         &krugP3,              
         &krugP4,              
         &krugP5               
         };
  TCircleButton          * const code Screen1_CircleButtons[3]=
         {
         &minus,               
         &plus,                
         &onOff                
         };
  TBox                   * const code Screen1_Boxes[3]=
         {
         &boksTzad,            
         &boksTambijenta,      
         &boksTdima            
         };
  TCBox                   * const code Screen1_CBoxes[2]=
         {
         &displej,             
         &sat                  
         };


  TCScreen                izbormeni=
         {
         0xFFF0                ,//  izbormeni.Color
         320                   ,//  izbormeni.Width                     
         240                   ,//  izbormeni.Height                    
         5                     ,//  izbormeni.ObjectsCount             
         0                     ,//  izbormeni.Buttons_RoundCount              
         0                     ,//  izbormeni.Buttons_Round
         4                     ,//  izbormeni.CButtons_RoundCount              
         Screen2_CButtons_Round,//  izbormeni.CButtons_Round
         0                     ,//  izbormeni.LabelsCount              
         0                     ,//  izbormeni.Labels
         0                     ,//  izbormeni.CLabelsCount              
         0                     ,//  izbormeni.CLabels
         0                     ,//  izbormeni.CirclesCount              
         0                     ,//  izbormeni.Circles
         1                     ,//  izbormeni.CircleButtonsCount              
         Screen2_CircleButtons ,//  izbormeni.CircleButtons
         0                     ,//  izbormeni.BoxesCount              
         0                     ,//  izbormeni.Boxes
         0                     ,//  izbormeni.CBoxesCount              
         0                     //  izbormeni.CBoxes
         };

  TCButton_Round          autoRad = 
         {
         32769                 , //   autoRad.OwnerScreenID
         0                     , //   autoRad.Order           
         12                    , //   autoRad.Left            
         11                    , //   autoRad.Top              
         250                   , //   autoRad.Width            
         40                    , //   autoRad.Height        
         1                     , //   autoRad.Pen_Width          
         0x0000                , //   autoRad.Pen_Color       
         1                     , //   autoRad.Visible         
         1                     , //   autoRad.Active           
         1                     , //   autoRad.Transparent     
         &autoRad_Caption      , //   autoRad.Caption        
         _taCenter             , //   autoRad.TextAlign        
         &Arial19x22_Bold      , //   autoRad.FontName        
         0x0000                , //
         1                     , //   autoRad.Gradient        
         0                     , //   autoRad.Gradient_Orientation    
         0xFFFF                , //   autoRad.Gradient_Start_Color    
         0xC618                , //  autoRad.Gradient_Start_Color    
         0xC618                , //   autoRad.Color           
         3                     , //   autoRad.CornerRadius           
         1                     , //   autoRad.PressColEnabled 
         0x8410                , //   autoRad.Press_Color     
         0                     , //   autoRad.OnUpPtr
         0                     , //   autoRad.OnDownPtr
         radAuto               , //   autoRad.OnClickPtr
         0                       //   autoRad.OnPressPtr
         };
const char autoRad_Caption[15] = "AUTOMATSKI RAD";

  TCButton_Round          podesavanje = 
         {
         32769                 , //   podesavanje.OwnerScreenID
         1                     , //   podesavanje.Order           
         12                    , //   podesavanje.Left            
         125                   , //   podesavanje.Top              
         250                   , //   podesavanje.Width            
         40                    , //   podesavanje.Height        
         1                     , //   podesavanje.Pen_Width          
         0x0000                , //   podesavanje.Pen_Color       
         1                     , //   podesavanje.Visible         
         1                     , //   podesavanje.Active           
         1                     , //   podesavanje.Transparent     
         &podesavanje_Caption  , //   podesavanje.Caption        
         _taCenter             , //   podesavanje.TextAlign        
         &Arial19x22_Bold      , //   podesavanje.FontName        
         0x0000                , //
         1                     , //   podesavanje.Gradient        
         0                     , //   podesavanje.Gradient_Orientation    
         0xFFFF                , //   podesavanje.Gradient_Start_Color    
         0xC618                , //  podesavanje.Gradient_Start_Color    
         0xC618                , //   podesavanje.Color           
         3                     , //   podesavanje.CornerRadius           
         1                     , //   podesavanje.PressColEnabled 
         0x8410                , //   podesavanje.Press_Color     
         0                     , //   podesavanje.OnUpPtr
         0                     , //   podesavanje.OnDownPtr
         podUlaz               , //   podesavanje.OnClickPtr
         0                       //   podesavanje.OnPressPtr
         };
const char podesavanje_Caption[12] = "PODESAVANJE";

  TCButton_Round          hronometar = 
         {
         32769                 , //   hronometar.OwnerScreenID
         2                     , //   hronometar.Order           
         12                    , //   hronometar.Left            
         181                   , //   hronometar.Top              
         250                   , //   hronometar.Width            
         40                    , //   hronometar.Height        
         1                     , //   hronometar.Pen_Width          
         0x0000                , //   hronometar.Pen_Color       
         1                     , //   hronometar.Visible         
         1                     , //   hronometar.Active           
         1                     , //   hronometar.Transparent     
         &hronometar_Caption   , //   hronometar.Caption        
         _taCenter             , //   hronometar.TextAlign        
         &Arial19x22_Bold      , //   hronometar.FontName        
         0x0000                , //
         1                     , //   hronometar.Gradient        
         0                     , //   hronometar.Gradient_Orientation    
         0xFFFF                , //   hronometar.Gradient_Start_Color    
         0xC618                , //  hronometar.Gradient_Start_Color    
         0xC618                , //   hronometar.Color           
         3                     , //   hronometar.CornerRadius           
         1                     , //   hronometar.PressColEnabled 
         0x8410                , //   hronometar.Press_Color     
         0                     , //   hronometar.OnUpPtr
         0                     , //   hronometar.OnDownPtr
         hrono                 , //   hronometar.OnClickPtr
         0                       //   hronometar.OnPressPtr
         };
const char hronometar_Caption[15] = "HRONOTERMOSTAT";

  TCircleButton          izlaz;
char izlaz_Caption[2] = "X";

  TCButton_Round          manRad = 
         {
         32769                 , //   manRad.OwnerScreenID
         4                     , //   manRad.Order           
         12                    , //   manRad.Left            
         68                    , //   manRad.Top              
         250                   , //   manRad.Width            
         40                    , //   manRad.Height        
         1                     , //   manRad.Pen_Width          
         0x0000                , //   manRad.Pen_Color       
         1                     , //   manRad.Visible         
         1                     , //   manRad.Active           
         1                     , //   manRad.Transparent     
         &manRad_Caption       , //   manRad.Caption        
         _taCenter             , //   manRad.TextAlign        
         &Arial19x22_Bold      , //   manRad.FontName        
         0x0000                , //
         1                     , //   manRad.Gradient        
         0                     , //   manRad.Gradient_Orientation    
         0xFFFF                , //   manRad.Gradient_Start_Color    
         0xC618                , //  manRad.Gradient_Start_Color    
         0xC618                , //   manRad.Color           
         3                     , //   manRad.CornerRadius           
         1                     , //   manRad.PressColEnabled 
         0x8410                , //   manRad.Press_Color     
         0                     , //   manRad.OnUpPtr
         0                     , //   manRad.OnDownPtr
         radManf               , //   manRad.OnClickPtr
         0                       //   manRad.OnPressPtr
         };
const char manRad_Caption[13] = "MANUELNI RAD";

  TCButton_Round          * const code Screen2_CButtons_Round[4]=
         {
         &autoRad,             
         &podesavanje,         
         &hronometar,          
         &manRad               
         };
  TCircleButton          * const code Screen2_CircleButtons[1]=
         {
         &izlaz                
         };


  TCScreen                podesavaj=
         {
         0xFFF0                ,//  podesavaj.Color
         320                   ,//  podesavaj.Width                     
         240                   ,//  podesavaj.Height                    
         5                     ,//  podesavaj.ObjectsCount             
         4                     ,//  podesavaj.Buttons_RoundCount              
         Screen3_Buttons_Round ,//  podesavaj.Buttons_Round
         0                     ,//  podesavaj.CButtons_RoundCount              
         0                     ,//  podesavaj.CButtons_Round
         0                     ,//  podesavaj.LabelsCount              
         0                     ,//  podesavaj.Labels
         0                     ,//  podesavaj.CLabelsCount              
         0                     ,//  podesavaj.CLabels
         0                     ,//  podesavaj.CirclesCount              
         0                     ,//  podesavaj.Circles
         1                     ,//  podesavaj.CircleButtonsCount              
         Screen3_CircleButtons ,//  podesavaj.CircleButtons
         0                     ,//  podesavaj.BoxesCount              
         0                     ,//  podesavaj.Boxes
         0                     ,//  podesavaj.CBoxesCount              
         0                     //  podesavaj.CBoxes
         };

  TButton_Round          datumIvreme;
char datumIvreme_Caption[14] = "DATUM I VREME";

  TButton_Round          kalibracija;
char kalibracija_Caption[21] = "KALIBRACIJA DISPLEJA";

  TCircleButton          CircleButton1;
char CircleButton1_Caption[2] = "X";

  TButton_Round          tKomunikacija;
char tKomunikacija_Caption[13] = "TCP/IP I GSM";

  TButton_Round          parametri;
char parametri_Caption[10] = "PARAMETRI";

  TButton_Round          * const code Screen3_Buttons_Round[4]=
         {
         &datumIvreme,         
         &kalibracija,         
         &tKomunikacija,       
         &parametri            
         };
  TCircleButton          * const code Screen3_CircleButtons[1]=
         {
         &CircleButton1        
         };


  TCScreen                lozinka=
         {
         0xFFF0                ,//  lozinka.Color
         320                   ,//  lozinka.Width                     
         240                   ,//  lozinka.Height                    
         15                    ,//  lozinka.ObjectsCount             
         0                     ,//  lozinka.Buttons_RoundCount              
         0                     ,//  lozinka.Buttons_Round
         9                     ,//  lozinka.CButtons_RoundCount              
         Screen4_CButtons_Round,//  lozinka.CButtons_Round
         0                     ,//  lozinka.LabelsCount              
         0                     ,//  lozinka.Labels
         0                     ,//  lozinka.CLabelsCount              
         0                     ,//  lozinka.CLabels
         4                     ,//  lozinka.CirclesCount              
         Screen4_Circles       ,//  lozinka.Circles
         1                     ,//  lozinka.CircleButtonsCount              
         Screen4_CircleButtons ,//  lozinka.CircleButtons
         0                     ,//  lozinka.BoxesCount              
         0                     ,//  lozinka.Boxes
         1                     ,//  lozinka.CBoxesCount              
         Screen4_CBoxes        //  lozinka.CBoxes
         };

  TCBox                disLoz = 
         {
         32771                 , //   disLoz.OwnerScreenID
         0                     , //   disLoz.Order           
         13                    , //   disLoz.Left           
         9                     , //   disLoz.Top             
         245                   , //   disLoz.Width           
         44                    , //   disLoz.Height          
         1                     , //   disLoz.Pen_Width       
         0x0000                , //   disLoz.Pen_Color       
         1                     , //   disLoz.Visible         
         1                     , //   disLoz.Active          
         1                     , //   disLoz.Transparent     
         0                     , //   disLoz.Gradient        
         0                     , //   disLoz.Gradient_Orientation    
         0xFFFF                , //   disLoz.Gradient_Start_Color    
         0xC618                , //   disLoz.Gradient_End_Color      
         0x07E8                , //   disLoz.Color           
         0                     , //   disLoz.PressColEnabled 
         0x8410                , //   disLoz.Press_Color     
         0                     , //   disLoz.OnUpPtr
         0                     , //   disLoz.OnDownPtr
         0                     , //   disLoz.OnClickPtr
         0                       //   disLoz.OnPressPtr
         };
  TCircle                sh1;
  TCircle                sh2;
  TCircle                sh3;
  TCircle                sh4;
  TCButton_Round          ButtonRound1 = 
         {
         32771                 , //   ButtonRound1.OwnerScreenID
         5                     , //   ButtonRound1.Order           
         15                    , //   ButtonRound1.Left            
         60                    , //   ButtonRound1.Top              
         70                    , //   ButtonRound1.Width            
         50                    , //   ButtonRound1.Height        
         1                     , //   ButtonRound1.Pen_Width          
         0x0000                , //   ButtonRound1.Pen_Color       
         1                     , //   ButtonRound1.Visible         
         1                     , //   ButtonRound1.Active           
         1                     , //   ButtonRound1.Transparent     
         &ButtonRound1_Caption , //   ButtonRound1.Caption        
         _taCenter             , //   ButtonRound1.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound1.FontName        
         0x0000                , //
         1                     , //   ButtonRound1.Gradient        
         0                     , //   ButtonRound1.Gradient_Orientation    
         0xFFFF                , //   ButtonRound1.Gradient_Start_Color    
         0xC618                , //  ButtonRound1.Gradient_Start_Color    
         0xC618                , //   ButtonRound1.Color           
         3                     , //   ButtonRound1.CornerRadius           
         1                     , //   ButtonRound1.PressColEnabled 
         0x8410                , //   ButtonRound1.Press_Color     
         0                     , //   ButtonRound1.OnUpPtr
         0                     , //   ButtonRound1.OnDownPtr
         cifra1                , //   ButtonRound1.OnClickPtr
         0                       //   ButtonRound1.OnPressPtr
         };
const char ButtonRound1_Caption[2] = "1";

  TCButton_Round          ButtonRound2 = 
         {
         32771                 , //   ButtonRound2.OwnerScreenID
         6                     , //   ButtonRound2.Order           
         235                   , //   ButtonRound2.Left            
         60                    , //   ButtonRound2.Top              
         70                    , //   ButtonRound2.Width            
         50                    , //   ButtonRound2.Height        
         1                     , //   ButtonRound2.Pen_Width          
         0x0000                , //   ButtonRound2.Pen_Color       
         1                     , //   ButtonRound2.Visible         
         1                     , //   ButtonRound2.Active           
         1                     , //   ButtonRound2.Transparent     
         &ButtonRound2_Caption , //   ButtonRound2.Caption        
         _taCenter             , //   ButtonRound2.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound2.FontName        
         0x0000                , //
         1                     , //   ButtonRound2.Gradient        
         0                     , //   ButtonRound2.Gradient_Orientation    
         0xFFFF                , //   ButtonRound2.Gradient_Start_Color    
         0xC618                , //  ButtonRound2.Gradient_Start_Color    
         0xC618                , //   ButtonRound2.Color           
         3                     , //   ButtonRound2.CornerRadius           
         1                     , //   ButtonRound2.PressColEnabled 
         0x8410                , //   ButtonRound2.Press_Color     
         0                     , //   ButtonRound2.OnUpPtr
         0                     , //   ButtonRound2.OnDownPtr
         cifra3                , //   ButtonRound2.OnClickPtr
         0                       //   ButtonRound2.OnPressPtr
         };
const char ButtonRound2_Caption[2] = "3";

  TCButton_Round          ButtonRound3 = 
         {
         32771                 , //   ButtonRound3.OwnerScreenID
         7                     , //   ButtonRound3.Order           
         130                   , //   ButtonRound3.Left            
         60                    , //   ButtonRound3.Top              
         70                    , //   ButtonRound3.Width            
         50                    , //   ButtonRound3.Height        
         1                     , //   ButtonRound3.Pen_Width          
         0x0000                , //   ButtonRound3.Pen_Color       
         1                     , //   ButtonRound3.Visible         
         1                     , //   ButtonRound3.Active           
         1                     , //   ButtonRound3.Transparent     
         &ButtonRound3_Caption , //   ButtonRound3.Caption        
         _taCenter             , //   ButtonRound3.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound3.FontName        
         0x0000                , //
         1                     , //   ButtonRound3.Gradient        
         0                     , //   ButtonRound3.Gradient_Orientation    
         0xFFFF                , //   ButtonRound3.Gradient_Start_Color    
         0xC618                , //  ButtonRound3.Gradient_Start_Color    
         0xC618                , //   ButtonRound3.Color           
         3                     , //   ButtonRound3.CornerRadius           
         1                     , //   ButtonRound3.PressColEnabled 
         0x8410                , //   ButtonRound3.Press_Color     
         0                     , //   ButtonRound3.OnUpPtr
         0                     , //   ButtonRound3.OnDownPtr
         cifra2                , //   ButtonRound3.OnClickPtr
         0                       //   ButtonRound3.OnPressPtr
         };
const char ButtonRound3_Caption[2] = "2";

  TCButton_Round          ButtonRound5 = 
         {
         32771                 , //   ButtonRound5.OwnerScreenID
         8                     , //   ButtonRound5.Order           
         15                    , //   ButtonRound5.Left            
         120                   , //   ButtonRound5.Top              
         70                    , //   ButtonRound5.Width            
         50                    , //   ButtonRound5.Height        
         1                     , //   ButtonRound5.Pen_Width          
         0x0000                , //   ButtonRound5.Pen_Color       
         1                     , //   ButtonRound5.Visible         
         1                     , //   ButtonRound5.Active           
         1                     , //   ButtonRound5.Transparent     
         &ButtonRound5_Caption , //   ButtonRound5.Caption        
         _taCenter             , //   ButtonRound5.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound5.FontName        
         0x0000                , //
         1                     , //   ButtonRound5.Gradient        
         0                     , //   ButtonRound5.Gradient_Orientation    
         0xFFFF                , //   ButtonRound5.Gradient_Start_Color    
         0xC618                , //  ButtonRound5.Gradient_Start_Color    
         0xC618                , //   ButtonRound5.Color           
         3                     , //   ButtonRound5.CornerRadius           
         1                     , //   ButtonRound5.PressColEnabled 
         0x8410                , //   ButtonRound5.Press_Color     
         0                     , //   ButtonRound5.OnUpPtr
         0                     , //   ButtonRound5.OnDownPtr
         cifra4                , //   ButtonRound5.OnClickPtr
         0                       //   ButtonRound5.OnPressPtr
         };
const char ButtonRound5_Caption[2] = "4";

  TCButton_Round          ButtonRound6 = 
         {
         32771                 , //   ButtonRound6.OwnerScreenID
         9                     , //   ButtonRound6.Order           
         130                   , //   ButtonRound6.Left            
         120                   , //   ButtonRound6.Top              
         70                    , //   ButtonRound6.Width            
         50                    , //   ButtonRound6.Height        
         1                     , //   ButtonRound6.Pen_Width          
         0x0000                , //   ButtonRound6.Pen_Color       
         1                     , //   ButtonRound6.Visible         
         1                     , //   ButtonRound6.Active           
         1                     , //   ButtonRound6.Transparent     
         &ButtonRound6_Caption , //   ButtonRound6.Caption        
         _taCenter             , //   ButtonRound6.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound6.FontName        
         0x0000                , //
         1                     , //   ButtonRound6.Gradient        
         0                     , //   ButtonRound6.Gradient_Orientation    
         0xFFFF                , //   ButtonRound6.Gradient_Start_Color    
         0xC618                , //  ButtonRound6.Gradient_Start_Color    
         0xC618                , //   ButtonRound6.Color           
         3                     , //   ButtonRound6.CornerRadius           
         1                     , //   ButtonRound6.PressColEnabled 
         0x8410                , //   ButtonRound6.Press_Color     
         0                     , //   ButtonRound6.OnUpPtr
         0                     , //   ButtonRound6.OnDownPtr
         cifra5                , //   ButtonRound6.OnClickPtr
         0                       //   ButtonRound6.OnPressPtr
         };
const char ButtonRound6_Caption[2] = "5";

  TCButton_Round          ButtonRound7 = 
         {
         32771                 , //   ButtonRound7.OwnerScreenID
         10                    , //   ButtonRound7.Order           
         235                   , //   ButtonRound7.Left            
         120                   , //   ButtonRound7.Top              
         70                    , //   ButtonRound7.Width            
         50                    , //   ButtonRound7.Height        
         1                     , //   ButtonRound7.Pen_Width          
         0x0000                , //   ButtonRound7.Pen_Color       
         1                     , //   ButtonRound7.Visible         
         1                     , //   ButtonRound7.Active           
         1                     , //   ButtonRound7.Transparent     
         &ButtonRound7_Caption , //   ButtonRound7.Caption        
         _taCenter             , //   ButtonRound7.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound7.FontName        
         0x0000                , //
         1                     , //   ButtonRound7.Gradient        
         0                     , //   ButtonRound7.Gradient_Orientation    
         0xFFFF                , //   ButtonRound7.Gradient_Start_Color    
         0xC618                , //  ButtonRound7.Gradient_Start_Color    
         0xC618                , //   ButtonRound7.Color           
         3                     , //   ButtonRound7.CornerRadius           
         1                     , //   ButtonRound7.PressColEnabled 
         0x8410                , //   ButtonRound7.Press_Color     
         0                     , //   ButtonRound7.OnUpPtr
         0                     , //   ButtonRound7.OnDownPtr
         cifra6                , //   ButtonRound7.OnClickPtr
         0                       //   ButtonRound7.OnPressPtr
         };
const char ButtonRound7_Caption[2] = "6";

  TCButton_Round          ButtonRound8 = 
         {
         32771                 , //   ButtonRound8.OwnerScreenID
         11                    , //   ButtonRound8.Order           
         15                    , //   ButtonRound8.Left            
         180                   , //   ButtonRound8.Top              
         70                    , //   ButtonRound8.Width            
         50                    , //   ButtonRound8.Height        
         1                     , //   ButtonRound8.Pen_Width          
         0x0000                , //   ButtonRound8.Pen_Color       
         1                     , //   ButtonRound8.Visible         
         1                     , //   ButtonRound8.Active           
         1                     , //   ButtonRound8.Transparent     
         &ButtonRound8_Caption , //   ButtonRound8.Caption        
         _taCenter             , //   ButtonRound8.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound8.FontName        
         0x0000                , //
         1                     , //   ButtonRound8.Gradient        
         0                     , //   ButtonRound8.Gradient_Orientation    
         0xFFFF                , //   ButtonRound8.Gradient_Start_Color    
         0xC618                , //  ButtonRound8.Gradient_Start_Color    
         0xC618                , //   ButtonRound8.Color           
         3                     , //   ButtonRound8.CornerRadius           
         1                     , //   ButtonRound8.PressColEnabled 
         0x8410                , //   ButtonRound8.Press_Color     
         0                     , //   ButtonRound8.OnUpPtr
         0                     , //   ButtonRound8.OnDownPtr
         cifra7                , //   ButtonRound8.OnClickPtr
         0                       //   ButtonRound8.OnPressPtr
         };
const char ButtonRound8_Caption[2] = "7";

  TCButton_Round          ButtonRound9 = 
         {
         32771                 , //   ButtonRound9.OwnerScreenID
         12                    , //   ButtonRound9.Order           
         130                   , //   ButtonRound9.Left            
         180                   , //   ButtonRound9.Top              
         70                    , //   ButtonRound9.Width            
         50                    , //   ButtonRound9.Height        
         1                     , //   ButtonRound9.Pen_Width          
         0x0000                , //   ButtonRound9.Pen_Color       
         1                     , //   ButtonRound9.Visible         
         1                     , //   ButtonRound9.Active           
         1                     , //   ButtonRound9.Transparent     
         &ButtonRound9_Caption , //   ButtonRound9.Caption        
         _taCenter             , //   ButtonRound9.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound9.FontName        
         0x0000                , //
         1                     , //   ButtonRound9.Gradient        
         0                     , //   ButtonRound9.Gradient_Orientation    
         0xFFFF                , //   ButtonRound9.Gradient_Start_Color    
         0xC618                , //  ButtonRound9.Gradient_Start_Color    
         0xC618                , //   ButtonRound9.Color           
         3                     , //   ButtonRound9.CornerRadius           
         1                     , //   ButtonRound9.PressColEnabled 
         0x8410                , //   ButtonRound9.Press_Color     
         0                     , //   ButtonRound9.OnUpPtr
         0                     , //   ButtonRound9.OnDownPtr
         cifra8                , //   ButtonRound9.OnClickPtr
         0                       //   ButtonRound9.OnPressPtr
         };
const char ButtonRound9_Caption[2] = "8";

  TCButton_Round          ButtonRound10 = 
         {
         32771                 , //   ButtonRound10.OwnerScreenID
         13                    , //   ButtonRound10.Order           
         235                   , //   ButtonRound10.Left            
         180                   , //   ButtonRound10.Top              
         70                    , //   ButtonRound10.Width            
         50                    , //   ButtonRound10.Height        
         1                     , //   ButtonRound10.Pen_Width          
         0x0000                , //   ButtonRound10.Pen_Color       
         1                     , //   ButtonRound10.Visible         
         1                     , //   ButtonRound10.Active           
         1                     , //   ButtonRound10.Transparent     
         &ButtonRound10_Caption, //   ButtonRound10.Caption        
         _taCenter             , //   ButtonRound10.TextAlign        
         &Arial19x22_Bold      , //   ButtonRound10.FontName        
         0x0000                , //
         1                     , //   ButtonRound10.Gradient        
         0                     , //   ButtonRound10.Gradient_Orientation    
         0xFFFF                , //   ButtonRound10.Gradient_Start_Color    
         0xC618                , //  ButtonRound10.Gradient_Start_Color    
         0xC618                , //   ButtonRound10.Color           
         3                     , //   ButtonRound10.CornerRadius           
         1                     , //   ButtonRound10.PressColEnabled 
         0x8410                , //   ButtonRound10.Press_Color     
         0                     , //   ButtonRound10.OnUpPtr
         0                     , //   ButtonRound10.OnDownPtr
         cifra9                , //   ButtonRound10.OnClickPtr
         0                       //   ButtonRound10.OnPressPtr
         };
const char ButtonRound10_Caption[2] = "9";

  TCircleButton          lozExit;
char lozExit_Caption[2] = "X";

  TCButton_Round          * const code Screen4_CButtons_Round[9]=
         {
         &ButtonRound1,        
         &ButtonRound2,        
         &ButtonRound3,        
         &ButtonRound5,        
         &ButtonRound6,        
         &ButtonRound7,        
         &ButtonRound8,        
         &ButtonRound9,        
         &ButtonRound10        
         };
  TCircle                * const code Screen4_Circles[4]=
         {
         &sh1,                 
         &sh2,                 
         &sh3,                 
         &sh4                  
         };
  TCircleButton          * const code Screen4_CircleButtons[1]=
         {
         &lozExit              
         };
  TCBox                   * const code Screen4_CBoxes[1]=
         {
         &disLoz               
         };


  TCScreen                podesavaParam=
         {
         0xFFF0                ,//  podesavaParam.Color
         320                   ,//  podesavaParam.Width                     
         240                   ,//  podesavaParam.Height                    
         11                    ,//  podesavaParam.ObjectsCount             
         1                     ,//  podesavaParam.Buttons_RoundCount              
         Screen5_Buttons_Round ,//  podesavaParam.Buttons_Round
         0                     ,//  podesavaParam.CButtons_RoundCount              
         0                     ,//  podesavaParam.CButtons_Round
         2                     ,//  podesavaParam.LabelsCount              
         Screen5_Labels        ,//  podesavaParam.Labels
         1                     ,//  podesavaParam.CLabelsCount              
         Screen5_CLabels       ,//  podesavaParam.CLabels
         0                     ,//  podesavaParam.CirclesCount              
         0                     ,//  podesavaParam.Circles
         5                     ,//  podesavaParam.CircleButtonsCount              
         Screen5_CircleButtons ,//  podesavaParam.CircleButtons
         2                     ,//  podesavaParam.BoxesCount              
         Screen5_Boxes         ,//  podesavaParam.Boxes
         0                     ,//  podesavaParam.CBoxesCount              
         0                     //  podesavaParam.CBoxes
         };

  TBox                   boksBrojParam;
  TBox                   boksVrednostParam;
  TCircleButton          paramMinus;
char paramMinus_Caption[2] = "-";

  TCircleButton          paramPlus;
char paramPlus_Caption[2] = "+";

  TCircleButton          paramNazad;
char paramNazad_Caption[3] = "<<";

  TCircleButton          paramNapred;
char paramNapred_Caption[3] = ">>";

  TCircleButton          parExit;
char parExit_Caption[2] = "X";

  TButton_Round          paramTest;
char paramTest_Caption[5] = "TEST";

  TCLabel          labelaP = 
         {
         32772                 , //   labelaP.OwnerScreenID
         8                     , //   labelaP.Order          
         118                   , //   labelaP.Left           
         38                    , //   labelaP.Top             
         14                    , //   labelaP.Width         
         26                    , //   labelaP.Height       
         &labelaP_Caption      , //   labelaP.Caption        
         &Arial21x24_Bold      , //   labelaP.FontName       
         0x6000                , //   labelaP.Font_Color     
         1                     , //   labelaP.Visible        
         1                     , //   labelaP.Active        
         0                     , //   labelaP.OnUpPtr
         0                     , //   labelaP.OnDownPtr
         0                     , //   labelaP.OnClickPtr
         0                       //   labelaP.OnPressPtr
         };
const char labelaP_Caption[21] = "P";

  TLabel                 brojParam;
char brojParam_Caption[4] = "23";

  TLabel                 paramVrednost;
char paramVrednost_Caption[11] = "12345 min";

  TButton_Round          * const code Screen5_Buttons_Round[1]=
         {
         &paramTest            
         };
  TLabel                 * const code Screen5_Labels[2]=
         {
         &brojParam,           
         &paramVrednost        
         };
  TCLabel                 * const code Screen5_CLabels[1]=
         {
         &labelaP              
         };
  TCircleButton          * const code Screen5_CircleButtons[5]=
         {
         &paramMinus,          
         &paramPlus,           
         &paramNazad,          
         &paramNapred,         
         &parExit              
         };
  TBox                   * const code Screen5_Boxes[2]=
         {
         &boksBrojParam,       
         &boksVrednostParam    
         };


  TCScreen                upuzorenje=
         {
         0xFFF0                ,//  upuzorenje.Color
         320                   ,//  upuzorenje.Width                     
         240                   ,//  upuzorenje.Height                    
         9                     ,//  upuzorenje.ObjectsCount             
         1                     ,//  upuzorenje.Buttons_RoundCount              
         Screen6_Buttons_Round ,//  upuzorenje.Buttons_Round
         0                     ,//  upuzorenje.CButtons_RoundCount              
         0                     ,//  upuzorenje.CButtons_Round
         7                     ,//  upuzorenje.LabelsCount              
         Screen6_Labels        ,//  upuzorenje.Labels
         0                     ,//  upuzorenje.CLabelsCount              
         0                     ,//  upuzorenje.CLabels
         0                     ,//  upuzorenje.CirclesCount              
         0                     ,//  upuzorenje.Circles
         0                     ,//  upuzorenje.CircleButtonsCount              
         0                     ,//  upuzorenje.CircleButtons
         1                     ,//  upuzorenje.BoxesCount              
         Screen6_Boxes         ,//  upuzorenje.Boxes
         0                     ,//  upuzorenje.CBoxesCount              
         0                     //  upuzorenje.CBoxes
         };

  TBox                   boksGreska;
  TLabel                 LabelGR1;
char LabelGR1_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR2;
char LabelGR2_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR3;
char LabelGR3_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR4;
char LabelGR4_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR5;
char LabelGR5_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR6;
char LabelGR6_Caption[16] = "GGGGGGGGGGGGGGG";

  TLabel                 LabelGR7;
char LabelGR7_Caption[16] = "GGGGGGGGGGGGGGG";

  TButton_Round          popravkaButon;
char popravkaButon_Caption[9] = "POPRAVKA";

  TButton_Round          * const code Screen6_Buttons_Round[1]=
         {
         &popravkaButon        
         };
  TLabel                 * const code Screen6_Labels[7]=
         {
         &LabelGR1,            
         &LabelGR2,            
         &LabelGR3,            
         &LabelGR4,            
         &LabelGR5,            
         &LabelGR6,            
         &LabelGR7             
         };
  TBox                   * const code Screen6_Boxes[1]=
         {
         &boksGreska           
         };


  TCScreen                podesavanjeSat=
         {
         0xFFF0                ,//  podesavanjeSat.Color
         320                   ,//  podesavanjeSat.Width                     
         240                   ,//  podesavanjeSat.Height                    
         25                    ,//  podesavanjeSat.ObjectsCount             
         10                    ,//  podesavanjeSat.Buttons_RoundCount              
         Screen7_Buttons_Round ,//  podesavanjeSat.Buttons_Round
         0                     ,//  podesavanjeSat.CButtons_RoundCount              
         0                     ,//  podesavanjeSat.CButtons_Round
         9                     ,//  podesavanjeSat.LabelsCount              
         Screen7_Labels        ,//  podesavanjeSat.Labels
         0                     ,//  podesavanjeSat.CLabelsCount              
         0                     ,//  podesavanjeSat.CLabels
         0                     ,//  podesavanjeSat.CirclesCount              
         0                     ,//  podesavanjeSat.Circles
         1                     ,//  podesavanjeSat.CircleButtonsCount              
         Screen7_CircleButtons ,//  podesavanjeSat.CircleButtons
         5                     ,//  podesavanjeSat.BoxesCount              
         Screen7_Boxes         ,//  podesavanjeSat.Boxes
         0                     ,//  podesavanjeSat.CBoxesCount              
         0                     //  podesavanjeSat.CBoxes
         };

  TCircleButton          satExit;
char satExit_Caption[2] = "X";

  TBox                   boksSati;
  TButton_Round          tSatiPlus;
char tSatiPlus_Caption[2] = "+";

  TButton_Round          tsatiMinus;
char tsatiMinus_Caption[2] = "-";

  TLabel                 labelSati;
char labelSati_Caption[4] = "23";

  TBox                   boksMinuti;
  TButton_Round          tMinutiPlus;
char tMinutiPlus_Caption[2] = "+";

  TButton_Round          tMinutiMinus;
char tMinutiMinus_Caption[2] = "-";

  TLabel                 labelMinuti;
char labelMinuti_Caption[4] = "23";

  TLabel                 dveTacke;
char dveTacke_Caption[4] = ":";

  TBox                   boksDani;
  TButton_Round          tDaniPlus;
char tDaniPlus_Caption[2] = "+";

  TButton_Round          tDaniMinus;
char tDaniMinus_Caption[2] = "-";

  TLabel                 labelDani;
char labelDani_Caption[4] = "23";

  TBox                   boksMesec;
  TButton_Round          tMeseciPlus;
char tMeseciPlus_Caption[2] = "+";

  TButton_Round          tMeseciMinus;
char tMeseciMinus_Caption[2] = "-";

  TLabel                 labelMesec;
char labelMesec_Caption[4] = "JAN";

  TLabel                 crtica1;
char crtica1_Caption[4] = "-";

  TLabel                 crtica2;
char crtica2_Caption[4] = "-";

  TBox                   boksGodina;
  TButton_Round          tGodinaPlus;
char tGodinaPlus_Caption[2] = "+";

  TButton_Round          tGodinaMinus;
char tGodinaMinus_Caption[2] = "-";

  TLabel                 labelGodina;
char labelGodina_Caption[4] = "00";

  TLabel                 god20;
char god20_Caption[4] = "20";

  TButton_Round          * const code Screen7_Buttons_Round[10]=
         {
         &tSatiPlus,           
         &tsatiMinus,          
         &tMinutiPlus,         
         &tMinutiMinus,        
         &tDaniPlus,           
         &tDaniMinus,          
         &tMeseciPlus,         
         &tMeseciMinus,        
         &tGodinaPlus,         
         &tGodinaMinus         
         };
  TLabel                 * const code Screen7_Labels[9]=
         {
         &labelSati,           
         &labelMinuti,         
         &dveTacke,            
         &labelDani,           
         &labelMesec,          
         &crtica1,             
         &crtica2,             
         &labelGodina,         
         &god20                
         };
  TCircleButton          * const code Screen7_CircleButtons[1]=
         {
         &satExit              
         };
  TBox                   * const code Screen7_Boxes[5]=
         {
         &boksSati,            
         &boksMinuti,          
         &boksDani,            
         &boksMesec,           
         &boksGodina           
         };


  TCScreen                pozdrav=
         {
         0xFFF0                ,//  pozdrav.Color
         320                   ,//  pozdrav.Width                     
         240                   ,//  pozdrav.Height                    
         2                     ,//  pozdrav.ObjectsCount             
         0                     ,//  pozdrav.Buttons_RoundCount              
         0                     ,//  pozdrav.Buttons_Round
         0                     ,//  pozdrav.CButtons_RoundCount              
         0                     ,//  pozdrav.CButtons_Round
         2                     ,//  pozdrav.LabelsCount              
         Screen8_Labels        ,//  pozdrav.Labels
         0                     ,//  pozdrav.CLabelsCount              
         0                     ,//  pozdrav.CLabels
         0                     ,//  pozdrav.CirclesCount              
         0                     ,//  pozdrav.Circles
         0                     ,//  pozdrav.CircleButtonsCount              
         0                     ,//  pozdrav.CircleButtons
         0                     ,//  pozdrav.BoxesCount              
         0                     ,//  pozdrav.Boxes
         0                     ,//  pozdrav.CBoxesCount              
         0                     //  pozdrav.CBoxes
         };

  TLabel                 pozdravPor1;
char pozdravPor1_Caption[6] = "EFMEL";

  TLabel                 pozdravPor2;
char pozdravPor2_Caption[18] = "GORIONIK NA PELET";

  TLabel                 * const code Screen8_Labels[2]=
         {
         &pozdravPor1,         
         &pozdravPor2          
         };


  TCScreen                podesavanjHrono=
         {
         0xFFF0                ,//  podesavanjHrono.Color
         320                   ,//  podesavanjHrono.Width                     
         240                   ,//  podesavanjHrono.Height                    
         23                    ,//  podesavanjHrono.ObjectsCount             
         8                     ,//  podesavanjHrono.Buttons_RoundCount              
         Screen9_Buttons_Round ,//  podesavanjHrono.Buttons_Round
         0                     ,//  podesavanjHrono.CButtons_RoundCount              
         0                     ,//  podesavanjHrono.CButtons_Round
         5                     ,//  podesavanjHrono.LabelsCount              
         Screen9_Labels        ,//  podesavanjHrono.Labels
         2                     ,//  podesavanjHrono.CLabelsCount              
         Screen9_CLabels       ,//  podesavanjHrono.CLabels
         1                     ,//  podesavanjHrono.CirclesCount              
         Screen9_Circles       ,//  podesavanjHrono.Circles
         2                     ,//  podesavanjHrono.CircleButtonsCount              
         Screen9_CircleButtons ,//  podesavanjHrono.CircleButtons
         5                     ,//  podesavanjHrono.BoxesCount              
         Screen9_Boxes         ,//  podesavanjHrono.Boxes
         0                     ,//  podesavanjHrono.CBoxesCount              
         0                     //  podesavanjHrono.CBoxes
         };

  TCircleButton          hronoExit;
char hronoExit_Caption[2] = "X";

  TBox                   boksDanHrono;
  TLabel                 labelHronoDan;
char labelHronoDan_Caption[4] = "PON";

  TButton_Round          tHronoProgPlus;
char tHronoProgPlus_Caption[2] = "+";

  TButton_Round          tHronoProgMinus;
char tHronoProgMinus_Caption[2] = "-";

  TCLabel          labelProg = 
         {
         32776                 , //   labelProg.OwnerScreenID
         5                     , //   labelProg.Order          
         73                    , //   labelProg.Left           
         68                    , //   labelProg.Top             
         64                    , //   labelProg.Width         
         26                    , //   labelProg.Height       
         &labelProg_Caption    , //   labelProg.Caption        
         &Arial21x24_Bold      , //   labelProg.FontName       
         0x6000                , //   labelProg.Font_Color     
         1                     , //   labelProg.Visible        
         1                     , //   labelProg.Active        
         0                     , //   labelProg.OnUpPtr
         0                     , //   labelProg.OnDownPtr
         0                     , //   labelProg.OnClickPtr
         0                       //   labelProg.OnPressPtr
         };
const char labelProg_Caption[5] = "PROG";

  TBox                   boksProgHrono;
  TLabel                 labelHronoProg;
char labelHronoProg_Caption[2] = "1";

  TButton_Round          tHronoSatPlus;
char tHronoSatPlus_Caption[2] = "+";

  TButton_Round          tHronoSatMinus;
char tHronoSatMinus_Caption[2] = "-";

  TBox                   boksSatHrono;
  TLabel                 labelHronoSat;
char labelHronoSat_Caption[6] = "00:00";

  TButton_Round          tHronoPosoMinus;
char tHronoPosoMinus_Caption[2] = "-";

  TButton_Round          tHronoPosoPlus;
char tHronoPosoPlus_Caption[2] = "+";

  TBox                   boksPosoHrono;
  TLabel                 lablHronoPoso;
char lablHronoPoso_Caption[5] = "AUTO";

  TButton_Round          tHronoZadTAPlus;
char tHronoZadTAPlus_Caption[2] = "+";

  TButton_Round          tHronoZadTAMinus;
char tHronoZadTAMinus_Caption[2] = "-";

  TBox                   boksZadTAhrono;
  TLabel                 labelHronoZadTA;
char labelHronoZadTA_Caption[8] = "80 0C";

  TCircleButton          thronoOnOff;
char thronoOnOff_Caption[4] = "OFF";

  TCLabel          labelHrono = 
         {
         32776                 , //   labelHrono.OwnerScreenID
         21                    , //   labelHrono.Order          
         239                   , //   labelHrono.Left           
         83                    , //   labelHrono.Top             
         81                    , //   labelHrono.Width         
         26                    , //   labelHrono.Height       
         &labelHrono_Caption   , //   labelHrono.Caption        
         &Arial21x24_Bold      , //   labelHrono.FontName       
         0x6000                , //   labelHrono.Font_Color     
         1                     , //   labelHrono.Visible        
         1                     , //   labelHrono.Active        
         0                     , //   labelHrono.OnUpPtr
         0                     , //   labelHrono.OnDownPtr
         0                     , //   labelHrono.OnClickPtr
         0                       //   labelHrono.OnPressPtr
         };
const char labelHrono_Caption[6] = "HRONO";

  TCircle                hronoPok;
  TButton_Round          * const code Screen9_Buttons_Round[8]=
         {
         &tHronoProgPlus,      
         &tHronoProgMinus,     
         &tHronoSatPlus,       
         &tHronoSatMinus,      
         &tHronoPosoMinus,     
         &tHronoPosoPlus,      
         &tHronoZadTAPlus,     
         &tHronoZadTAMinus     
         };
  TLabel                 * const code Screen9_Labels[5]=
         {
         &labelHronoDan,       
         &labelHronoProg,      
         &labelHronoSat,       
         &lablHronoPoso,       
         &labelHronoZadTA      
         };
  TCLabel                 * const code Screen9_CLabels[2]=
         {
         &labelProg,           
         &labelHrono           
         };
  TCircle                * const code Screen9_Circles[1]=
         {
         &hronoPok             
         };
  TCircleButton          * const code Screen9_CircleButtons[2]=
         {
         &hronoExit,           
         &thronoOnOff          
         };
  TBox                   * const code Screen9_Boxes[5]=
         {
         &boksDanHrono,        
         &boksProgHrono,       
         &boksSatHrono,        
         &boksPosoHrono,       
         &boksZadTAhrono       
         };


  TCScreen                podesavajKom=
         {
         0xFFF0                ,//  podesavajKom.Color
         320                   ,//  podesavajKom.Width                     
         240                   ,//  podesavajKom.Height                    
         13                    ,//  podesavajKom.ObjectsCount             
         2                     ,//  podesavajKom.Buttons_RoundCount              
         Screen10_Buttons_Round,//  podesavajKom.Buttons_Round
         0                     ,//  podesavajKom.CButtons_RoundCount              
         0                     ,//  podesavajKom.CButtons_Round
         2                     ,//  podesavajKom.LabelsCount              
         Screen10_Labels       ,//  podesavajKom.Labels
         4                     ,//  podesavajKom.CLabelsCount              
         Screen10_CLabels      ,//  podesavajKom.CLabels
         1                     ,//  podesavajKom.CirclesCount              
         Screen10_Circles      ,//  podesavajKom.Circles
         2                     ,//  podesavajKom.CircleButtonsCount              
         Screen10_CircleButtons,//  podesavajKom.CircleButtons
         2                     ,//  podesavajKom.BoxesCount              
         Screen10_Boxes        ,//  podesavajKom.Boxes
         0                     ,//  podesavajKom.CBoxesCount              
         0                     //  podesavajKom.CBoxes
         };

  TCircleButton          exitKom;
char exitKom_Caption[2] = "X";

  TCLabel          labelIP = 
         {
         32777                 , //   labelIP.OwnerScreenID
         1                     , //   labelIP.Order          
         18                    , //   labelIP.Left           
         13                    , //   labelIP.Top             
         118                   , //   labelIP.Width         
         26                    , //   labelIP.Height       
         &labelIP_Caption      , //   labelIP.Caption        
         &Arial21x24_Bold      , //   labelIP.FontName       
         0x6000                , //   labelIP.Font_Color     
         1                     , //   labelIP.Visible        
         1                     , //   labelIP.Active        
         0                     , //   labelIP.OnUpPtr
         0                     , //   labelIP.OnDownPtr
         0                     , //   labelIP.OnClickPtr
         0                       //   labelIP.OnPressPtr
         };
const char labelIP_Caption[10] = "IP ADRESA";

  TCLabel          labelAdresa = 
         {
         32777                 , //   labelAdresa.OwnerScreenID
         2                     , //   labelAdresa.Order          
         43                    , //   labelAdresa.Left           
         60                    , //   labelAdresa.Top             
         114                   , //   labelAdresa.Width         
         26                    , //   labelAdresa.Height       
         &labelAdresa_Caption  , //   labelAdresa.Caption        
         &Arial21x24_Bold      , //   labelAdresa.FontName       
         0x6000                , //   labelAdresa.Font_Color     
         1                     , //   labelAdresa.Visible        
         1                     , //   labelAdresa.Active        
         0                     , //   labelAdresa.OnUpPtr
         0                     , //   labelAdresa.OnDownPtr
         0                     , //   labelAdresa.OnClickPtr
         0                       //   labelAdresa.OnPressPtr
         };
const char labelAdresa_Caption[12] = "192.168.20.";

  TBox                   boksIP;
  TLabel                 labelAdresiranje;
char labelAdresiranje_Caption[4] = "000";

  TCLabel          labelGSM = 
         {
         32777                 , //   labelGSM.OwnerScreenID
         5                     , //   labelGSM.Order          
         18                    , //   labelGSM.Left           
         143                   , //   labelGSM.Top             
         49                    , //   labelGSM.Width         
         26                    , //   labelGSM.Height       
         &labelGSM_Caption     , //   labelGSM.Caption        
         &Arial21x24_Bold      , //   labelGSM.FontName       
         0x6000                , //   labelGSM.Font_Color     
         1                     , //   labelGSM.Visible        
         1                     , //   labelGSM.Active        
         0                     , //   labelGSM.OnUpPtr
         0                     , //   labelGSM.OnDownPtr
         0                     , //   labelGSM.OnClickPtr
         0                       //   labelGSM.OnPressPtr
         };
const char labelGSM_Caption[4] = "GSM";

  TButton_Round          ButtonRound4;
char ButtonRound4_Caption[2] = "-";

  TButton_Round          tIPplus;
char tIPplus_Caption[2] = "+";

  TCircle                GSMpok;
  TCircleButton          tGSMonOff;
char tGSMonOff_Caption[4] = "OFF";

  TBox                   bokBrojMob;
  TLabel                 labelBorjMob;
char labelBorjMob_Caption[13] = "0643324210";

  TCLabel          labelBrojZaAlarm = 
         {
         32777                 , //   labelBrojZaAlarm.OwnerScreenID
         12                    , //   labelBrojZaAlarm.Order          
         133                   , //   labelBrojZaAlarm.Left           
         148                   , //   labelBrojZaAlarm.Top             
         185                   , //   labelBrojZaAlarm.Width         
         26                    , //   labelBrojZaAlarm.Height       
         &labelBrojZaAlarm_Caption, //   labelBrojZaAlarm.Caption        
         &Arial21x24_Bold      , //   labelBrojZaAlarm.FontName       
         0x6000                , //   labelBrojZaAlarm.Font_Color     
         1                     , //   labelBrojZaAlarm.Visible        
         1                     , //   labelBrojZaAlarm.Active        
         0                     , //   labelBrojZaAlarm.OnUpPtr
         0                     , //   labelBrojZaAlarm.OnDownPtr
         0                     , //   labelBrojZaAlarm.OnClickPtr
         0                       //   labelBrojZaAlarm.OnPressPtr
         };
const char labelBrojZaAlarm_Caption[14] = "BROJ MOBILNOG";

  TButton_Round          * const code Screen10_Buttons_Round[2]=
         {
         &ButtonRound4,        
         &tIPplus              
         };
  TLabel                 * const code Screen10_Labels[2]=
         {
         &labelAdresiranje,    
         &labelBorjMob         
         };
  TCLabel                 * const code Screen10_CLabels[4]=
         {
         &labelIP,             
         &labelAdresa,         
         &labelGSM,            
         &labelBrojZaAlarm     
         };
  TCircle                * const code Screen10_Circles[1]=
         {
         &GSMpok               
         };
  TCircleButton          * const code Screen10_CircleButtons[2]=
         {
         &exitKom,             
         &tGSMonOff            
         };
  TBox                   * const code Screen10_Boxes[2]=
         {
         &boksIP,              
         &bokBrojMob           
         };


  TMyScreen        CurrentMyScreen;


static void InitializeObjects() {











  boksTzad.OwnerScreenID   = 32768;
  boksTzad.Order           = 0;
  boksTzad.Left            = 120;
  boksTzad.Top             = 120;
  boksTzad.Width           = 81;
  boksTzad.Height          = 28;
  boksTzad.Pen_Width       = 1;
  boksTzad.Pen_Color       = 0x0000;
  boksTzad.Visible         = 1;
  boksTzad.Active          = 1;
  boksTzad.Transparent     = 1;
  boksTzad.Gradient        = 0;
  boksTzad.Gradient_Orientation    = 0;
  boksTzad.Gradient_Start_Color    = 0xFFFF;
  boksTzad.Gradient_End_Color      = 0xC618;
  boksTzad.Color           = 0xFFFF;
  boksTzad.PressColEnabled     = 1;
  boksTzad.Press_Color     = 0x8410;
  boksTzad.OnUpPtr         = 0;
  boksTzad.OnDownPtr       = 0;
  boksTzad.OnClickPtr      = 0;
  boksTzad.OnPressPtr      = 0;

  boksTambijenta.OwnerScreenID   = 32768;
  boksTambijenta.Order           = 1;
  boksTambijenta.Left            = 132;
  boksTambijenta.Top             = 194;
  boksTambijenta.Width           = 81;
  boksTambijenta.Height          = 28;
  boksTambijenta.Pen_Width       = 1;
  boksTambijenta.Pen_Color       = 0x0000;
  boksTambijenta.Visible         = 1;
  boksTambijenta.Active          = 1;
  boksTambijenta.Transparent     = 1;
  boksTambijenta.Gradient        = 0;
  boksTambijenta.Gradient_Orientation    = 0;
  boksTambijenta.Gradient_Start_Color    = 0xFFFF;
  boksTambijenta.Gradient_End_Color      = 0xC618;
  boksTambijenta.Color           = 0xFFFF;
  boksTambijenta.PressColEnabled     = 1;
  boksTambijenta.Press_Color     = 0x8410;
  boksTambijenta.OnUpPtr         = 0;
  boksTambijenta.OnDownPtr       = 0;
  boksTambijenta.OnClickPtr      = 0;
  boksTambijenta.OnPressPtr      = 0;

  boksTdima.OwnerScreenID   = 32768;
  boksTdima.Order           = 2;
  boksTdima.Left            = 2;
  boksTdima.Top             = 194;
  boksTdima.Width           = 81;
  boksTdima.Height          = 28;
  boksTdima.Pen_Width       = 1;
  boksTdima.Pen_Color       = 0x0000;
  boksTdima.Visible         = 1;
  boksTdima.Active          = 1;
  boksTdima.Transparent     = 1;
  boksTdima.Gradient        = 0;
  boksTdima.Gradient_Orientation    = 0;
  boksTdima.Gradient_Start_Color    = 0xFFFF;
  boksTdima.Gradient_End_Color      = 0xC618;
  boksTdima.Color           = 0xFFFF;
  boksTdima.PressColEnabled     = 1;
  boksTdima.Press_Color     = 0x8410;
  boksTdima.OnUpPtr         = 0;
  boksTdima.OnDownPtr       = 0;
  boksTdima.OnClickPtr      = 0;
  boksTdima.OnPressPtr      = 0;

  fazaRada.OwnerScreenID   = 32768;
  fazaRada.Order           = 6;
  fazaRada.Left            = 8;
  fazaRada.Top             = 62;
  fazaRada.Width           = 208;
  fazaRada.Height          = 26;
  fazaRada.Visible         = 1;
  fazaRada.Active          = 1;
  fazaRada.Caption         = fazaRada_Caption;
  fazaRada.FontName        = Arial21x24_Bold;
  fazaRada.Font_Color      = 0x6000;
  fazaRada.OnUpPtr         = 0;
  fazaRada.OnDownPtr       = 0;
  fazaRada.OnClickPtr      = 0;
  fazaRada.OnPressPtr      = 0;

  vreme.OwnerScreenID   = 32768;
  vreme.Order           = 7;
  vreme.Left            = 161;
  vreme.Top             = 14;
  vreme.Width           = 146;
  vreme.Height          = 20;
  vreme.Visible         = 1;
  vreme.Active          = 1;
  vreme.Caption         = vreme_Caption;
  vreme.FontName        = Arial16x19_Bold;
  vreme.Font_Color      = 0xFFFF;
  vreme.OnUpPtr         = 0;
  vreme.OnDownPtr       = 0;
  vreme.OnClickPtr      = 0;
  vreme.OnPressPtr      = 0;

  minus.OwnerScreenID   = 32768;
  minus.Order           = 8;
  minus.Left            = 34;
  minus.Top             = 102;
  minus.Radius          = 30;
  minus.Pen_Width       = 1;
  minus.Pen_Color       = 0x0000;
  minus.Visible         = 1;
  minus.Active          = 1;
  minus.Transparent     = 1;
  minus.Caption         = minus_Caption;
  minus.TextAlign       = _taCenter;
  minus.FontName        = Arial23x29_Bold;
  minus.PressColEnabled = 1;
  minus.Font_Color     = 0x0000;
  minus.Gradient       = 1;
  minus.Gradient_Orientation    = 0;
  minus.Gradient_Start_Color    = 0xFFFF;
  minus.Gradient_End_Color      = 0xC618;
  minus.Color          = 0xC618;
  minus.Press_Color    = 0x8410;
  minus.OnUpPtr         = 0;
  minus.OnDownPtr       = 0;
  minus.OnClickPtr      = 0;
  minus.OnPressPtr      = klikMinus;

  plus.OwnerScreenID   = 32768;
  plus.Order           = 9;
  plus.Left            = 223;
  plus.Top             = 103;
  plus.Radius          = 30;
  plus.Pen_Width       = 1;
  plus.Pen_Color       = 0x0000;
  plus.Visible         = 1;
  plus.Active          = 1;
  plus.Transparent     = 1;
  plus.Caption         = plus_Caption;
  plus.TextAlign       = _taCenter;
  plus.FontName        = Arial23x29_Bold;
  plus.PressColEnabled = 1;
  plus.Font_Color     = 0x0000;
  plus.Gradient       = 1;
  plus.Gradient_Orientation    = 0;
  plus.Gradient_Start_Color    = 0xFFFF;
  plus.Gradient_End_Color      = 0xC618;
  plus.Color          = 0xC618;
  plus.Press_Color    = 0x8410;
  plus.OnUpPtr         = 0;
  plus.OnDownPtr       = 0;
  plus.OnClickPtr      = 0;
  plus.OnPressPtr      = klikPlus;

  zadataT.OwnerScreenID   = 32768;
  zadataT.Order           = 10;
  zadataT.Left            = 111;
  zadataT.Top             = 101;
  zadataT.Width           = 88;
  zadataT.Height          = 20;
  zadataT.Visible         = 1;
  zadataT.Active          = 1;
  zadataT.Caption         = zadataT_Caption;
  zadataT.FontName        = Arial16x19_Bold;
  zadataT.Font_Color      = 0x0000;
  zadataT.OnUpPtr         = 0;
  zadataT.OnDownPtr       = 0;
  zadataT.OnClickPtr      = 0;
  zadataT.OnPressPtr      = 0;

  onOff.OwnerScreenID   = 32768;
  onOff.Order           = 13;
  onOff.Left            = 250;
  onOff.Top             = 170;
  onOff.Radius          = 33;
  onOff.Pen_Width       = 1;
  onOff.Pen_Color       = 0x0000;
  onOff.Visible         = 1;
  onOff.Active          = 1;
  onOff.Transparent     = 1;
  onOff.Caption         = onOff_Caption;
  onOff.TextAlign       = _taCenter;
  onOff.FontName        = Arial23x29_Bold;
  onOff.PressColEnabled = 1;
  onOff.Font_Color     = 0x0000;
  onOff.Gradient       = 1;
  onOff.Gradient_Orientation    = 0;
  onOff.Gradient_Start_Color    = 0xFC10;
  onOff.Gradient_End_Color      = 0xF800;
  onOff.Color          = 0xF800;
  onOff.Press_Color    = 0x8410;
  onOff.OnUpPtr         = 0;
  onOff.OnDownPtr       = 0;
  onOff.OnClickPtr      = paliGasi;
  onOff.OnPressPtr      = 0;

  tasterP1.OwnerScreenID   = 32768;
  tasterP1.Order           = 14;
  tasterP1.Left            = 4;
  tasterP1.Top             = 105;
  tasterP1.Width           = 57;
  tasterP1.Height          = 37;
  tasterP1.Pen_Width       = 1;
  tasterP1.Pen_Color       = 0x0000;
  tasterP1.Visible         = 1;
  tasterP1.Active          = 1;
  tasterP1.Transparent     = 1;
  tasterP1.Caption         = tasterP1_Caption;
  tasterP1.TextAlign       = _taCenter;
  tasterP1.FontName        = Arial19x22_Bold;
  tasterP1.PressColEnabled = 1;
  tasterP1.Font_Color      = 0x0000;
  tasterP1.Gradient        = 1;
  tasterP1.Gradient_Orientation    = 0;
  tasterP1.Gradient_Start_Color    = 0xFFFF;
  tasterP1.Gradient_End_Color      = 0xC618;
  tasterP1.Color           = 0xC618;
  tasterP1.Press_Color     = 0x8410;
  tasterP1.Corner_Radius   = 3;
  tasterP1.OnUpPtr         = 0;
  tasterP1.OnDownPtr       = 0;
  tasterP1.OnClickPtr      = klikP1;
  tasterP1.OnPressPtr      = 0;

  tasterP2.OwnerScreenID   = 32768;
  tasterP2.Order           = 15;
  tasterP2.Left            = 68;
  tasterP2.Top             = 105;
  tasterP2.Width           = 57;
  tasterP2.Height          = 37;
  tasterP2.Pen_Width       = 1;
  tasterP2.Pen_Color       = 0x0000;
  tasterP2.Visible         = 1;
  tasterP2.Active          = 1;
  tasterP2.Transparent     = 1;
  tasterP2.Caption         = tasterP2_Caption;
  tasterP2.TextAlign       = _taCenter;
  tasterP2.FontName        = Arial19x22_Bold;
  tasterP2.PressColEnabled = 1;
  tasterP2.Font_Color      = 0x0000;
  tasterP2.Gradient        = 1;
  tasterP2.Gradient_Orientation    = 0;
  tasterP2.Gradient_Start_Color    = 0xFFFF;
  tasterP2.Gradient_End_Color      = 0xC618;
  tasterP2.Color           = 0xC618;
  tasterP2.Press_Color     = 0x8410;
  tasterP2.Corner_Radius   = 3;
  tasterP2.OnUpPtr         = 0;
  tasterP2.OnDownPtr       = 0;
  tasterP2.OnClickPtr      = klikP2;
  tasterP2.OnPressPtr      = 0;

  tasterP3.OwnerScreenID   = 32768;
  tasterP3.Order           = 16;
  tasterP3.Left            = 132;
  tasterP3.Top             = 105;
  tasterP3.Width           = 57;
  tasterP3.Height          = 37;
  tasterP3.Pen_Width       = 1;
  tasterP3.Pen_Color       = 0x0000;
  tasterP3.Visible         = 1;
  tasterP3.Active          = 1;
  tasterP3.Transparent     = 1;
  tasterP3.Caption         = tasterP3_Caption;
  tasterP3.TextAlign       = _taCenter;
  tasterP3.FontName        = Arial19x22_Bold;
  tasterP3.PressColEnabled = 1;
  tasterP3.Font_Color      = 0x0000;
  tasterP3.Gradient        = 1;
  tasterP3.Gradient_Orientation    = 0;
  tasterP3.Gradient_Start_Color    = 0xFFFF;
  tasterP3.Gradient_End_Color      = 0xC618;
  tasterP3.Color           = 0xC618;
  tasterP3.Press_Color     = 0x8410;
  tasterP3.Corner_Radius   = 3;
  tasterP3.OnUpPtr         = 0;
  tasterP3.OnDownPtr       = 0;
  tasterP3.OnClickPtr      = klikP3;
  tasterP3.OnPressPtr      = 0;

  tasterP4.OwnerScreenID   = 32768;
  tasterP4.Order           = 17;
  tasterP4.Left            = 196;
  tasterP4.Top             = 105;
  tasterP4.Width           = 57;
  tasterP4.Height          = 37;
  tasterP4.Pen_Width       = 1;
  tasterP4.Pen_Color       = 0x0000;
  tasterP4.Visible         = 1;
  tasterP4.Active          = 1;
  tasterP4.Transparent     = 1;
  tasterP4.Caption         = tasterP4_Caption;
  tasterP4.TextAlign       = _taCenter;
  tasterP4.FontName        = Arial19x22_Bold;
  tasterP4.PressColEnabled = 1;
  tasterP4.Font_Color      = 0x0000;
  tasterP4.Gradient        = 1;
  tasterP4.Gradient_Orientation    = 0;
  tasterP4.Gradient_Start_Color    = 0xFFFF;
  tasterP4.Gradient_End_Color      = 0xC618;
  tasterP4.Color           = 0xC618;
  tasterP4.Press_Color     = 0x8410;
  tasterP4.Corner_Radius   = 3;
  tasterP4.OnUpPtr         = 0;
  tasterP4.OnDownPtr       = 0;
  tasterP4.OnClickPtr      = klikP4;
  tasterP4.OnPressPtr      = 0;

  tasterP5.OwnerScreenID   = 32768;
  tasterP5.Order           = 18;
  tasterP5.Left            = 259;
  tasterP5.Top             = 105;
  tasterP5.Width           = 57;
  tasterP5.Height          = 37;
  tasterP5.Pen_Width       = 1;
  tasterP5.Pen_Color       = 0x0000;
  tasterP5.Visible         = 1;
  tasterP5.Active          = 1;
  tasterP5.Transparent     = 1;
  tasterP5.Caption         = tasterP5_Caption;
  tasterP5.TextAlign       = _taCenter;
  tasterP5.FontName        = Arial19x22_Bold;
  tasterP5.PressColEnabled = 1;
  tasterP5.Font_Color      = 0x0000;
  tasterP5.Gradient        = 1;
  tasterP5.Gradient_Orientation    = 0;
  tasterP5.Gradient_Start_Color    = 0xFFFF;
  tasterP5.Gradient_End_Color      = 0xC618;
  tasterP5.Color           = 0xC618;
  tasterP5.Press_Color     = 0x8410;
  tasterP5.Corner_Radius   = 3;
  tasterP5.OnUpPtr         = 0;
  tasterP5.OnDownPtr       = 0;
  tasterP5.OnClickPtr      = klikP5;
  tasterP5.OnPressPtr      = 0;

  krugP1.OwnerScreenID   = 32768;
  krugP1.Order           = 19;
  krugP1.Left            = 20;
  krugP1.Top             = 145;
  krugP1.Radius          = 12;
  krugP1.Pen_Width       = 1;
  krugP1.Pen_Color       = 0x0000;
  krugP1.Visible         = 1;
  krugP1.Active          = 1;
  krugP1.Transparent     = 1;
  krugP1.Gradient        = 0;
  krugP1.Gradient_Orientation    = 0;
  krugP1.Gradient_Start_Color    = 0xFFFF;
  krugP1.Gradient_End_Color      = 0xC618;
  krugP1.Color           = 0x07E0;
  krugP1.PressColEnabled = 1;
  krugP1.Press_Color     = 0x8410;
  krugP1.OnUpPtr         = 0;
  krugP1.OnDownPtr       = 0;
  krugP1.OnClickPtr      = 0;
  krugP1.OnPressPtr      = 0;

  krugP2.OwnerScreenID   = 32768;
  krugP2.Order           = 20;
  krugP2.Left            = 84;
  krugP2.Top             = 145;
  krugP2.Radius          = 12;
  krugP2.Pen_Width       = 1;
  krugP2.Pen_Color       = 0x0000;
  krugP2.Visible         = 1;
  krugP2.Active          = 1;
  krugP2.Transparent     = 1;
  krugP2.Gradient        = 0;
  krugP2.Gradient_Orientation    = 0;
  krugP2.Gradient_Start_Color    = 0xFFFF;
  krugP2.Gradient_End_Color      = 0xC618;
  krugP2.Color           = 0x07E0;
  krugP2.PressColEnabled = 1;
  krugP2.Press_Color     = 0x8410;
  krugP2.OnUpPtr         = 0;
  krugP2.OnDownPtr       = 0;
  krugP2.OnClickPtr      = 0;
  krugP2.OnPressPtr      = 0;

  krugP3.OwnerScreenID   = 32768;
  krugP3.Order           = 21;
  krugP3.Left            = 149;
  krugP3.Top             = 144;
  krugP3.Radius          = 12;
  krugP3.Pen_Width       = 1;
  krugP3.Pen_Color       = 0x0000;
  krugP3.Visible         = 1;
  krugP3.Active          = 1;
  krugP3.Transparent     = 1;
  krugP3.Gradient        = 0;
  krugP3.Gradient_Orientation    = 0;
  krugP3.Gradient_Start_Color    = 0xFFFF;
  krugP3.Gradient_End_Color      = 0xC618;
  krugP3.Color           = 0x07E0;
  krugP3.PressColEnabled = 1;
  krugP3.Press_Color     = 0x8410;
  krugP3.OnUpPtr         = 0;
  krugP3.OnDownPtr       = 0;
  krugP3.OnClickPtr      = 0;
  krugP3.OnPressPtr      = 0;

  krugP4.OwnerScreenID   = 32768;
  krugP4.Order           = 22;
  krugP4.Left            = 212;
  krugP4.Top             = 144;
  krugP4.Radius          = 12;
  krugP4.Pen_Width       = 1;
  krugP4.Pen_Color       = 0x0000;
  krugP4.Visible         = 1;
  krugP4.Active          = 1;
  krugP4.Transparent     = 1;
  krugP4.Gradient        = 0;
  krugP4.Gradient_Orientation    = 0;
  krugP4.Gradient_Start_Color    = 0xFFFF;
  krugP4.Gradient_End_Color      = 0xC618;
  krugP4.Color           = 0x07E0;
  krugP4.PressColEnabled = 1;
  krugP4.Press_Color     = 0x8410;
  krugP4.OnUpPtr         = 0;
  krugP4.OnDownPtr       = 0;
  krugP4.OnClickPtr      = 0;
  krugP4.OnPressPtr      = 0;

  krugP5.OwnerScreenID   = 32768;
  krugP5.Order           = 23;
  krugP5.Left            = 276;
  krugP5.Top             = 143;
  krugP5.Radius          = 12;
  krugP5.Pen_Width       = 1;
  krugP5.Pen_Color       = 0x0000;
  krugP5.Visible         = 1;
  krugP5.Active          = 1;
  krugP5.Transparent     = 1;
  krugP5.Gradient        = 0;
  krugP5.Gradient_Orientation    = 0;
  krugP5.Gradient_Start_Color    = 0xFFFF;
  krugP5.Gradient_End_Color      = 0xC618;
  krugP5.Color           = 0x07E0;
  krugP5.PressColEnabled = 1;
  krugP5.Press_Color     = 0x8410;
  krugP5.OnUpPtr         = 0;
  krugP5.OnDownPtr       = 0;
  krugP5.OnClickPtr      = 0;
  krugP5.OnPressPtr      = 0;

  TD.OwnerScreenID   = 32768;
  TD.Order           = 24;
  TD.Left            = 7;
  TD.Top             = 196;
  TD.Width           = 70;
  TD.Height          = 26;
  TD.Visible         = 1;
  TD.Active          = 1;
  TD.Caption         = TD_Caption;
  TD.FontName        = Arial21x24_Bold;
  TD.Font_Color      = 0x6000;
  TD.OnUpPtr         = 0;
  TD.OnDownPtr       = 0;
  TD.OnClickPtr      = 0;
  TD.OnPressPtr      = 0;

  TA.OwnerScreenID   = 32768;
  TA.Order           = 25;
  TA.Left            = 146;
  TA.Top             = 196;
  TA.Width           = 58;
  TA.Height          = 26;
  TA.Visible         = 1;
  TA.Active          = 1;
  TA.Caption         = TA_Caption;
  TA.FontName        = Arial21x24_Bold;
  TA.Font_Color      = 0x6000;
  TA.OnUpPtr         = 0;
  TA.OnDownPtr       = 0;
  TA.OnClickPtr      = 0;
  TA.OnPressPtr      = 0;

  ZTA.OwnerScreenID   = 32768;
  ZTA.Order           = 26;
  ZTA.Left            = 133;
  ZTA.Top             = 121;
  ZTA.Width           = 58;
  ZTA.Height          = 26;
  ZTA.Visible         = 1;
  ZTA.Active          = 1;
  ZTA.Caption         = ZTA_Caption;
  ZTA.FontName        = Arial21x24_Bold;
  ZTA.Font_Color      = 0x6000;
  ZTA.OnUpPtr         = 0;
  ZTA.OnDownPtr       = 0;
  ZTA.OnClickPtr      = 0;
  ZTA.OnPressPtr      = 0;

  izlaz.OwnerScreenID   = 32769;
  izlaz.Order           = 3;
  izlaz.Left            = 267;
  izlaz.Top             = 3;
  izlaz.Radius          = 25;
  izlaz.Pen_Width       = 1;
  izlaz.Pen_Color       = 0x0000;
  izlaz.Visible         = 1;
  izlaz.Active          = 1;
  izlaz.Transparent     = 1;
  izlaz.Caption         = izlaz_Caption;
  izlaz.TextAlign       = _taCenter;
  izlaz.FontName        = Arial23x29_Bold;
  izlaz.PressColEnabled = 1;
  izlaz.Font_Color     = 0x0000;
  izlaz.Gradient       = 1;
  izlaz.Gradient_Orientation    = 0;
  izlaz.Gradient_Start_Color    = 0xFC10;
  izlaz.Gradient_End_Color      = 0xF800;
  izlaz.Color          = 0xC618;
  izlaz.Press_Color    = 0x8410;
  izlaz.OnUpPtr         = 0;
  izlaz.OnDownPtr       = 0;
  izlaz.OnClickPtr      = nazad1;
  izlaz.OnPressPtr      = 0;

  datumIvreme.OwnerScreenID   = 32770;
  datumIvreme.Order           = 0;
  datumIvreme.Left            = 12;
  datumIvreme.Top             = 11;
  datumIvreme.Width           = 250;
  datumIvreme.Height          = 40;
  datumIvreme.Pen_Width       = 1;
  datumIvreme.Pen_Color       = 0x0000;
  datumIvreme.Visible         = 1;
  datumIvreme.Active          = 1;
  datumIvreme.Transparent     = 1;
  datumIvreme.Caption         = datumIvreme_Caption;
  datumIvreme.TextAlign       = _taCenter;
  datumIvreme.FontName        = Arial19x22_Bold;
  datumIvreme.PressColEnabled = 1;
  datumIvreme.Font_Color      = 0x0000;
  datumIvreme.Gradient        = 1;
  datumIvreme.Gradient_Orientation    = 0;
  datumIvreme.Gradient_Start_Color    = 0xFFFF;
  datumIvreme.Gradient_End_Color      = 0xC618;
  datumIvreme.Color           = 0xC618;
  datumIvreme.Press_Color     = 0x8410;
  datumIvreme.Corner_Radius   = 3;
  datumIvreme.OnUpPtr         = 0;
  datumIvreme.OnDownPtr       = 0;
  datumIvreme.OnClickPtr      = podesiDiV;
  datumIvreme.OnPressPtr      = 0;

  kalibracija.OwnerScreenID   = 32770;
  kalibracija.Order           = 1;
  kalibracija.Left            = 12;
  kalibracija.Top             = 68;
  kalibracija.Width           = 250;
  kalibracija.Height          = 40;
  kalibracija.Pen_Width       = 1;
  kalibracija.Pen_Color       = 0x0000;
  kalibracija.Visible         = 1;
  kalibracija.Active          = 1;
  kalibracija.Transparent     = 1;
  kalibracija.Caption         = kalibracija_Caption;
  kalibracija.TextAlign       = _taCenter;
  kalibracija.FontName        = Arial19x22_Bold;
  kalibracija.PressColEnabled = 1;
  kalibracija.Font_Color      = 0x0000;
  kalibracija.Gradient        = 1;
  kalibracija.Gradient_Orientation    = 0;
  kalibracija.Gradient_Start_Color    = 0xFFFF;
  kalibracija.Gradient_End_Color      = 0xC618;
  kalibracija.Color           = 0xC618;
  kalibracija.Press_Color     = 0x8410;
  kalibracija.Corner_Radius   = 3;
  kalibracija.OnUpPtr         = 0;
  kalibracija.OnDownPtr       = 0;
  kalibracija.OnClickPtr      = kalibrisi;
  kalibracija.OnPressPtr      = 0;

  CircleButton1.OwnerScreenID   = 32770;
  CircleButton1.Order           = 2;
  CircleButton1.Left            = 267;
  CircleButton1.Top             = 3;
  CircleButton1.Radius          = 25;
  CircleButton1.Pen_Width       = 1;
  CircleButton1.Pen_Color       = 0x0000;
  CircleButton1.Visible         = 1;
  CircleButton1.Active          = 1;
  CircleButton1.Transparent     = 1;
  CircleButton1.Caption         = CircleButton1_Caption;
  CircleButton1.TextAlign       = _taCenter;
  CircleButton1.FontName        = Arial23x29_Bold;
  CircleButton1.PressColEnabled = 1;
  CircleButton1.Font_Color     = 0x0000;
  CircleButton1.Gradient       = 1;
  CircleButton1.Gradient_Orientation    = 0;
  CircleButton1.Gradient_Start_Color    = 0xFC10;
  CircleButton1.Gradient_End_Color      = 0xF800;
  CircleButton1.Color          = 0xC618;
  CircleButton1.Press_Color    = 0x8410;
  CircleButton1.OnUpPtr         = 0;
  CircleButton1.OnDownPtr       = 0;
  CircleButton1.OnClickPtr      = nazad2;
  CircleButton1.OnPressPtr      = 0;

  tKomunikacija.OwnerScreenID   = 32770;
  tKomunikacija.Order           = 3;
  tKomunikacija.Left            = 12;
  tKomunikacija.Top             = 125;
  tKomunikacija.Width           = 250;
  tKomunikacija.Height          = 40;
  tKomunikacija.Pen_Width       = 1;
  tKomunikacija.Pen_Color       = 0x0000;
  tKomunikacija.Visible         = 1;
  tKomunikacija.Active          = 1;
  tKomunikacija.Transparent     = 1;
  tKomunikacija.Caption         = tKomunikacija_Caption;
  tKomunikacija.TextAlign       = _taCenter;
  tKomunikacija.FontName        = Arial19x22_Bold;
  tKomunikacija.PressColEnabled = 1;
  tKomunikacija.Font_Color      = 0x0000;
  tKomunikacija.Gradient        = 1;
  tKomunikacija.Gradient_Orientation    = 0;
  tKomunikacija.Gradient_Start_Color    = 0xFFFF;
  tKomunikacija.Gradient_End_Color      = 0xC618;
  tKomunikacija.Color           = 0xC618;
  tKomunikacija.Press_Color     = 0x8410;
  tKomunikacija.Corner_Radius   = 3;
  tKomunikacija.OnUpPtr         = 0;
  tKomunikacija.OnDownPtr       = 0;
  tKomunikacija.OnClickPtr      = fKomunikacija;
  tKomunikacija.OnPressPtr      = 0;

  parametri.OwnerScreenID   = 32770;
  parametri.Order           = 4;
  parametri.Left            = 12;
  parametri.Top             = 181;
  parametri.Width           = 250;
  parametri.Height          = 40;
  parametri.Pen_Width       = 1;
  parametri.Pen_Color       = 0x0000;
  parametri.Visible         = 1;
  parametri.Active          = 1;
  parametri.Transparent     = 1;
  parametri.Caption         = parametri_Caption;
  parametri.TextAlign       = _taCenter;
  parametri.FontName        = Arial19x22_Bold;
  parametri.PressColEnabled = 1;
  parametri.Font_Color      = 0x0000;
  parametri.Gradient        = 1;
  parametri.Gradient_Orientation    = 0;
  parametri.Gradient_Start_Color    = 0xFFFF;
  parametri.Gradient_End_Color      = 0xC618;
  parametri.Color           = 0xC618;
  parametri.Press_Color     = 0x8410;
  parametri.Corner_Radius   = 3;
  parametri.OnUpPtr         = 0;
  parametri.OnDownPtr       = 0;
  parametri.OnClickPtr      = parametrija;
  parametri.OnPressPtr      = 0;

  sh1.OwnerScreenID   = 32771;
  sh1.Order           = 1;
  sh1.Left            = 30;
  sh1.Top             = 20;
  sh1.Radius          = 12;
  sh1.Pen_Width       = 1;
  sh1.Pen_Color       = 0x0000;
  sh1.Visible         = 1;
  sh1.Active          = 1;
  sh1.Transparent     = 1;
  sh1.Gradient        = 0;
  sh1.Gradient_Orientation    = 0;
  sh1.Gradient_Start_Color    = 0xFFFF;
  sh1.Gradient_End_Color      = 0xC618;
  sh1.Color           = 0x8008;
  sh1.PressColEnabled = 1;
  sh1.Press_Color     = 0x8410;
  sh1.OnUpPtr         = 0;
  sh1.OnDownPtr       = 0;
  sh1.OnClickPtr      = 0;
  sh1.OnPressPtr      = 0;

  sh2.OwnerScreenID   = 32771;
  sh2.Order           = 2;
  sh2.Left            = 90;
  sh2.Top             = 20;
  sh2.Radius          = 12;
  sh2.Pen_Width       = 1;
  sh2.Pen_Color       = 0x0000;
  sh2.Visible         = 1;
  sh2.Active          = 1;
  sh2.Transparent     = 1;
  sh2.Gradient        = 0;
  sh2.Gradient_Orientation    = 0;
  sh2.Gradient_Start_Color    = 0xFFFF;
  sh2.Gradient_End_Color      = 0xC618;
  sh2.Color           = 0x8008;
  sh2.PressColEnabled = 1;
  sh2.Press_Color     = 0x8410;
  sh2.OnUpPtr         = 0;
  sh2.OnDownPtr       = 0;
  sh2.OnClickPtr      = 0;
  sh2.OnPressPtr      = 0;

  sh3.OwnerScreenID   = 32771;
  sh3.Order           = 3;
  sh3.Left            = 150;
  sh3.Top             = 20;
  sh3.Radius          = 12;
  sh3.Pen_Width       = 1;
  sh3.Pen_Color       = 0x0000;
  sh3.Visible         = 1;
  sh3.Active          = 1;
  sh3.Transparent     = 1;
  sh3.Gradient        = 0;
  sh3.Gradient_Orientation    = 0;
  sh3.Gradient_Start_Color    = 0xFFFF;
  sh3.Gradient_End_Color      = 0xC618;
  sh3.Color           = 0x8008;
  sh3.PressColEnabled = 1;
  sh3.Press_Color     = 0x8410;
  sh3.OnUpPtr         = 0;
  sh3.OnDownPtr       = 0;
  sh3.OnClickPtr      = 0;
  sh3.OnPressPtr      = 0;

  sh4.OwnerScreenID   = 32771;
  sh4.Order           = 4;
  sh4.Left            = 210;
  sh4.Top             = 20;
  sh4.Radius          = 12;
  sh4.Pen_Width       = 1;
  sh4.Pen_Color       = 0x0000;
  sh4.Visible         = 1;
  sh4.Active          = 1;
  sh4.Transparent     = 1;
  sh4.Gradient        = 0;
  sh4.Gradient_Orientation    = 0;
  sh4.Gradient_Start_Color    = 0xFFFF;
  sh4.Gradient_End_Color      = 0xC618;
  sh4.Color           = 0x8008;
  sh4.PressColEnabled = 1;
  sh4.Press_Color     = 0x8410;
  sh4.OnUpPtr         = 0;
  sh4.OnDownPtr       = 0;
  sh4.OnClickPtr      = 0;
  sh4.OnPressPtr      = 0;

  lozExit.OwnerScreenID   = 32771;
  lozExit.Order           = 14;
  lozExit.Left            = 267;
  lozExit.Top             = 3;
  lozExit.Radius          = 25;
  lozExit.Pen_Width       = 1;
  lozExit.Pen_Color       = 0x0000;
  lozExit.Visible         = 1;
  lozExit.Active          = 1;
  lozExit.Transparent     = 1;
  lozExit.Caption         = lozExit_Caption;
  lozExit.TextAlign       = _taCenter;
  lozExit.FontName        = Arial23x29_Bold;
  lozExit.PressColEnabled = 1;
  lozExit.Font_Color     = 0x0000;
  lozExit.Gradient       = 1;
  lozExit.Gradient_Orientation    = 0;
  lozExit.Gradient_Start_Color    = 0xFC10;
  lozExit.Gradient_End_Color      = 0xF800;
  lozExit.Color          = 0xC618;
  lozExit.Press_Color    = 0x8410;
  lozExit.OnUpPtr         = 0;
  lozExit.OnDownPtr       = 0;
  lozExit.OnClickPtr      = nazad3;
  lozExit.OnPressPtr      = 0;

  boksBrojParam.OwnerScreenID   = 32772;
  boksBrojParam.Order           = 0;
  boksBrojParam.Left            = 142;
  boksBrojParam.Top             = 34;
  boksBrojParam.Width           = 45;
  boksBrojParam.Height          = 33;
  boksBrojParam.Pen_Width       = 1;
  boksBrojParam.Pen_Color       = 0x0000;
  boksBrojParam.Visible         = 1;
  boksBrojParam.Active          = 1;
  boksBrojParam.Transparent     = 1;
  boksBrojParam.Gradient        = 0;
  boksBrojParam.Gradient_Orientation    = 0;
  boksBrojParam.Gradient_Start_Color    = 0xFFFF;
  boksBrojParam.Gradient_End_Color      = 0xC618;
  boksBrojParam.Color           = 0xFFFF;
  boksBrojParam.PressColEnabled     = 1;
  boksBrojParam.Press_Color     = 0x8410;
  boksBrojParam.OnUpPtr         = 0;
  boksBrojParam.OnDownPtr       = 0;
  boksBrojParam.OnClickPtr      = 0;
  boksBrojParam.OnPressPtr      = 0;

  boksVrednostParam.OwnerScreenID   = 32772;
  boksVrednostParam.Order           = 1;
  boksVrednostParam.Left            = 90;
  boksVrednostParam.Top             = 124;
  boksVrednostParam.Width           = 133;
  boksVrednostParam.Height          = 36;
  boksVrednostParam.Pen_Width       = 1;
  boksVrednostParam.Pen_Color       = 0x0000;
  boksVrednostParam.Visible         = 1;
  boksVrednostParam.Active          = 1;
  boksVrednostParam.Transparent     = 1;
  boksVrednostParam.Gradient        = 0;
  boksVrednostParam.Gradient_Orientation    = 0;
  boksVrednostParam.Gradient_Start_Color    = 0xFFFF;
  boksVrednostParam.Gradient_End_Color      = 0xC618;
  boksVrednostParam.Color           = 0xFFFF;
  boksVrednostParam.PressColEnabled     = 1;
  boksVrednostParam.Press_Color     = 0x8410;
  boksVrednostParam.OnUpPtr         = 0;
  boksVrednostParam.OnDownPtr       = 0;
  boksVrednostParam.OnClickPtr      = 0;
  boksVrednostParam.OnPressPtr      = 0;

  paramMinus.OwnerScreenID   = 32772;
  paramMinus.Order           = 2;
  paramMinus.Left            = 23;
  paramMinus.Top             = 110;
  paramMinus.Radius          = 30;
  paramMinus.Pen_Width       = 1;
  paramMinus.Pen_Color       = 0x0000;
  paramMinus.Visible         = 1;
  paramMinus.Active          = 1;
  paramMinus.Transparent     = 1;
  paramMinus.Caption         = paramMinus_Caption;
  paramMinus.TextAlign       = _taCenter;
  paramMinus.FontName        = Arial23x29_Bold;
  paramMinus.PressColEnabled = 1;
  paramMinus.Font_Color     = 0x0000;
  paramMinus.Gradient       = 1;
  paramMinus.Gradient_Orientation    = 0;
  paramMinus.Gradient_Start_Color    = 0xFFFF;
  paramMinus.Gradient_End_Color      = 0xC618;
  paramMinus.Color          = 0xC618;
  paramMinus.Press_Color    = 0x8410;
  paramMinus.OnUpPtr         = 0;
  paramMinus.OnDownPtr       = 0;
  paramMinus.OnClickPtr      = 0;
  paramMinus.OnPressPtr      = klikMinusP;

  paramPlus.OwnerScreenID   = 32772;
  paramPlus.Order           = 3;
  paramPlus.Left            = 229;
  paramPlus.Top             = 109;
  paramPlus.Radius          = 30;
  paramPlus.Pen_Width       = 1;
  paramPlus.Pen_Color       = 0x0000;
  paramPlus.Visible         = 1;
  paramPlus.Active          = 1;
  paramPlus.Transparent     = 1;
  paramPlus.Caption         = paramPlus_Caption;
  paramPlus.TextAlign       = _taCenter;
  paramPlus.FontName        = Arial23x29_Bold;
  paramPlus.PressColEnabled = 1;
  paramPlus.Font_Color     = 0x0000;
  paramPlus.Gradient       = 1;
  paramPlus.Gradient_Orientation    = 0;
  paramPlus.Gradient_Start_Color    = 0xFFFF;
  paramPlus.Gradient_End_Color      = 0xC618;
  paramPlus.Color          = 0xC618;
  paramPlus.Press_Color    = 0x8410;
  paramPlus.OnUpPtr         = 0;
  paramPlus.OnDownPtr       = 0;
  paramPlus.OnClickPtr      = 0;
  paramPlus.OnPressPtr      = klikPlusP;

  paramNazad.OwnerScreenID   = 32772;
  paramNazad.Order           = 4;
  paramNazad.Left            = 40;
  paramNazad.Top             = 20;
  paramNazad.Radius          = 30;
  paramNazad.Pen_Width       = 1;
  paramNazad.Pen_Color       = 0x0000;
  paramNazad.Visible         = 1;
  paramNazad.Active          = 1;
  paramNazad.Transparent     = 1;
  paramNazad.Caption         = paramNazad_Caption;
  paramNazad.TextAlign       = _taCenter;
  paramNazad.FontName        = Arial23x29_Bold;
  paramNazad.PressColEnabled = 1;
  paramNazad.Font_Color     = 0x0000;
  paramNazad.Gradient       = 1;
  paramNazad.Gradient_Orientation    = 0;
  paramNazad.Gradient_Start_Color    = 0xFFFF;
  paramNazad.Gradient_End_Color      = 0xC618;
  paramNazad.Color          = 0xC618;
  paramNazad.Press_Color    = 0x8410;
  paramNazad.OnUpPtr         = 0;
  paramNazad.OnDownPtr       = 0;
  paramNazad.OnClickPtr      = 0;
  paramNazad.OnPressPtr      = paramNatrag;

  paramNapred.OwnerScreenID   = 32772;
  paramNapred.Order           = 5;
  paramNapred.Left            = 192;
  paramNapred.Top             = 20;
  paramNapred.Radius          = 30;
  paramNapred.Pen_Width       = 1;
  paramNapred.Pen_Color       = 0x0000;
  paramNapred.Visible         = 1;
  paramNapred.Active          = 1;
  paramNapred.Transparent     = 1;
  paramNapred.Caption         = paramNapred_Caption;
  paramNapred.TextAlign       = _taCenter;
  paramNapred.FontName        = Arial23x29_Bold;
  paramNapred.PressColEnabled = 1;
  paramNapred.Font_Color     = 0x0000;
  paramNapred.Gradient       = 1;
  paramNapred.Gradient_Orientation    = 0;
  paramNapred.Gradient_Start_Color    = 0xFFFF;
  paramNapred.Gradient_End_Color      = 0xC618;
  paramNapred.Color          = 0xC618;
  paramNapred.Press_Color    = 0x8410;
  paramNapred.OnUpPtr         = 0;
  paramNapred.OnDownPtr       = 0;
  paramNapred.OnClickPtr      = 0;
  paramNapred.OnPressPtr      = paramDalje;

  parExit.OwnerScreenID   = 32772;
  parExit.Order           = 6;
  parExit.Left            = 267;
  parExit.Top             = 3;
  parExit.Radius          = 25;
  parExit.Pen_Width       = 1;
  parExit.Pen_Color       = 0x0000;
  parExit.Visible         = 1;
  parExit.Active          = 1;
  parExit.Transparent     = 1;
  parExit.Caption         = parExit_Caption;
  parExit.TextAlign       = _taCenter;
  parExit.FontName        = Arial23x29_Bold;
  parExit.PressColEnabled = 1;
  parExit.Font_Color     = 0x0000;
  parExit.Gradient       = 1;
  parExit.Gradient_Orientation    = 0;
  parExit.Gradient_Start_Color    = 0xFC10;
  parExit.Gradient_End_Color      = 0xF800;
  parExit.Color          = 0xC618;
  parExit.Press_Color    = 0x8410;
  parExit.OnUpPtr         = 0;
  parExit.OnDownPtr       = 0;
  parExit.OnClickPtr      = nazadP;
  parExit.OnPressPtr      = 0;

  paramTest.OwnerScreenID   = 32772;
  paramTest.Order           = 7;
  paramTest.Left            = 85;
  paramTest.Top             = 200;
  paramTest.Width           = 144;
  paramTest.Height          = 27;
  paramTest.Pen_Width       = 1;
  paramTest.Pen_Color       = 0x0000;
  paramTest.Visible         = 1;
  paramTest.Active          = 1;
  paramTest.Transparent     = 1;
  paramTest.Caption         = paramTest_Caption;
  paramTest.TextAlign       = _taCenter;
  paramTest.FontName        = Arial19x22_Bold;
  paramTest.PressColEnabled = 1;
  paramTest.Font_Color      = 0x0000;
  paramTest.Gradient        = 1;
  paramTest.Gradient_Orientation    = 0;
  paramTest.Gradient_Start_Color    = 0xFFFF;
  paramTest.Gradient_End_Color      = 0xC618;
  paramTest.Color           = 0xC618;
  paramTest.Press_Color     = 0x8410;
  paramTest.Corner_Radius   = 3;
  paramTest.OnUpPtr         = 0;
  paramTest.OnDownPtr       = 0;
  paramTest.OnClickPtr      = paramTestiranje;
  paramTest.OnPressPtr      = 0;

  brojParam.OwnerScreenID   = 32772;
  brojParam.Order           = 9;
  brojParam.Left            = 154;
  brojParam.Top             = 38;
  brojParam.Width           = 24;
  brojParam.Height          = 26;
  brojParam.Visible         = 1;
  brojParam.Active          = 1;
  brojParam.Caption         = brojParam_Caption;
  brojParam.FontName        = Arial21x24_Bold;
  brojParam.Font_Color      = 0x6000;
  brojParam.OnUpPtr         = 0;
  brojParam.OnDownPtr       = 0;
  brojParam.OnClickPtr      = 0;
  brojParam.OnPressPtr      = 0;

  paramVrednost.OwnerScreenID   = 32772;
  paramVrednost.Order           = 10;
  paramVrednost.Left            = 99;
  paramVrednost.Top             = 130;
  paramVrednost.Width           = 105;
  paramVrednost.Height          = 26;
  paramVrednost.Visible         = 1;
  paramVrednost.Active          = 1;
  paramVrednost.Caption         = paramVrednost_Caption;
  paramVrednost.FontName        = Arial21x24_Bold;
  paramVrednost.Font_Color      = 0x6000;
  paramVrednost.OnUpPtr         = 0;
  paramVrednost.OnDownPtr       = 0;
  paramVrednost.OnClickPtr      = 0;
  paramVrednost.OnPressPtr      = 0;

  boksGreska.OwnerScreenID   = 32773;
  boksGreska.Order           = 0;
  boksGreska.Left            = 3;
  boksGreska.Top             = 4;
  boksGreska.Width           = 314;
  boksGreska.Height          = 188;
  boksGreska.Pen_Width       = 1;
  boksGreska.Pen_Color       = 0x0000;
  boksGreska.Visible         = 1;
  boksGreska.Active          = 1;
  boksGreska.Transparent     = 1;
  boksGreska.Gradient        = 0;
  boksGreska.Gradient_Orientation    = 0;
  boksGreska.Gradient_Start_Color    = 0xFFFF;
  boksGreska.Gradient_End_Color      = 0xC618;
  boksGreska.Color           = 0xFFFF;
  boksGreska.PressColEnabled     = 1;
  boksGreska.Press_Color     = 0x8410;
  boksGreska.OnUpPtr         = 0;
  boksGreska.OnDownPtr       = 0;
  boksGreska.OnClickPtr      = 0;
  boksGreska.OnPressPtr      = 0;

  LabelGR1.OwnerScreenID   = 32773;
  LabelGR1.Order           = 1;
  LabelGR1.Left            = 30;
  LabelGR1.Top             = 9;
  LabelGR1.Width           = 252;
  LabelGR1.Height          = 26;
  LabelGR1.Visible         = 1;
  LabelGR1.Active          = 1;
  LabelGR1.Caption         = LabelGR1_Caption;
  LabelGR1.FontName        = Arial21x24_Bold;
  LabelGR1.Font_Color      = 0x6000;
  LabelGR1.OnUpPtr         = 0;
  LabelGR1.OnDownPtr       = 0;
  LabelGR1.OnClickPtr      = 0;
  LabelGR1.OnPressPtr      = 0;

  LabelGR2.OwnerScreenID   = 32773;
  LabelGR2.Order           = 2;
  LabelGR2.Left            = 10;
  LabelGR2.Top             = 40;
  LabelGR2.Width           = 252;
  LabelGR2.Height          = 26;
  LabelGR2.Visible         = 1;
  LabelGR2.Active          = 1;
  LabelGR2.Caption         = LabelGR2_Caption;
  LabelGR2.FontName        = Arial21x24_Bold;
  LabelGR2.Font_Color      = 0x6000;
  LabelGR2.OnUpPtr         = 0;
  LabelGR2.OnDownPtr       = 0;
  LabelGR2.OnClickPtr      = 0;
  LabelGR2.OnPressPtr      = 0;

  LabelGR3.OwnerScreenID   = 32773;
  LabelGR3.Order           = 3;
  LabelGR3.Left            = 10;
  LabelGR3.Top             = 65;
  LabelGR3.Width           = 252;
  LabelGR3.Height          = 26;
  LabelGR3.Visible         = 1;
  LabelGR3.Active          = 1;
  LabelGR3.Caption         = LabelGR3_Caption;
  LabelGR3.FontName        = Arial21x24_Bold;
  LabelGR3.Font_Color      = 0x6000;
  LabelGR3.OnUpPtr         = 0;
  LabelGR3.OnDownPtr       = 0;
  LabelGR3.OnClickPtr      = 0;
  LabelGR3.OnPressPtr      = 0;

  LabelGR4.OwnerScreenID   = 32773;
  LabelGR4.Order           = 4;
  LabelGR4.Left            = 10;
  LabelGR4.Top             = 90;
  LabelGR4.Width           = 252;
  LabelGR4.Height          = 26;
  LabelGR4.Visible         = 1;
  LabelGR4.Active          = 1;
  LabelGR4.Caption         = LabelGR4_Caption;
  LabelGR4.FontName        = Arial21x24_Bold;
  LabelGR4.Font_Color      = 0x6000;
  LabelGR4.OnUpPtr         = 0;
  LabelGR4.OnDownPtr       = 0;
  LabelGR4.OnClickPtr      = 0;
  LabelGR4.OnPressPtr      = 0;

  LabelGR5.OwnerScreenID   = 32773;
  LabelGR5.Order           = 5;
  LabelGR5.Left            = 10;
  LabelGR5.Top             = 115;
  LabelGR5.Width           = 252;
  LabelGR5.Height          = 26;
  LabelGR5.Visible         = 1;
  LabelGR5.Active          = 1;
  LabelGR5.Caption         = LabelGR5_Caption;
  LabelGR5.FontName        = Arial21x24_Bold;
  LabelGR5.Font_Color      = 0x6000;
  LabelGR5.OnUpPtr         = 0;
  LabelGR5.OnDownPtr       = 0;
  LabelGR5.OnClickPtr      = 0;
  LabelGR5.OnPressPtr      = 0;

  LabelGR6.OwnerScreenID   = 32773;
  LabelGR6.Order           = 6;
  LabelGR6.Left            = 10;
  LabelGR6.Top             = 140;
  LabelGR6.Width           = 252;
  LabelGR6.Height          = 26;
  LabelGR6.Visible         = 1;
  LabelGR6.Active          = 1;
  LabelGR6.Caption         = LabelGR6_Caption;
  LabelGR6.FontName        = Arial21x24_Bold;
  LabelGR6.Font_Color      = 0x6000;
  LabelGR6.OnUpPtr         = 0;
  LabelGR6.OnDownPtr       = 0;
  LabelGR6.OnClickPtr      = 0;
  LabelGR6.OnPressPtr      = 0;

  LabelGR7.OwnerScreenID   = 32773;
  LabelGR7.Order           = 7;
  LabelGR7.Left            = 10;
  LabelGR7.Top             = 165;
  LabelGR7.Width           = 252;
  LabelGR7.Height          = 26;
  LabelGR7.Visible         = 1;
  LabelGR7.Active          = 1;
  LabelGR7.Caption         = LabelGR7_Caption;
  LabelGR7.FontName        = Arial21x24_Bold;
  LabelGR7.Font_Color      = 0x6000;
  LabelGR7.OnUpPtr         = 0;
  LabelGR7.OnDownPtr       = 0;
  LabelGR7.OnClickPtr      = 0;
  LabelGR7.OnPressPtr      = 0;

  popravkaButon.OwnerScreenID   = 32773;
  popravkaButon.Order           = 8;
  popravkaButon.Left            = 88;
  popravkaButon.Top             = 204;
  popravkaButon.Width           = 144;
  popravkaButon.Height          = 27;
  popravkaButon.Pen_Width       = 1;
  popravkaButon.Pen_Color       = 0x0000;
  popravkaButon.Visible         = 1;
  popravkaButon.Active          = 1;
  popravkaButon.Transparent     = 1;
  popravkaButon.Caption         = popravkaButon_Caption;
  popravkaButon.TextAlign       = _taCenter;
  popravkaButon.FontName        = Arial19x22_Bold;
  popravkaButon.PressColEnabled = 1;
  popravkaButon.Font_Color      = 0x0000;
  popravkaButon.Gradient        = 1;
  popravkaButon.Gradient_Orientation    = 0;
  popravkaButon.Gradient_Start_Color    = 0xFFFF;
  popravkaButon.Gradient_End_Color      = 0xC618;
  popravkaButon.Color           = 0xC618;
  popravkaButon.Press_Color     = 0x8410;
  popravkaButon.Corner_Radius   = 3;
  popravkaButon.OnUpPtr         = 0;
  popravkaButon.OnDownPtr       = 0;
  popravkaButon.OnClickPtr      = popraviti;
  popravkaButon.OnPressPtr      = 0;

  satExit.OwnerScreenID   = 32774;
  satExit.Order           = 0;
  satExit.Left            = 267;
  satExit.Top             = 3;
  satExit.Radius          = 25;
  satExit.Pen_Width       = 1;
  satExit.Pen_Color       = 0x0000;
  satExit.Visible         = 1;
  satExit.Active          = 1;
  satExit.Transparent     = 1;
  satExit.Caption         = satExit_Caption;
  satExit.TextAlign       = _taCenter;
  satExit.FontName        = Arial23x29_Bold;
  satExit.PressColEnabled = 1;
  satExit.Font_Color     = 0x0000;
  satExit.Gradient       = 1;
  satExit.Gradient_Orientation    = 0;
  satExit.Gradient_Start_Color    = 0xFC10;
  satExit.Gradient_End_Color      = 0xF800;
  satExit.Color          = 0xC618;
  satExit.Press_Color    = 0x8410;
  satExit.OnUpPtr         = 0;
  satExit.OnDownPtr       = 0;
  satExit.OnClickPtr      = nazadSat;
  satExit.OnPressPtr      = 0;

  boksSati.OwnerScreenID   = 32774;
  boksSati.Order           = 1;
  boksSati.Left            = 82;
  boksSati.Top             = 46;
  boksSati.Width           = 50;
  boksSati.Height          = 25;
  boksSati.Pen_Width       = 1;
  boksSati.Pen_Color       = 0x0000;
  boksSati.Visible         = 1;
  boksSati.Active          = 1;
  boksSati.Transparent     = 1;
  boksSati.Gradient        = 0;
  boksSati.Gradient_Orientation    = 0;
  boksSati.Gradient_Start_Color    = 0xFFFF;
  boksSati.Gradient_End_Color      = 0xC618;
  boksSati.Color           = 0xFFFF;
  boksSati.PressColEnabled     = 1;
  boksSati.Press_Color     = 0x8410;
  boksSati.OnUpPtr         = 0;
  boksSati.OnDownPtr       = 0;
  boksSati.OnClickPtr      = 0;
  boksSati.OnPressPtr      = 0;

  tSatiPlus.OwnerScreenID   = 32774;
  tSatiPlus.Order           = 2;
  tSatiPlus.Left            = 82;
  tSatiPlus.Top             = 4;
  tSatiPlus.Width           = 50;
  tSatiPlus.Height          = 40;
  tSatiPlus.Pen_Width       = 1;
  tSatiPlus.Pen_Color       = 0x0000;
  tSatiPlus.Visible         = 1;
  tSatiPlus.Active          = 1;
  tSatiPlus.Transparent     = 1;
  tSatiPlus.Caption         = tSatiPlus_Caption;
  tSatiPlus.TextAlign       = _taCenter;
  tSatiPlus.FontName        = Arial19x22_Bold;
  tSatiPlus.PressColEnabled = 1;
  tSatiPlus.Font_Color      = 0x0000;
  tSatiPlus.Gradient        = 1;
  tSatiPlus.Gradient_Orientation    = 0;
  tSatiPlus.Gradient_Start_Color    = 0xFFFF;
  tSatiPlus.Gradient_End_Color      = 0xC618;
  tSatiPlus.Color           = 0xC618;
  tSatiPlus.Press_Color     = 0x8410;
  tSatiPlus.Corner_Radius   = 3;
  tSatiPlus.OnUpPtr         = 0;
  tSatiPlus.OnDownPtr       = 0;
  tSatiPlus.OnClickPtr      = 0;
  tSatiPlus.OnPressPtr      = satPlus;

  tsatiMinus.OwnerScreenID   = 32774;
  tsatiMinus.Order           = 3;
  tsatiMinus.Left            = 82;
  tsatiMinus.Top             = 73;
  tsatiMinus.Width           = 50;
  tsatiMinus.Height          = 40;
  tsatiMinus.Pen_Width       = 1;
  tsatiMinus.Pen_Color       = 0x0000;
  tsatiMinus.Visible         = 1;
  tsatiMinus.Active          = 1;
  tsatiMinus.Transparent     = 1;
  tsatiMinus.Caption         = tsatiMinus_Caption;
  tsatiMinus.TextAlign       = _taCenter;
  tsatiMinus.FontName        = Arial19x22_Bold;
  tsatiMinus.PressColEnabled = 1;
  tsatiMinus.Font_Color      = 0x0000;
  tsatiMinus.Gradient        = 1;
  tsatiMinus.Gradient_Orientation    = 0;
  tsatiMinus.Gradient_Start_Color    = 0xFFFF;
  tsatiMinus.Gradient_End_Color      = 0xC618;
  tsatiMinus.Color           = 0xC618;
  tsatiMinus.Press_Color     = 0x8410;
  tsatiMinus.Corner_Radius   = 3;
  tsatiMinus.OnUpPtr         = 0;
  tsatiMinus.OnDownPtr       = 0;
  tsatiMinus.OnClickPtr      = 0;
  tsatiMinus.OnPressPtr      = satMinus;

  labelSati.OwnerScreenID   = 32774;
  labelSati.Order           = 4;
  labelSati.Left            = 95;
  labelSati.Top             = 46;
  labelSati.Width           = 24;
  labelSati.Height          = 26;
  labelSati.Visible         = 1;
  labelSati.Active          = 1;
  labelSati.Caption         = labelSati_Caption;
  labelSati.FontName        = Arial21x24_Bold;
  labelSati.Font_Color      = 0x6000;
  labelSati.OnUpPtr         = 0;
  labelSati.OnDownPtr       = 0;
  labelSati.OnClickPtr      = 0;
  labelSati.OnPressPtr      = 0;

  boksMinuti.OwnerScreenID   = 32774;
  boksMinuti.Order           = 5;
  boksMinuti.Left            = 178;
  boksMinuti.Top             = 46;
  boksMinuti.Width           = 50;
  boksMinuti.Height          = 25;
  boksMinuti.Pen_Width       = 1;
  boksMinuti.Pen_Color       = 0x0000;
  boksMinuti.Visible         = 1;
  boksMinuti.Active          = 1;
  boksMinuti.Transparent     = 1;
  boksMinuti.Gradient        = 0;
  boksMinuti.Gradient_Orientation    = 0;
  boksMinuti.Gradient_Start_Color    = 0xFFFF;
  boksMinuti.Gradient_End_Color      = 0xC618;
  boksMinuti.Color           = 0xFFFF;
  boksMinuti.PressColEnabled     = 1;
  boksMinuti.Press_Color     = 0x8410;
  boksMinuti.OnUpPtr         = 0;
  boksMinuti.OnDownPtr       = 0;
  boksMinuti.OnClickPtr      = 0;
  boksMinuti.OnPressPtr      = 0;

  tMinutiPlus.OwnerScreenID   = 32774;
  tMinutiPlus.Order           = 6;
  tMinutiPlus.Left            = 178;
  tMinutiPlus.Top             = 4;
  tMinutiPlus.Width           = 50;
  tMinutiPlus.Height          = 40;
  tMinutiPlus.Pen_Width       = 1;
  tMinutiPlus.Pen_Color       = 0x0000;
  tMinutiPlus.Visible         = 1;
  tMinutiPlus.Active          = 1;
  tMinutiPlus.Transparent     = 1;
  tMinutiPlus.Caption         = tMinutiPlus_Caption;
  tMinutiPlus.TextAlign       = _taCenter;
  tMinutiPlus.FontName        = Arial19x22_Bold;
  tMinutiPlus.PressColEnabled = 1;
  tMinutiPlus.Font_Color      = 0x0000;
  tMinutiPlus.Gradient        = 1;
  tMinutiPlus.Gradient_Orientation    = 0;
  tMinutiPlus.Gradient_Start_Color    = 0xFFFF;
  tMinutiPlus.Gradient_End_Color      = 0xC618;
  tMinutiPlus.Color           = 0xC618;
  tMinutiPlus.Press_Color     = 0x8410;
  tMinutiPlus.Corner_Radius   = 3;
  tMinutiPlus.OnUpPtr         = 0;
  tMinutiPlus.OnDownPtr       = 0;
  tMinutiPlus.OnClickPtr      = 0;
  tMinutiPlus.OnPressPtr      = minutiPlus;

  tMinutiMinus.OwnerScreenID   = 32774;
  tMinutiMinus.Order           = 7;
  tMinutiMinus.Left            = 178;
  tMinutiMinus.Top             = 73;
  tMinutiMinus.Width           = 50;
  tMinutiMinus.Height          = 40;
  tMinutiMinus.Pen_Width       = 1;
  tMinutiMinus.Pen_Color       = 0x0000;
  tMinutiMinus.Visible         = 1;
  tMinutiMinus.Active          = 1;
  tMinutiMinus.Transparent     = 1;
  tMinutiMinus.Caption         = tMinutiMinus_Caption;
  tMinutiMinus.TextAlign       = _taCenter;
  tMinutiMinus.FontName        = Arial19x22_Bold;
  tMinutiMinus.PressColEnabled = 1;
  tMinutiMinus.Font_Color      = 0x0000;
  tMinutiMinus.Gradient        = 1;
  tMinutiMinus.Gradient_Orientation    = 0;
  tMinutiMinus.Gradient_Start_Color    = 0xFFFF;
  tMinutiMinus.Gradient_End_Color      = 0xC618;
  tMinutiMinus.Color           = 0xC618;
  tMinutiMinus.Press_Color     = 0x8410;
  tMinutiMinus.Corner_Radius   = 3;
  tMinutiMinus.OnUpPtr         = 0;
  tMinutiMinus.OnDownPtr       = 0;
  tMinutiMinus.OnClickPtr      = 0;
  tMinutiMinus.OnPressPtr      = minutiMinus;

  labelMinuti.OwnerScreenID   = 32774;
  labelMinuti.Order           = 8;
  labelMinuti.Left            = 191;
  labelMinuti.Top             = 46;
  labelMinuti.Width           = 24;
  labelMinuti.Height          = 26;
  labelMinuti.Visible         = 1;
  labelMinuti.Active          = 1;
  labelMinuti.Caption         = labelMinuti_Caption;
  labelMinuti.FontName        = Arial21x24_Bold;
  labelMinuti.Font_Color      = 0x6000;
  labelMinuti.OnUpPtr         = 0;
  labelMinuti.OnDownPtr       = 0;
  labelMinuti.OnClickPtr      = 0;
  labelMinuti.OnPressPtr      = 0;

  dveTacke.OwnerScreenID   = 32774;
  dveTacke.Order           = 9;
  dveTacke.Left            = 152;
  dveTacke.Top             = 45;
  dveTacke.Width           = 7;
  dveTacke.Height          = 26;
  dveTacke.Visible         = 1;
  dveTacke.Active          = 1;
  dveTacke.Caption         = dveTacke_Caption;
  dveTacke.FontName        = Arial21x24_Bold;
  dveTacke.Font_Color      = 0x6000;
  dveTacke.OnUpPtr         = 0;
  dveTacke.OnDownPtr       = 0;
  dveTacke.OnClickPtr      = 0;
  dveTacke.OnPressPtr      = 0;

  boksDani.OwnerScreenID   = 32774;
  boksDani.Order           = 10;
  boksDani.Left            = 36;
  boksDani.Top             = 163;
  boksDani.Width           = 50;
  boksDani.Height          = 25;
  boksDani.Pen_Width       = 1;
  boksDani.Pen_Color       = 0x0000;
  boksDani.Visible         = 1;
  boksDani.Active          = 1;
  boksDani.Transparent     = 1;
  boksDani.Gradient        = 0;
  boksDani.Gradient_Orientation    = 0;
  boksDani.Gradient_Start_Color    = 0xFFFF;
  boksDani.Gradient_End_Color      = 0xC618;
  boksDani.Color           = 0xFFFF;
  boksDani.PressColEnabled     = 1;
  boksDani.Press_Color     = 0x8410;
  boksDani.OnUpPtr         = 0;
  boksDani.OnDownPtr       = 0;
  boksDani.OnClickPtr      = 0;
  boksDani.OnPressPtr      = 0;

  tDaniPlus.OwnerScreenID   = 32774;
  tDaniPlus.Order           = 11;
  tDaniPlus.Left            = 36;
  tDaniPlus.Top             = 121;
  tDaniPlus.Width           = 50;
  tDaniPlus.Height          = 40;
  tDaniPlus.Pen_Width       = 1;
  tDaniPlus.Pen_Color       = 0x0000;
  tDaniPlus.Visible         = 1;
  tDaniPlus.Active          = 1;
  tDaniPlus.Transparent     = 1;
  tDaniPlus.Caption         = tDaniPlus_Caption;
  tDaniPlus.TextAlign       = _taCenter;
  tDaniPlus.FontName        = Arial19x22_Bold;
  tDaniPlus.PressColEnabled = 1;
  tDaniPlus.Font_Color      = 0x0000;
  tDaniPlus.Gradient        = 1;
  tDaniPlus.Gradient_Orientation    = 0;
  tDaniPlus.Gradient_Start_Color    = 0xFFFF;
  tDaniPlus.Gradient_End_Color      = 0xC618;
  tDaniPlus.Color           = 0xC618;
  tDaniPlus.Press_Color     = 0x8410;
  tDaniPlus.Corner_Radius   = 3;
  tDaniPlus.OnUpPtr         = 0;
  tDaniPlus.OnDownPtr       = 0;
  tDaniPlus.OnClickPtr      = 0;
  tDaniPlus.OnPressPtr      = danPlus;

  tDaniMinus.OwnerScreenID   = 32774;
  tDaniMinus.Order           = 12;
  tDaniMinus.Left            = 36;
  tDaniMinus.Top             = 190;
  tDaniMinus.Width           = 50;
  tDaniMinus.Height          = 40;
  tDaniMinus.Pen_Width       = 1;
  tDaniMinus.Pen_Color       = 0x0000;
  tDaniMinus.Visible         = 1;
  tDaniMinus.Active          = 1;
  tDaniMinus.Transparent     = 1;
  tDaniMinus.Caption         = tDaniMinus_Caption;
  tDaniMinus.TextAlign       = _taCenter;
  tDaniMinus.FontName        = Arial19x22_Bold;
  tDaniMinus.PressColEnabled = 1;
  tDaniMinus.Font_Color      = 0x0000;
  tDaniMinus.Gradient        = 1;
  tDaniMinus.Gradient_Orientation    = 0;
  tDaniMinus.Gradient_Start_Color    = 0xFFFF;
  tDaniMinus.Gradient_End_Color      = 0xC618;
  tDaniMinus.Color           = 0xC618;
  tDaniMinus.Press_Color     = 0x8410;
  tDaniMinus.Corner_Radius   = 3;
  tDaniMinus.OnUpPtr         = 0;
  tDaniMinus.OnDownPtr       = 0;
  tDaniMinus.OnClickPtr      = 0;
  tDaniMinus.OnPressPtr      = danMinus;

  labelDani.OwnerScreenID   = 32774;
  labelDani.Order           = 13;
  labelDani.Left            = 49;
  labelDani.Top             = 163;
  labelDani.Width           = 24;
  labelDani.Height          = 26;
  labelDani.Visible         = 1;
  labelDani.Active          = 1;
  labelDani.Caption         = labelDani_Caption;
  labelDani.FontName        = Arial21x24_Bold;
  labelDani.Font_Color      = 0x6000;
  labelDani.OnUpPtr         = 0;
  labelDani.OnDownPtr       = 0;
  labelDani.OnClickPtr      = 0;
  labelDani.OnPressPtr      = 0;

  boksMesec.OwnerScreenID   = 32774;
  boksMesec.Order           = 14;
  boksMesec.Left            = 132;
  boksMesec.Top             = 162;
  boksMesec.Width           = 50;
  boksMesec.Height          = 25;
  boksMesec.Pen_Width       = 1;
  boksMesec.Pen_Color       = 0x0000;
  boksMesec.Visible         = 1;
  boksMesec.Active          = 1;
  boksMesec.Transparent     = 1;
  boksMesec.Gradient        = 0;
  boksMesec.Gradient_Orientation    = 0;
  boksMesec.Gradient_Start_Color    = 0xFFFF;
  boksMesec.Gradient_End_Color      = 0xC618;
  boksMesec.Color           = 0xFFFF;
  boksMesec.PressColEnabled     = 1;
  boksMesec.Press_Color     = 0x8410;
  boksMesec.OnUpPtr         = 0;
  boksMesec.OnDownPtr       = 0;
  boksMesec.OnClickPtr      = 0;
  boksMesec.OnPressPtr      = 0;

  tMeseciPlus.OwnerScreenID   = 32774;
  tMeseciPlus.Order           = 15;
  tMeseciPlus.Left            = 132;
  tMeseciPlus.Top             = 120;
  tMeseciPlus.Width           = 50;
  tMeseciPlus.Height          = 40;
  tMeseciPlus.Pen_Width       = 1;
  tMeseciPlus.Pen_Color       = 0x0000;
  tMeseciPlus.Visible         = 1;
  tMeseciPlus.Active          = 1;
  tMeseciPlus.Transparent     = 1;
  tMeseciPlus.Caption         = tMeseciPlus_Caption;
  tMeseciPlus.TextAlign       = _taCenter;
  tMeseciPlus.FontName        = Arial19x22_Bold;
  tMeseciPlus.PressColEnabled = 1;
  tMeseciPlus.Font_Color      = 0x0000;
  tMeseciPlus.Gradient        = 1;
  tMeseciPlus.Gradient_Orientation    = 0;
  tMeseciPlus.Gradient_Start_Color    = 0xFFFF;
  tMeseciPlus.Gradient_End_Color      = 0xC618;
  tMeseciPlus.Color           = 0xC618;
  tMeseciPlus.Press_Color     = 0x8410;
  tMeseciPlus.Corner_Radius   = 3;
  tMeseciPlus.OnUpPtr         = 0;
  tMeseciPlus.OnDownPtr       = 0;
  tMeseciPlus.OnClickPtr      = 0;
  tMeseciPlus.OnPressPtr      = mesecPlus;

  tMeseciMinus.OwnerScreenID   = 32774;
  tMeseciMinus.Order           = 16;
  tMeseciMinus.Left            = 132;
  tMeseciMinus.Top             = 189;
  tMeseciMinus.Width           = 50;
  tMeseciMinus.Height          = 40;
  tMeseciMinus.Pen_Width       = 1;
  tMeseciMinus.Pen_Color       = 0x0000;
  tMeseciMinus.Visible         = 1;
  tMeseciMinus.Active          = 1;
  tMeseciMinus.Transparent     = 1;
  tMeseciMinus.Caption         = tMeseciMinus_Caption;
  tMeseciMinus.TextAlign       = _taCenter;
  tMeseciMinus.FontName        = Arial19x22_Bold;
  tMeseciMinus.PressColEnabled = 1;
  tMeseciMinus.Font_Color      = 0x0000;
  tMeseciMinus.Gradient        = 1;
  tMeseciMinus.Gradient_Orientation    = 0;
  tMeseciMinus.Gradient_Start_Color    = 0xFFFF;
  tMeseciMinus.Gradient_End_Color      = 0xC618;
  tMeseciMinus.Color           = 0xC618;
  tMeseciMinus.Press_Color     = 0x8410;
  tMeseciMinus.Corner_Radius   = 3;
  tMeseciMinus.OnUpPtr         = 0;
  tMeseciMinus.OnDownPtr       = 0;
  tMeseciMinus.OnClickPtr      = 0;
  tMeseciMinus.OnPressPtr      = mesecMinus;

  labelMesec.OwnerScreenID   = 32774;
  labelMesec.Order           = 17;
  labelMesec.Left            = 135;
  labelMesec.Top             = 163;
  labelMesec.Width           = 43;
  labelMesec.Height          = 26;
  labelMesec.Visible         = 1;
  labelMesec.Active          = 1;
  labelMesec.Caption         = labelMesec_Caption;
  labelMesec.FontName        = Arial21x24_Bold;
  labelMesec.Font_Color      = 0x6000;
  labelMesec.OnUpPtr         = 0;
  labelMesec.OnDownPtr       = 0;
  labelMesec.OnClickPtr      = 0;
  labelMesec.OnPressPtr      = 0;

  crtica1.OwnerScreenID   = 32774;
  crtica1.Order           = 18;
  crtica1.Left            = 106;
  crtica1.Top             = 160;
  crtica1.Width           = 7;
  crtica1.Height          = 26;
  crtica1.Visible         = 1;
  crtica1.Active          = 1;
  crtica1.Caption         = crtica1_Caption;
  crtica1.FontName        = Arial21x24_Bold;
  crtica1.Font_Color      = 0x6000;
  crtica1.OnUpPtr         = 0;
  crtica1.OnDownPtr       = 0;
  crtica1.OnClickPtr      = 0;
  crtica1.OnPressPtr      = 0;

  crtica2.OwnerScreenID   = 32774;
  crtica2.Order           = 19;
  crtica2.Left            = 200;
  crtica2.Top             = 161;
  crtica2.Width           = 7;
  crtica2.Height          = 26;
  crtica2.Visible         = 1;
  crtica2.Active          = 1;
  crtica2.Caption         = crtica2_Caption;
  crtica2.FontName        = Arial21x24_Bold;
  crtica2.Font_Color      = 0x6000;
  crtica2.OnUpPtr         = 0;
  crtica2.OnDownPtr       = 0;
  crtica2.OnClickPtr      = 0;
  crtica2.OnPressPtr      = 0;

  boksGodina.OwnerScreenID   = 32774;
  boksGodina.Order           = 20;
  boksGodina.Left            = 251;
  boksGodina.Top             = 162;
  boksGodina.Width           = 50;
  boksGodina.Height          = 25;
  boksGodina.Pen_Width       = 1;
  boksGodina.Pen_Color       = 0x0000;
  boksGodina.Visible         = 1;
  boksGodina.Active          = 1;
  boksGodina.Transparent     = 1;
  boksGodina.Gradient        = 0;
  boksGodina.Gradient_Orientation    = 0;
  boksGodina.Gradient_Start_Color    = 0xFFFF;
  boksGodina.Gradient_End_Color      = 0xC618;
  boksGodina.Color           = 0xFFFF;
  boksGodina.PressColEnabled     = 1;
  boksGodina.Press_Color     = 0x8410;
  boksGodina.OnUpPtr         = 0;
  boksGodina.OnDownPtr       = 0;
  boksGodina.OnClickPtr      = 0;
  boksGodina.OnPressPtr      = 0;

  tGodinaPlus.OwnerScreenID   = 32774;
  tGodinaPlus.Order           = 21;
  tGodinaPlus.Left            = 251;
  tGodinaPlus.Top             = 120;
  tGodinaPlus.Width           = 50;
  tGodinaPlus.Height          = 40;
  tGodinaPlus.Pen_Width       = 1;
  tGodinaPlus.Pen_Color       = 0x0000;
  tGodinaPlus.Visible         = 1;
  tGodinaPlus.Active          = 1;
  tGodinaPlus.Transparent     = 1;
  tGodinaPlus.Caption         = tGodinaPlus_Caption;
  tGodinaPlus.TextAlign       = _taCenter;
  tGodinaPlus.FontName        = Arial19x22_Bold;
  tGodinaPlus.PressColEnabled = 1;
  tGodinaPlus.Font_Color      = 0x0000;
  tGodinaPlus.Gradient        = 1;
  tGodinaPlus.Gradient_Orientation    = 0;
  tGodinaPlus.Gradient_Start_Color    = 0xFFFF;
  tGodinaPlus.Gradient_End_Color      = 0xC618;
  tGodinaPlus.Color           = 0xC618;
  tGodinaPlus.Press_Color     = 0x8410;
  tGodinaPlus.Corner_Radius   = 3;
  tGodinaPlus.OnUpPtr         = 0;
  tGodinaPlus.OnDownPtr       = 0;
  tGodinaPlus.OnClickPtr      = 0;
  tGodinaPlus.OnPressPtr      = godinaPlus;

  tGodinaMinus.OwnerScreenID   = 32774;
  tGodinaMinus.Order           = 22;
  tGodinaMinus.Left            = 251;
  tGodinaMinus.Top             = 189;
  tGodinaMinus.Width           = 50;
  tGodinaMinus.Height          = 40;
  tGodinaMinus.Pen_Width       = 1;
  tGodinaMinus.Pen_Color       = 0x0000;
  tGodinaMinus.Visible         = 1;
  tGodinaMinus.Active          = 1;
  tGodinaMinus.Transparent     = 1;
  tGodinaMinus.Caption         = tGodinaMinus_Caption;
  tGodinaMinus.TextAlign       = _taCenter;
  tGodinaMinus.FontName        = Arial19x22_Bold;
  tGodinaMinus.PressColEnabled = 1;
  tGodinaMinus.Font_Color      = 0x0000;
  tGodinaMinus.Gradient        = 1;
  tGodinaMinus.Gradient_Orientation    = 0;
  tGodinaMinus.Gradient_Start_Color    = 0xFFFF;
  tGodinaMinus.Gradient_End_Color      = 0xC618;
  tGodinaMinus.Color           = 0xC618;
  tGodinaMinus.Press_Color     = 0x8410;
  tGodinaMinus.Corner_Radius   = 3;
  tGodinaMinus.OnUpPtr         = 0;
  tGodinaMinus.OnDownPtr       = 0;
  tGodinaMinus.OnClickPtr      = 0;
  tGodinaMinus.OnPressPtr      = godinaMinus;

  labelGodina.OwnerScreenID   = 32774;
  labelGodina.Order           = 23;
  labelGodina.Left            = 264;
  labelGodina.Top             = 163;
  labelGodina.Width           = 24;
  labelGodina.Height          = 26;
  labelGodina.Visible         = 1;
  labelGodina.Active          = 1;
  labelGodina.Caption         = labelGodina_Caption;
  labelGodina.FontName        = Arial21x24_Bold;
  labelGodina.Font_Color      = 0x6000;
  labelGodina.OnUpPtr         = 0;
  labelGodina.OnDownPtr       = 0;
  labelGodina.OnClickPtr      = 0;
  labelGodina.OnPressPtr      = 0;

  god20.OwnerScreenID   = 32774;
  god20.Order           = 24;
  god20.Left            = 223;
  god20.Top             = 163;
  god20.Width           = 24;
  god20.Height          = 26;
  god20.Visible         = 1;
  god20.Active          = 1;
  god20.Caption         = god20_Caption;
  god20.FontName        = Arial21x24_Bold;
  god20.Font_Color      = 0x6000;
  god20.OnUpPtr         = 0;
  god20.OnDownPtr       = 0;
  god20.OnClickPtr      = 0;
  god20.OnPressPtr      = 0;

  pozdravPor1.OwnerScreenID   = 32775;
  pozdravPor1.Order           = 0;
  pozdravPor1.Left            = 114;
  pozdravPor1.Top             = 75;
  pozdravPor1.Width           = 84;
  pozdravPor1.Height          = 30;
  pozdravPor1.Visible         = 1;
  pozdravPor1.Active          = 1;
  pozdravPor1.Caption         = pozdravPor1_Caption;
  pozdravPor1.FontName        = Arial23x29_Bold;
  pozdravPor1.Font_Color      = 0x0000;
  pozdravPor1.OnUpPtr         = 0;
  pozdravPor1.OnDownPtr       = 0;
  pozdravPor1.OnClickPtr      = 0;
  pozdravPor1.OnPressPtr      = 0;

  pozdravPor2.OwnerScreenID   = 32775;
  pozdravPor2.Order           = 1;
  pozdravPor2.Left            = 29;
  pozdravPor2.Top             = 115;
  pozdravPor2.Width           = 254;
  pozdravPor2.Height          = 30;
  pozdravPor2.Visible         = 1;
  pozdravPor2.Active          = 1;
  pozdravPor2.Caption         = pozdravPor2_Caption;
  pozdravPor2.FontName        = Arial23x29_Bold;
  pozdravPor2.Font_Color      = 0x0000;
  pozdravPor2.OnUpPtr         = 0;
  pozdravPor2.OnDownPtr       = 0;
  pozdravPor2.OnClickPtr      = 0;
  pozdravPor2.OnPressPtr      = 0;

  hronoExit.OwnerScreenID   = 32776;
  hronoExit.Order           = 0;
  hronoExit.Left            = 267;
  hronoExit.Top             = 3;
  hronoExit.Radius          = 25;
  hronoExit.Pen_Width       = 1;
  hronoExit.Pen_Color       = 0x0000;
  hronoExit.Visible         = 1;
  hronoExit.Active          = 1;
  hronoExit.Transparent     = 1;
  hronoExit.Caption         = hronoExit_Caption;
  hronoExit.TextAlign       = _taCenter;
  hronoExit.FontName        = Arial23x29_Bold;
  hronoExit.PressColEnabled = 1;
  hronoExit.Font_Color     = 0x0000;
  hronoExit.Gradient       = 1;
  hronoExit.Gradient_Orientation    = 0;
  hronoExit.Gradient_Start_Color    = 0xFC10;
  hronoExit.Gradient_End_Color      = 0xF800;
  hronoExit.Color          = 0xC618;
  hronoExit.Press_Color    = 0x8410;
  hronoExit.OnUpPtr         = 0;
  hronoExit.OnDownPtr       = 0;
  hronoExit.OnClickPtr      = nazadHrono;
  hronoExit.OnPressPtr      = 0;

  boksDanHrono.OwnerScreenID   = 32776;
  boksDanHrono.Order           = 1;
  boksDanHrono.Left            = 102;
  boksDanHrono.Top             = 8;
  boksDanHrono.Width           = 50;
  boksDanHrono.Height          = 40;
  boksDanHrono.Pen_Width       = 1;
  boksDanHrono.Pen_Color       = 0x0000;
  boksDanHrono.Visible         = 1;
  boksDanHrono.Active          = 1;
  boksDanHrono.Transparent     = 1;
  boksDanHrono.Gradient        = 0;
  boksDanHrono.Gradient_Orientation    = 0;
  boksDanHrono.Gradient_Start_Color    = 0xFFFF;
  boksDanHrono.Gradient_End_Color      = 0xC618;
  boksDanHrono.Color           = 0xFFFF;
  boksDanHrono.PressColEnabled     = 1;
  boksDanHrono.Press_Color     = 0x8410;
  boksDanHrono.OnUpPtr         = 0;
  boksDanHrono.OnDownPtr       = 0;
  boksDanHrono.OnClickPtr      = 0;
  boksDanHrono.OnPressPtr      = 0;

  labelHronoDan.OwnerScreenID   = 32776;
  labelHronoDan.Order           = 2;
  labelHronoDan.Left            = 104;
  labelHronoDan.Top             = 15;
  labelHronoDan.Width           = 47;
  labelHronoDan.Height          = 26;
  labelHronoDan.Visible         = 1;
  labelHronoDan.Active          = 1;
  labelHronoDan.Caption         = labelHronoDan_Caption;
  labelHronoDan.FontName        = Arial21x24_Bold;
  labelHronoDan.Font_Color      = 0x6000;
  labelHronoDan.OnUpPtr         = 0;
  labelHronoDan.OnDownPtr       = 0;
  labelHronoDan.OnClickPtr      = 0;
  labelHronoDan.OnPressPtr      = 0;

  tHronoProgPlus.OwnerScreenID   = 32776;
  tHronoProgPlus.Order           = 3;
  tHronoProgPlus.Left            = 186;
  tHronoProgPlus.Top             = 9;
  tHronoProgPlus.Width           = 50;
  tHronoProgPlus.Height          = 50;
  tHronoProgPlus.Pen_Width       = 1;
  tHronoProgPlus.Pen_Color       = 0x0000;
  tHronoProgPlus.Visible         = 1;
  tHronoProgPlus.Active          = 1;
  tHronoProgPlus.Transparent     = 1;
  tHronoProgPlus.Caption         = tHronoProgPlus_Caption;
  tHronoProgPlus.TextAlign       = _taCenter;
  tHronoProgPlus.FontName        = Arial19x22_Bold;
  tHronoProgPlus.PressColEnabled = 1;
  tHronoProgPlus.Font_Color      = 0x0000;
  tHronoProgPlus.Gradient        = 1;
  tHronoProgPlus.Gradient_Orientation    = 0;
  tHronoProgPlus.Gradient_Start_Color    = 0xFFFF;
  tHronoProgPlus.Gradient_End_Color      = 0xC618;
  tHronoProgPlus.Color           = 0xC618;
  tHronoProgPlus.Press_Color     = 0x8410;
  tHronoProgPlus.Corner_Radius   = 3;
  tHronoProgPlus.OnUpPtr         = 0;
  tHronoProgPlus.OnDownPtr       = 0;
  tHronoProgPlus.OnClickPtr      = 0;
  tHronoProgPlus.OnPressPtr      = hronoProgPlus;

  tHronoProgMinus.OwnerScreenID   = 32776;
  tHronoProgMinus.Order           = 4;
  tHronoProgMinus.Left            = 16;
  tHronoProgMinus.Top             = 9;
  tHronoProgMinus.Width           = 50;
  tHronoProgMinus.Height          = 50;
  tHronoProgMinus.Pen_Width       = 1;
  tHronoProgMinus.Pen_Color       = 0x0000;
  tHronoProgMinus.Visible         = 1;
  tHronoProgMinus.Active          = 1;
  tHronoProgMinus.Transparent     = 1;
  tHronoProgMinus.Caption         = tHronoProgMinus_Caption;
  tHronoProgMinus.TextAlign       = _taCenter;
  tHronoProgMinus.FontName        = Arial19x22_Bold;
  tHronoProgMinus.PressColEnabled = 1;
  tHronoProgMinus.Font_Color      = 0x0000;
  tHronoProgMinus.Gradient        = 1;
  tHronoProgMinus.Gradient_Orientation    = 0;
  tHronoProgMinus.Gradient_Start_Color    = 0xFFFF;
  tHronoProgMinus.Gradient_End_Color      = 0xC618;
  tHronoProgMinus.Color           = 0xC618;
  tHronoProgMinus.Press_Color     = 0x8410;
  tHronoProgMinus.Corner_Radius   = 3;
  tHronoProgMinus.OnUpPtr         = 0;
  tHronoProgMinus.OnDownPtr       = 0;
  tHronoProgMinus.OnClickPtr      = 0;
  tHronoProgMinus.OnPressPtr      = hronoProgMinus;

  boksProgHrono.OwnerScreenID   = 32776;
  boksProgHrono.Order           = 6;
  boksProgHrono.Left            = 142;
  boksProgHrono.Top             = 60;
  boksProgHrono.Width           = 40;
  boksProgHrono.Height          = 40;
  boksProgHrono.Pen_Width       = 1;
  boksProgHrono.Pen_Color       = 0x0000;
  boksProgHrono.Visible         = 1;
  boksProgHrono.Active          = 1;
  boksProgHrono.Transparent     = 1;
  boksProgHrono.Gradient        = 0;
  boksProgHrono.Gradient_Orientation    = 0;
  boksProgHrono.Gradient_Start_Color    = 0xFFFF;
  boksProgHrono.Gradient_End_Color      = 0xC618;
  boksProgHrono.Color           = 0xFFFF;
  boksProgHrono.PressColEnabled     = 1;
  boksProgHrono.Press_Color     = 0x8410;
  boksProgHrono.OnUpPtr         = 0;
  boksProgHrono.OnDownPtr       = 0;
  boksProgHrono.OnClickPtr      = 0;
  boksProgHrono.OnPressPtr      = 0;

  labelHronoProg.OwnerScreenID   = 32776;
  labelHronoProg.Order           = 7;
  labelHronoProg.Left            = 156;
  labelHronoProg.Top             = 68;
  labelHronoProg.Width           = 12;
  labelHronoProg.Height          = 26;
  labelHronoProg.Visible         = 1;
  labelHronoProg.Active          = 1;
  labelHronoProg.Caption         = labelHronoProg_Caption;
  labelHronoProg.FontName        = Arial21x24_Bold;
  labelHronoProg.Font_Color      = 0x6000;
  labelHronoProg.OnUpPtr         = 0;
  labelHronoProg.OnDownPtr       = 0;
  labelHronoProg.OnClickPtr      = 0;
  labelHronoProg.OnPressPtr      = 0;

  tHronoSatPlus.OwnerScreenID   = 32776;
  tHronoSatPlus.Order           = 8;
  tHronoSatPlus.Left            = 186;
  tHronoSatPlus.Top             = 111;
  tHronoSatPlus.Width           = 50;
  tHronoSatPlus.Height          = 40;
  tHronoSatPlus.Pen_Width       = 1;
  tHronoSatPlus.Pen_Color       = 0x0000;
  tHronoSatPlus.Visible         = 1;
  tHronoSatPlus.Active          = 1;
  tHronoSatPlus.Transparent     = 1;
  tHronoSatPlus.Caption         = tHronoSatPlus_Caption;
  tHronoSatPlus.TextAlign       = _taCenter;
  tHronoSatPlus.FontName        = Arial19x22_Bold;
  tHronoSatPlus.PressColEnabled = 1;
  tHronoSatPlus.Font_Color      = 0x0000;
  tHronoSatPlus.Gradient        = 1;
  tHronoSatPlus.Gradient_Orientation    = 0;
  tHronoSatPlus.Gradient_Start_Color    = 0xFFFF;
  tHronoSatPlus.Gradient_End_Color      = 0xC618;
  tHronoSatPlus.Color           = 0xC618;
  tHronoSatPlus.Press_Color     = 0x8410;
  tHronoSatPlus.Corner_Radius   = 3;
  tHronoSatPlus.OnUpPtr         = 0;
  tHronoSatPlus.OnDownPtr       = 0;
  tHronoSatPlus.OnClickPtr      = 0;
  tHronoSatPlus.OnPressPtr      = hronoSatPlus;

  tHronoSatMinus.OwnerScreenID   = 32776;
  tHronoSatMinus.Order           = 9;
  tHronoSatMinus.Left            = 16;
  tHronoSatMinus.Top             = 111;
  tHronoSatMinus.Width           = 50;
  tHronoSatMinus.Height          = 40;
  tHronoSatMinus.Pen_Width       = 1;
  tHronoSatMinus.Pen_Color       = 0x0000;
  tHronoSatMinus.Visible         = 1;
  tHronoSatMinus.Active          = 1;
  tHronoSatMinus.Transparent     = 1;
  tHronoSatMinus.Caption         = tHronoSatMinus_Caption;
  tHronoSatMinus.TextAlign       = _taCenter;
  tHronoSatMinus.FontName        = Arial19x22_Bold;
  tHronoSatMinus.PressColEnabled = 1;
  tHronoSatMinus.Font_Color      = 0x0000;
  tHronoSatMinus.Gradient        = 1;
  tHronoSatMinus.Gradient_Orientation    = 0;
  tHronoSatMinus.Gradient_Start_Color    = 0xFFFF;
  tHronoSatMinus.Gradient_End_Color      = 0xC618;
  tHronoSatMinus.Color           = 0xC618;
  tHronoSatMinus.Press_Color     = 0x8410;
  tHronoSatMinus.Corner_Radius   = 3;
  tHronoSatMinus.OnUpPtr         = 0;
  tHronoSatMinus.OnDownPtr       = 0;
  tHronoSatMinus.OnClickPtr      = 0;
  tHronoSatMinus.OnPressPtr      = hronoSatMinus;

  boksSatHrono.OwnerScreenID   = 32776;
  boksSatHrono.Order           = 10;
  boksSatHrono.Left            = 77;
  boksSatHrono.Top             = 111;
  boksSatHrono.Width           = 97;
  boksSatHrono.Height          = 40;
  boksSatHrono.Pen_Width       = 1;
  boksSatHrono.Pen_Color       = 0x0000;
  boksSatHrono.Visible         = 1;
  boksSatHrono.Active          = 1;
  boksSatHrono.Transparent     = 1;
  boksSatHrono.Gradient        = 0;
  boksSatHrono.Gradient_Orientation    = 0;
  boksSatHrono.Gradient_Start_Color    = 0xFFFF;
  boksSatHrono.Gradient_End_Color      = 0xC618;
  boksSatHrono.Color           = 0xFFFF;
  boksSatHrono.PressColEnabled     = 1;
  boksSatHrono.Press_Color     = 0x8410;
  boksSatHrono.OnUpPtr         = 0;
  boksSatHrono.OnDownPtr       = 0;
  boksSatHrono.OnClickPtr      = 0;
  boksSatHrono.OnPressPtr      = 0;

  labelHronoSat.OwnerScreenID   = 32776;
  labelHronoSat.Order           = 11;
  labelHronoSat.Left            = 98;
  labelHronoSat.Top             = 118;
  labelHronoSat.Width           = 55;
  labelHronoSat.Height          = 26;
  labelHronoSat.Visible         = 1;
  labelHronoSat.Active          = 1;
  labelHronoSat.Caption         = labelHronoSat_Caption;
  labelHronoSat.FontName        = Arial21x24_Bold;
  labelHronoSat.Font_Color      = 0x6000;
  labelHronoSat.OnUpPtr         = 0;
  labelHronoSat.OnDownPtr       = 0;
  labelHronoSat.OnClickPtr      = 0;
  labelHronoSat.OnPressPtr      = 0;

  tHronoPosoMinus.OwnerScreenID   = 32776;
  tHronoPosoMinus.Order           = 12;
  tHronoPosoMinus.Left            = 16;
  tHronoPosoMinus.Top             = 161;
  tHronoPosoMinus.Width           = 50;
  tHronoPosoMinus.Height          = 40;
  tHronoPosoMinus.Pen_Width       = 1;
  tHronoPosoMinus.Pen_Color       = 0x0000;
  tHronoPosoMinus.Visible         = 1;
  tHronoPosoMinus.Active          = 1;
  tHronoPosoMinus.Transparent     = 1;
  tHronoPosoMinus.Caption         = tHronoPosoMinus_Caption;
  tHronoPosoMinus.TextAlign       = _taCenter;
  tHronoPosoMinus.FontName        = Arial19x22_Bold;
  tHronoPosoMinus.PressColEnabled = 1;
  tHronoPosoMinus.Font_Color      = 0x0000;
  tHronoPosoMinus.Gradient        = 1;
  tHronoPosoMinus.Gradient_Orientation    = 0;
  tHronoPosoMinus.Gradient_Start_Color    = 0xFFFF;
  tHronoPosoMinus.Gradient_End_Color      = 0xC618;
  tHronoPosoMinus.Color           = 0xC618;
  tHronoPosoMinus.Press_Color     = 0x8410;
  tHronoPosoMinus.Corner_Radius   = 3;
  tHronoPosoMinus.OnUpPtr         = 0;
  tHronoPosoMinus.OnDownPtr       = 0;
  tHronoPosoMinus.OnClickPtr      = hronoPosoMinus;
  tHronoPosoMinus.OnPressPtr      = 0;

  tHronoPosoPlus.OwnerScreenID   = 32776;
  tHronoPosoPlus.Order           = 13;
  tHronoPosoPlus.Left            = 186;
  tHronoPosoPlus.Top             = 161;
  tHronoPosoPlus.Width           = 50;
  tHronoPosoPlus.Height          = 40;
  tHronoPosoPlus.Pen_Width       = 1;
  tHronoPosoPlus.Pen_Color       = 0x0000;
  tHronoPosoPlus.Visible         = 1;
  tHronoPosoPlus.Active          = 1;
  tHronoPosoPlus.Transparent     = 1;
  tHronoPosoPlus.Caption         = tHronoPosoPlus_Caption;
  tHronoPosoPlus.TextAlign       = _taCenter;
  tHronoPosoPlus.FontName        = Arial19x22_Bold;
  tHronoPosoPlus.PressColEnabled = 1;
  tHronoPosoPlus.Font_Color      = 0x0000;
  tHronoPosoPlus.Gradient        = 1;
  tHronoPosoPlus.Gradient_Orientation    = 0;
  tHronoPosoPlus.Gradient_Start_Color    = 0xFFFF;
  tHronoPosoPlus.Gradient_End_Color      = 0xC618;
  tHronoPosoPlus.Color           = 0xC618;
  tHronoPosoPlus.Press_Color     = 0x8410;
  tHronoPosoPlus.Corner_Radius   = 3;
  tHronoPosoPlus.OnUpPtr         = 0;
  tHronoPosoPlus.OnDownPtr       = 0;
  tHronoPosoPlus.OnClickPtr      = hronoPosoPlus;
  tHronoPosoPlus.OnPressPtr      = 0;

  boksPosoHrono.OwnerScreenID   = 32776;
  boksPosoHrono.Order           = 14;
  boksPosoHrono.Left            = 87;
  boksPosoHrono.Top             = 161;
  boksPosoHrono.Width           = 79;
  boksPosoHrono.Height          = 40;
  boksPosoHrono.Pen_Width       = 1;
  boksPosoHrono.Pen_Color       = 0x0000;
  boksPosoHrono.Visible         = 1;
  boksPosoHrono.Active          = 1;
  boksPosoHrono.Transparent     = 1;
  boksPosoHrono.Gradient        = 0;
  boksPosoHrono.Gradient_Orientation    = 0;
  boksPosoHrono.Gradient_Start_Color    = 0xFFFF;
  boksPosoHrono.Gradient_End_Color      = 0xC618;
  boksPosoHrono.Color           = 0xFFFF;
  boksPosoHrono.PressColEnabled     = 1;
  boksPosoHrono.Press_Color     = 0x8410;
  boksPosoHrono.OnUpPtr         = 0;
  boksPosoHrono.OnDownPtr       = 0;
  boksPosoHrono.OnClickPtr      = 0;
  boksPosoHrono.OnPressPtr      = 0;

  lablHronoPoso.OwnerScreenID   = 32776;
  lablHronoPoso.Order           = 15;
  lablHronoPoso.Left            = 97;
  lablHronoPoso.Top             = 169;
  lablHronoPoso.Width           = 55;
  lablHronoPoso.Height          = 24;
  lablHronoPoso.Visible         = 1;
  lablHronoPoso.Active          = 1;
  lablHronoPoso.Caption         = lablHronoPoso_Caption;
  lablHronoPoso.FontName        = Arial19x22_Bold;
  lablHronoPoso.Font_Color      = 0x6000;
  lablHronoPoso.OnUpPtr         = 0;
  lablHronoPoso.OnDownPtr       = 0;
  lablHronoPoso.OnClickPtr      = 0;
  lablHronoPoso.OnPressPtr      = 0;

  tHronoZadTAPlus.OwnerScreenID   = 32776;
  tHronoZadTAPlus.Order           = 16;
  tHronoZadTAPlus.Left            = 186;
  tHronoZadTAPlus.Top             = 212;
  tHronoZadTAPlus.Width           = 50;
  tHronoZadTAPlus.Height          = 26;
  tHronoZadTAPlus.Pen_Width       = 1;
  tHronoZadTAPlus.Pen_Color       = 0x0000;
  tHronoZadTAPlus.Visible         = 1;
  tHronoZadTAPlus.Active          = 1;
  tHronoZadTAPlus.Transparent     = 1;
  tHronoZadTAPlus.Caption         = tHronoZadTAPlus_Caption;
  tHronoZadTAPlus.TextAlign       = _taCenter;
  tHronoZadTAPlus.FontName        = Arial19x22_Bold;
  tHronoZadTAPlus.PressColEnabled = 1;
  tHronoZadTAPlus.Font_Color      = 0x0000;
  tHronoZadTAPlus.Gradient        = 1;
  tHronoZadTAPlus.Gradient_Orientation    = 0;
  tHronoZadTAPlus.Gradient_Start_Color    = 0xFFFF;
  tHronoZadTAPlus.Gradient_End_Color      = 0xC618;
  tHronoZadTAPlus.Color           = 0xC618;
  tHronoZadTAPlus.Press_Color     = 0x8410;
  tHronoZadTAPlus.Corner_Radius   = 3;
  tHronoZadTAPlus.OnUpPtr         = 0;
  tHronoZadTAPlus.OnDownPtr       = 0;
  tHronoZadTAPlus.OnClickPtr      = 0;
  tHronoZadTAPlus.OnPressPtr      = hronoZadTAPlus;

  tHronoZadTAMinus.OwnerScreenID   = 32776;
  tHronoZadTAMinus.Order           = 17;
  tHronoZadTAMinus.Left            = 16;
  tHronoZadTAMinus.Top             = 212;
  tHronoZadTAMinus.Width           = 50;
  tHronoZadTAMinus.Height          = 26;
  tHronoZadTAMinus.Pen_Width       = 1;
  tHronoZadTAMinus.Pen_Color       = 0x0000;
  tHronoZadTAMinus.Visible         = 1;
  tHronoZadTAMinus.Active          = 1;
  tHronoZadTAMinus.Transparent     = 1;
  tHronoZadTAMinus.Caption         = tHronoZadTAMinus_Caption;
  tHronoZadTAMinus.TextAlign       = _taCenter;
  tHronoZadTAMinus.FontName        = Arial19x22_Bold;
  tHronoZadTAMinus.PressColEnabled = 1;
  tHronoZadTAMinus.Font_Color      = 0x0000;
  tHronoZadTAMinus.Gradient        = 1;
  tHronoZadTAMinus.Gradient_Orientation    = 0;
  tHronoZadTAMinus.Gradient_Start_Color    = 0xFFFF;
  tHronoZadTAMinus.Gradient_End_Color      = 0xC618;
  tHronoZadTAMinus.Color           = 0xC618;
  tHronoZadTAMinus.Press_Color     = 0x8410;
  tHronoZadTAMinus.Corner_Radius   = 3;
  tHronoZadTAMinus.OnUpPtr         = 0;
  tHronoZadTAMinus.OnDownPtr       = 0;
  tHronoZadTAMinus.OnClickPtr      = 0;
  tHronoZadTAMinus.OnPressPtr      = hronoZadTAMinus;

  boksZadTAhrono.OwnerScreenID   = 32776;
  boksZadTAhrono.Order           = 18;
  boksZadTAhrono.Left            = 87;
  boksZadTAhrono.Top             = 210;
  boksZadTAhrono.Width           = 79;
  boksZadTAhrono.Height          = 28;
  boksZadTAhrono.Pen_Width       = 1;
  boksZadTAhrono.Pen_Color       = 0x0000;
  boksZadTAhrono.Visible         = 1;
  boksZadTAhrono.Active          = 1;
  boksZadTAhrono.Transparent     = 1;
  boksZadTAhrono.Gradient        = 0;
  boksZadTAhrono.Gradient_Orientation    = 0;
  boksZadTAhrono.Gradient_Start_Color    = 0xFFFF;
  boksZadTAhrono.Gradient_End_Color      = 0xC618;
  boksZadTAhrono.Color           = 0xFFFF;
  boksZadTAhrono.PressColEnabled     = 1;
  boksZadTAhrono.Press_Color     = 0x8410;
  boksZadTAhrono.OnUpPtr         = 0;
  boksZadTAhrono.OnDownPtr       = 0;
  boksZadTAhrono.OnClickPtr      = 0;
  boksZadTAhrono.OnPressPtr      = 0;

  labelHronoZadTA.OwnerScreenID   = 32776;
  labelHronoZadTA.Order           = 19;
  labelHronoZadTA.Left            = 100;
  labelHronoZadTA.Top             = 211;
  labelHronoZadTA.Width           = 58;
  labelHronoZadTA.Height          = 26;
  labelHronoZadTA.Visible         = 1;
  labelHronoZadTA.Active          = 1;
  labelHronoZadTA.Caption         = labelHronoZadTA_Caption;
  labelHronoZadTA.FontName        = Arial21x24_Bold;
  labelHronoZadTA.Font_Color      = 0x6000;
  labelHronoZadTA.OnUpPtr         = 0;
  labelHronoZadTA.OnDownPtr       = 0;
  labelHronoZadTA.OnClickPtr      = 0;
  labelHronoZadTA.OnPressPtr      = 0;

  thronoOnOff.OwnerScreenID   = 32776;
  thronoOnOff.Order           = 20;
  thronoOnOff.Left            = 261;
  thronoOnOff.Top             = 182;
  thronoOnOff.Radius          = 27;
  thronoOnOff.Pen_Width       = 1;
  thronoOnOff.Pen_Color       = 0x0000;
  thronoOnOff.Visible         = 1;
  thronoOnOff.Active          = 1;
  thronoOnOff.Transparent     = 1;
  thronoOnOff.Caption         = thronoOnOff_Caption;
  thronoOnOff.TextAlign       = _taCenter;
  thronoOnOff.FontName        = Arial23x29_Bold;
  thronoOnOff.PressColEnabled = 1;
  thronoOnOff.Font_Color     = 0x0000;
  thronoOnOff.Gradient       = 1;
  thronoOnOff.Gradient_Orientation    = 0;
  thronoOnOff.Gradient_Start_Color    = 0xFC10;
  thronoOnOff.Gradient_End_Color      = 0xF800;
  thronoOnOff.Color          = 0xF800;
  thronoOnOff.Press_Color    = 0x8410;
  thronoOnOff.OnUpPtr         = 0;
  thronoOnOff.OnDownPtr       = 0;
  thronoOnOff.OnClickPtr      = hronoOnOff;
  thronoOnOff.OnPressPtr      = 0;

  hronoPok.OwnerScreenID   = 32776;
  hronoPok.Order           = 22;
  hronoPok.Left            = 252;
  hronoPok.Top             = 105;
  hronoPok.Radius          = 28;
  hronoPok.Pen_Width       = 1;
  hronoPok.Pen_Color       = 0x0000;
  hronoPok.Visible         = 1;
  hronoPok.Active          = 1;
  hronoPok.Transparent     = 1;
  hronoPok.Gradient        = 0;
  hronoPok.Gradient_Orientation    = 0;
  hronoPok.Gradient_Start_Color    = 0xFFFF;
  hronoPok.Gradient_End_Color      = 0xC618;
  hronoPok.Color           = 0x07E0;
  hronoPok.PressColEnabled = 1;
  hronoPok.Press_Color     = 0x8410;
  hronoPok.OnUpPtr         = 0;
  hronoPok.OnDownPtr       = 0;
  hronoPok.OnClickPtr      = 0;
  hronoPok.OnPressPtr      = 0;

  exitKom.OwnerScreenID   = 32777;
  exitKom.Order           = 0;
  exitKom.Left            = 267;
  exitKom.Top             = 3;
  exitKom.Radius          = 25;
  exitKom.Pen_Width       = 1;
  exitKom.Pen_Color       = 0x0000;
  exitKom.Visible         = 1;
  exitKom.Active          = 1;
  exitKom.Transparent     = 1;
  exitKom.Caption         = exitKom_Caption;
  exitKom.TextAlign       = _taCenter;
  exitKom.FontName        = Arial23x29_Bold;
  exitKom.PressColEnabled = 1;
  exitKom.Font_Color     = 0x0000;
  exitKom.Gradient       = 1;
  exitKom.Gradient_Orientation    = 0;
  exitKom.Gradient_Start_Color    = 0xFC10;
  exitKom.Gradient_End_Color      = 0xF800;
  exitKom.Color          = 0xC618;
  exitKom.Press_Color    = 0x8410;
  exitKom.OnUpPtr         = 0;
  exitKom.OnDownPtr       = 0;
  exitKom.OnClickPtr      = nazadKom;
  exitKom.OnPressPtr      = 0;

  boksIP.OwnerScreenID   = 32777;
  boksIP.Order           = 3;
  boksIP.Left            = 162;
  boksIP.Top             = 53;
  boksIP.Width           = 50;
  boksIP.Height          = 40;
  boksIP.Pen_Width       = 1;
  boksIP.Pen_Color       = 0x0000;
  boksIP.Visible         = 1;
  boksIP.Active          = 1;
  boksIP.Transparent     = 1;
  boksIP.Gradient        = 0;
  boksIP.Gradient_Orientation    = 0;
  boksIP.Gradient_Start_Color    = 0xFFFF;
  boksIP.Gradient_End_Color      = 0xC618;
  boksIP.Color           = 0xFFFF;
  boksIP.PressColEnabled     = 1;
  boksIP.Press_Color     = 0x8410;
  boksIP.OnUpPtr         = 0;
  boksIP.OnDownPtr       = 0;
  boksIP.OnClickPtr      = 0;
  boksIP.OnPressPtr      = 0;

  labelAdresiranje.OwnerScreenID   = 32777;
  labelAdresiranje.Order           = 4;
  labelAdresiranje.Left            = 169;
  labelAdresiranje.Top             = 60;
  labelAdresiranje.Width           = 36;
  labelAdresiranje.Height          = 26;
  labelAdresiranje.Visible         = 1;
  labelAdresiranje.Active          = 1;
  labelAdresiranje.Caption         = labelAdresiranje_Caption;
  labelAdresiranje.FontName        = Arial21x24_Bold;
  labelAdresiranje.Font_Color      = 0x6000;
  labelAdresiranje.OnUpPtr         = 0;
  labelAdresiranje.OnDownPtr       = 0;
  labelAdresiranje.OnClickPtr      = 0;
  labelAdresiranje.OnPressPtr      = 0;

  ButtonRound4.OwnerScreenID   = 32777;
  ButtonRound4.Order           = 6;
  ButtonRound4.Left            = 161;
  ButtonRound4.Top             = 96;
  ButtonRound4.Width           = 50;
  ButtonRound4.Height          = 40;
  ButtonRound4.Pen_Width       = 1;
  ButtonRound4.Pen_Color       = 0x0000;
  ButtonRound4.Visible         = 1;
  ButtonRound4.Active          = 1;
  ButtonRound4.Transparent     = 1;
  ButtonRound4.Caption         = ButtonRound4_Caption;
  ButtonRound4.TextAlign       = _taCenter;
  ButtonRound4.FontName        = Arial19x22_Bold;
  ButtonRound4.PressColEnabled = 1;
  ButtonRound4.Font_Color      = 0x0000;
  ButtonRound4.Gradient        = 1;
  ButtonRound4.Gradient_Orientation    = 0;
  ButtonRound4.Gradient_Start_Color    = 0xFFFF;
  ButtonRound4.Gradient_End_Color      = 0xC618;
  ButtonRound4.Color           = 0xC618;
  ButtonRound4.Press_Color     = 0x8410;
  ButtonRound4.Corner_Radius   = 3;
  ButtonRound4.OnUpPtr         = 0;
  ButtonRound4.OnDownPtr       = 0;
  ButtonRound4.OnClickPtr      = 0;
  ButtonRound4.OnPressPtr      = IPminus;

  tIPplus.OwnerScreenID   = 32777;
  tIPplus.Order           = 7;
  tIPplus.Left            = 161;
  tIPplus.Top             = 11;
  tIPplus.Width           = 50;
  tIPplus.Height          = 40;
  tIPplus.Pen_Width       = 1;
  tIPplus.Pen_Color       = 0x0000;
  tIPplus.Visible         = 1;
  tIPplus.Active          = 1;
  tIPplus.Transparent     = 1;
  tIPplus.Caption         = tIPplus_Caption;
  tIPplus.TextAlign       = _taCenter;
  tIPplus.FontName        = Arial19x22_Bold;
  tIPplus.PressColEnabled = 1;
  tIPplus.Font_Color      = 0x0000;
  tIPplus.Gradient        = 1;
  tIPplus.Gradient_Orientation    = 0;
  tIPplus.Gradient_Start_Color    = 0xFFFF;
  tIPplus.Gradient_End_Color      = 0xC618;
  tIPplus.Color           = 0xC618;
  tIPplus.Press_Color     = 0x8410;
  tIPplus.Corner_Radius   = 3;
  tIPplus.OnUpPtr         = 0;
  tIPplus.OnDownPtr       = 0;
  tIPplus.OnClickPtr      = 0;
  tIPplus.OnPressPtr      = IPplus;

  GSMpok.OwnerScreenID   = 32777;
  GSMpok.Order           = 8;
  GSMpok.Left            = 12;
  GSMpok.Top             = 170;
  GSMpok.Radius          = 28;
  GSMpok.Pen_Width       = 1;
  GSMpok.Pen_Color       = 0x0000;
  GSMpok.Visible         = 1;
  GSMpok.Active          = 1;
  GSMpok.Transparent     = 1;
  GSMpok.Gradient        = 0;
  GSMpok.Gradient_Orientation    = 0;
  GSMpok.Gradient_Start_Color    = 0xFFFF;
  GSMpok.Gradient_End_Color      = 0xC618;
  GSMpok.Color           = 0x07E0;
  GSMpok.PressColEnabled = 1;
  GSMpok.Press_Color     = 0x8410;
  GSMpok.OnUpPtr         = 0;
  GSMpok.OnDownPtr       = 0;
  GSMpok.OnClickPtr      = 0;
  GSMpok.OnPressPtr      = 0;

  tGSMonOff.OwnerScreenID   = 32777;
  tGSMonOff.Order           = 9;
  tGSMonOff.Left            = 81;
  tGSMonOff.Top             = 172;
  tGSMonOff.Radius          = 27;
  tGSMonOff.Pen_Width       = 1;
  tGSMonOff.Pen_Color       = 0x0000;
  tGSMonOff.Visible         = 1;
  tGSMonOff.Active          = 1;
  tGSMonOff.Transparent     = 1;
  tGSMonOff.Caption         = tGSMonOff_Caption;
  tGSMonOff.TextAlign       = _taCenter;
  tGSMonOff.FontName        = Arial23x29_Bold;
  tGSMonOff.PressColEnabled = 1;
  tGSMonOff.Font_Color     = 0x0000;
  tGSMonOff.Gradient       = 1;
  tGSMonOff.Gradient_Orientation    = 0;
  tGSMonOff.Gradient_Start_Color    = 0xFC10;
  tGSMonOff.Gradient_End_Color      = 0xF800;
  tGSMonOff.Color          = 0xF800;
  tGSMonOff.Press_Color    = 0x8410;
  tGSMonOff.OnUpPtr         = 0;
  tGSMonOff.OnDownPtr       = 0;
  tGSMonOff.OnClickPtr      = GSMonOff;
  tGSMonOff.OnPressPtr      = 0;

  bokBrojMob.OwnerScreenID   = 32777;
  bokBrojMob.Order           = 10;
  bokBrojMob.Left            = 147;
  bokBrojMob.Top             = 178;
  bokBrojMob.Width           = 165;
  bokBrojMob.Height          = 40;
  bokBrojMob.Pen_Width       = 1;
  bokBrojMob.Pen_Color       = 0x0000;
  bokBrojMob.Visible         = 1;
  bokBrojMob.Active          = 1;
  bokBrojMob.Transparent     = 1;
  bokBrojMob.Gradient        = 0;
  bokBrojMob.Gradient_Orientation    = 0;
  bokBrojMob.Gradient_Start_Color    = 0xFFFF;
  bokBrojMob.Gradient_End_Color      = 0xC618;
  bokBrojMob.Color           = 0xFFFF;
  bokBrojMob.PressColEnabled     = 1;
  bokBrojMob.Press_Color     = 0x8410;
  bokBrojMob.OnUpPtr         = 0;
  bokBrojMob.OnDownPtr       = 0;
  bokBrojMob.OnClickPtr      = 0;
  bokBrojMob.OnPressPtr      = 0;

  labelBorjMob.OwnerScreenID   = 32777;
  labelBorjMob.Order           = 11;
  labelBorjMob.Left            = 159;
  labelBorjMob.Top             = 185;
  labelBorjMob.Width           = 120;
  labelBorjMob.Height          = 26;
  labelBorjMob.Visible         = 1;
  labelBorjMob.Active          = 1;
  labelBorjMob.Caption         = labelBorjMob_Caption;
  labelBorjMob.FontName        = Arial21x24_Bold;
  labelBorjMob.Font_Color      = 0x6000;
  labelBorjMob.OnUpPtr         = 0;
  labelBorjMob.OnDownPtr       = 0;
  labelBorjMob.OnClickPtr      = 0;
  labelBorjMob.OnPressPtr      = 0;
}

static char IsInsideObject (unsigned int X, unsigned int Y, unsigned int Left, unsigned int Top, unsigned int Width, unsigned int Height) { // static
  if ( (Left<= X) && (Left+ Width - 1 >= X) &&
       (Top <= Y)  && (Top + Height - 1 >= Y) )
    return 1;
  else
    return 0;
}


#define GetRoundButton(index)         CurrentMyScreen.Buttons_Round[index]
#define GetCRoundButton(index)        CurrentMyScreen.CButtons_Round[index]
#define GetLabel(index)               CurrentMyScreen.Labels[index]
#define GetCLabel(index)              CurrentMyScreen.CLabels[index]
#define GetCircle(index)              CurrentMyScreen.Circles[index]
#define GetCircleButton(index)        CurrentMyScreen.CircleButtons[index]
#define GetBox(index)                 CurrentMyScreen.Boxes[index]
#define GetCBox(index)                CurrentMyScreen.CBoxes[index]


void DrawRoundButton(TButton_Round *Around_button) {
    if (Around_button->Visible == 1) {
      if (object_pressed == 1) {
        object_pressed = 0;
        TFT_Set_Brush(Around_button->Transparent, Around_button->Press_Color, Around_button->Gradient, Around_button->Gradient_Orientation,
                      Around_button->Gradient_End_Color, Around_button->Gradient_Start_Color);
      }
      else {
        TFT_Set_Brush(Around_button->Transparent, Around_button->Color, Around_button->Gradient, Around_button->Gradient_Orientation,
                      Around_button->Gradient_Start_Color, Around_button->Gradient_End_Color);
      }
      TFT_Set_Pen(Around_button->Pen_Color, Around_button->Pen_Width);
      TFT_Rectangle_Round_Edges(Around_button->Left + 1, Around_button->Top + 1,
        Around_button->Left + Around_button->Width - 2,
        Around_button->Top + Around_button->Height - 2, Around_button->Corner_Radius);
      TFT_Set_Font(Around_button->FontName, Around_button->Font_Color, FO_HORIZONTAL);
      TFT_Write_Text_Return_Pos(Around_button->Caption, Around_button->Left, Around_button->Top);
      if (Around_button->TextAlign == _taLeft)
        TFT_Write_Text(Around_button->Caption, Around_button->Left + 4, (Around_button->Top + ((Around_button->Height - caption_height) / 2)));
      else if (Around_button->TextAlign == _taCenter)
        TFT_Write_Text(Around_button->Caption, (Around_button->Left + (Around_button->Width - caption_length) / 2), (Around_button->Top + ((Around_button->Height - caption_height) / 2)));
      else if (Around_button->TextAlign == _taRight)
        TFT_Write_Text(Around_button->Caption, Around_button->Left + (Around_button->Width - caption_length - 4), (Around_button->Top + (Around_button->Height - caption_height) / 2));
    }
}

void DrawCRoundButton(TCButton_Round *Around_button) {
    if (Around_button->Visible == 1) {
      if (object_pressed == 1) {
        object_pressed = 0;
        TFT_Set_Brush(Around_button->Transparent, Around_button->Press_Color, Around_button->Gradient, Around_button->Gradient_Orientation,
                      Around_button->Gradient_End_Color, Around_button->Gradient_Start_Color);
      }
      else {
        TFT_Set_Brush(Around_button->Transparent, Around_button->Color, Around_button->Gradient, Around_button->Gradient_Orientation,
                      Around_button->Gradient_Start_Color, Around_button->Gradient_End_Color);
      }
      TFT_Set_Pen(Around_button->Pen_Color, Around_button->Pen_Width);
      TFT_Rectangle_Round_Edges(Around_button->Left + 1, Around_button->Top + 1,
        Around_button->Left + Around_button->Width - 2,
        Around_button->Top + Around_button->Height - 2, Around_button->Corner_Radius);
      TFT_Set_Font(Around_button->FontName, Around_button->Font_Color, FO_HORIZONTAL);
      TFT_Write_Text_Return_Pos(Around_button->Caption, Around_button->Left, Around_button->Top);
      if (Around_button->TextAlign == _taLeft)
        TFT_Write_Text(Around_button->Caption, Around_button->Left + 4, (Around_button->Top + ((Around_button->Height - caption_height) / 2)));
      else if (Around_button->TextAlign == _taCenter)
        TFT_Write_Text(Around_button->Caption, (Around_button->Left + (Around_button->Width - caption_length) / 2), (Around_button->Top + ((Around_button->Height - caption_height) / 2)));
      else if (Around_button->TextAlign == _taRight)
        TFT_Write_Text(Around_button->Caption, Around_button->Left + (Around_button->Width - caption_length - 4), (Around_button->Top + (Around_button->Height - caption_height) / 2));
    }
}

void DrawLabel(TLabel *ALabel) {
  if (ALabel->Visible == 1) {
    TFT_Set_Font(ALabel->FontName, ALabel->Font_Color, FO_HORIZONTAL);
    TFT_Write_Text(ALabel->Caption, ALabel->Left, ALabel->Top);
  }
}

void DrawCLabel(TCLabel *ALabel) {
  if (ALabel->Visible == 1) {
    TFT_Set_Font(ALabel->FontName, ALabel->Font_Color, FO_HORIZONTAL);
    TFT_Write_Text(ALabel->Caption, ALabel->Left, ALabel->Top);
  }
}

void DrawCircle(TCircle *ACircle) {
  if (ACircle->Visible == 1) {
    if (object_pressed == 1) {
      object_pressed = 0;
      TFT_Set_Brush(ACircle->Transparent, ACircle->Press_Color, ACircle->Gradient, ACircle->Gradient_Orientation,
                    ACircle->Gradient_End_Color, ACircle->Gradient_Start_Color);
    }
    else {
      TFT_Set_Brush(ACircle->Transparent, ACircle->Color, ACircle->Gradient, ACircle->Gradient_Orientation,
                    ACircle->Gradient_Start_Color, ACircle->Gradient_End_Color);
    }
    TFT_Set_Pen(ACircle->Pen_Color, ACircle->Pen_Width);
    TFT_Circle(ACircle->Left + ACircle->Radius,
               ACircle->Top + ACircle->Radius,
               ACircle->Radius);
  }
}

void DrawCircleButton(TCircleButton *ACircle_button) {
  if (ACircle_button->Visible == 1) {
    if (object_pressed == 1) {
      object_pressed = 0;
      TFT_Set_Brush(ACircle_button->Transparent, ACircle_button->Press_Color, ACircle_button->Gradient, ACircle_button->Gradient_Orientation,
                    ACircle_button->Gradient_End_Color, ACircle_button->Gradient_Start_Color);
    }
    else {
      TFT_Set_Brush(ACircle_button->Transparent, ACircle_button->Color, ACircle_button->Gradient, ACircle_button->Gradient_Orientation,
                    ACircle_button->Gradient_Start_Color, ACircle_button->Gradient_End_Color);
    }
    TFT_Set_Pen(ACircle_button->Pen_Color, ACircle_button->Pen_Width);
    TFT_Circle(ACircle_button->Left + ACircle_button->Radius,
               ACircle_button->Top + ACircle_button->Radius,
               ACircle_button->Radius);
    TFT_Set_Font(ACircle_button->FontName, ACircle_button->Font_Color, FO_HORIZONTAL);
    TFT_Write_Text_Return_Pos(ACircle_button->Caption, ACircle_button->Left, ACircle_button->Top);
    if (ACircle_button->TextAlign == _taLeft)
      TFT_Write_Text(ACircle_button->Caption, ACircle_button->Left + 4, (ACircle_button->Top + ACircle_button->Radius - caption_height / 2));
    else if (ACircle_button->TextAlign == _taCenter)
      TFT_Write_Text(ACircle_button->Caption, (ACircle_button->Left + (ACircle_button->Radius*2 - caption_length) / 2), (ACircle_button->Top + ACircle_button->Radius - caption_height / 2));
    else if (ACircle_button->TextAlign == _taRight)
      TFT_Write_Text(ACircle_button->Caption, ACircle_button->Left + (ACircle_button->Radius*2 - caption_length - 4), (ACircle_button->Top + ACircle_button->Radius - caption_height / 2));
  }
}

void DrawBox(TBox *ABox) {
  if (ABox->Visible == 1) {
    if (object_pressed == 1) {
      object_pressed = 0;
      TFT_Set_Brush(ABox->Transparent, ABox->Press_Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_End_Color, ABox->Gradient_Start_Color);
    }
    else {
      TFT_Set_Brush(ABox->Transparent, ABox->Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_Start_Color, ABox->Gradient_End_Color);
    }
    TFT_Set_Pen(ABox->Pen_Color, ABox->Pen_Width);
    TFT_Rectangle(ABox->Left, ABox->Top, ABox->Left + ABox->Width - 1, ABox->Top + ABox->Height - 1);
  }
}

void DrawCBox(TCBox *ABox) {
  if (ABox->Visible == 1) {
    if (object_pressed == 1) {
      object_pressed = 0;
      TFT_Set_Brush(ABox->Transparent, ABox->Press_Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_End_Color, ABox->Gradient_Start_Color);
    }
    else {
      TFT_Set_Brush(ABox->Transparent, ABox->Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_Start_Color, ABox->Gradient_End_Color);
    }
    TFT_Set_Pen(ABox->Pen_Color, ABox->Pen_Width);
    TFT_Rectangle(ABox->Left, ABox->Top, ABox->Left + ABox->Width - 1, ABox->Top + ABox->Height - 1);
  }
}

void SetCurrentMyScreenByIndex(unsigned aScreenID){
  memset(&CurrentMyScreen, 0, sizeof(CurrentMyScreen));
  if (aScreenID & 0x8000){
    aScreenID ^= 0x8000;
    CurrentMyScreen.Color        =  CScreens[aScreenID]->Color;
    CurrentMyScreen.Width        =  CScreens[aScreenID]->Width;
    CurrentMyScreen.Height       =  CScreens[aScreenID]->Height;
    CurrentMyScreen.ObjectsCount =  CScreens[aScreenID]->ObjectsCount;
    CurrentMyScreen.Buttons_RoundCount   =  CScreens[aScreenID]->Buttons_RoundCount;
    CurrentMyScreen.Buttons_Round        =  CScreens[aScreenID]->Buttons_Round;
    CurrentMyScreen.CButtons_RoundCount   =  CScreens[aScreenID]->CButtons_RoundCount;
    CurrentMyScreen.CButtons_Round        =  CScreens[aScreenID]->CButtons_Round;
    CurrentMyScreen.LabelsCount   =  CScreens[aScreenID]->LabelsCount;
    CurrentMyScreen.Labels        =  CScreens[aScreenID]->Labels;
    CurrentMyScreen.CLabelsCount   =  CScreens[aScreenID]->CLabelsCount;
    CurrentMyScreen.CLabels        =  CScreens[aScreenID]->CLabels;
    CurrentMyScreen.CirclesCount   =  CScreens[aScreenID]->CirclesCount;
    CurrentMyScreen.Circles        =  CScreens[aScreenID]->Circles;
    CurrentMyScreen.CircleButtonsCount   =  CScreens[aScreenID]->CircleButtonsCount;
    CurrentMyScreen.CircleButtons        =  CScreens[aScreenID]->CircleButtons;
    CurrentMyScreen.BoxesCount   =  CScreens[aScreenID]->BoxesCount;
    CurrentMyScreen.Boxes        =  CScreens[aScreenID]->Boxes;
    CurrentMyScreen.CBoxesCount   =  CScreens[aScreenID]->CBoxesCount;
    CurrentMyScreen.CBoxes        =  CScreens[aScreenID]->CBoxes;
  }
}

void DrawScreen(unsigned int aScreenID) {
 unsigned int order;
  unsigned short round_button_idx;
  TButton_Round *local_round_button;
  unsigned short round_cbutton_idx;
  TCButton_Round *local_round_cbutton;
  unsigned short label_idx;
  TLabel *local_label;
  unsigned short clabel_idx;
  TCLabel *local_clabel;
  unsigned short circle_idx;
  TCircle *local_circle;
  unsigned short circle_button_idx;
  TCircleButton *local_circle_button;
  unsigned short box_idx;
  TBox *local_box;
  unsigned short cbox_idx;
  TCBox *local_cbox;
  char save_bled, save_bled_direction;

  object_pressed = 0;
  order = 0;
  round_button_idx = 0;
  round_cbutton_idx = 0;
  label_idx = 0;
  clabel_idx = 0;
  circle_idx = 0;
  circle_button_idx = 0;
  box_idx = 0;
  cbox_idx = 0;
  SetCurrentMyScreenByIndex(aScreenID);
  CurrentScreenID = aScreenID;

  if ((display_width != CurrentMyScreen.Width) || (display_height != CurrentMyScreen.Height)) {
    save_bled = TFT_BLED;
    save_bled_direction = TFT_BLED_Direction;
    TFT_BLED_Direction = 1;
    TFT_BLED           = 0;
    TFT_Init(CurrentMyScreen.Width, CurrentMyScreen.Height);
    TP_TFT_Init(CurrentMyScreen.Width, CurrentMyScreen.Height, 11, 10);                                  // Initialize touch panel
    TP_TFT_Set_ADC_Threshold(ADC_THRESHOLD);                              // Set touch panel ADC threshold
    TFT_Fill_Screen(CurrentMyScreen.Color);
    display_width = CurrentMyScreen.Width;
    display_height = CurrentMyScreen.Height;
    TFT_BLED           = save_bled;
    TFT_BLED_Direction = save_bled_direction;
  }
  else
    TFT_Fill_Screen(CurrentMyScreen.Color);


  while (order < CurrentMyScreen.ObjectsCount) {
    if (round_button_idx < CurrentMyScreen.Buttons_RoundCount) {
      local_round_button = GetRoundButton(round_button_idx);
      if (order == local_round_button->Order) {
        order++;
        round_button_idx++;
        DrawRoundButton(local_round_button);
      }
    }

    if (round_cbutton_idx < CurrentMyScreen.CButtons_RoundCount) {
      local_round_cbutton = GetCRoundButton(round_cbutton_idx);
      if (order == local_round_cbutton->Order) {
        order++;
        round_cbutton_idx++;
        DrawCRoundButton(local_round_cbutton);
      }
    }

    if (label_idx < CurrentMyScreen.LabelsCount) {
      local_label = GetLabel(label_idx);
      if (order == local_label->Order) {
        label_idx++;
        order++;
        DrawLabel(local_label);
      }
    }

    if (clabel_idx < CurrentMyScreen.CLabelsCount) {
      local_clabel = GetCLabel(clabel_idx);
      if (order == local_clabel->Order) {
        clabel_idx++;
        order++;
        DrawCLabel(local_clabel);
      }
    }

    if (circle_idx < CurrentMyScreen.CirclesCount) {
      local_circle = GetCircle(circle_idx);
      if (order == local_circle->Order) {
        circle_idx++;
        order++;
        DrawCircle(local_circle);
      }
    }

    if (circle_button_idx < CurrentMyScreen.CircleButtonsCount) {
      local_circle_button = GetCircleButton(circle_button_idx);
      if (order == local_circle_button->Order) {
        circle_button_idx++;
        order++;
        DrawCircleButton(local_circle_button);
      }
    }

    if (box_idx < CurrentMyScreen.BoxesCount) {
      local_box = GetBox(box_idx);
      if (order == local_box->Order) {
        box_idx++;
        order++;
        DrawBox(local_box);
      }
    }

    if (cbox_idx < CurrentMyScreen.CBoxesCount) {
      local_cbox = GetCBox(cbox_idx);
      if (order == local_cbox->Order) {
        cbox_idx++;
        order++;
        DrawCBox(local_cbox);
      }
    }

  }
}

void Get_Object(unsigned int X, unsigned int Y) {
  round_button_order  = -1;
  round_cbutton_order = -1;
  label_order         = -1;
  clabel_order        = -1;
  circle_order        = -1;
  circle_button_order = -1;
  box_order           = -1;
  cbox_order          = -1;
  //  Buttons with Round Edges
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.Buttons_RoundCount ; _object_count++ ) {
    local_round_button = GetRoundButton(_object_count);
    if (local_round_button->Active == 1) {
      if (IsInsideObject(X, Y, local_round_button->Left, local_round_button->Top,
                         local_round_button->Width, local_round_button->Height) == 1) {
        round_button_order = local_round_button->Order;
        exec_round_button = local_round_button;
      }
    }
  }

  //  CButtons with Round Edges
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.CButtons_RoundCount ; _object_count++ ) {
    local_round_cbutton = GetCRoundButton(_object_count);
    if (local_round_cbutton->Active == 1) {
      if (IsInsideObject(X, Y, local_round_cbutton->Left, local_round_cbutton->Top,
                         local_round_cbutton->Width, local_round_cbutton->Height) == 1) {
        round_cbutton_order = local_round_cbutton->Order;
        exec_round_cbutton = local_round_cbutton;
      }
    }
  }

  //  Labels
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.LabelsCount ; _object_count++ ) {
    local_label = GetLabel(_object_count);
    if (local_label->Active == 1) {
      if (IsInsideObject(X, Y, local_label->Left, local_label->Top,
                         local_label->Width, local_label->Height) == 1) {
        label_order = local_label->Order;
        exec_label = local_label;
      }
    }
  }

  //  CLabels
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.CLabelsCount ; _object_count++ ) {
    local_clabel = GetCLabel(_object_count);
    if (local_clabel->Active == 1) {
      if (IsInsideObject(X, Y, local_clabel->Left, local_clabel->Top,
                         local_clabel->Width, local_clabel->Height) == 1) {
        clabel_order = local_clabel->Order;
        exec_clabel = local_clabel;
      }
    }
  }

  //  Circles
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.CirclesCount ; _object_count++ ) {
    local_circle = GetCircle(_object_count);
    if (local_circle->Active == 1) {
      if (IsInsideObject(X, Y, local_circle->Left, local_circle->Top,
                        (local_circle->Radius * 2), (local_circle->Radius * 2)) == 1) {
        circle_order = local_circle->Order;
        exec_circle = local_circle;
      }
    }
  }

  //  Circle Buttons
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.CircleButtonsCount ; _object_count++ ) {
    local_circle_button = GetCircleButton(_object_count);
    if (local_circle_button->Active == 1) {
      if (IsInsideObject(X, Y, local_circle_button->Left, local_circle_button->Top,
                        (local_circle_button->Radius * 2), (local_circle_button->Radius * 2)) == 1) {
        circle_button_order = local_circle_button->Order;
        exec_circle_button = local_circle_button;
      }
    }
  }

  //  Boxes
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.BoxesCount ; _object_count++ ) {
    local_box = GetBox(_object_count);
    if (local_box->Active == 1) {
      if (IsInsideObject(X, Y, local_box->Left, local_box->Top,
                         local_box->Width, local_box->Height) == 1) {
        box_order = local_box->Order;
        exec_box = local_box;
      }
    }
  }

  //  CBoxes
  for ( _object_count = 0 ; _object_count < CurrentMyScreen.CBoxesCount ; _object_count++ ) {
    local_cbox = GetCBox(_object_count);
    if (local_cbox->Active == 1) {
      if (IsInsideObject(X, Y, local_cbox->Left, local_cbox->Top,
                         local_cbox->Width, local_cbox->Height) == 1) {
        cbox_order = local_cbox->Order;
        exec_cbox = local_cbox;
      }
    }
  }

  _object_count = -1;
  if (round_button_order > _object_count)
    _object_count = round_button_order;
  if (round_cbutton_order >  _object_count )
    _object_count = round_cbutton_order;
  if (label_order >  _object_count )
    _object_count = label_order;
  if (clabel_order >  _object_count )
    _object_count = clabel_order;
  if (circle_order >  _object_count )
    _object_count = circle_order;
  if (circle_button_order >  _object_count )
    _object_count = circle_button_order;
  if (box_order >  _object_count )
    _object_count = box_order;
  if (cbox_order >  _object_count )
    _object_count = cbox_order;
}


static void Process_TP_Press(unsigned int X, unsigned int Y) {
  exec_round_button   = 0;
  exec_round_cbutton  = 0;
  exec_label          = 0;
  exec_clabel         = 0;
  exec_circle         = 0;
  exec_circle_button  = 0;
  exec_box            = 0;
  exec_cbox           = 0;

  Get_Object(X, Y);


  if (_object_count != -1) {
    if (_object_count == round_button_order) {
      if (exec_round_button->Active == 1) {
        if (exec_round_button->OnPressPtr != 0) {
          exec_round_button->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == round_cbutton_order) {
      if (exec_round_cbutton->Active == 1) {
        if (exec_round_cbutton->OnPressPtr != 0) {
          exec_round_cbutton->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == label_order) {
      if (exec_label->Active == 1) {
        if (exec_label->OnPressPtr != 0) {
          exec_label->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == clabel_order) {
      if (exec_clabel->Active == 1) {
        if (exec_clabel->OnPressPtr != 0) {
          exec_clabel->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == circle_order) {
      if (exec_circle->Active == 1) {
        if (exec_circle->OnPressPtr != 0) {
          exec_circle->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == circle_button_order) {
      if (exec_circle_button->Active == 1) {
        if (exec_circle_button->OnPressPtr != 0) {
          exec_circle_button->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->OnPressPtr != 0) {
          exec_box->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == cbox_order) {
      if (exec_cbox->Active == 1) {
        if (exec_cbox->OnPressPtr != 0) {
          exec_cbox->OnPressPtr();
          return;
        }
      }
    }

  }
}

static void Process_TP_Up(unsigned int X, unsigned int Y) {

  switch (PressedObjectType) {
    // Round Button
    case 1: {
      if (PressedObject != 0) {
        exec_round_button = (TButton_Round*)PressedObject;
        if ((exec_round_button->PressColEnabled == 1) && (exec_round_button->OwnerScreenID == CurrentScreenID)) {
          DrawRoundButton(exec_round_button);
        }
        break;
      }
      break;
    }
    // Round CButton
    case 9: {
      if (PressedObject != 0) {
        exec_round_cbutton = (TCButton_Round*)PressedObject;
        if ((exec_round_cbutton->PressColEnabled == 1) && (exec_round_cbutton->OwnerScreenID == CurrentScreenID)) {
          DrawCRoundButton(exec_round_cbutton);
        }
        break;
      }
      break;
    }
    // Circle
    case 4: {
      if (PressedObject != 0) {
        exec_circle = (TCircle*)PressedObject;
        if ((exec_circle->PressColEnabled == 1) && (exec_circle->OwnerScreenID == CurrentScreenID)) {
          DrawCircle(exec_circle);
        }
        break;
      }
      break;
    }
    // Circle Button
    case 5: {
      if (PressedObject != 0) {
        exec_circle_button = (TCircleButton*)PressedObject;
        if ((exec_circle_button->PressColEnabled == 1) && (exec_circle_button->OwnerScreenID == CurrentScreenID)) {
          DrawCircleButton(exec_circle_button);
        }
        break;
      }
      break;
    }
    // Box
    case 6: {
      if (PressedObject != 0) {
        exec_box = (TBox*)PressedObject;
        if ((exec_box->PressColEnabled == 1) && (exec_box->OwnerScreenID == CurrentScreenID)) {
          DrawBox(exec_box);
        }
        break;
      }
      break;
    }
    // CBox
    case 14: {
      if (PressedObject != 0) {
        exec_cbox = (TCBox*)PressedObject;
        if ((exec_cbox->PressColEnabled == 1) && (exec_cbox->OwnerScreenID == CurrentScreenID)) {
          DrawCBox(exec_cbox);
        }
        break;
      }
      break;
    }
  }

  exec_label          = 0;
  exec_clabel          = 0;

  Get_Object(X, Y);


  if (_object_count != -1) {
  // Buttons with Round Edges
    if (_object_count == round_button_order) {
      if (exec_round_button->Active == 1) {
        if (exec_round_button->OnUpPtr != 0)
          exec_round_button->OnUpPtr();
        if (PressedObject == (TPointer)exec_round_button)
          if (exec_round_button->OnClickPtr != 0)
            exec_round_button->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // CButtons with Round Edges
    if (_object_count == round_cbutton_order) {
      if (exec_round_cbutton->Active == 1) {
        if (exec_round_cbutton->OnUpPtr != 0)
          exec_round_cbutton->OnUpPtr();
        if (PressedObject == (TPointer)exec_round_cbutton)
          if (exec_round_cbutton->OnClickPtr != 0)
            exec_round_cbutton->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // Labels
    if (_object_count == label_order) {
      if (exec_label->Active == 1) {
        if (exec_label->OnUpPtr != 0)
          exec_label->OnUpPtr();
        if (PressedObject == (TPointer)exec_label)
          if (exec_label->OnClickPtr != 0)
            exec_label->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // CLabels
    if (_object_count == clabel_order) {
      if (exec_clabel->Active == 1) {
        if (exec_clabel->OnUpPtr != 0)
          exec_clabel->OnUpPtr();
        if (PressedObject == (TPointer)exec_clabel)
          if (exec_clabel->OnClickPtr != 0)
            exec_clabel->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // Circles
    if (_object_count == circle_order) {
      if (exec_circle->Active == 1) {
        if (exec_circle->OnUpPtr != 0)
          exec_circle->OnUpPtr();
        if (PressedObject == (TPointer)exec_circle)
          if (exec_circle->OnClickPtr != 0)
            exec_circle->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // Circle Buttons
    if (_object_count == circle_button_order) {
      if (exec_circle_button->Active == 1) {
        if (exec_circle_button->OnUpPtr != 0)
          exec_circle_button->OnUpPtr();
        if (PressedObject == (TPointer)exec_circle_button)
          if (exec_circle_button->OnClickPtr != 0)
            exec_circle_button->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // Boxes
    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->OnUpPtr != 0)
          exec_box->OnUpPtr();
        if (PressedObject == (TPointer)exec_box)
          if (exec_box->OnClickPtr != 0)
            exec_box->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // CBoxes
    if (_object_count == cbox_order) {
      if (exec_cbox->Active == 1) {
        if (exec_cbox->OnUpPtr != 0)
          exec_cbox->OnUpPtr();
        if (PressedObject == (TPointer)exec_cbox)
          if (exec_cbox->OnClickPtr != 0)
            exec_cbox->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  }
  PressedObject = 0;
  PressedObjectType = -1;
}

static void Process_TP_Down(unsigned int X, unsigned int Y) {

  object_pressed      = 0;
  exec_round_button   = 0;
  exec_round_cbutton = 0;
  exec_label          = 0;
  exec_clabel         = 0;
  exec_circle         = 0;
  exec_circle_button  = 0;
  exec_box            = 0;
  exec_cbox           = 0;

  Get_Object(X, Y);

  if (_object_count != -1) {
    if (_object_count == round_button_order) {
      if (exec_round_button->Active == 1) {
        if (exec_round_button->PressColEnabled == 1) {
          object_pressed = 1;
          DrawRoundButton(exec_round_button);
        }
        PressedObject = (TPointer)exec_round_button;
        PressedObjectType = 1;
        if (exec_round_button->OnDownPtr != 0) {
          exec_round_button->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == round_cbutton_order) {
      if (exec_round_cbutton->Active == 1) {
        if (exec_round_cbutton->PressColEnabled == 1) {
          object_pressed = 1;
          DrawCRoundButton(exec_round_cbutton);
        }
        PressedObject = (TPointer)exec_round_cbutton;
        PressedObjectType = 9;
        if (exec_round_cbutton->OnDownPtr != 0) {
          exec_round_cbutton->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == label_order) {
      if (exec_label->Active == 1) {
        PressedObject = (TPointer)exec_label;
        PressedObjectType = 2;
        if (exec_label->OnDownPtr != 0) {
          exec_label->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == clabel_order) {
      if (exec_clabel->Active == 1) {
        PressedObject = (TPointer)exec_clabel;
        PressedObjectType = 10;
        if (exec_clabel->OnDownPtr != 0) {
          exec_clabel->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == circle_order) {
      if (exec_circle->Active == 1) {
        if (exec_circle->PressColEnabled == 1) {
          object_pressed = 1;
          DrawCircle(exec_circle);
        }
        PressedObject = (TPointer)exec_circle;
        PressedObjectType = 4;
        if (exec_circle->OnDownPtr != 0) {
          exec_circle->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == circle_button_order) {
      if (exec_circle_button->Active == 1) {
        if (exec_circle_button->PressColEnabled == 1) {
          object_pressed = 1;
          DrawCircleButton(exec_circle_button);
        }
        PressedObject = (TPointer)exec_circle_button;
        PressedObjectType = 5;
        if (exec_circle_button->OnDownPtr != 0) {
          exec_circle_button->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->PressColEnabled == 1) {
          object_pressed = 1;
          DrawBox(exec_box);
        }
        PressedObject = (TPointer)exec_box;
        PressedObjectType = 6;
        if (exec_box->OnDownPtr != 0) {
          exec_box->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == cbox_order) {
      if (exec_cbox->Active == 1) {
        if (exec_cbox->PressColEnabled == 1) {
          object_pressed = 1;
          DrawCBox(exec_cbox);
        }
        PressedObject = (TPointer)exec_cbox;
        PressedObjectType = 14;
        if (exec_cbox->OnDownPtr != 0) {
          exec_cbox->OnDownPtr();
          return;
        }
      }
    }

  }
}

void Check_TP() {
  if (TP_TFT_Press_Detect()) {
    // After a PRESS is detected read X-Y and convert it to Display dimensions space
    if (TP_TFT_Get_Coordinates(&Xcoord, &Ycoord) == 0) {
      Process_TP_Press(Xcoord, Ycoord);
      if (PenDown == 0) {
        PenDown = 1;
        Process_TP_Down(Xcoord, Ycoord);
      }
    }
  }
  else if (PenDown == 1) {
    PenDown = 0;
    Process_TP_Up(Xcoord, Ycoord);
  }
}

void Init_MCU() {
  GPIO_Config(&GPIO_PORTA_DATA_BITS, 0b00001000, _GPIO_DIR_OUTPUT, _GPIO_CFG_DIGITAL_ENABLE | _GPIO_CFG_DRIVE_8mA, 0);
  TFT_BLED = 0;
  TP_TFT_Rotate_180(0);
}

void Start_TP() {
  Init_MCU();

  InitializeTouchPanel();

  /*Delay_ms(1000);
  TFT_Fill_Screen(0);
  Calibrate();
  TFT_Fill_Screen(0);*/

  InitializeObjects();
  display_width = pozdrav.Width;
  display_height = pozdrav.Height;
  DrawScreen(32775);
}