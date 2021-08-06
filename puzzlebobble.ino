#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SD.h>
#include <JPEGDecoder.h>


// DRIVER
#define ILI9341_DRIVER

// FUENTES. Lo que se comente no se compilará y ahorrará memoria

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7
#define LOAD_FONT8  
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define SPI_FREQUENCY  27000000 // Actually sets it to 26.67MHz = 80/3
#define SPI_TOUCH_FREQUENCY  2500000


#define C   955
#define CS  901   
#define D   851
#define DS  803
#define E   758
#define F   715
#define FS  675
#define G   637
#define GS  601
#define A   568
#define AS  536
#define B   506 
#define C2  477
#define D2  425

#define DO   955
#define DOS  901   
#define RE   851
#define RES  803
#define MI   758
#define FA   715
#define FAS  675
#define SOL   637
#define SOLS  601
#define LA   568
#define LAS  536
#define SI   506 
#define DO2  477
#define RE2  425

#define Do0 3822
#define Do0S 3607
#define Re0 3405
#define Re0S 3214
#define Mi0 3033
#define Fa0 2863
#define Fa0S 2702
#define Sol0 2551
#define Sol0S 2407
#define La0 2272
#define La0S 2145
#define Si0 2024

#define Do1 1911
#define Do1S 1803
#define Re1 1702
#define Re1S 1607
#define Mi1 1516
#define Fa1 1431
#define Fa1S 1351
#define Sol1 1275
#define Sol1S 1203
#define La1 1136
#define La1S 1072
#define Si1 1012

#define Do2 956
#define Do2S 902
#define Re2 851
#define Re2S 803
#define Mi2 758
#define Fa2 716
#define Fa2S 678
#define Sol2 638
#define Sol2S 602
#define La2 568
#define La2S 536
#define Si2 506


#define Do3 478
#define Do3S 451
#define Re3 425
#define Re3S 401
#define Mi3 379
#define Fa3 358
#define Fa3S 338
#define Sol3 319
#define Sol3S 301
#define La3 284
#define La3S 268
#define Si3 253

#define Do4 238
#define Do4S 225
#define Re4 212
#define Re4S 200
#define Mi4 189
#define Fa4 179
#define Fa4S 168
#define Sol4 159
#define Sol4S 150
#define La4 142
#define La4S 134
#define Si4 126

#define Do5 119
#define Do5S 112
#define Re5 106
#define Re5S 100
#define Mi5 94
#define Fa5 89
#define Fa5S 84
#define Sol5 79
#define Sol5S 75
#define La5 71
#define La5S 67
#define Si5 63

#define Do6 59


TaskHandle_t Task1;  // declaramos para poder ejecutar en otro core

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library


int tiempo;

boolean espera=true;


//Variables
int contador=0;
  
int flechax=159;
int flechay=197;

int colorbola=0;
int colorbolaTFT=0;
int bolax=160;
int bolay=217;
int bolaax=0;
int bolaay=0;


boolean nuevabola=true;

int bolas[6][9];
int bolas2[6][9];
int bolas3[6][9];


int i=0;
int j=0;

int i2=0;

boolean boton1=false;
boolean boton2=false;
boolean boton3=false;
boolean boton4=false;

int palancax=1700;
int palancay=1700;

boolean cambio=false;
boolean colision=false;

boolean fin=false;

int contadorbolas=0;

int puntos=0;
int antpuntos=0;

int fase=1;

int iii=0;
int jjj=0;

boolean caenbolas=false;
int caeny=25;

boolean s_bola=false;int s_bolac=0;
boolean s_choquebola=false;int s_choquebolac=0;

int musica=1;
float velocidadmusica=1.5;

boolean faseterminada=false;

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos) {
 
  //jpegInfo(); // Print information from the JPEG file (could comment this line out)
 
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
 
  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);
  
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = min(mcu_w, max_x % mcu_w);
  uint32_t min_h = min(mcu_h, max_y % mcu_h);
 
  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
 
  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();
 
  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;
 
  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)
 
    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
 
    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
 
    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;
 
    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }
 
    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;
 
    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ( (mcu_y + win_h) >= tft.height())
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }
 
  tft.setSwapBytes(swapBytes); 
  
}


//####################################################################################################
// Draw a JPEG on the TFT pulled from SD Card
//####################################################################################################
// xpos, ypos is top left corner of plotted image

