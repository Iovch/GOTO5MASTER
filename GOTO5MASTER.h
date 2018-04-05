/*
 * GOTO5MASTER.h Written by Igor Ovchinnikov 17/07/2016
 */
 
long Stepper_step(long ipSteps, unsigned uStepPin, unsigned uDirPin, unsigned uStepsPS)
{
 long iSteps=ipSteps, lRetVal=0;
 if((uStepPin>53)||(uDirPin>53)) return lRetVal;
 
 if(iSteps > 0) digitalWrite(uDirPin,  LOW);
 if(iSteps < 0) digitalWrite(uDirPin,  HIGH);
 iSteps=abs(iSteps);

 while (iSteps>0)
 {
  digitalWrite(uStepPin,  HIGH);
  delay(1000/uStepsPS);
  delayMicroseconds(1000*(1000%uStepsPS));
  digitalWrite(uStepPin,  LOW);
  iSteps--;
  if (ipSteps>0) lRetVal++; else lRetVal--;
 }
 return lRetVal;
}

void Stepper_X_step(int ipSteps)
{
  Stepper_step(ipSteps, DX_STEP_PIN, DX_DIR_PIN, imStepsXPS);
}

void Stepper_Y_step(int ipSteps)
{
  Stepper_step(ipSteps, DY_STEP_PIN, DY_DIR_PIN, imStepsYPS);
}

// Функция int AskJoy() возвращает при ее вызове следующие значения:

//     0 - когда ничего не надо делать
//   256 - когда надо сделать микрошаг вперед по оси Х
//   512 - когда надо сделать полныйшаг вперед по оси Х
//  1024 - когда надо сделать микрошаг назад по оси Х
//  2048 - когда надо сделать полныйшаг назад по оси Х
//  4096 - когда надо сделать микрошаг вверх по оси У
//  8192 - когда надо сделать полныйшаг вверх по оси У
// 16384 - когда надо сделать микрошаг вниз по оси У
// 32768 - когда надо сделать полныйшаг вниз по оси У
// 65536 - включить/отключить трекинг
// Используются суммы указанных значений

long AskJOY()
{
  int iA1=0, iA2=0, iA3=0;
  long iRetValue=0;

  iA1 = analogRead(X_JOY_SENCE);
  iA2 = analogRead(Y_JOY_SENCE);
  iA3 = analogRead(SW_JOY_SENCE);
    
  if(iA1<25)                { iRetValue=iRetValue | 512;  } // Полный шаг X+
  if(iA1>=25 && iA1 < 490)  { iRetValue=iRetValue | 256;  } // Микрошаг X+
  if(iA1>520 && iA1<=1000)  { iRetValue=iRetValue | 1024; } // Микрошаг X-
  if(iA1>1000)              { iRetValue=iRetValue | 2048; } // Полный шаг X-
  
  if(iA2<25)                { iRetValue=iRetValue | 8192;  } // Полный шаг Y+
  if(iA2>=25  && iA2 < 490) { iRetValue=iRetValue | 4096;  } // Микрошаг Y+
  if(iA2>520  && iA2<=1000) { iRetValue=iRetValue | 16384; } // Микрошаг Y-
  if(iA2>1000)              { iRetValue=iRetValue | 32768; } // Полный шаг Y-
  
  if(iA3<500) {iRetValue=65536; delay(250);}    // Включить/отключить трекинг
  
  return iRetValue;
}

long AskSlaveI2C()
{
  long iRetValue=0;
  Wire.requestFrom(10,1); // Считать 1 байт с адреса 10
  if (Wire.available())   // slave may send less than requested
  {
    iRetValue = Wire.read(); // Полученный байт
  }
 switch (iRetValue)
 {
  case 14: {iRetValue=65536; break;}
  case 15: {iRetValue=    4; break;}
  case 18: {iRetValue=    3; break;}
  case 20: {iRetValue= 8192; break;}
  case 21: {iRetValue= 2048; break;}
  case 23: {iRetValue=  512; break;}
  case 24: {iRetValue=32768; break;}
  default: iRetValue=0;
 }
 return iRetValue; 
}

