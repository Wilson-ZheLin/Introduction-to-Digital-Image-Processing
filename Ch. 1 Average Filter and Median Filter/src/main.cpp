#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include "proto.h"

void SavePNMImage(Image *, char *);
Image *SwapImage(Image *);
Image *AverageImage(Image *);
Image *MedianImage(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *);

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
    Image *outimage_average;
    Image *outimage_median;

    image=ReadPNMImage(file_in);
    outimage_average = AverageImage(image);
    outimage_median = MedianImage(image);
    // Please adjust the output filenames and paths to suit your needs:
    SavePNMImage(outimage_average, (char*)"lena_avg.pgm");
    SavePNMImage(outimage_median, (char*)"lena_med.pgm");
    return(0);
}

// Algorithms Code:
Image *AverageImage(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    int size;
    
    outimage = CreateNewImage(image, (char*)"#testing Average Filter");
    tempin = image->data;
    tempout = outimage->data;
    
    if(image->Type == GRAY) size = image->Width * image->Height;
    else if(image->Type == COLOR) size = image->Width * image->Height * 3;
    
    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int sum = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    sum += tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            tempout[(image->Width)*i + j] = sum/9;
        }
    }
    return (outimage);
}

Image *MedianImage(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[9];
    Image *outimage;
    int size;
    
    outimage = CreateNewImage(image, (char*)"#testing Average Filter");
    tempin = image->data;
    tempout = outimage->data;
    
    if(image->Type == GRAY) size = image->Width * image->Height;
    else if(image->Type == COLOR) size = image->Width * image->Height * 3;
    
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
    return (outimage);
}
// Algorithms End.

Image *SwapImage(Image *image)
{
    unsigned char *tempin, *tempout;
    int i, size;
    Image *outimage;
    
    outimage=CreateNewImage(image, (char*)"#testing Swap");
    tempin=image->data;
    tempout=outimage->data;
    
    if(image->Type == GRAY)   size  = image->Width * image->Height;
    else if(image->Type == COLOR) size  = image->Width * image->Height * 3;
    
    for(i=0;i<size;i++)
    {
        *tempout=*tempin;
        tempin++;
        tempout++;
    }
    return(outimage);
}

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
    
    outimage->Type  =image->Type;
    if(outimage->Type == GRAY)   size  = image->Width * image->Height;
    else if(outimage->Type == COLOR) size  = image->Width * image->Height * 3;
    
    outimage->Width =image->Width;
    outimage->Height=image->Height;
    outimage->num_comment_lines=image->num_comment_lines;
    
    /*--------------------------------------------------------*/
    /* Copy Comments for Original Image      */
    for(j=0;j<outimage->num_comment_lines;j++)
        strcpy(outimage->comments[j],image->comments[j]);
    
    /*----------- Add New Comment  ---------------------------*/
    strcpy(outimage->comments[outimage->num_comment_lines],comment);
    outimage->num_comment_lines++;
    
    
    outimage->data   = (unsigned char *) malloc(size);
    if (!outimage->data){
        printf("cannot allocate memory for new image");
        exit(0);
    }
    return(outimage);
}