void drawSdJpeg(const char *filename, int xpos, int ypos) {
 
  File jpegFile = SD.open(filename, FILE_READ);  // or, file handle reference for SD library
 
  boolean decoded = JpegDec.decodeSdFile(jpegFile);  // Pass the SD file handle to the decoder,
 
  if (decoded) {   jpegRender(xpos, ypos);  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

void fases(){

  ///Toda la pantalla con bolas
  for(i=0;i<6;i++){
    for(j=0;j<9;j++){       
        bolas[i][j]=0;        
    }  
  }

  
  if(fase==1) {
    bolas[0][0]=1;bolas[1][0]=1;bolas[2][0]=1;bolas[3][0]=1;bolas[4][0]=1;bolas[5][0]=1;
    bolas[0][1]=2;bolas[1][1]=2;bolas[2][1]=2;bolas[3][1]=2;bolas[4][1]=2;
    bolas[0][2]=1;bolas[1][2]=1;bolas[2][2]=1;bolas[3][2]=1;bolas[4][2]=1;bolas[5][2]=1;
  }else if(fase==2) {
    bolas[0][0]=1;bolas[1][0]=1;bolas[2][0]=1;bolas[3][0]=2;bolas[4][0]=2;bolas[5][0]=2;
    bolas[0][1]=3;bolas[1][1]=3;bolas[2][1]=3;bolas[3][1]=4;bolas[4][1]=4;
    bolas[0][2]=1;bolas[1][2]=1;bolas[2][2]=1;bolas[3][2]=2;bolas[4][2]=2;bolas[5][2]=2;
  }else if(fase==3) {
    bolas[0][0]=1;bolas[1][0]=1;bolas[2][0]=0;bolas[3][0]=0;bolas[4][0]=2;bolas[5][0]=2;
    bolas[0][1]=3;bolas[1][1]=0;bolas[2][1]=0;bolas[3][1]=0;bolas[4][1]=4;
    bolas[0][2]=1;bolas[1][2]=1;bolas[2][2]=0;bolas[3][2]=0;bolas[4][2]=2;bolas[5][2]=2;
    bolas[0][3]=1;bolas[1][3]=1;bolas[2][3]=0;bolas[3][3]=0;bolas[4][3]=2;
  }else if(fase==4) {
    bolas[0][0]=0;bolas[1][0]=0;bolas[2][0]=3;bolas[3][0]=3;bolas[4][0]=0;bolas[5][0]=0;
    bolas[2][1]=1;
    bolas[2][2]=2;
    bolas[2][3]=4;
    bolas[2][4]=5;  
  }else if(fase==5) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=3;bolas[3][0]=4;bolas[4][0]=5;bolas[5][0]=6;
    bolas[0][1]=5;bolas[1][1]=4;bolas[2][1]=3;bolas[3][1]=2;bolas[4][1]=1;
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=3;bolas[3][0]=4;bolas[4][0]=5;bolas[5][0]=6;
  }else if(fase==6) {
    bolas[2][0]=3;
    bolas[2][1]=3;
    bolas[0][2]=1;bolas[1][2]=2;bolas[2][2]=3;bolas[3][2]=4;bolas[4][2]=5;bolas[5][2]=6;
  }else if(fase==7) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=3;bolas[3][0]=4;bolas[4][0]=5;bolas[5][0]=6;
    bolas[0][1]=1;bolas[1][1]=2;bolas[2][1]=3;bolas[3][1]=4;bolas[4][1]=5;
    bolas[0][2]=1;bolas[1][2]=2;bolas[2][2]=3;bolas[3][2]=4;bolas[4][2]=5;bolas[5][2]=6;
    bolas[0][3]=1;bolas[1][3]=2;bolas[2][3]=3;bolas[3][3]=4;bolas[4][3]=5;    
  }else if(fase==8) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[4][0]=5;bolas[5][0]=6;
    bolas[0][1]=6;bolas[1][1]=2;bolas[3][1]=4;bolas[4][1]=5;
    bolas[0][0]=1;bolas[1][0]=2;bolas[4][0]=5;bolas[5][0]=6;
    bolas[0][1]=6;bolas[1][1]=2;bolas[2][1]=3;bolas[3][1]=4;bolas[4][1]=5;    
  }else if(fase==9) {
    bolas[0][0]=1;bolas[5][0]=1;
    bolas[0][1]=1;bolas[4][1]=1;
    bolas[1][2]=1;bolas[4][2]=1;
    bolas[0][3]=2;bolas[1][3]=3;bolas[2][3]=4;bolas[3][3]=5;bolas[4][3]=6;    
  }else if(fase==10) {
    bolas[2][0]=1;bolas[3][0]=4;bolas[4][0]=1;
    bolas[2][1]=2;bolas[3][1]=3;bolas[4][1]=2;
    bolas[2][2]=3;bolas[3][2]=2;bolas[4][2]=3;
    bolas[2][3]=4;bolas[3][3]=1;bolas[4][3]=4;    
  }else if(fase==11) {
    bolas[1][0]=4;bolas[2][0]=1;bolas[3][0]=4;bolas[4][0]=1;
    bolas[1][1]=3;bolas[2][1]=2;bolas[3][1]=3;bolas[4][1]=2;
    bolas[1][2]=2;bolas[2][2]=3;bolas[3][2]=2;bolas[4][2]=3;
    bolas[1][3]=1;bolas[2][3]=4;bolas[3][3]=1;bolas[4][3]=4;    
  }else if(fase==12) {
    bolas[0][0]=1;bolas[1][0]=1;bolas[2][0]=1;bolas[3][0]=1;bolas[4][0]=1;bolas[5][0]=1;
    bolas[0][1]=2;bolas[4][1]=2;
    bolas[0][2]=1;bolas[1][2]=1;bolas[2][2]=1;bolas[3][2]=1;bolas[4][2]=1;bolas[5][2]=1;
    bolas[0][3]=3;bolas[4][3]=3;    
    bolas[0][4]=1;bolas[1][4]=1;bolas[2][4]=1;bolas[3][4]=1;bolas[4][4]=1;bolas[5][4]=1;        
  }else if(fase==13) {
    bolas[1][0]=1;bolas[2][0]=1;bolas[3][0]=1;bolas[4][0]=1;
    bolas[0][1]=2;bolas[4][1]=2;
    bolas[0][2]=4;bolas[4][2]=4;    
    bolas[0][3]=3;bolas[4][3]=3;    
    bolas[0][4]=6;bolas[4][4]=6;    
  }else if(fase==14) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=3;bolas[3][0]=3;bolas[4][0]=2;bolas[5][0]=1;
    bolas[0][1]=1;bolas[1][1]=2;bolas[2][1]=3;bolas[3][1]=2;bolas[4][1]=1;
    bolas[1][2]=1;bolas[2][2]=2;bolas[3][2]=2;bolas[4][2]=1;
    bolas[1][3]=1;bolas[2][3]=1;bolas[3][3]=1;
  }else if(fase==15) {
    bolas[0][0]=4;bolas[1][0]=4;bolas[2][0]=1;bolas[3][0]=1;bolas[4][0]=4;bolas[5][0]=4;
    bolas[0][1]=5;bolas[1][1]=4;bolas[2][1]=1;bolas[3][1]=4;bolas[4][1]=5;
    bolas[0][2]=2;bolas[1][2]=5;bolas[2][2]=4;bolas[3][2]=4;bolas[4][2]=5;bolas[5][2]=2;
    bolas[0][3]=2;bolas[1][3]=5;bolas[2][3]=5;bolas[3][3]=5;bolas[4][3]=2;    
    bolas[1][4]=2;bolas[2][4]=2;bolas[3][4]=2;bolas[4][4]=2;
  }else if(fase==16) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=2;bolas[3][0]=3;bolas[4][0]=3;bolas[5][0]=4;
    bolas[0][1]=1;bolas[1][1]=1;bolas[2][1]=2;bolas[3][1]=3;bolas[4][1]=4;
    bolas[0][2]=2;bolas[1][2]=1;bolas[2][2]=2;bolas[3][2]=3;bolas[4][2]=4;bolas[5][2]=3;
   
  }else if(fase==17) {
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=2;bolas[3][0]=3;bolas[4][0]=3;bolas[5][0]=4;
    bolas[0][1]=1;bolas[1][1]=1;bolas[2][1]=2;bolas[3][1]=3;bolas[4][1]=4;
    bolas[0][2]=2;bolas[1][2]=1;bolas[2][2]=2;bolas[3][2]=3;bolas[4][2]=4;bolas[5][2]=3;
       
  }else if(fase==18) {
    bolas[0][0]=1;bolas[2][0]=3;bolas[3][0]=3;bolas[5][0]=1;
    bolas[0][1]=4;bolas[2][1]=3;bolas[4][1]=4;
    bolas[0][2]=2;bolas[5][2]=2;
    bolas[0][3]=5;bolas[4][3]=5;
    bolas[0][4]=6;bolas[1][4]=6;bolas[4][4]=6;bolas[5][4]=6;
    
  }else if(fase==19) {
    bolas[2][0]=3;bolas[3][0]=3;
    bolas[1][1]=3;bolas[2][1]=4;bolas[3][1]=3;
    bolas[1][2]=3;bolas[2][2]=4;bolas[3][2]=4;bolas[4][2]=3;
    bolas[0][3]=3;bolas[1][3]=3;bolas[2][3]=3;bolas[3][3]=3;bolas[4][3]=3;    
    bolas[0][4]=1;bolas[1][4]=1;bolas[2][4]=2;bolas[3][4]=2;bolas[4][4]=1;bolas[5][4]=1;  
        
  }else if(fase==20) {
        
    bolas[0][0]=1;bolas[1][0]=2;bolas[2][0]=1;bolas[3][0]=2;bolas[4][0]=1;bolas[5][0]=2;
    bolas[0][1]=1;bolas[1][1]=2;bolas[2][1]=1;bolas[3][1]=2;bolas[4][1]=1;
    bolas[0][2]=1;bolas[1][2]=2;bolas[2][2]=1;bolas[3][2]=2;bolas[4][2]=1;bolas[5][2]=2;
    bolas[0][3]=1;bolas[1][3]=2;bolas[2][3]=1;bolas[3][3]=2;bolas[4][3]=1;
    bolas[0][4]=1;bolas[1][4]=2;bolas[2][4]=1;bolas[3][4]=2;bolas[4][4]=1;bolas[5][2]=2;
    
  }else if(fase==21) {
        
    bolas[0][0]=1;bolas[1][0]=5;bolas[2][0]=3;bolas[3][0]=5;bolas[4][0]=5;bolas[5][0]=5;
    bolas[0][1]=5;bolas[1][1]=5;bolas[2][1]=3;bolas[3][1]=5;bolas[4][1]=5;
    bolas[0][2]=5;bolas[1][2]=5;bolas[2][2]=3;bolas[3][2]=5;bolas[4][2]=5;bolas[5][2]=5;
    bolas[0][3]=3;bolas[1][3]=3;bolas[2][3]=3;bolas[3][3]=3;bolas[4][3]=3;
    bolas[0][4]=1;bolas[1][4]=1;bolas[2][4]=1;bolas[3][4]=1;bolas[4][4]=1;bolas[5][2]=1;
    
  }else if(fase==22) {
        
    bolas[1][0]=1;bolas[2][0]=1;bolas[3][0]=1;bolas[4][0]=1;
    bolas[0][1]=1;bolas[1][1]=2;bolas[2][1]=2;bolas[3][1]=6;bolas[4][1]=1;
    bolas[0][2]=1;bolas[1][2]=2;bolas[2][2]=2;bolas[3][2]=6;bolas[4][2]=6;bolas[5][2]=1;
    bolas[0][3]=1;bolas[1][3]=6;bolas[2][3]=6;bolas[3][3]=6;bolas[4][3]=1;
    bolas[0][4]=2;bolas[1][4]=3;bolas[2][4]=4;bolas[3][4]=5;bolas[4][4]=6;bolas[5][2]=7;
    
  }else if(fase==23) {
        
    bolas[0][0]=1;bolas[1][0]=1;bolas[2][0]=2;bolas[3][0]=2;bolas[4][0]=1;bolas[5][0]=1;
    bolas[0][1]=1;bolas[1][1]=2;bolas[2][1]=2;bolas[3][1]=2;bolas[4][1]=1;
    bolas[0][2]=1;bolas[1][2]=1;bolas[2][2]=2;bolas[3][2]=2;bolas[4][2]=1;bolas[5][2]=1;
    bolas[0][3]=3;bolas[1][3]=4;bolas[2][3]=5;bolas[3][3]=6;bolas[4][3]=7;
    bolas[0][4]=7;bolas[1][4]=7;bolas[2][4]=6;bolas[3][4]=6;bolas[4][4]=7;bolas[5][2]=7;    
  }else if(fase==24) {
    
    for(j=0;j<4;j++){
      for(i=0;i<6;i++){
        int cb=random(1,4);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
    
  }else if(fase==25) {
    
    for(j=0;j<5;j++){
      for(i=0;i<6;i++){
        int cb=random(1,4);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
  }else if(fase==26) {
    
    for(j=0;j<4;j++){
      for(i=0;i<6;i++){
        int cb=random(1,5);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
    
  }else if(fase==27) {
    
    for(j=0;j<5;j++){
      for(i=0;i<6;i++){
        int cb=random(1,5);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
  }else if(fase==28) {
    
    for(j=0;j<4;j++){
      for(i=0;i<6;i++){
        int cb=random(1,6);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
    
  }else if(fase==29) {    
    for(j=0;j<5;j++){
      for(i=0;i<6;i++){
        int cb=random(1,6);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
  }else {
    for(j=0;j<6;j++){
      for(i=0;i<6;i++){
        int cb=random(1,7);      
        if(i!=5) { bolas[i][j]=cb; }
        else if((j!=1)&&(j!=3)&&(j!=5)&&(j!=7)){  bolas[i][j]=cb; }
      }
    }
    
  }
  
}
void dibujapuntos(boolean pinta) {

  if((antpuntos!=puntos)||(pinta)) {    
  
    tft.fillRect(1,10,81,120,TFT_WHITE);
    tft.fillRect(3,12,77,116,TFT_BLACK);

    tft.drawString("LEVEL",5,22);
  
    String faseS="";
    faseS+=fase;
    int str_len = faseS.length() + 1; 
    char fase2[str_len];
    faseS.toCharArray(fase2,str_len);

    tft.drawString(faseS,5,42);

    tft.drawString("SCORE",5,72);

    String puntosS="";
    puntosS+=puntos;
    str_len = puntosS.length() + 1; 
    char puntos2[str_len];
    puntosS.toCharArray(puntos2,str_len);
  
    tft.drawString(puntos2,5,92);

  }

  antpuntos=puntos;
  
}

void pintamosfases(){

    
  String fondo="/";
  
  if(fase>30) { 
    int nn=random(1,30);
    fondo+=nn; 
  }else { 
    fondo+=fase; 
  }
  fondo+=".jpg";
  
  int str_len = fondo.length() + 1; 
  char imagenfondo[str_len];

  fondo.toCharArray(imagenfondo,str_len);

  //Serial.println(imagenfondo);
  
  drawSdJpeg(imagenfondo,0,0);

  esperar(500);
    
  String titulo="FASE ";
  titulo+=fase;

  dibujapuntos(true);
  
  
  tft.fillRect(80,10,160,220,TFT_WHITE);
  tft.fillRect(82,12,156,216,TFT_BLACK);

  tft.drawString(titulo,125,90);    

  esperar(1000);
  tft.fillRect(82,12,156,216,TFT_BLACK);
  pintatodaslasbolas();

  pintapersonajes();
  
  cambio=true;
  
}


int colorbolas(int c){

    int cb=0;
    
    if(c==1) { cb=TFT_BLUE;}
    else if(c==2) { cb=TFT_RED;}
    else if(c==3) { cb=TFT_YELLOW;}
    else if(c==4) { cb=TFT_DARKGREEN;}    
    else if(c==5) { cb=TFT_PURPLE;}
    else if(c==6) { cb=TFT_SILVER;}    
    else if(c==7) { cb=TFT_MAGENTA;}

    return cb;
    
}

void teclas(){
  
  cambio=false;
  
  if(digitalRead(21)==HIGH) { boton1=true;cambio=true;s_bola=true;s_bolac=0;}else{boton1=false;}
  if(digitalRead(16)==HIGH) { boton2=true;cambio=true;s_bola=true;}else{boton2=false;}
  if(digitalRead(5)==HIGH) { boton3=true;cambio=true;s_bola=true;}else{boton3=false;}
  if(digitalRead(17)==HIGH) { boton4=true;cambio=true;s_bola=true;}else{boton4=false;}

  palancax=analogRead(34);if((palancax>2000)||(palancax<1200)) {cambio=true;}
  palancay=analogRead(35);if((palancay>2000)||(palancay<1200)) {cambio=true;}
  
}

void recorre(int ii,int jj){
    yield();
    if(bolas2[ii][jj]==-1){ return; }
     
    bolas2[ii][jj]=-1;
  
    if(ii>0) { if(bolas[ii-1][jj]!=0){recorre(ii-1,jj); }}
    if(ii<5) { if(bolas[ii+1][jj]!=0){recorre(ii+1,jj); }}
    if(jj>0) { if(bolas[ii][jj-1]!=0){recorre(ii,jj-1); }}
    if(jj<8) { if(bolas[ii][jj+1]!=0){recorre(ii,jj+1); }}

    if((jj!=1)&&(jj!=3)&&(jj!=5)&&(jj!=7)){
      
      if((ii>0)&&(jj>0)){ if(bolas[ii-1][jj-1]!=0){recorre(ii-1,jj-1); }   }
      if((ii>0)&&(jj<8)){ if(bolas[ii-1][jj+1]!=0){recorre(ii-1,jj+1); }   }

    }
    
    if((jj!=0)&&(jj!=2)&&(jj!=4)&&(jj!=6)&&(jj!=8)){      
    
      if((ii<5)&&(jj>0)) { if(bolas[ii+1][jj-1]!=0){recorre(ii+1,jj-1); }    }
      if((ii<5)&&(jj<8)) { if(bolas[ii+1][jj+1]!=0){recorre(ii+1,jj+1); }    }

    }
  
  
}

void compruebabolascolganderas() {

  // Serial.println("Comprueba bolas colganderas");
   
   for(i=0;i<6;i++){for(j=0;j<9;j++){bolas2[i][j]=0;bolas3[i][j]=0;}}

   for(i=0;i<6;i++){  
      if(bolas[i][0]!=0){ recorre(i,0); }
   }
   
   caenbolas=false;

   for(j=0;j<9;j++){      
      for(i=0;i<6;i++){
        
        if(bolas2[i][j]==0){ 
                    
          if(bolas[i][j]!=0) {
              bolas3[i][j]=bolas[i][j]; 
              puntos+=contadorbolas;
              caenbolas=true;
              caeny=25;
          }
          bolas[i][j]=0;                    

        }
      }        
   }
   
}

void pintapersonajes() {
  drawSdJpeg("/estrella1.jpg",100,196);
  drawSdJpeg("/estrella2.jpg",185,196);
}


void compruebabolas(int ii,int jj) {

    bolas2[ii][jj]=0;
    /*
    Serial.print(ii);
    Serial.print(",");
    Serial.print(jj);
    Serial.print(",");
    Serial.println(contadorbolas);
    */
    yield();
    
    if(ii>0) { if(bolas2[ii-1][jj]==colorbola){contadorbolas++;compruebabolas(ii-1,jj); }}
    if(ii<5) { if(bolas2[ii+1][jj]==colorbola){contadorbolas++;compruebabolas(ii+1,jj); }}
    if(jj>0) { if(bolas2[ii][jj-1]==colorbola){contadorbolas++;compruebabolas(ii,jj-1); }}
    if(jj<8) { if(bolas2[ii][jj+1]==colorbola){contadorbolas++;compruebabolas(ii,jj+1); }}

    if((jj!=1)&&(jj!=3)&&(jj!=5)&&(jj!=7)){
      
      if((ii>0)&&(jj>0)){ if(bolas2[ii-1][jj-1]==colorbola){contadorbolas++;compruebabolas(ii-1,jj-1); }   }
      if((ii>0)&&(jj<8)){ if(bolas2[ii-1][jj+1]==colorbola){contadorbolas++;compruebabolas(ii-1,jj+1); }   }

    }
    
    if((jj!=0)&&(jj!=2)&&(jj!=4)&&(jj!=6)&&(jj!=8)){      
    
      if((ii<5)&&(jj>0)) { if(bolas2[ii+1][jj-1]==colorbola){contadorbolas++;compruebabolas(ii+1,jj-1); }    }
      if((ii<5)&&(jj<8)) { if(bolas2[ii+1][jj+1]==colorbola){contadorbolas++;compruebabolas(ii+1,jj+1); }    }

    }

    return;
    
}

void colisionbolas() {
  int ii=0;
  int jj=0;
  
  colision=false;
  for(i=0;i<6;i++){
    for(j=0;j<8;j++){       
      int ppx=97+(i*25);
      int ppy=25+(j*22);
      if((j==1)||(j==3)||(j==5)||(j==7)) { ppx+=12;}  

      if(bolas[i][j]!=0) {
        if((((bolax-8)>(ppx-8))&&((bolax-8)<(ppx+8)))||(((bolax+10)>(ppx-8))&&((bolax+8)<(ppx+8))))  {
          if((((bolay-8)>(ppy-8))&&((bolay-8)<(ppy+8)))||(((bolay+10)>(ppy-8))&&((bolay+8)<(ppy+8))))  {
          
            if(bolax<ppx) {               
                if((j==0)||(j==2)||(j==4)||(j==6)){ 
                  if(bolas[i-1][j+1]==0) {  bolas[i-1][j+1]=colorbola;ii=i-1;jj=j+1; }
                  else if(bolas[i-1][j]==0) {  bolas[i-1][j]=colorbola;ii=i-1;jj=j; }                  
                }else { 
                  if(bolas[i][j+1]==0){ bolas[i][j+1]=colorbola;ii=i;jj=j+1; }
                  else if(bolas[i-1][j]==0){ bolas[i-1][j]=colorbola;ii=i-1;jj=j; }                  
                }
                
            }else{                
                if((j==0)||(j==2)||(j==4)||(j==6)){ 
                  if(bolas[i][j+1]==0) {  bolas[i][j+1]=colorbola;ii=i;jj=j+1; }
                  else if(bolas[i+1][j]==0) {  bolas[i+1][j]=colorbola;ii=i+1;jj=j; }
                }else { 
                  if(bolas[i+1][j+1]==0) { bolas[i+1][j+1]=colorbola;ii=i+1;jj=j+1; }
                  else if(bolas[i+1][j]==0) { bolas[i+1][j]=colorbola;ii=i+1;jj=j; }
                }
            }

            
            colision=true;
            bolaay=0;bolaax=0;bolax=160;bolay=217;  
            
          } 
        }        
      }  
      if(colision){break;}          
    }  
    if(colision){break;}
  }
  if((colision)||(nuevabola)){ 

    if(nuevabola) { ii=iii;jj=jjj; }
    
    pintatodaslasbolas();

    for(i=0;i<6;i++){for(j=0;j<9;j++){bolas2[i][j]=bolas[i][j];}}
    
    contadorbolas=0;
    compruebabolas(ii,jj); 

    if(contadorbolas>=2) {
        m_bien();
        for(i=0;i<6;i++){for(j=0;j<9;j++){if(bolas2[i][j]==0){puntos+=contadorbolas;bolas[i][j]=0;}}}        
    }

    compruebabolascolganderas();

    pintatodaslasbolas(); 
    
    for(i=0;i<6;i++) { if(bolas[i][8]!=0) { fin=true; } }
    
    nuevabola=true;
    
 }
}

void pintatodaslasbolas(){
  //pintamos bolas pantalla
  
  tft.fillRect(82,12,156,216,TFT_BLACK);
  pintapersonajes();

  
  for(i=0;i<6;i++){
    for(j=0;j<9;j++){
        int ppx=97+(i*25);
        int ppy=25+(j*22);
        
        if((j==1)||(j==3)||(j==5)||(j==7)) { ppx+=12;}
        
        if(colorbolas(bolas[i][j])!=0){
          tft.fillCircle(ppx,ppy,10,colorbolas(bolas[i][j]));
        }
    }
  }  
}

void pintabolascaidas(){
  for(j=0;j<9;j++){
    for(i=0;i<6;i++){
        int ppx=97+(i*25);
        int ppy=caeny+(j*22);

        if(ppy>180){j=9;i=6;break; }
                
        if((j==1)||(j==3)||(j==5)||(j==7)) { ppx+=12;}
        
        if(colorbolas(bolas3[i][j])!=0){          
          tft.fillCircle(ppx,ppy,10,colorbolas(bolas3[i][j]));
        }
    }
  }    
}

void borrabolascaidas(){
  for(j=0;j<9;j++){
    for(i=0;i<6;i++){
        int ppx=97+(i*25);
        int ppy=caeny+(j*22);

        if(ppy>180){j=9;i=6;break; }
                
        if((j==1)||(j==3)||(j==5)||(j==7)) { ppx+=12;}
        
        if(colorbolas(bolas3[i][j])!=0){          
          tft.fillCircle(ppx,ppy,10,TFT_BLACK);
        }
    }
  }    
}


void esperar(int t) {
  
  espera=true;
  t=t/100;
  int contador=0;
  while(espera) {
      if(digitalRead(21)==HIGH) { espera=false;}
      if(digitalRead(16)==HIGH) { espera=false;}
      if(digitalRead(5)==HIGH) { espera=false;}
      if(digitalRead(17)==HIGH) { espera=false;}   
      delay(100);
      contador++;
      if(contador>t) { espera=false;}
  }  
}

int notaL(int n,int duracion){
    int tiemponota=(duracion*1000)/(n*2);
    
    tiemponota=(int)(tiemponota/velocidadmusica);
    
    int i2=0;
    
    for(i2=0;i2<tiemponota;i2++) {      
      
      digitalWrite(32,HIGH);digitalWrite(39,HIGH);delayMicroseconds(n);
      digitalWrite(32,LOW);digitalWrite(39,LOW);delayMicroseconds(n);
    }  
    
    delay(100);  
}

int nota(int n,int duracion){
    int tiemponota=(duracion*1000)/(n*2);
    
    tiemponota=(int)(tiemponota/velocidadmusica);
    
    int i2=0;
    
    for(i2=0;i2<tiemponota;i2++) {

      while(faseterminada) { delay(10); }
      
      digitalWrite(32,HIGH);digitalWrite(39,HIGH);delayMicroseconds(n);
      digitalWrite(32,LOW);digitalWrite(39,LOW);delayMicroseconds(n);
    }  
    
    delay(100);  
}

void m_bien(){
  notaL(SOL,75);notaL(RE2,300);notaL(SOL,75);notaL(RE2,300);
}

void m_faseganada(){
  notaL(C2,75);notaL(G,150);notaL(G,150);notaL(A,75);notaL(G,75);delay(75);notaL(B,75);notaL(C2,75);    
}

//////Canciones//////

void retardo(int tt){

    int retardotiempo=tt/velocidadmusica;
    delay(retardotiempo);
  
}

void alan(){

  nota(Fa2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(La2S,250);retardo(500);nota(Re3S,250);
  retardo(500);nota(Re3S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Do3S,250);retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);
  retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(Fa2,250);retardo(500);nota(Fa2,250);retardo(500);nota(Fa2,250);
  retardo(500);nota(Re2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(La2S,250);
  retardo(500);nota(Re3S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Do3S,250);retardo(500);nota(La2S,250);
  retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(Fa2,250);retardo(500);nota(Fa2,250);
  retardo(500);nota(Fa2,250);retardo(500);nota(Re2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(Fa2S,250);retardo(500);nota(Fa2S,250);
  retardo(500);nota(La2S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Re3S,250);retardo(500);nota(Do3S,250);
  retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(La2S,250);retardo(500);nota(Fa2,250);
  retardo(500);nota(Fa2,250);retardo(500);nota(Fa2,250);retardo(500);nota(Re2S,250);retardo(500);nota(Fa2S,208);retardo(83);nota(Fa2S,208);
  retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,250);
  nota(La2S,250);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);
  retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Do3S,208);retardo(83);nota(Do3S,208);retardo(83);nota(La2S,208);
  retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);
  retardo(83);nota(La2S,187);retardo(125);nota(La2S,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);
  retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);
  retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);
  retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,250);nota(La2S,250);nota(Re3S,208);retardo(83);nota(Re3S,208);
  retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Do3S,208);
  retardo(83);nota(Do3S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);
  retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,187);retardo(125);nota(La2S,250);nota(Fa2,208);
  retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);
  retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);
  retardo(83);nota(Fa2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,229);retardo(41);nota(Fa3S,229);retardo(41);nota(Fa3,229);
  retardo(41);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Do3S,208);retardo(83);nota(Do3S,208);
  retardo(83);nota(Do3S,208);retardo(83);nota(Sol2S,208);retardo(83);nota(Sol2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);
  retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(Do3S,208);retardo(83);nota(Si2,208);retardo(83);nota(La2S,229);
  retardo(41);nota(Fa2S,250);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);
  retardo(83);nota(Sol2S,500);nota(Fa2,500);nota(Fa2S,208);retardo(83);nota(Fa2S,208);retardo(83);nota(Fa2S,208);
  retardo(83);nota(Fa2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,229);retardo(41);nota(Fa3S,229);retardo(41);nota(Fa3,229);
  retardo(41);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Re3S,208);retardo(83);nota(Do3S,208);retardo(83);nota(Do3S,208);
  retardo(83);nota(Do3S,208);retardo(83);nota(Sol2S,208);retardo(83);nota(Sol2S,208);retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);
  retardo(83);nota(La2S,208);retardo(83);nota(La2S,208);retardo(83);nota(Do3S,208);retardo(83);nota(Si2,208);retardo(83);nota(La2S,229);
  retardo(41);nota(Fa2S,250);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);retardo(83);nota(Fa2,208);
  retardo(83);nota(Fa2S,500);nota(Sol2S,250);nota(Fa2S,500);
    
}