unsigned long StrToHEX (String STR)
{
  int  i;
  char c;
  unsigned long ulVal=0;
  for (i=0; i<STR.length(); i++)
  {
   ulVal=ulVal*16;
   c=STR.charAt(i);
   switch (c) 
    {
      case 'f': ;
      case 'F': ulVal++;
      case 'e': ;
      case 'E': ulVal++;
      case 'd': ;
      case 'D': ulVal++;
      case 'c': ;
      case 'C': ulVal++;
      case 'b': ;
      case 'B': ulVal++;
      case 'a': ;
      case 'A': ulVal++;
      case '9': ulVal++;
      case '8': ulVal++;
      case '7': ulVal++;
      case '6': ulVal++;
      case '5': ulVal++;
      case '4': ulVal++;
      case '3': ulVal++;
      case '2': ulVal++;
      case '1': ulVal++;
    };
  };
 return ulVal;
};

String HexTo8D (unsigned long Hex)
{
  String STR0="";
  char c = '0';
  if (Hex<0x10000000) STR0 += c;
  if (Hex<0x1000000)  STR0 += c;
  if (Hex<0x100000)   STR0 += c;
  if (Hex<0x10000)    STR0 += c;
  if (Hex<0x1000)     STR0 += c;
  if (Hex<0x100)      STR0 += c;
  if (Hex<0x10)       STR0 += c;
  return STR0;
};

void HexRaToString(unsigned long ulRaVal, unsigned long ulMaxRaVal)
{
  double udRa=0;
  int iRaH=0, iRaM=0, iRaS=0;
  udRa=double(ulRaVal>>8)/double(ulMaxRaVal>>8);
  iRaH=udRa*24;
  udRa=udRa-double(iRaH)/24;
  iRaM=udRa*24*60;
  udRa=udRa-double(iRaM)/24/60;
  iRaS=round(udRa*24*60*60);
  LCDString1=String("Ra="+String(iRaH)+"h"+String(iRaM)+"m"+String(iRaS)+"s");
  switch (iStDX)
  {
  case -1: {LCDString1=LCDString1+" S"; break;}
  case  1: {LCDString1=LCDString1+" N"; break;}
  }
}

void HexDeToString(unsigned long ulDeVal, unsigned long ulMaxDeVal)
{
  double udDe=0;
  int iDeG=0, iDeM=0, iDeS=0;
  udDe=double(ulDeVal>>8)/double(ulMaxDeVal>>8);
  if(udDe>0.5){udDe=udDe-1;}
  iDeG=udDe*360;
  udDe=udDe-double(iDeG)/360;
  iDeM=udDe*360*60;
  udDe=udDe-double(iDeM)/360/60;
  iDeM=abs(iDeM);
  iDeS=round(abs(udDe*360*60*60));
  LCDString2=String("De="+String(iDeG)+"*"+String(iDeM)+"m"+String(iDeS)+"s");
  switch (iStDY)
  {
  case -1: {LCDString2=LCDString2+" N/E"; break;}
  case  1: {LCDString2=LCDString2+" N/W"; break;}
  }
}

int LCDPrintString (String str, int row, int kol)
{
  int i=0;
  while (i<16 && i<str.length())
  {
    lcd.setCursor(kol-1+i, row-1);
    lcd.print(str.charAt(i));
    i++;
  }
}

int LCDPrintSTR (char* str, int row, int kol)
{
  int i=0;
  while (i<16 && str[i]!='\0')
  {
    lcd.setCursor(kol-1+i, row-1);
    lcd.print(str[i]);
    i++;
  }
}

