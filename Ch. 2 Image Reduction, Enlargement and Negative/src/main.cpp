#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"

void SavePNMImage(Image *, char *);
int TestReadImage(char *, char *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment, int flag, float ratio);
Image *ImageReduce_AlternativeLine(Image *, float ratio);
Image *ImageEnlarge_PixelRep(Image *, float ratio);
Image *ImageEnlarge_Nearest(Image *, float ratio);
Image *ImageEnlarge_Bilinear(Image *, float ratio);
Image *Image_FractionalLinear(Image *, float ratio);
Image *Negative(Image *);

int main(int argc, char **argv)
{
    // Please adjust the input filename and path to suit your needs:
    char* file_in = (char*)"bridge.pgm";
    char* file_out = (char*)"";
    TestReadImage(file_in, file_out);
    return(0);
}

int TestReadImage(char *file_in, char *file_out)
{
    Image *image;
    Image *outimage1, *outimage2, *outimage3, *outimage4, *outimage5, *outimage6;
    float ratio;// image scaling ratio, >1 is enlargering, <1 is reducing
    printf("Please input the proportionality coefficient (For example: 0.8 is to x0.8)\n");
    printf("Value: ");
    scanf("%f", &ratio);
    if(ratio <= 0 || ratio > 25) {
        printf("Invalid Input!\n");
        exit(0);
    }
    image = ReadPNMImage(file_in);
    outimage1 = ImageReduce_AlternativeLine(image, ratio);
    outimage2 = ImageEnlarge_PixelRep(image, ratio);
    outimage3 = ImageEnlarge_Nearest(image, ratio);
    outimage4 = ImageEnlarge_Bilinear(image, ratio);
    outimage5 = Image_FractionalLinear(image, ratio);
    outimage6 = Negative(image);
    
    // Please adjust the output filenames and paths to suit your needs:
    SavePNMImage(outimage1, (char*)"bridge_reduce.pgm");
    SavePNMImage(outimage2, (char*)"bridge_large1.pgm");
    SavePNMImage(outimage3, (char*)"bridge_large2.pgm");
    SavePNMImage(outimage4, (char*)"bridge_large3.pgm");
    SavePNMImage(outimage5, (char*)"bridge_FractionalLinear.pgm");
    SavePNMImage(outimage6, (char*)"bridge_negative.pgm");
    return(0);
}

// Algorithms Code:
Image *Image_FractionalLinear(Image *image, float ratio) {
    // Fractional linear method.
    // This algorithm can resize the image to any size(Both enlarge and reduce).
    unsigned char *tempin, *tempout;
    Image *outimage;
    int x_in, y_in;// relative position coordinates of the input image
    
    outimage = CreateNewImage(image, (char*)"#testing Enlargement", 3, ratio);
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++) {
            x_in = round((float)j / ratio);
            y_in = round((float)i / ratio);
            tempout[outimage->Width * i + j] = tempin[image->Width * y_in + x_in];
        }
    }
    return (outimage);
}

Image *ImageReduce_AlternativeLine(Image *image, float ratio) {
    // Image Reduction: Alternative line method.
    // This algorithm is to fixedly reduce the image to 1/2 size.
    unsigned char *tempin, *tempout;
    Image *outimage;
    int row = 0, column = 0;
    
    outimage = CreateNewImage(image, (char*)"#testing Reduction", 1, 0.5);
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 0; i < image->Height/2 - 1; i++) {
        for(int j = 0; j < image->Width/2 - 1; j++) {
            tempout[(outimage->Width)*i + j] = tempin[(image->Width)*row + column];
            column += 2;
        }
        row += 2;
        column = 0;
    }
    return (outimage);
}

Image *ImageEnlarge_PixelRep(Image *image, float ratio) {
    // Image Enlargement: Pixel replication method.
    // This algorithm is to fixedly enlarge the image to twice the size.
    unsigned char *tempin, *tempout;
    Image *outimage;
    int row = 0, column = 0;
    
    outimage = CreateNewImage(image, (char*)"#testing Enlargement", 2, 3);
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 0; i < image->Height - 1; i++) {
        for(int j = 0; j < image->Width - 1; j++) {
            unsigned char currPixel = tempin[(image->Width)*i + j];
            for(int x = row; x <= row+2; x++) {
                for(int y = column; y <= column+2; y++) {
                    tempout[(outimage->Width)*x + y] = currPixel;
                }
            }
            column += 3;
        }
        row += 3;
        column = 0;
    }
    return (outimage);
}