void supermario() {

nota(Mi3,250);nota(Mi3,250);delay(200);nota(Mi3,250);delay(200);nota(Do3,250);nota(Mi3,500);
nota(Sol3,500);nota(Sol2,500);nota(Do3,750);nota(Sol2,250);nota(Mi2,750);
nota(La2,500);nota(Si2,500);nota(La2S,250);nota(La2,500);nota(Sol2,333);
nota(Mi3,333);nota(Sol3,333);nota(La3,500);nota(Fa3,250);nota(Sol3,250);
delay(200);nota(Mi3,500);nota(Do3,250);nota(Re3,250);nota(Si2,500);delay(200);nota(Do3,750);
nota(Sol2,250);nota(Mi2,750);nota(La2,500);nota(Si2,500);nota(La2S,250);
nota(La2,500);nota(Sol2,333);nota(Mi3,333);nota(Sol3,333);nota(La3,500);
nota(Fa3,250);nota(Sol3,250);delay(200);nota(Mi3,500);nota(Do3,250);nota(Re3,250);
nota(Si2,500);nota(Sol3,250);nota(Fa3S,250);nota(Fa3,250);nota(Re3S,500);
nota(Mi3,250);delay(200);nota(Sol2S,250);nota(La2,250);nota(Do3,250);delay(200);
nota(La2,250);nota(Do3,250);nota(Re3,250);nota(Sol3,250);nota(Fa3S,250);nota(Fa3,250);
nota(Re3S,500);nota(Mi3,250);delay(200);nota(Do4,250);delay(200);nota(Do4,250);nota(Do4,1000);
nota(Sol3,250);nota(Fa3S,250);nota(Fa3,250);nota(Re3S,500);nota(Mi3,250);
delay(200);nota(Sol2S,250);nota(La2,250);nota(Do3,250);delay(200);nota(La2,250);
nota(Do3,250);nota(Re3,250);nota(Re3S,500);delay(200);nota(Re3,500);delay(200);nota(Do3,1000);
nota(Sol3,250);nota(Fa3S,250);nota(Fa3,250);nota(Re3S,500);nota(Mi3,250);delay(200);nota(Sol2S,250);
nota(La2,250);nota(Do3,250);delay(200);nota(La2,250);nota(Do3,250);nota(Re3,250);
nota(Sol3,250);nota(Fa3S,250);nota(Fa3,250);nota(Re3S,500);nota(Mi3,250);
delay(200);nota(Do4,250);delay(200);nota(Do4,250);nota(Do4,1000);nota(Sol3,250);
nota(Fa3S,250);nota(Fa3,250);nota(Re3S,500);nota(Mi3,250);delay(200);nota(Sol2S,250);nota(La2,250);
nota(Do3,250);delay(200);nota(La2,250);nota(Do3,250);nota(Re3,250);nota(Re3S,500);
delay(200);nota(Re3,500);delay(200);nota(Do3,1000);nota(Do3,250);nota(Do3,500);nota(Do3,250);
delay(200);nota(Do3,250);nota(Re3,500);nota(Mi3,250);nota(Do3,500);nota(La2,250);
nota(Sol2,1000);nota(Do3,250);nota(Do3,500);nota(Do3,250);delay(200);nota(Do3,250);
nota(Re3,250);nota(Mi3,250);nota(Do3,250);nota(Do3,500);nota(Do3,250);
delay(200);nota(Do3,250);nota(Re3,500);nota(Mi3,250);nota(Do3,500);nota(La2,250);
nota(Sol2,1000);nota(Mi3,250);nota(Mi3,250);delay(200);nota(Mi3,250);delay(200);
nota(Do3,250);nota(Mi3,500);nota(Sol3,500);nota(Sol2,500);nota(Do3,750);nota(Sol2,250);
nota(Mi2,750);nota(La2,500);nota(Si2,500);nota(La2S,250);nota(La2,500);
nota(Sol2,333);nota(Mi3,333);nota(Sol3,333);nota(La3,500);nota(Fa3,250);
nota(Sol3,250);delay(200);nota(Mi3,500);nota(Do3,250);nota(Re3,250);nota(Si2,500);
delay(200);nota(Do3,750);nota(Sol2,250);nota(Mi2,750);nota(La2,500);nota(Si2,500);
nota(La2S,250);nota(La2,500);nota(Sol2,333);nota(Mi3,333);nota(Sol3,333);
nota(La3,500);nota(Fa3,250);nota(Sol3,250);delay(200);nota(Mi3,500);nota(Do3,250);
nota(Re3,250);nota(Si2,500);delay(200);nota(Mi3,250);nota(Do3,500);nota(Sol2,250);
nota(Sol2S,500);nota(La2,250);nota(Fa3,500);nota(Fa3,250);nota(La2,1000);
nota(Si2,333);nota(La3,333);nota(La3,333);nota(La3,333);nota(Sol3,333);
nota(Fa3,333);nota(Mi3,250);nota(Do3,500);nota(La2,250);nota(Sol2,1000);
nota(Mi3,250);nota(Do3,500);nota(Sol2,250);nota(Sol2S,500);nota(La2,250);
nota(Fa3,500);nota(Fa3,250);nota(La2,1000);nota(Si2,333);nota(Fa3,333);
nota(Fa3,333);nota(Fa3,333);nota(Mi3,333);nota(Re3,333);nota(Do3,1000);
nota(Do2,1000);nota(Mi3,250);nota(Do3,500);nota(Sol2,250);nota(Sol2S,500);
nota(La2,250);nota(Fa3,500);nota(Fa3,250);nota(La2,1000);nota(Si2,333);
nota(La3,333);nota(La3,333);nota(La3,333);nota(Sol3,333);nota(Fa3,333);
nota(Mi3,250);nota(Do3,500);nota(La2,250);nota(Sol2,1000);nota(Mi3,250);
nota(Do3,500);nota(Sol2,250);nota(Sol2S,500);nota(La2,250);nota(Fa3,500);
nota(Fa3,250);nota(La2,1000);nota(Si2,333);nota(Fa3,333);nota(Fa3,333);
nota(Fa3,333);nota(Mi3,333);nota(Re3,333);nota(Do3,1000);nota(Do2,1000);
nota(Do3,250);nota(Do3,500);nota(Do3,250);delay(200);nota(Do3,250);nota(Re3,500);
nota(Mi3,250);nota(Do3,500);nota(La2,250);nota(Sol2,1000);nota(Do3,250);
nota(Do3,500);nota(Do3,250);delay(200);nota(Do3,250);nota(Re3,250);nota(Mi3,250);
nota(Do3,250);nota(Do3,500);nota(Do3,250);delay(200);nota(Do3,250);nota(Re3,500);
nota(Mi3,250);nota(Do3,500);nota(La2,250);nota(Sol2,1000);nota(Mi3,250);
nota(Mi3,250);delay(200);nota(Mi3,250);delay(200);nota(Do3,250);nota(Mi3,500);nota(Sol3,500);
nota(Sol2,500);nota(Mi3,250);nota(Do3,500);nota(Sol2,250);nota(Sol2S,500);
nota(La2,250);nota(Fa3,500);nota(Fa3,250);nota(La2,1000);nota(Si2,333);
nota(La3,333);nota(La3,333);nota(La3,333);nota(Sol3,333);nota(Fa3,333);
nota(Mi3,250);nota(Do3,500);nota(La2,250);nota(Sol2,1000);nota(Mi3,250);
nota(Do3,500);nota(Sol2,250);nota(Sol2S,500);nota(La2,250);nota(Fa3,500);
nota(Fa3,250);nota(La2,1000);nota(Si2,333);nota(Fa3,333);nota(Fa3,333);
nota(Fa3,333);nota(Mi3,333);nota(Re3,333);nota(Do3,1000);nota(Do2,1000);
nota(Do3,750);nota(Sol2,750);nota(Mi2,500);nota(La2,333);nota(Si2,333);
nota(La2,333);nota(Sol2S,333);nota(La2S,333);nota(Sol2S,333);nota(Sol2,2000);


  
}