void LCDPrint()
 {
  if(!bLCD) // Экваториальный режим
  {
   if(bAlignment)
   {
    HexRaToString(ulRA, MVRA);
    HexDeToString(ulDE, MVDE);
   }
   else
   {
    if (iStDY==-1) LCDString1="   N/W or S/E   "; //Телескоп слева от полярной оси
    if (iStDY== 1) LCDString1="   N/E or S/W   "; //Телескоп справа от полярной оси
    if (iStDY== 0) LCDString1=" Arduino GOTO4  "; //Телескоп не сориентирован по склонению
    if (iStDX== 0) LCDString1+=" RAERR"; //Не задано направление ведения телескопа
    if (bRun){LCDString2=" TRACKING"; if(iStDX>0) LCDString2+=" SOUTH"; if(iStDX<0) LCDString2+=" NORHT";}
    else      LCDString2="    STOPPED     ";
   }
    lcd.clear();
//  SetLCDLight();
  LCDPrintString(LCDString1,1,1);
  LCDPrintString(LCDString2,2,1);
  bLCD=true;
  }
 }

//     0 - когда ничего не надо делать
//   256 - когда надо сделать микрошаг вперед по оси Х
//   512 - когда надо сделать полныйшаг вперед по оси Х
//  1024 - когда надо сделать микрошаг назад по оси Х
//  2048 - когда надо сделать полныйшаг назад по оси Х
//  4096 - когда надо сделать микрошаг вверх по оси У
//  8192 - когда надо сделать полныйшаг вверх по оси У
// 16384 - когда надо сделать микрошаг вниз по оси У
// 32768 - когда надо сделать полныйшаг вниз по оси У
// 65536 - включить/отключить трекинг
// Используются суммы указанных значений

void LCDCOR (int pKey)
{
 if(true)
 {
  if(pKey>=256 && !bAlignment) LCDPrintSTR (" Correction ", 2, 1);
  switch (pKey)
  {
   case   256: if(iStDX!=0) LCDPrintSTR ("  > ", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break;
   case   512: if(iStDX!=0) LCDPrintSTR ("  >>", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break; 
   case  1024: if(iStDX!=0) LCDPrintSTR (" <  ", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break;
   case  2048: if(iStDX!=0) LCDPrintSTR ("<<  ", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break;
   case  4096: if(iStDY!=0) LCDPrintSTR ("  ^ ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  4352: if(iStDX!=0) LCDPrintSTR (" ^> ", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break;    
   case  4608: if(iStDY!=0) LCDPrintSTR (" ^>>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  5120: if(iStDX!=0) LCDPrintSTR (" <^ ", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break; 
   case  6144: if(iStDY!=0) LCDPrintSTR ("<<^ ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  8192: if(iStDY!=0) LCDPrintSTR (" ^^ ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  8448: if(iStDY!=0) LCDPrintSTR (" ^^>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  8704: if(iStDY!=0) LCDPrintSTR ("^^>>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case  9216: if(iStDY!=0) LCDPrintSTR ("<^^ ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 10240: if(iStDX!=0) LCDPrintSTR ("<<^^", 2, 13); else LCDPrintSTR ("N/S Position ERR", 2, 1); break;   
   case 16384: if(iStDY!=0) LCDPrintSTR ("  v ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 16640: if(iStDY!=0) LCDPrintSTR (" v> ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 16896: if(iStDY!=0) LCDPrintSTR (" v>>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 17408: if(iStDY!=0) LCDPrintSTR (" <v ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 18432: if(iStDY!=0) LCDPrintSTR ("<<v ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 33024: if(iStDY!=0) LCDPrintSTR (" vv>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 33280: if(iStDY!=0) LCDPrintSTR ("vv>>", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 32768: if(iStDY!=0) LCDPrintSTR (" vv ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 33792: if(iStDY!=0) LCDPrintSTR ("<vv ", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
   case 34816: if(iStDY!=0) LCDPrintSTR ("<<vv", 2, 13); else LCDPrintSTR ("E/W Position ERR", 2, 1); break;
  };
 }
}

