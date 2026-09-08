//https://www.cg.alphacodingskills.com/compile-cpp-online.php

#include<iostream>
#include <fstream>

using namespace std;

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




int main(){
  ofstream fout;
  long int i, sze, cnt=0;
  unsigned char *memalloc;
  memalloc = (unsigned char *)malloc(400000);
  
  sze = Rle_Encode(dta, sizeof(dta), memalloc, 400000);
  cout<<endl<<endl<<endl<<sze<<endl<<endl<<endl;
  
  fout.open("file.txt");
  
    for(i=0; i<sze; i++){
      fout<< (unsigned int)memalloc[i];
      fout<< ", ";
      if(cnt++>32){fout<<endl; cnt=0;}
    }
    return 0;
}
