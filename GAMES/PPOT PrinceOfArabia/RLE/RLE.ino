uint8_t rom[] = {
0xC3, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x0C, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xE1, 0x5F, 0x16, 0x00, 0x19, 0x5E, 0x23, 
0x56, 0xD5, 0xE1, 0xE9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xC3, 0x7E, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0xBE, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xC3, 0xBE, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x5B, 0x00, 0xF5, 0xE5, 0xD5, 0xC5, 0xCD, 
0x6B, 0x00, 0x3E, 0x01, 0xE0, 0xCC, 0xC1, 0xD1, 0xE1, 0xF1, 0xD9, 0xF0, 0xCD, 0xEF, 0x78, 0x00, 
};


uint8_t compressed[16*60];



int IsrepetitionStart(unsigned char *src,int srcLeft){
    if(srcLeft<3) 
      return 0;
    if((src[0]==src[1])&&(src[1]==src[2])) 
      return 1;
    return 0;
}

int  GetRepetitionCount(unsigned char *src,int srcLeft){
    int repeatedbuf=src[0];
    int length=1;
    while(length<srcLeft&&length<0x7f&&src[length]==repeatedbuf)
      length++;
    return length;
}

int GetNonRepetitionCount(unsigned char *src,int srcLeft){
    if(srcLeft<3){
        return srcLeft;}
    int length=2;
    int a=src[0],b=src[1];
    while(length<srcLeft&&length<0x7f&&((a!=b)||(b!=src[length]))){
        a=b;
        b=src[length];
        length++;}
    return length;
}



int Rle_Encode(unsigned char *inbuf, int inSize, unsigned char *outbuf, int onuBufSize){
    unsigned char *src=inbuf; 
    int i;
    int encSize=0;
    int srcLeft=inSize;       
    while(srcLeft>0){         
        int count=0;
        if(IsrepetitionStart(src,srcLeft)){ 
            if((encSize+2)>onuBufSize){      
                return -1;
            } 
            count=GetRepetitionCount(src,srcLeft); 
            outbuf[encSize++]=count|0x80;          
            outbuf[encSize++]=*src;             
            src+=count;                            
            srcLeft-=count;
        }else{
            count=GetNonRepetitionCount(src,srcLeft); 
            if((encSize+count+1)>onuBufSize){ 
                return -1;
            }
            outbuf[encSize++]=count;
            for(i=0;i<count;i++){           
                outbuf[encSize++]=*src++;
            }
            srcLeft-=count;
        }
    }
    return encSize;
}



int Rle_Decode(unsigned char *inbuf,int inSize,unsigned char *outbuf,int onuBufSize){
    unsigned char *src=inbuf;
    int i;
    int decSize=0;
    int count=0;
    while(src<(inbuf+inSize)){
        unsigned char sign=*src++;
        int count=sign & 0x7F;
        if((decSize+count)>onuBufSize){ 
            return -1;
        }
        if((sign&0x80)==0x80){          
            for(i=0;i<count;i++){
                outbuf[decSize++]=*src;
            }
            src++;
        }else{
            for(i=0;i<count;i++){
                outbuf[decSize++]=*src++;
            }
        }           
    }
    return decSize;
}



#define CNT_MAX 40

void setup() {
  uint16_t cntln=0;
  
  Serial.begin(115200);

  Serial.println();Serial.println();Serial.println();Serial.println();Serial.println();
 
  for(uint16_t i=0; i<sizeof(rom); i++){
    Serial.print(rom[i]); Serial.print(", ");
    if(cntln++ > CNT_MAX) {cntln=0; Serial.println();}
  }

  cntln=0;
  Serial.println();Serial.println();Serial.println();Serial.println();Serial.println();

  memset(compressed, 0, sizeof(compressed));
  Serial.print("Encode size:");
  Serial.println(Rle_Encode(rom, sizeof(rom), compressed, sizeof(compressed)));

  for(uint16_t i=0; i<sizeof(compressed); i++){
    Serial.print(compressed[i]); Serial.print(", ");
    if(cntln++ > CNT_MAX) {cntln=0; Serial.println();}
  }

  cntln=0;
  Serial.println();Serial.println();Serial.println();Serial.println();Serial.println();

  memset(rom, 0, sizeof(rom));
  Serial.print("Decode size:");
  Serial.println(Rle_Decode(compressed, sizeof(compressed), rom, sizeof(rom)));
  
  for(uint16_t i=0; i<sizeof(rom); i++){
    Serial.print(rom[i]); Serial.print(", ");
    if(cntln++ > CNT_MAX) {cntln=0; Serial.println();}
}
}



void loop() {
delay(1000);
}