Image *ImageEnlarge_Nearest(Image *image, float ratio) {
    // Image Enlargement: Nearest enlargement method.
    // This algorithm can enlarge the image to any size.
    unsigned char *tempin, *tempout;
    Image *outimage;
    float ratio_row, ratio_column;
    
    // If the input ratio < 1, then automatically enlarge to 3 times:
    if(ratio <= 1) outimage = CreateNewImage(image, (char*)"#testing Enlargement", 2, 3);
    else outimage = CreateNewImage(image, (char*)"#testing Enlargement", 3, ratio);
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 0; i < outimage->Height - 1; i++) {
        ratio_column = (float)(i) / (float)outimage->Height;
        for(int j = 0; j < outimage->Width - 1; j++) {
            ratio_row = (float)(j) / (float)outimage->Width;
            int temp = image->Width * round(image->Height * ratio_column) + round(image->Width * ratio_row);
            tempout[outimage->Width * i + j] = tempin[temp];
        }
    }
    return (outimage);
}

Image *ImageEnlarge_Bilinear(Image *image, float ratio) {
    // Image Enlargement: Bilinear interpolation method.
    // This algorithm can enlarge the image to any size.
    unsigned char *tempin, *tempout;
    Image *outimage;
    int x1, x2, y1, y2;// coordinate of known pixels
    unsigned char color1, color2, color3, color4;// color of known pixels
    float x_in, y_in;

    // If the input ratio < 1, then automatically enlarge to 3 times:
    if(ratio <= 1) outimage = CreateNewImage(image, (char*)"#testing Enlargement", 2, 3);
    else outimage = CreateNewImage(image, (char*)"#testing Enlargement", 3, ratio);
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++) {
            if(ratio <= 1) {
                // In case the input ratio <1.
                x_in = (float)j / 3;
                y_in = (float)i / 3;
            }
            else {
                x_in = (float)j / ratio;
                y_in = (float)i / ratio;
            }
            x1 = (int)(x_in + 1);
            x2 = (int)(x_in - 1);
            y1 = (int)(y_in + 1);
            y2 = (int)(y_in - 1);
            color1 = tempin[image->Width * y1 + x1];
            color2 = tempin[image->Width * y2 + x1];
            color3 = tempin[image->Width * y1 + x2];
            color4 = tempin[image->Width * y2 + x2];
            tempout[outimage->Width * i + j] = (unsigned char)(((color1*(x2-x_in)*(y2-y_in)) + (color2*(x2-x_in)*(y_in-y1)) + (color3*(x_in-x1)*(y2-y_in)) + (color4*(x_in-x1)*(y_in-y1))) / ((x2-x1) * (y2-y1)));
        }
    }
    return (outimage);
}

Image *Negative(Image *image) {
    // Perform negative image operation.
    unsigned char *tempin, *tempout;
    Image *outimage;
    int size;
    
    outimage = CreateNewImage(image, (char*)"#testing Nagetive", 0, 1);
    tempin = image->data;
    tempout = outimage->data;

    if(image->Type == GRAY) size = image->Width * image->Height;
    else if(image->Type == COLOR) size = image->Width * image->Height * 3;
    
    for(int i = 0; i < size; i++) {
        *tempout = 255 - *tempin;
        tempin++;
        tempout++;
    }
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

Image *CreateNewImage(Image * image, char *comment, int flag, float ratio)
{
    // "flag" is to determine the size of output image.
    Image *outimage;
    int size,j;
    
    outimage=(Image *)malloc(sizeof(Image));
    
    outimage->Type = image->Type;
    outimage->num_comment_lines = image->num_comment_lines;

    //
    if(flag == 0) {
        // Case 0 : keep the original size.
        if(outimage->Type == GRAY)   size = image->Width * image->Height;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3;
        outimage->Width = image->Width;
        outimage->Height = image->Height;
    }
    else if(flag == 1) {
        // Case 1 : reduce to 1/2 size fixedly.
        if(outimage->Type == GRAY)   size = image->Width * image->Height / 4;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3 / 4;
        outimage->Width = image->Width / 2;
        outimage->Height = image->Height / 2;
    }
    else if(flag == 2) {
        // Case 2 : enlarge to double the size fixedly.
        if(outimage->Type == GRAY)   size = image->Width * image->Height * 9;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3 * 9;
        outimage->Width = image->Width * 3;
        outimage->Height = image->Height * 3;
    }
    else {
        // Case 3 : change to any sizes depending on the input value.
        if(outimage->Type == GRAY)   size = image->Width * image->Height * ratio * ratio;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3 * ratio * ratio;
        outimage->Width = image->Width * ratio;
        outimage->Height = image->Height * ratio;
    }
    
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

