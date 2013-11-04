void test(){
  g_syncd = true;
  if (g_syncd && g_write_bit){
    Serial.print("write");
    //interval(writeclock);
    delayMicroseconds(100);
    g_bitcount++;
    g_write_bit = false;
    if (g_bitcount == 8){
      g_bitcount = 0;
      Serial.print("byte");
      //interval(byteclock);
      Serial.println();
    }      
  }

  if (g_syncd && g_sample){
    Serial.print("\tsample");
    //interval(bitclock);
    delayMicroseconds(100);
    g_sample = false;
  }
}


void debugBitSample(){
  volatile double x = (g_samplevalue/(double)g_samplecount);
//  volatile double z = (phantom/(double)g_samplecount);
  Serial.println();
  //-----------------------------------------------------
//  Serial.print("phantom: ");
//  Serial.print(phantom);
//  Serial.print("/");
//  Serial.print(g_samplecount);
//  Serial.print(" = ");
//  Serial.print(z);
//  Serial.print("-->  \t");
//  phantom = 0;
  //-----------------------------------------------------
  Serial.print(g_samplevalue);
  Serial.print("/");
  Serial.print(g_samplecount);
  Serial.print(" = ");
  Serial.print(x);
  Serial.print("-->  \t");
  //-----------------------------------------------------
  if (x>0.2 && x<0.8){ Serial.print("X"); CLOCK_COUNTER=0;}
  Serial.print("\t");
  Serial.print(g_bitcount);
  Serial.print(": ");
  
  
  volatile double confidence;
  if(x>0.5){confidence = x;}else{confidence = 1-x;}
  Serial.print("confidence: ");
  Serial.print(confidence);
  Serial.print(": \n");
    
}



void interval(long & time){
  long t = micros() - time;
  time = micros();
  Serial.print("\t");  
  Serial.print(t);  
  Serial.println(" microseconds "); 
}




//--------------------------------------------------------------------------
void configure(){
  status();
  while(!Serial.available()) {
    ;
  }
  while(Serial.available())
  {
    byte s = Serial.read();
    switch(s){
    case 'b':
      g_baudrate = Serial.parseInt();
      Serial.print("baud rate = ");  
      Serial.println(g_baudrate);
      break;
    case 's':
      Serial.print("synchronise...");
      g_syncd = false;
      synchronise();
      break;
    case '\\':
      Serial.println();
      Serial.println("--------------------------------------------------------");
      break;
    default:
      Serial.println("default");
      break;
    }
  }
}


void status(){
  Serial.println("---------------------------------------------");
  Serial.print("baud rate = ");    
  Serial.println(g_baudrate);
  Serial.println("---------------------------------------------");
}



