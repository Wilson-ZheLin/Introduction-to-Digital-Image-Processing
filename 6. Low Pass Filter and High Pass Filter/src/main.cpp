#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

void DFT(Image *, float *, float *);
void IDLPF(Image *, float *, float *);
void BLPF(Image *, float *, float *);
void GLPF(Image *, float *, float *);
void HPF(Image *, float *, float *);
Image *iDFT(Image *, float *, float *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *);
void SavePNMImage(Image *, char *);

int main(int argc, char **argv)
{
    // Please adjust the input filename and path to suit your needs:
    char* file_in = (char*)"lena.pgm";
    char* file_out = (char*)"";
    TestReadImage(file_in, file_out);
    return(0);
}

int TestReadImage(char *file_in, char *file_out)
{
    Image *image;
    float real_array[65536], imaginary_array[65536];
    
    image = ReadPNMImage(file_in);
    DFT(image, real_array, imaginary_array);
    IDLPF(image, real_array, imaginary_array);
    BLPF(image, real_array, imaginary_array);
    GLPF(image, real_array, imaginary_array);
    HPF(image, real_array, imaginary_array);
    return(0);
}

// Algorithms Code:
void DFT(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float real, imaginary;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    printf("DFT algorithm is executing...\n");
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            real = 0;
            imaginary = 0;
            
            for(int m = 0; m < image->Height; m++) {
                for(int n = 0; n < image->Width; n++) {
                    float temp = (float)i * m / (float)image->Height + (float)j * n / (float)image->Width;
                    int offset = (m + n) % 2 == 0 ? 1 : -1;
                    real += tempin[image->Width * m + n] * cos(-2 * pi * temp) * offset;
                    imaginary += tempin[image->Width * m + n] * sin(-2 * pi * temp) * offset;
                }
            }
            real_array[image->Width * i + j] = real;
            imaginary_array[image->Width * i + j] = imaginary;
        }
    }
    printf("DFT Finished!\n");
}

Image *iDFT(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float real;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    printf("iDFT algorithm is executing...\n");
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            real = 0;
            
            for (int m = 0; m < image->Height; m++){
                for (int n = 0; n < image->Width; n++){
                    float temp = (float)i * m / (float)image->Height + (float)j * n / (float)image->Width;
                    real += real_array[image->Width * m + n] * cos(2 * pi * temp) - imaginary_array[image->Width * m + n] * sin(2 * pi * temp);
                }
            }
            int offset = (i + j) % 2 == 0 ? 1 : -1;
            int temp = (int)(real / ((float)image->Height*(float)image->Width) * offset);
            if(temp < 0) temp = 0;
            if(temp > 255) temp = 255;
            tempout[image->Width * i + j] = temp;
        }
    }
    printf("iDFT Finished!\n");
    return (outimage);
}