void world() {

  nota(Re2,250);nota(Re2,500);nota(Re2,500);nota(Do2,1000);nota(La1S,1000);
  nota(La1,1500);nota(La1,250);nota(La1S,250);nota(Do2,500);nota(La2,500);
  nota(Fa2,500);nota(Sol2,250);nota(Fa2,250);nota(Fa2,500);nota(Mi2,500);
  nota(Mi2,500);nota(Sol1,250);nota(La1,250);nota(La1S,500);nota(Sol2,500);
  nota(Mi2,500);nota(Fa2,250);nota(Mi2,250);nota(Re2,500);nota(Do2,500);
  nota(Do2,500);nota(La1,250);nota(La1S,250);nota(Do2,500);nota(Fa2,250);
  nota(Sol2,250);nota(La2,500);nota(Sol2,250);nota(Fa2,250);nota(Re2,500);
  nota(Sol2,250);nota(La2,250);nota(La2S,500);nota(La2,250);nota(Sol2,250);
  nota(Do2,500);nota(La2S,500);nota(La2,500);nota(Sol2,500);nota(Fa2,1250);
  retardo(1500);
  nota(Fa2,750);nota(Fa2,250);nota(La2,500);nota(Fa2,500);nota(Sol2,750);
  nota(Sol2,250);nota(Sol2,750);retardo(500);
  nota(Sol2,750);nota(Sol2,250);nota(La2S,500);
  nota(Sol2,500);nota(La2,750);nota(La2,250);nota(La2,750);retardo(500);
  nota(La2,750);
  nota(La2,250);nota(Do3,500);nota(La2,500);nota(La2S,750);nota(La2S,250);
  nota(La2S,500);nota(La2,250);nota(Sol2,250);nota(Do2,1000);nota(Mi2,1000);
  nota(Fa2,1250);retardo(500);
  nota(La1,250);nota(La1S,250);nota(Do2,500);nota(La2,500);
  nota(Fa2,500);nota(Sol2,250);nota(Fa2,250);nota(Fa2,500);nota(Mi2,500);
  nota(Mi2,500);nota(Sol1,250);nota(La1,250);nota(La1S,500);nota(Sol2,500);
  nota(Mi2,500);nota(Fa2,250);nota(Mi2,250);nota(Re2,500);nota(Do2,500);
  nota(Do2,500);nota(La1,250);nota(La1S,250);nota(Do2,500);nota(Fa2,250);
  nota(Sol2,250);nota(La2,500);nota(Sol2,250);nota(Fa2,250);nota(Re2,500);
  nota(Sol2,250);nota(La2,250);nota(La2S,500);nota(La2,250);nota(Sol2,250);
  nota(Do2,500);nota(La2S,500);nota(La2,500);nota(Sol2,500);nota(Fa2,1250);
  retardo(500);
  nota(La1,250);nota(La1S,250);nota(Do2,500);nota(La2,500);nota(Fa2,500);
  nota(Sol2,250);nota(Fa2,250);nota(Fa2,500);nota(Mi2,500);nota(Mi2,500);
  nota(Sol1,250);nota(La1,250);nota(La1S,500);nota(Sol2,500);nota(Mi2,500);
  nota(Fa2,250);nota(Mi2,250);nota(Re2,500);nota(Do2,500);nota(Do2,500);
  nota(La1,250);nota(La1S,250);nota(Do2,500);nota(Fa2,250);nota(Sol2,250);
  nota(La2,500);nota(Sol2,250);nota(Fa2,250);nota(Re2,500);nota(Sol2,250);
  nota(La2,250);nota(La2S,500);nota(La2,250);nota(Sol2,250);nota(Do2,500);
  nota(La2S,500);nota(La2,500);nota(Sol2,500);nota(Fa2,1250);retardo(1500);
  nota(Fa2,750);
  nota(Fa2,250);nota(La2,500);nota(Fa2,500);nota(Sol2,750);nota(Sol2,250);
  nota(Sol2,750);retardo(500);
  nota(Sol2,750);nota(Sol2,250);nota(La2S,500);nota(Sol2,500);
  nota(La2,750);nota(La2,250);nota(La2,750);retardo(500);
  nota(La2,750);nota(La2,250);
  nota(Do3,500);nota(La2,500);nota(La2S,750);nota(La2S,250);nota(La2S,500);
  nota(La2,250);nota(Sol2,250);nota(Do2,1000);nota(Mi2,1000);nota(Fa2,1000);
  nota(Fa2,250);retardo(500);

}

