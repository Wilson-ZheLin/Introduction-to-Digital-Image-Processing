#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

void DFT(Image *, float *, float *);
Image *iDFT(Image *, float *, float *);
void Homomorphic(Image *, float *, float *);
void Sinusoidal(Image *);
void BandrejectFilter(Image *, float *, float *);
void noiseCancel1(Image *, float *, float *);
void ArithmeticMeanFilter(Image *);
void GeometricMeanFilter(Image *);
void AlphaTrimmedMean(Image *);
void AdaptiveMedian(Image *);
void MedianFilter(Image *);
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
    Image *image, *sin_image;
    float real_array[236196], imaginary_array[236196];
    
    image = ReadPNMImage(file_in);
    //Topic 1:
    //DFT(image, real_array, imaginary_array);
    //Homomorphic(image, real_array, imaginary_array);
    
    //Topic 2:
    //Sinusoidal(image);
    //DFT(sin_image, real_array, imaginary_array);
    //BandrejectFilter(image, real_array, imaginary_array);
    
    //Topic 3:
    //DFT(image, real_array, imaginary_array);
    //noiseCancel1(image, real_array, imaginary_array);
    //AdaptiveMedian(image);

    //Topic 4:
    /*
    ArithmeticMeanFilter(image);
    GeometricMeanFilter(image);
    AlphaTrimmedMean(image);
    AdaptiveMedian(image);
    MedianFilter(image);
    */
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

            int temp = (int)(sqrt(real*real + imaginary*imaginary) / sqrt((float)image->Height*(float)image->Width));
            if(temp < 0) temp = 0;
            if(temp > 255) temp = 255;
            tempout[image->Width * i + j] = temp;
        }
    }
    printf("DFT Finished!\n");
    SavePNMImage(outimage, (char*)"DFT.pgm");
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

void Homomorphic(Image *image, float *real_array, float *imaginary_array) {
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
            float H = (1.5 - 0.75) * (1 - pow(M_E, pow(dis / 30, 2) * -1)) + 0.75;
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("Homomorphic Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"Homomorphic.pgm");
}

void Sinusoidal(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            float noise = 20 * (sin(i * 40) + sin(j * 40));
            tempout[image->Width * i + j] = tempin[image->Height * i + j] + noise;
        }
    }
    SavePNMImage(outimage, (char*)"SinNoise.pgm");
}

void BandrejectFilter(Image *image, float *real_array, float *imaginary_array) {
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
            float H;
            if(dis > 75 && dis < 110) H = 0;
            else H = 1;
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    printf("Bandreject Filter Finished!\n");
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"Bandreject Filter.pgm");
}

void noiseCancel1(Image *image, float *real_array, float *imaginary_array) {
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
            float H;
            if(j > 240 && j < 246 && dis >= 45) H = 0;
            else H = 1;
            real[(int)(i*height + j)] = real_array[(int)(i*height + j)] * H;
            imaginary[(int)(i*height + j)] = imaginary_array[(int)(i*height + j)] * H;
        }
    }
    outimage = iDFT(image, real, imaginary);
    SavePNMImage(outimage, (char*)"LenaWithoutNoise.pgm");
}

void ArithmeticMeanFilter(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    Sudoku[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            int sum = 0;
            for(int k = 0; k < 9; k++) {
                sum += Sudoku[k];
            }
            sum /= 9;
            if(sum > 255) sum = 255;
            if(sum < 0) sum = 0;
            tempout[image->Width * i + j] = sum;
        }
    }
    SavePNMImage(outimage, (char*)"ArithmeticMeanFilter.pgm");
}

void GeometricMeanFilter(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    Sudoku[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            float product = 1.0;
            for(int k = 0; k < 9; k++) {
                product *= Sudoku[k];
            }
            product = pow(product, 1.0/9.0);
            int temp = product;
            if(temp > 255) temp = 255;
            if(temp < 0) temp = 0;
            tempout[image->Width * i + j] = temp;
        }
    }
    SavePNMImage(outimage, (char*)"GeometricMeanFilter.pgm");
}

void MedianFilter(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    Sudoku[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            // Use Insertion Sort:
            for(int m = 1; m < 9; m++) {
                int currNum = Sudoku[m];
                int n = m;
                while(n >= 1 && Sudoku[n-1] > currNum) {
                    Sudoku[n] = Sudoku[n-1];
                    n--;
                }
                Sudoku[n] = currNum;
            }
            tempout[(image->Width)*i + j] = Sudoku[4];
        }
    }
    SavePNMImage(outimage, (char*)"Median Filter.pgm");
}

void AlphaTrimmedMean(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Average Filter");
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    Sudoku[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            // Use Insertion Sort:
            for(int m = 1; m < 9; m++) {
                int currNum = Sudoku[m];
                int n = m;
                while(n >= 1 && Sudoku[n-1] > currNum) {
                    Sudoku[n] = Sudoku[n-1];
                    n--;
                }
                Sudoku[n] = currNum;
            }
            // set the d/2 to 2:
            int sum = 0, d = 2;
            for(int k = d; k < 9-d; k++) {
                sum += Sudoku[k];
            }
            sum /= 9 - 2 * d;
            if(sum > 255) sum = 255;
            if(sum < 0) sum = 0;
            tempout[image->Width * i + j] = sum;
        }
    }
    SavePNMImage(outimage, (char*)"AlphaTrimmedMean.pgm");
}

void AdaptiveMedian(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9], enlarge[25];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    Sudoku[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            // Use Insertion Sort:
            for(int m = 1; m < 9; m++) {
                int currNum = Sudoku[m];
                int n = m;
                while(n >= 1 && Sudoku[n-1] > currNum) {
                    Sudoku[n] = Sudoku[n-1];
                    n--;
                }
                Sudoku[n] = currNum;
            }
            // Case 1: the median one is not a noise:
            if(Sudoku[0] < Sudoku[4] && Sudoku[4] < Sudoku[8]) {
                // check whether the current central pixel is a noise:
                int temp = tempin[image->Height * i + j];
                if(Sudoku[0] < temp && temp < Sudoku[8]) tempout[image->Width * i + j] = temp;
                else tempout[image->Width * i + j] = Sudoku[4];
            }
            // Case 2: the median one is a noise, so expand the mask:
            else {
                int num = 0;
                for(int x = -2; x <= 2; x++) {
                    for(int y = -2; y <= 2; y++) {
                        enlarge[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                    }
                }
                // Use Insertion Sort:
                for(int m = 1; m < 25; m++) {
                    int currNum = enlarge[m];
                    int n = m;
                    while(n >= 1 && enlarge[n-1] > currNum) {
                        enlarge[n] = enlarge[n-1];
                        n--;
                    }
                    enlarge[n] = currNum;
                }
                // check whether the current central pixel is a noise:
                int temp = tempin[image->Height * i + j];
                if(enlarge[0] < temp && temp < enlarge[8]) tempout[image->Width * i + j] = temp;
                else tempout[image->Width * i + j] = enlarge[12];
            }
        }
    }
    SavePNMImage(outimage, (char*)"AdaptiveMedian.pgm");
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