void IDLPF(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float height = image->Height, width = image->Width;
    float real[(int)(height*width)], imaginary[(int)(height*width)];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            float dis = sqrt(pow((float)i - height/2, 2) + pow((float)j - width/2, 2));
            int H;
            if(dis <= 60) H = 1;
            else H = 0;
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("IDLPF Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"IDLPF.pgm");
}

void BLPF(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float height = image->Height, width = image->Width;
    float real[(int)(height*width)], imaginary[(int)(height*width)];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            float dis = sqrt(pow((float)i - height/2, 2) + pow((float)j - width/2, 2));
            float H = 1 / (1 + pow(dis / (float)60, 2));
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("BLPF Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"BLPF.pgm");
}

void GLPF(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float height = image->Height, width = image->Width;
    float real[(int)(height*width)], imaginary[(int)(height*width)];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            float dis = sqrt(pow((float)i - height/2, 2) + pow((float)j - width/2, 2));
            float H = pow(M_E, pow(dis, 2) * -1 / (pow(60, 2) * 2));
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("GLPF Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"GLPF.pgm");
}

void HPF(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float height = image->Height, width = image->Width;
    float real[(int)(height*width)], imaginary[(int)(height*width)];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            float dis = sqrt(pow((float)i - height/2, 2) + pow((float)j - width/2, 2));
            float H = 1 / (1 + pow((float)30 / dis, 2));
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("HPF Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"HPF.pgm");
}
// Algorithms End.


/*******************************************************************************/
//Read PPM image and return an image pointer
/**************************************************************************/
Image *ReadPNMImage(char *filename)
{
    char ch;
    int  maxval, Width, Height;
    int size, num,j;
    FILE *fp;
    Image *image;
    int num_comment_lines=0;
    
    
    image=(Image *)malloc(sizeof(Image));
    
    if((fp=fopen(filename,"rb")) == NULL){
        printf("Cannot open %s\n", filename);
        exit(0);
    }
    
    printf("Loading %s ...",filename);
    
    if (fscanf(fp, "P%c\n", &ch) != 1) {
        printf("File is not in ppm/pgm raw format; cannot read\n");
        exit(0);
    }
    if( ch != '6' && ch !='5') {
        printf("File is not in ppm/pgm raw format; cannot read\n");
        exit(0);
    }
    
    if(ch == '5')image->Type=GRAY;  // Gray (pgm)
    else if(ch == '6')image->Type=COLOR;  //Color (ppm)
    /* skip comments */
    ch = getc(fp);
    j=0;
    while (ch == '#')
    {
        image->comments[num_comment_lines][j]=ch;
        j++;
        do {
            ch = getc(fp);
            image->comments[num_comment_lines][j]=ch;
            j++;
        } while (ch != '\n');     /* read to the end of the line */
        image->comments[num_comment_lines][j-1]='\0';
        j=0;
        num_comment_lines++;
        ch = getc(fp);            /* thanks, Elliot */
    }
    
    if (!isdigit((int)ch)){
        printf("Cannot read header information from ppm file");
        exit(0);
    }
    
    ungetc(ch, fp);               /* put that digit back */
    
    /* read the width, height, and maximum value for a pixel */
    fscanf(fp, "%d%d%d\n", &Width, &Height, &maxval);
    
    /*
     if (maxval != 255){
     printf("image is not true-color (24 bit); read failed");
     exit(0);
     }
     */
    
    if(image->Type == GRAY)
        size          = Width * Height;
    else  if(image->Type == COLOR)
        size          = Width * Height *3;
    image->data   = (unsigned char *) malloc(size);
    image->Width  = Width;
    image->Height = Height;
    image->num_comment_lines= num_comment_lines;
    
    if (!image->data){
        printf("cannot allocate memory for new image");
        exit(0);
    }
    
    num = fread((void *) image->data, 1, (size_t) size, fp);
    //printf("Complete reading of %d bytes \n", num);
    if (num != size){
        printf("cannot read image data from file");
        exit(0);
    }
    
    //for(j=0;j<image->num_comment_lines;j++){
    //      printf("%s\n",image->comments[j]);
    //      }
    
    fclose(fp);
    
    /*-----  Debug  ------*/
    
    if(image->Type == GRAY)printf("..Image Type PGM\n");
    else printf("..Image Type PPM Color\n");
    /*
     printf("Width %d\n", Width);
     printf("Height %d\n",Height);
     printf("Size of image %d bytes\n",size);
     printf("maxvalue %d\n", maxval);
     */
    return(image);
}

void SavePNMImage(Image *temp_image, char *filename)
{
    int num,j;
    int size ;
    FILE *fp;
    //char comment[100];
    
    
    printf("Saving Image %s\n", filename);
    fp=fopen(filename, "w");
    if (!fp){
        printf("cannot open file for writing");
        exit(0);
    }
    
    //strcpy(comment,"#Created by Dr Mohamed N. Ahmed");
    
    if(temp_image->Type == GRAY){  // Gray (pgm)
        fprintf(fp,"P5\n");
        size = temp_image->Width * temp_image->Height;
    }
    else  if(temp_image->Type == COLOR){  // Color (ppm)
        fprintf(fp,"P6\n");
        size = temp_image->Width * temp_image->Height*3;
    }
    
    for(j=0;j<temp_image->num_comment_lines;j++)
        fprintf(fp,"%s\n",temp_image->comments[j]);
    
    fprintf(fp, "%d %d\n%d\n", temp_image->Width, temp_image->Height, 255);
    
    num = fwrite((void *) temp_image->data, 1, (size_t) size, fp);
    
    if (num != size){
        printf("cannot write image data to file");
        exit(0);
    }
    
    fclose(fp);
}

/*************************************************************************/
/*Create a New Image with same dimensions as input image*/
/*************************************************************************/

Image *CreateNewImage(Image * image, char *comment)
{
    Image *outimage;
    int size,j;
    
    outimage=(Image *)malloc(sizeof(Image));
    
    outimage->Type = image->Type;
    outimage->num_comment_lines = image->num_comment_lines;

    if(outimage->Type == GRAY)   size = image->Width * image->Height;
    if(outimage->Type == COLOR) size  = image->Width * image->Height * 3;
    outimage->Width = image->Width;
    outimage->Height = image->Height;
    
    /*--------------------------------------------------------*/
    /* Copy Comments for Original Image      */
    for(j=0;j<outimage->num_comment_lines;j++)
        strcpy(outimage->comments[j],image->comments[j]);
    
    /*----------- Add New Comment  ---------------------------*/
    strcpy(outimage->comments[outimage->num_comment_lines],comment);
    outimage->num_comment_lines++;
    
    
    outimage->data = (unsigned char *) malloc(size);
    if (!outimage->data){
        printf("cannot allocate memory for new image");
        exit(0);
    }
    return(outimage);
}

