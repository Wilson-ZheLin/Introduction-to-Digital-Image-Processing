#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

Image *DFT(Image *, float *, float *);
Image *iDFT(Image *, float *, float *);
Image *Reconstruct_phaseAngle(Image *, float *, float *);
Image *Reconstruct_magnitude(Image *, float *, float *);
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
    Image *DFT_image, *iDFT_image, *Reconstruct_1, *Reconstruct_2;
    float real_array[65536], imaginary_array[65536];
    
    image = ReadPNMImage(file_in);
    DFT_image = DFT(image, real_array, imaginary_array);
    iDFT_image = iDFT(DFT_image, real_array, imaginary_array);
    Reconstruct_1 = Reconstruct_phaseAngle(DFT_image, real_array, imaginary_array);
    Reconstruct_2 = Reconstruct_magnitude(DFT_image, real_array, imaginary_array);

    // Please adjust the output filenames and paths to suit your needs:
    SavePNMImage(DFT_image, (char*)"DFT.pgm");
    SavePNMImage(iDFT_image, (char*)"iDFT.pgm");
    SavePNMImage(Reconstruct_1, (char*)"PhaseAngle_Reconstruct.pgm");
    SavePNMImage(Reconstruct_2, (char*)"Magnitude_Reconstruct.pgm");
    return(0);
}

// Algorithms Code:
Image *DFT(Image *image, float *real_array, float *imaginary_array) {
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

            int temp = (int)(sqrt(real*real + imaginary*imaginary) / sqrt((float)image->Height*(float)image->Width));
            if(temp < 0) temp = 0;
            if(temp > 255) temp = 255;
            tempout[image->Width * i + j] = temp;
        }
    }
    printf("DFT Finished!\n");
    return (outimage);
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

Image *Reconstruct_phaseAngle(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float *tempArray;
    float real, angle, size = image->Width * image->Height;
    int max = 0, min = 10000; // used to normalization
    Image *outimage;
    
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    tempArray = (float*)malloc(size * sizeof(float));
    
    printf("Phase Angle Reconstruction is executing...\n");
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            real = 0;
            
            for (int m = 0; m < image->Height; m++){
                for (int n = 0; n < image->Width; n++){
                    float temp = (float)i * m / (float)image->Height + (float)j * n / (float)image->Width;
                    angle = atan2(imaginary_array[image->Width * m + n], real_array[image->Width * m + n]);
                    real += cos(angle) * cos(2 * pi * temp) - sin(angle) * sin(2 * pi * temp);
                }
            }
            int temp = round(fabs(real));
            tempArray[image->Width * i + j] = temp;
            if(temp > max) max = temp;
            if(temp < min) min = temp;
        }
    }
    // Normalization:
    for(int i = 0; i < size; i++) {
        float temp = ((float)tempArray[i] - (float)min) / ((float)max - (float)min);
        tempout[i] = (int)(temp * 256);
    }
    printf("Phase Angle Reconstruction Finished!\n");
    return (outimage);
}

Image *Reconstruct_magnitude(Image *image, float *real_array, float *imaginary_array) {
    unsigned char *tempin, *tempout;
    float *tempArray;
    float real, imaginary, size = image->Width * image->Height;
    int max = 0, min = 10000; // used to normalization
    
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    tempArray = (float*)malloc(size * sizeof(float));

    printf("Magnitude Reconstruction is executing...\n");
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            real = 0;
            imaginary = 0;
            
            for (int m = 0; m < image->Height; m++){
                for (int n = 0; n < image->Width; n++){
                    float temp = (float)i * m / (float)image->Height + (float)j * n / (float)image->Width;
                    float mag = sqrt(pow(real_array[image->Width * m + n], 2) + pow(imaginary_array[image->Width * m + n], 2)) / sqrt(size);
                    real += mag * cos(2 * pi * temp) * pow(-1, (float)m + (float)n);
                    imaginary += mag * sin(2 * pi * temp) * pow(-1, (float)m + (float)n);
                }
            }
            int temp = (int)sqrt(pow(real, 2) + pow(imaginary, 2));
            tempArray[image->Width * i + j] = temp;
            if(temp > max) max = temp;
            if(temp < min) min = temp;
        }
    }
    // Normalization:
    for(int i = 0; i < size; i++) {
        float temp = ((float)tempArray[i] - (float)min) / ((float)max - (float)min);
        tempout[i] = (int)(temp * 256);
    }
    printf("Magnitude Reconstruction Finished!\n");
    return (outimage);
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