void equipoa() {

  nota(Re3S,500);nota(La2S,250);nota(Re3S,1244);retardo(10);
  nota(Sol2S,250);nota(La2S,500);
  nota(Re2S,744);retardo(510);
  nota(Sol2,125);nota(La2S,125);nota(Re3S,250);nota(La2S,250);
  nota(Fa3,250);nota(Re3S,1244);retardo(10);
  nota(Do3S,375);nota(Do3,125);nota(La2S,125);
  nota(Sol2S,375);nota(La2S,1000);nota(Re3S,375);nota(Re3S,125);nota(La2S,250);
  nota(Re3S,1244);retardo(10);
  nota(Sol2,250);nota(Sol2S,250);nota(Fa2,250);nota(La2S,250);
  nota(Re2S,1000);nota(Fa2S,375);nota(Sol2S,125);nota(Do3,250);nota(Do3S,3244);
  retardo(10);
  nota(Do3S,250);nota(Do3,250);retardo(500);
  nota(Sol2S,250);nota(Do3S,500);nota(Do3,500);
  nota(Sol2,375);nota(Sol2S,125);nota(La2S,250);nota(Re3S,1244);retardo(10);
  nota(La2S,250);
  nota(Sol2S,250);retardo(500);
  nota(Re2S,250);nota(La2S,500);nota(Sol2S,500);nota(Sol2S,250);
  nota(Sol2,250);nota(Re2S,250);nota(Re2,250);nota(Re2S,1000);retardo(5000);



  
}

