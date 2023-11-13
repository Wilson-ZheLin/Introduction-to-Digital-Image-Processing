#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"

Image *Laplacian(Image *);
Image *Sobel(Image *);
Image *Gamma(Image *, float ratio);
Image *Global_histogram(Image *);
Image *Local_histogram(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int boundaryCheck(int index_x, int index_y, int width, int height);
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
    Image *laplacian, *sobel, *histogram_global, *histogram_local;
    Image *gama_01, *gama_04, *gama_07, *gama_1;
    
    image = ReadPNMImage(file_in);
    laplacian = Laplacian(image);
    sobel = Sobel(image);
    gama_01 = Gamma(image, 0.1);
    gama_04 = Gamma(image, 0.4);
    gama_07 = Gamma(image, 0.7);
    gama_1 = Gamma(image, 1);
    histogram_global = Global_histogram(image);
    histogram_local = Local_histogram(image);

    // Please adjust the output filenames and paths to suit your needs:
    SavePNMImage(laplacian, (char*)"laplacian.pgm");
    SavePNMImage(sobel, (char*)"sobel.pgm");
    SavePNMImage(gama_01, (char*)"gama_01.pgm");
    SavePNMImage(gama_04, (char*)"gama_04.pgm");
    SavePNMImage(gama_07, (char*)"gama_07.pgm");
    SavePNMImage(gama_1, (char*)"gama_1.pgm");
    SavePNMImage(histogram_global, (char*)"histogram_global.pgm");
    SavePNMImage(histogram_local, (char*)"histogram_local.pgm");
    return(0);
}

// Algorithms Code:
Image *Laplacian(Image *image) {
    unsigned char *tempin, *tempout;
    int sum = 0;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            sum = 0;
            for(int m = -1; m <= 1; m += 2) {
                for(int n = -1; n <= 1; n += 2) {
                    // use boundary check:
                    sum += boundaryCheck(j + n, i + m, image->Width, image->Height) ? tempin[image->Width * (i + m) + (j + n)] : 0;
                }
            }
            int temp = tempin[image->Width * i + j] * 4 - sum;
            // handle excess values:
            if(temp > 255) temp = 255;
            if(temp < 0) temp = 0;
            tempout[image->Width * i + j] = temp;
        }
    }
    return (outimage);
}

Image *Sobel(Image *image) {
    unsigned char *tempin, *tempout;
    int index, square[9], temp1, temp2;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            index = 0;
            // record the values in the 3x3 square:
            for(int m = -1; m <= 1; m++) {
                for(int n = -1; n <= 1; n++) {
                    // use boundary check:
                    square[index++] = boundaryCheck(j + n, i + m, image->Width, image->Height) ? tempin[image->Width * (i + m) + (j + n)] : 0;
                }
            }
            temp1 = abs(square[2] + 2*square[5] + square[8] - square[0] - 2*square[3] - square[6]);
            temp2 = abs(square[6] + 2*square[7] + square[8] - square[0] - 2*square[1] - square[2]);
            tempout[image->Width * i + j] = sqrt(pow(temp1, 2) + pow(temp2, 2));
        }
    }
    return (outimage);
}

Image *Gamma(Image *image, float ratio) {
    unsigned char *tempin, *tempout;
    float temp;
    float variance, average, sum = 0, N = image->Width * image->Height; // calculate variance
    
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            temp = ((float)tempin[image->Width * i + j] + 0.5) / 256; // normalized
            temp = pow(temp, ratio); // power the parameter
            temp = (int)(temp * 256 - 0.5); // denormalization
            tempout[outimage->Width * i + j] = (unsigned char)temp;
            sum += temp;
        }
    }
    
    // calculate & output the variance:
    average = sum / N;
    sum = 0;
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            sum += pow(tempout[outimage->Width * i + j] - average, 2);
        }
    }
    variance = sum / N;
    printf("The variance of gamma value %.1f is: %.2f\n", ratio, variance);
    return (outimage);
}

Image *Global_histogram(Image *image) {
    unsigned char *tempin, *tempout, temp;
    int histogram_sum[256], histogram[256], currSum = 0; // used for statistics
    float constant = (float)255 / (float)(image->Width * image->Height);// (L-1)/(M*N)
    
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    // initialize the array:
    for(int i = 0; i < 256; i++) histogram[i] = 0;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            temp = tempin[image->Width * i + j];
            histogram[temp] += 1;
        }
    }
    for(int i = 0; i < 256; i++) {
        currSum += histogram[i];
        histogram_sum[i] = currSum;
    }
    
    // output the image:
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            temp = tempin[image->Width * i + j];
            tempout[outimage->Width * i + j] = (int)(histogram_sum[temp] * constant);
        }
    }
    return (outimage);
}

Image *Local_histogram(Image *image) {
    unsigned char *tempin, *tempout, temp;
    int histogram_sum[256], histogram[256]; // used for statistics
    float constant = (float)255 / (float)9; // M*N changed to 9
    
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    // process all the pixels:
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            // initialize the array:
            for(int k = 0; k < 256; k++) histogram[k] = 0;

            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    // use boundary check:
                    temp = boundaryCheck(j + y, i + x, image->Width, image->Height) ? tempin[image->Width * (i + x) + (j + y)] : 0;
                    histogram[temp] += 1;
                }
            }
            for(int k = 0, currSum = 0; k < 256; k++) {
                currSum += histogram[k];
                histogram_sum[k] = currSum;
            }
            
            // output the image:
            temp = tempin[image->Width * i + j];
            tempout[outimage->Width * i + j] = (int)(histogram_sum[temp] * constant);
        }
    }
    return (outimage);
}

int boundaryCheck(int index_x, int index_y, int width, int height) {
    if(index_x >= 0 && index_y >= 0 && index_x < width && index_y < height) return 1;
    else return 0;
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

