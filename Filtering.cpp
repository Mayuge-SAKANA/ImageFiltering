#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <cmath>
#include <limits>

void filter_image(double *dImage,double *filteredImage,double *filter,int step,int fsize,int pad,int rWidth, int rStride, int rHeight)
{
  double *f_st= &dImage[0] ;
  double *t_ind;
  int ind=0;
  int fh_st,fh_en,fw_st,fw_en,imf_ind,next_x;

  for(int i=0;i<rHeight ;i+=step){
    for(int j=0;j<rWidth ;j+=step){
      double res=0;
      double *f_ind= filter;
      t_ind=f_st;
      fh_st=0;fh_en=fsize;
      fw_st=0;fw_en=fsize;
      imf_ind=0;
      next_x=step;

    if(j==0){//left end of image
      fw_st=pad;
      imf_ind=pad;
      next_x=pad;
    }else if(j==rWidth-step){//right end of image
      fw_en=fsize-pad;
      next_x=pad+step+(step-1)*rStride;
      }
    if(i==0){//upper end of image
      f_ind+=pad*fsize;
      fh_st=pad;
      if(j==0)next_x=pad;//left upper corner
      else if(j==rWidth-step)next_x=pad+step+(pad-1)*rStride; //right upper corner
      else next_x=step;
    }else if(i==rHeight-step){//bottom end of image
      fh_en=fsize-pad;
      if(j==0)next_x=pad; //left bottom corner
      else next_x=step;
    }
    //Inner product
    for (int k=fh_st;k<fh_en;k++){
      for (int m=fw_st;m<fw_en;m++){
      res+=(f_ind[m])*(t_ind[m-imf_ind]);
      }
      t_ind+=rStride;
      f_ind+=fsize;
      }
      f_st+=next_x;
      filteredImage[ind]=res;
      ind++;
    }
  }
}
void load_RAW(double *dImage,int smWidth,int smStride,int smHeight,char *FileName,int scale)
{
  unsigned char *pImage;
  pImage=(unsigned char *)malloc(smHeight * smStride*  sizeof(unsigned char));
  FILE *fpRaw;
    if ((fpRaw = fopen(FileName, "rb")) != NULL){
      for (int j = 0; j < smHeight; j++){
        fread(&pImage[j*smStride], sizeof(unsigned char), smWidth, fpRaw);
      }
      fclose(fpRaw);
    }
    for(int i=0;i<smHeight ;i+=scale){
      for(int j=0;j<smStride;j+=scale){
      dImage[(i/scale)*(smStride/scale)+(j/scale)]=(double)pImage[i*smStride+j];
    }
    }
}
void loadData(double *W,std::string FileName)
{
  int ind=0;
  std::ifstream stream(FileName);
  std::string line;
  while ( getline(stream, line) ) {
    W[ind]=stod(line);
    ind++;
    }
}
struct Filters{
  double *filter1;//size 256
  double *filter2;//size 1024
  double *filter3;//size 1024
  double *filter4;//size 16
};
struct Data{
  double *Data2D;
  double **Datalist;
  int Width;//Input Image Width
  int Stride;//Input Image Stride
  int Height;//Input Image Height
};

int main()
{
  //get512×512Image
  double *dImage;
  char FileName[]="imgs/hogehoge.raw";
  int smWidth,smHeight,smStride;
  int  i,j;
  smWidth=128;
  smHeight=128;
  smStride=128;
  int step=4;
  int pad=2;
  Data org;
  org.Width=smWidth;
  org.Stride=smStride;
  org.Height=smStride;
  org.Data2D=(double *)malloc(org.Height * org.Stride*  sizeof(double));
  load_RAW(org.Data2D,smWidth,smStride,smHeight,FileName,1);

  //set 1stLayer
  int fsize=8;
  Filters F;
  F.filter1=(double *)malloc(fsize*fsize*  sizeof(double));
  loadData(F.filter1,"W/W1_conv.csv");

  Data L1;
  L1.Width=(org.Width+pad*2)/step-1;
  L1.Height=(org.Height+pad*2)/step-1;
  L1.Stride=(org.Stride+pad*2)/step-1;
  L1.Data2D=(double *)malloc( L1.Stride * L1.Height*  sizeof(double));
  L1.Datalist=(double **)malloc( sizeof(double *));

  //start 1st Layer
  double *filter=&F.filter1[0];
    L1.Datalist[i]=&L1.Data2D[i * L1.Stride * L1.Height];
    memset(L1.Datalist[i], 0, L1.Stride * L1.Height * sizeof(double));
    filter_image(org.Data2D,L1.Datalist[i],filter,step,fsize,pad,org.Width,org.Stride,org.Height);
    filter+=fsize*fsize;


}