void hollywood() {

  nota(Mi2,500);nota(Sol2,375);nota(Mi2,250);nota(Mi2,125);nota(La2,250);
  nota(Mi2,250);nota(Re2,250);nota(Mi2,500);nota(Si2,375);nota(Mi2,250);
  nota(Mi2,125);nota(Do3,250);nota(Si2,250);nota(Sol2,250);nota(Mi2,250);
  nota(Si2,250);nota(Mi3,250);nota(Mi2,125);nota(Re2,250);nota(Re2,125);
  nota(Si1,250);nota(Fa2S,250);nota(Mi2,250);retardo(4000);
  nota(Mi2,500);nota(Sol2,375);
  nota(Mi2,250);nota(Mi2,125);nota(La2,250);nota(Mi2,250);nota(Re2,250);
  nota(Mi2,500);nota(Si2,375);nota(Mi2,250);nota(Mi2,125);nota(Do3,250);
  nota(Si2,250);nota(Sol2,250);nota(Mi2,250);nota(Si2,250);nota(Mi3,250);
  nota(Mi2,125);nota(Re2,250);nota(Re2,125);nota(Si1,250);nota(Fa2S,250);
  nota(Mi2,250);retardo(5000);
  nota(Mi2,500);nota(Sol2,375);nota(Mi2,250);nota(Mi2,125);
  nota(La2,250);nota(Mi2,250);nota(Re2,250);nota(Mi2,500);nota(Si2,375);
  nota(Mi2,250);nota(Mi2,125);nota(Do3,250);nota(Si2,250);nota(Sol2,250);
  nota(Mi2,250);nota(Si2,250);nota(Mi3,250);nota(Mi2,125);nota(Re2,250);
  nota(Re2,125);nota(Si1,250);nota(Fa2S,250);nota(Mi2,500);retardo(3500);
  nota(Mi2,500);
  nota(Sol2,375);nota(Mi2,250);nota(Mi2,125);nota(La2,250);nota(Mi2,250);
  nota(Re2,250);nota(Mi2,500);nota(Si2,375);nota(Mi2,250);nota(Mi2,125);
  nota(Do3,250);nota(Si2,250);nota(Sol2,250);nota(Mi2,250);nota(Si2,250);
  nota(Mi3,250);nota(Mi2,125);nota(Re2,250);nota(Re2,125);nota(Si1,250);
  nota(Fa2S,250);nota(Mi2,500);retardo(5000);
  nota(Mi2,500);nota(Sol2,375);nota(Mi2,250);
  nota(Mi2,125);nota(La2,250);nota(Mi2,250);nota(Re2,250);nota(Mi2,500);
  nota(Si2,375);nota(Mi2,250);nota(Mi2,125);nota(Do3,250);nota(Si2,250);
  nota(Sol2,250);nota(Mi2,250);nota(Si2,250);nota(Mi3,250);nota(Mi2,125);
  nota(Re2,250);nota(Re2,125);nota(Si1,250);nota(Fa2S,250);nota(Mi2,500);


  
}


