#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"

Image *Translation(Image *);
Image *Rotation(Image *);
Image *Shear_vertical(Image *);
Image *Shear_horizontal(Image *);
Image *AverageImage_3x3(Image *);
Image *MedianImage_3x3(Image *);
Image *AverageImage_5x5(Image *);
Image *MedianImage_5x5(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment, int flag);
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
    Image *translation, *rotation, *shear_vertical, *shear_horizontal;
    Image *outimage_average1, *outimage_average2, *outimage_median1, *outimage_median2;

    image = ReadPNMImage(file_in);
    translation = Translation(image);
    rotation = Rotation(image);
    shear_vertical = Shear_vertical(image);
    shear_horizontal = Shear_horizontal(image);
    outimage_average1 = AverageImage_3x3(image);
    outimage_average2 = AverageImage_5x5(image);
    outimage_median1 = MedianImage_3x3(image);
    outimage_median2 = MedianImage_5x5(image);

    // Please adjust the output filenames and paths to suit your needs:
    SavePNMImage(translation, (char*)"translation.pgm");
    SavePNMImage(rotation, (char*)"rotation.pgm");
    SavePNMImage(shear_vertical, (char*)"shear_vertical.pgm");
    SavePNMImage(shear_horizontal, (char*)"shear_horizontal.pgm");
    SavePNMImage(outimage_average1, (char*)"lena_avg1.pgm");
    SavePNMImage(outimage_average2, (char*)"lena_avg2.pgm");
    SavePNMImage(outimage_median1, (char*)"lena_med1.pgm");
    SavePNMImage(outimage_median2, (char*)"lena_med2.pgm");
    return(0);
}

// Algorithms Code:
Image *Translation(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 0);
    tempin = image->data;
    tempout = outimage->data;
    // set the background of the whole image to black(0):
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++){
            tempout[(outimage->Width)*i + j] = 0;
        }
    }
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            // in case the coordinates + offsets beyond the boundary:
            if((j+72) >= outimage->Width || (i+72) >= outimage->Height) continue;
            else tempout[(outimage->Width)*(i+72) + (j+72)] = tempin[(image->Width)*i + j];
        }
    }
    return (outimage);
}

Image *Rotation(Image *image) {
    unsigned char *tempin, *tempout, mask[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 1);
    tempin = image->data;
    tempout = outimage->data;
    // set the background of the whole image to black(0):
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++){
            tempout[(outimage->Width)*i + j] = 0;
        }
    }
    // cos(-45°) = √2/2, about 0.707.
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int x = round(i*0.707+j*0.707);
            int y = round(i*0.707-j*0.707);
            // move the rotated image to the center:
            tempout[(outimage->Width)*(y+192) + x] = tempin[(image->Width)*(i) + (j)];
        }
    }
    // Then use 3x3 Median Filter to fill the missing pixels:
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    mask[num++] = tempout[(outimage->Width)*(i+x) + (j+y)];
                }
            }
            // Insertion Sort:
            for(int m = 1; m < 9; m++) {
                int currNum = mask[m];
                int n = m;
                while(n >= 1 && mask[n-1] > currNum) {
                    mask[n] = mask[n-1];
                    n--;
                }
                mask[n] = currNum;
            }
            tempout[(outimage->Width)*i + j] = mask[4];
        }
    }
    return (outimage);
}

Image *Shear_vertical(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 1);
    tempin = image->data;
    tempout = outimage->data;
    // set the background of the whole image to black(0):
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++){
            tempout[(outimage->Width)*i + j] = 0;
        }
    }
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int y = round(i + (float)j*0.5);
            // in case the vertical coordinate + offset beyond the boundarys:
            if(y >= outimage->Height) continue;
            tempout[(outimage->Width)*y + j] = tempin[(image->Width)*i + j];
        }
    }
    return (outimage);
}

Image *Shear_horizontal(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 1);
    tempin = image->data;
    tempout = outimage->data;
    // set the background of the whole image to black(0):
    for(int i = 0; i < outimage->Height; i++) {
        for(int j = 0; j < outimage->Width; j++){
            tempout[(outimage->Width)*i + j] = 0;
        }
    }
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int x = round(j + (float)i*0.5);
            // in case the horizontal coordinate + offset beyond the current row:
            if(x >= outimage->Width) continue;
            else tempout[(outimage->Width)*i + x] = tempin[(image->Width)*i + j];
        }
    }
    return (outimage);
}

Image *AverageImage_3x3(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 0);
    tempin = image->data;
    tempout = outimage->data;

    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            // Boundary check:
            if(i == 0 || j == 0 || i == image->Height-1 || j == image->Width-1) {
                tempout[(image->Width)*i + j] = tempin[(image->Width)*i + j];
                continue;
            }
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

Image *AverageImage_5x5(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 0);
    tempin = image->data;
    tempout = outimage->data;
    // ignore the edge of the image:
    for(int i = 1; i < image->Height-1; i++) {
        for(int j = 1; j < image->Width-1; j++){
            int sum = 0;
            for(int x = -2; x <= 2; x++) {
                for(int y = -2; y <= 2; y++) {
                    sum += tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            tempout[(image->Width)*i + j] = sum/25;
        }
    }
    return (outimage);
}

Image *MedianImage_3x3(Image *image) {
    unsigned char *tempin, *tempout, mask[9];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 0);
    tempin = image->data;
    tempout = outimage->data;
        
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int num = 0;
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    mask[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            // Use Insertion Sort:
            for(int m = 1; m < 9; m++) {
                int currNum = mask[m];
                int n = m;
                while(n >= 1 && mask[n-1] > currNum) {
                    mask[n] = mask[n-1];
                    n--;
                }
                mask[n] = currNum;
            }
            tempout[(image->Width)*i + j] = mask[4];
        }
    }
    return (outimage);
}

Image *MedianImage_5x5(Image *image) {
    unsigned char *tempin, *tempout, mask[25];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function", 0);
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int num = 0;
            for(int x = -2; x <= 2; x++) {
                for(int y = -2; y <= 2; y++) {
                    mask[num++] = tempin[(image->Width)*(i+x) + (j+y)];
                }
            }
            // Use Insertion Sort:
            for(int m = 1; m < 25; m++) {
                int currNum = mask[m];
                int n = m;
                while(n >= 1 && mask[n-1] > currNum) {
                    mask[n] = mask[n-1];
                    n--;
                }
                mask[n] = currNum;
            }
            tempout[(image->Width)*i + j] = mask[12];
        }
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

Image *CreateNewImage(Image * image, char *comment, int flag)
{
    Image *outimage;
    int size,j;
    
    outimage=(Image *)malloc(sizeof(Image));
    
    outimage->Type = image->Type;
    outimage->num_comment_lines = image->num_comment_lines;

    // "flag" is to determine the size of output image:
    if(flag == 0) {
        // Case 0 : keep the original size.
        if(outimage->Type == GRAY)   size = image->Width * image->Height;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3;
        outimage->Width = image->Width;
        outimage->Height = image->Height;
    }
    if(flag == 1) {
        // Case 1 : enlarge to 1.5 times.
        if(outimage->Type == GRAY)   size = image->Width * image->Height * 2.25;
        if(outimage->Type == COLOR) size  = image->Width * image->Height * 3 * 2.25;
        outimage->Width = image->Width * 1.5;
        outimage->Height = image->Height * 1.5;
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