void macgyver() {

  nota(Si1,166);nota(Mi2,166);nota(La2,166);nota(Si2,166);
  nota(La2,333);retardo(333);nota(Mi2,166);nota(Si1,401);retardo(197);nota(Mi2,166);
  nota(La2,166);nota(Si2,166);nota(La2,166);nota(Mi2,166);
  nota(Si1,166);nota(Mi2,166);nota(Si1,166);nota(Mi2,166);
  nota(La2,166);nota(Si2,166);nota(La2,333);retardo(333);nota(Mi2,166);
  nota(Si1,401);retardo(197);nota(Mi2,166);nota(Re3,166);nota(Do3,166);
  nota(Re3,166);nota(Do3,166);nota(Si2,166);nota(La2,166);
  nota(Si1,166);nota(Mi2,166);nota(La2,166);nota(Si2,166);
  nota(La2,333);retardo(333);nota(Mi2,166);nota(Si1,401);retardo(197);nota(Mi2,166);
  nota(La2,166);nota(Si2,166);nota(La2,166);nota(Mi2,166);
  nota(Si1,166);nota(Mi2,166);nota(Si1,166);nota(Mi2,166);
  nota(La2,166);nota(Si2,166);nota(La2,333);retardo(333);nota(Mi2,166);
  nota(Si1,401);retardo(197);nota(Mi2,166);nota(Re3,166);nota(Do3,166);
  nota(Re3,166);nota(Do3,166);nota(Si2,166);nota(La2,166);
  

}
void harrypotter(){
  
  nota(Si2,250);nota(Mi3,375);nota(Sol3,125);nota(Fa3S,250);nota(Mi3,500);
  nota(Si3,250);nota(La3,750);nota(Fa3S,750);nota(Mi3,375);nota(Sol3,125);
  nota(Fa3S,250);nota(Re3,500);nota(Fa3,250);nota(Si2,1250);nota(Si2,250);
  nota(Mi3,375);nota(Sol3,125);nota(Fa3S,250);nota(Mi3,500);nota(Si3,250);
  nota(Re4,500);nota(Do4S,250);nota(Do4,500);nota(Sol3S,250);nota(Do4,375);
  nota(Si3,125);nota(La3S,250);nota(Fa3S,500);nota(Sol3,250);nota(Mi3,1250);
  nota(Sol3,250);nota(Si3,500);nota(Sol3,250);nota(Si3,500);nota(Sol3,250);
  nota(Do4,500);nota(Si3,250);nota(La3S,500);nota(Fa3S,250);nota(Sol3,375);
  nota(Si3,125);nota(La3S,250);nota(La2S,500);nota(Si2,250);nota(Si3,1250);
  nota(Sol3,250);nota(Si3,500);nota(Sol3,250);nota(Si3,500);nota(Sol3,250);
  nota(Re4,500);nota(Do4S,250);nota(Do4,500);nota(Sol3S,250);nota(Do4,375);
  nota(Si3,125);nota(La3S,250);nota(Fa3S,500);nota(Sol3,250);nota(Mi3,1250);  
  
}


void badhabits(){
    /*
      nota(MI,150);nota(FA,150);nota(MI,150);nota(FA,150);nota(MI,150);nota(FA,150);
      nota(MI,600);nota(MI,150);nota(FA,150);nota(MI,150);
      nota(DO,600);nota(RE,600);retardo(600);   
    */     
    /*
      nota(LA,150);nota(SOL,150);nota(LA,150);nota(SOL,150);nota(LA,150);nota(SOL,150);
      nota(SOL,300);retardo(150);
      nota(LA,150);nota(LA,150);nota(LA,150);nota(LA,150);
      nota(DO2,450);retardo(150); nota(RE,300);retardo(600);
      nota(FA,150);nota(FA,150);nota(FA,450);
      nota(SOL,450);nota(SOL,150);nota(LA,150);     
      retardo(150);nota(DO2,450);nota(DO2,300);nota(LA,300);   
      nota(RE2,1200);
*/
      nota(LA,300);nota(LA,150);nota(SOL,150);nota(SOL,150);nota(FA,150);
      nota(FA,300);nota(RE,300);nota(FA,150);nota(FA,150);nota(FA,150);nota(FA,150);
      retardo(300); nota(LA,300);nota(LA,150);nota(SOL,150);nota(SOL,150);nota(FA,150);
      nota(SOL,300);nota(RE,300);nota(FA,150);nota(FA,150);nota(FA,150);nota(SOL,150);

      nota(SOL,300);nota(LA,300);nota(LA,150);nota(SOL,150);nota(SOL,150);nota(FA,150);
      nota(RE,300);retardo(450);nota(RE2,150);nota(DO2,150);nota(RE2,150);
      nota(LA,450);nota(SOL,150);retardo(150);nota(DO2,150);nota(LA,300);
      retardo(1200);
/*
      retardo(300);nota(LA,300);nota(LA,150);nota(SOL,150);nota(SOL,150);nota(FA,150);
      nota(RE,300);retardo(450);nota(RE2,150);nota(DO2,150);nota(RE2,150);
      nota(LA,450);nota(FA,150);nota(SOL,150);nota(LA,150);nota(RE,300);

      retardo(1200);
      retardo(300);
      nota(LA,300);nota(LA,150);nota(SOL,150);nota(SOL,150);nota(FA,150);
      nota(RE,300);retardo(900);

      nota(FA,300);retardo(450);nota(MI,150);nota(FA,150);nota(SOL,150);
      nota(LA,450);nota(DO,600);nota(RE,150);

      retardo(1200);
      retardo(1200);
  */    
}

void tetris() {

 nota(Do1,242);
nota(Sol2,242);nota(Mi2,242);nota(Sol0,242);
nota(Sol2,242);nota(Mi2,242);nota(Do1,242);
nota(Sol2,242);nota(Mi2,242);nota(Sol0,242);
nota(Sol2,242);nota(Mi2,242);nota(Re1,242);
nota(La2,242);nota(Fa2,242);nota(Do1S,242);
nota(La2,242);nota(Fa2,242);nota(Do1,242);
nota(La2,242);nota(Fa2,242);nota(Do1S,242);
nota(La2,242);nota(Fa2,242);nota(Re1,242);
nota(La2,242);nota(Fa2,242);nota(La0,242);
nota(La2,242);nota(Fa2,242);nota(Sol0,242);
nota(Fa2,242);nota(Re2,242);nota(Sol0,242);
nota(Fa2,242);nota(Re2,242);nota(Do1,242);
nota(Mi2,242);nota(Sol2,242);nota(Sol0,242);
nota(Mi2,242);nota(Sol2,242);nota(Do1,242);
nota(Do1,242);nota(Mi2,242);nota(Sol2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Do1,242);nota(Mi2,242);nota(Sol2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Re1,242);nota(Fa2,242);nota(La2,242);
nota(Do1S,242);nota(La2,242);nota(Fa2,242);
nota(Do1,242);nota(Fa2,242);nota(La2,242);
nota(Do1S,242);nota(Fa2,242);nota(La2,242);
nota(Re1,242);nota(Fa2,242);nota(La2,242);
nota(La0,242);nota(La2,242);nota(Fa2,242);
nota(Sol0,242);nota(Fa2,242);nota(Re2,242);
nota(Sol0,242);nota(Fa2,242);nota(Re2,242);
nota(Do1,242);nota(Sol2,242);nota(Mi2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Do1,242);nota(Re1,242);nota(Fa2,242);
nota(Re2,242);nota(Do1S,242);nota(Fa2,242);
nota(Re2,242);nota(Do1,242);nota(Fa2,242);
nota(Re2,242);nota(La0,242);nota(Fa2,242);
nota(Re2,242);nota(Do1,242);nota(Sol2,242);
nota(Mi2,242);nota(Sol0,242);nota(Sol2,242);
nota(Mi2,242);nota(Do1,242);nota(Sol2,242);
nota(Mi2,242);nota(Sol0,242);nota(Sol2,242);
nota(Mi2,242);nota(Re1,242);nota(Fa2,242);
nota(La2,242);nota(La0,242);nota(Fa2,242);
nota(La2,242);nota(Re1,242);nota(La2,242);
nota(Fa2,242);nota(La0,242);nota(La2,242);
nota(Fa2,242);nota(Sol0,159);retardo(347);nota(Sol0,159);
retardo(347);nota(Sol0,159);retardo(347);nota(Sol0,159);retardo(347);nota(Sol0,159);
retardo(347);nota(Sol0,159);retardo(347);nota(Sol1,242);nota(Do1,242);
nota(Sol2,242);nota(Mi2,242);nota(Sol0,242);
nota(Sol2,242);nota(Mi2,242);nota(Do1,242);
nota(Sol2,242);nota(Mi2,242);nota(Sol0,242);
nota(Sol2,242);nota(Mi2,242);nota(Re1,242);
nota(La2,242);nota(Fa2,242);nota(Do1S,242);
nota(La2,242);nota(Fa2,242);nota(Do1,242);
nota(La2,242);nota(Fa2,242);nota(Do1S,242);
nota(La2,242);nota(Fa2,242);nota(Re1,242);
nota(La2,242);nota(Fa2,242);nota(La0,242);
nota(La2,242);nota(Fa2,242);nota(Sol0,242);
nota(Fa2,242);nota(Re2,242);nota(Sol0,242);
nota(Fa2,242);nota(Re2,242);nota(Do1,242);
nota(Mi2,242);nota(Sol2,242);nota(Sol0,242);
nota(Mi2,242);nota(Sol2,242);nota(Do1,242);
nota(Do1,242);nota(Mi2,242);nota(Sol2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Do1,242);nota(Mi2,242);nota(Sol2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Re1,242);nota(Fa2,242);nota(La2,242);
nota(Do1S,242);nota(La2,242);nota(Fa2,242);
nota(Do1,242);nota(Fa2,242);nota(La2,242);
nota(Do1S,242);nota(Fa2,242);nota(La2,242);
nota(Re1,242);nota(Fa2,242);nota(La2,242);
nota(La0,242);nota(La2,242);nota(Fa2,242);
nota(Sol0,242);nota(Fa2,242);nota(Re2,242);
nota(Sol0,242);nota(Fa2,242);nota(Re2,242);
nota(Do1,242);nota(Sol2,242);nota(Mi2,242);
nota(Sol0,242);nota(Sol2,242);nota(Mi2,242);
nota(Do1,242);nota(Re1,242);nota(Fa2,242);
nota(Re2,242);nota(Do1S,242);nota(Fa2,242);
nota(Re2,242);nota(Do1,242);nota(Fa2,242);
nota(Re2,242);nota(La0,242);nota(Fa2,242);
nota(Re2,242);nota(Do1,242);nota(Sol2,242);
nota(Mi2,242);nota(Sol0,242);nota(Sol2,242);
nota(Mi2,242);nota(Do1,242);nota(Sol2,242);
nota(Mi2,242);nota(Sol0,242);nota(Sol2,242);
nota(Mi2,242);nota(Re1,242);nota(Fa2,242);
nota(La2,242);nota(La0,242);nota(La2,242);
nota(Fa2,242);nota(Re1,242);nota(La2,242);
nota(Fa2,242);nota(La0,242);nota(La2,242);
nota(Fa2,242);nota(Sol0,159);retardo(347);nota(Sol0,159);
retardo(347);nota(Sol0,159);retardo(347);nota(Sol0,159);retardo(347);nota(Sol0,159);
retardo(347);nota(Sol0,159);retardo(347);nota(Sol1,242);
  
}

void setup(void) {
  
  Serial.begin(115200);

  fases();
  
  pinMode(21,INPUT); //Boton 1
  pinMode(16,INPUT); //Boton 2
  pinMode(5,INPUT); //Boton 3
  pinMode(17,INPUT); //Boton 4

  pinMode(32,OUTPUT); //Altavoz 1
  pinMode(39,OUTPUT); //Altavoz 2

  SD.begin(12);
  tft.init();
  
  tft.setRotation(1);
 // tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);

  delay(500);
    
  drawSdJpeg("/presentacion.jpg",0,0);
  
  esperar(5000);
    
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.drawString("PRESS TO START !!",62,90);
   
  espera=true;
  
  while(espera) {
      if(digitalRead(21)==HIGH) { espera=false;}
      if(digitalRead(16)==HIGH) { espera=false;}
      if(digitalRead(5)==HIGH) { espera=false;}
      if(digitalRead(17)==HIGH) { espera=false;}   
      delay(100);
  }

  pintamosfases();
  
  cambio=true;

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(100); 


}

void loop() {  //en este core nos encargamos del sonido
    
    //Escala Musical
    /*  
    nota(C,500);
    nota(D,500);
    nota(E,500);
    nota(F,500);
    nota(G,500);
    nota(A,500);
    nota(B,500);
    nota(C2,500);
    */

    
        
    velocidadmusica=6;
    alan();yield();
    
    velocidadmusica=2;
    supermario();yield();

    velocidadmusica=3;
    tetris();
    
    velocidadmusica=3;
    world();yield();
    
    velocidadmusica=6;
    macgyver();yield();

    velocidadmusica=1.5;
    equipoa();yield();
    
    velocidadmusica=1.5;
    hollywood();yield();
    
    velocidadmusica=1.5;
    harrypotter();yield();
    
    velocidadmusica=1.5;
    badhabits();yield();
         
    
    
} 


void Task1code( void * pvParameters ){    // en este Core recogemos las peticiones web
  
  for(;;){    
    
    if((colision)||(caenbolas)){ cambio=true;}
    
    tft.fillRect(80,190,160,2,TFT_WHITE);

    //Palanca

    if(cambio) {    

      dibujapuntos(false);
    
      if(palancax>2000) {     //control izquierda
          tft.fillCircle(flechax,flechay,3,TFT_BLACK);
          tft.drawLine(159,226,flechax-1,flechay-1,TFT_BLACK);
          tft.drawLine(160,226,flechax,flechay,TFT_BLACK);
          tft.drawLine(161,226,flechax+1,flechay+1,TFT_BLACK);
          if(flechax>130) { flechax--; }
      }else if(palancax<1200) {    //control derecha
          tft.fillCircle(flechax,flechay,3,TFT_BLACK);
          tft.drawLine(159,226,flechax-1,flechay-1,TFT_BLACK);
          tft.drawLine(160,226,flechax,flechay,TFT_BLACK);
          tft.drawLine(161,226,flechax+1,flechay+1,TFT_BLACK);        
          if(flechax<190) { flechax++; }        
      }
    
      tft.drawLine(159,226,flechax-1,flechay-1,TFT_ORANGE);
      tft.drawLine(160,226,flechax,flechay,TFT_ORANGE);
      tft.drawLine(161,226,flechax+1,flechay+1,TFT_ORANGE);

      tft.fillCircle(flechax,flechay,3,TFT_ORANGE);

    }
    
    if(nuevabola) {    
      do{
          colorbola=random(1,7);
          boolean ok=false;
          for(i=0;i<6;i++){
            for(j=0;j<8;j++){
              if(bolas[i][j]==colorbola) { ok=true;j=8;i=6;}  
            }
          }
          if(!ok){ colorbola=0; }
      }while(colorbola==0);
      
      colorbolaTFT=colorbolas(colorbola);
      
      nuevabola=false;
      
    }

    if((boton1)||(boton2)||(boton3)||(boton4)) {
        bolaay=5;
        bolaax=(flechax-160)/5;      
    }


    ///Bola 

    if((cambio)||(bolaay!=0)){
    
      tft.fillCircle(bolax,bolay,10,TFT_BLACK);
    
      bolax+=bolaax;
      bolay-=bolaay;

      if((bolax<=92)||(bolax>=228)){ bolaax=-bolaax;bolax=bolax+(2*bolaax);}
     
      if(bolay<20) { 

        jjj=0;
        
        if(bolax<106) { iii=0; }
        else if(bolax<132) { iii=1; }
        else if(bolax<158) { iii=2; }
        else if(bolax<184) { iii=3; }
        else if(bolax<210) { iii=4; }
        else { iii=5; }

        bolas[iii][0]=colorbola;
        
        pintatodaslasbolas();
        
        nuevabola=true;
        bolaay=0;bolaax=0;bolax=160;bolay=217;  
        
        
      }
    
      tft.fillCircle(bolax,bolay,10,colorbolaTFT);

      colisionbolas();

      if(nuevabola) { colision=true; }
      
      
    }

    ///Caen Bolas

    if(caenbolas) {
      borrabolascaidas();
      caeny+=10;
      pintabolascaidas();
      if(caeny>240) { 
        caenbolas=false;
        borrabolascaidas();
      }
    }
    
    

    
    boolean terminado=true;

    //Comprobamos que halla al menos una bola
    for(i=0;i<6;i++){
      for(j=0;j<8;j++){
        if(bolas[i][j]!=0) { terminado=false;j=8;i=6;}  
      }
    }
      
    if(terminado){
        caenbolas=false;
        Serial.println("Fase Terminada"); 
        fase++;
        faseterminada=true;
        delay(200);
        m_faseganada();
        faseterminada=false;
        fases();
        pintamosfases();
    }
       
    teclas();    
     

    if(fin) {
        
        tft.fillRect(0,0,320,240,TFT_WHITE);
        tft.fillRect(2,2,316,236,TFT_BLACK);

        tft.drawString("SCORE :",70,90);

        String puntosS="";
        puntosS+=puntos;
        int str_len = puntosS.length() + 1; 
        char puntos2[str_len];
        puntosS.toCharArray(puntos2,str_len);
  
        tft.drawString(puntos2,170,90);

        esperar(2000);

        tft.fillRect(0,0,320,240,TFT_WHITE);
        tft.fillRect(2,2,316,236,TFT_BLACK);
        
        tft.drawString("THE END",105,90);
        esperar(1000);
        
        tft.fillRect(0,0,320,240,TFT_WHITE);
        tft.fillRect(2,2,316,236,TFT_BLACK);
        
        tft.drawString("MAKE BY TURMAN ",80,90);
        tft.drawString("  FOR HIS ",90,120);
        tft.drawString(" DAUGHTERS",90,150);
        
        esperar(3000);
        
      
      ESP.restart();
    }

    delay(20);
    yield();
    
  } ///fin del for
  
}
