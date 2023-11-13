#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

Image *Erosion(Image *);
Image *Dilation(Image *);
void Opening(Image *);
void Closing(Image *);
void ExtractBoundaries(Image *);
void connectedComponent(Image *);
void SeparateBubbles(Image *);
int DFS(Image *, int *, int x, int y);
void DFS_MarkPixels(Image *, int *, int x, int y, int label);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *, int flag);
void SavePNMImage(Image *, char *);

int main(int argc, char **argv)
{
    // Please adjust the input filename and path to suit your needs:
    char* file_in1_1 = (char*)"noisy_fingerprint.pgm";
    char* file_in1_2 = (char*)"noisy_rectangle.pgm";
    char* file_in2_1 = (char*)"licoln.pgm";
    char* file_in2_2 = (char*)"U.pgm";
    char* file_in3 = (char*)"connected.pgm";
    char* file_in4 = (char*)"bubbles_on_black_background.pgm";
    TestReadImage(file_in1_1, (char*)"", 1);
    //TestReadImage(file_in1_2, (char*)"", 1);
    TestReadImage(file_in2_1, (char*)"", 2);
    //TestReadImage(file_in2_2, (char*)"", 2);
    TestReadImage(file_in3, (char*)"", 3);
    TestReadImage(file_in4, (char*)"", 4);
    return(0);
}

int TestReadImage(char *file_in, char *file_out, int flag)
{
    Image *image, *image_ero, *image_dil;
    image = ReadPNMImage(file_in);
    //Topic 1:
    if(flag == 1) {
        image_ero = Erosion(image);
        image_dil = Dilation(image);
        SavePNMImage(image_ero, (char*)"Erosion.pgm");
        SavePNMImage(image_dil, (char*)"Dilation.pgm");
        Opening(image);
        Closing(image);
    }
    //Topic 2:
    if(flag == 2) ExtractBoundaries(image);
    //Topic 3:
    if(flag == 3) connectedComponent(image);
    //Topic 4:
    if(flag == 4) SeparateBubbles(image);
    return 0;
}

// Algorithms Code:
Image *Erosion(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int min = 255;
            // the size of structual element can be changed values of x and y:
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    int temp = tempin[(image->Width)*(i+x) + (j+y)];
                    if(temp < min) min = temp;
                }
            }
            tempout[image->Width * i + j] = min;
        }
    }
    return(outimage);
}

Image *Dilation(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int max = 0;
            // the size of structual element can be changed values of x and y:
            for(int x = -1; x <= 1; x++) {
                for(int y = -1; y <= 1; y++) {
                    int temp = tempin[(image->Width)*(i+x) + (j+y)];
                    if(temp > max) max = temp;
                }
            }
            tempout[image->Width * i + j] = max;
        }
    }
    return(outimage);
}

void Opening(Image *image) {
    unsigned char *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempout = outimage->data;
    
    outimage = Dilation(Erosion(image));
    SavePNMImage(outimage, (char*)"Opening.pgm");
}

void Closing(Image *image) {
    unsigned char *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempout = outimage->data;
    
    outimage = Erosion(Dilation(image));
    SavePNMImage(outimage, (char*)"Closing.pgm");
}

void ExtractBoundaries(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    int size = image->Width * image->Height;
    
    outimage = Erosion(image);
    tempin = image->data;
    tempout = outimage->data;
    for(int i = 0; i < size; i++) {
        tempout[i] = tempin[i] - tempout[i];
    }
    SavePNMImage(outimage, (char*)"Boundaries.pgm");
}

void connectedComponent(Image *image) {
    int size = image->Width * image->Height;
    int checkBoard[size];// mark the traversed pixels
    FILE *fp;
    fp = fopen("Connected Components.txt", "w");
    int index = 1;
    
    for(int i = 0; i < size; i++) {
        checkBoard[i] = 0;// initialize the checkboard
    }
    
    fprintf(fp, "  No.    Count\n");
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int count = DFS(image, checkBoard, j, i);// use Deep First Search here
            if(count != 0) fprintf(fp, "%3d: %8d\n", index++, count);
        }
    }
    fclose(fp);
}

int DFS(Image * image, int *checkBoard, int x, int y) {
    unsigned char *tempin;
    tempin = image->data;
    int currPosition = image->Width * y + x;
    // Base Case:
    if(x < 0 || y < 0 || x >= image->Width || y >= image->Height || checkBoard[currPosition] == 1 || tempin[currPosition] == 0) {
        return 0;
    }
    // Recursive Steps:
    checkBoard[currPosition] = 1;
    int tempSum = 0;
    // use 8-adjacent checking:
    for(int m = -1; m <= 1; m++) {
        for(int n = -1; n <= 1; n++) {
            tempSum += DFS(image, checkBoard, x+m, y+n);
        }
    }
    return (1 + tempSum);
}

void SeparateBubbles(Image *image) {
    unsigned char *tempout_boundary, *tempout_single, *tempout_overlap;
    Image *boundary_image, *single_image, *overlap_image;
    boundary_image = CreateNewImage(image, (char*)"#testing function");
    single_image = CreateNewImage(image, (char*)"#testing function");
    overlap_image = CreateNewImage(image, (char*)"#testing function");
    tempout_boundary = boundary_image->data;
    tempout_single = single_image->data;
    tempout_overlap = overlap_image->data;
    
    int size = image->Width * image->Height;
    int checkBoard[size], labelBoard[size];
    // initialize the checkboard and labelboard,
    // and set the background of the output images to black:
    for(int i = 0; i < size; i++) {
        tempout_boundary[i] = 0;
        tempout_single[i] = 0;
        tempout_overlap[i] = 0;
        labelBoard[i] = 0;
        checkBoard[i] = 0;
    }
    
    // 1: Extract all the particles that merged with boundaries:
    DFS_MarkPixels(image, labelBoard, 1, 1, 1);
    DFS_MarkPixels(image, labelBoard, 200, 781, 1);
    
    // 2 & 3: Extract all the single, and overlapping particles:
    for(int i = 7; i < image->Height-7-1; i++) {
        for(int j = 7; j < image->Width-7-3; j++) {
            int area = DFS(image, checkBoard, j, i);// record the size of a connected area
            if(area > 350 && area < 450) DFS_MarkPixels(image, labelBoard, j, i, 2);
            else DFS_MarkPixels(image, labelBoard, j, i, 3);
        }
    }
    // output the images:
    for(int i = 0; i < size; i++) {
        if(labelBoard[i] == 1) tempout_boundary[i] = 255;
        if(labelBoard[i] == 2) tempout_single[i] = 255;
        if(labelBoard[i] == 3) tempout_overlap[i] = 255;
    }
    SavePNMImage(boundary_image, (char*)"Boundary_image.pgm");
    SavePNMImage(single_image, (char*)"Single_image.pgm");
    SavePNMImage(overlap_image, (char*)"Overlap_image.pgm");
}

void DFS_MarkPixels(Image *image, int *labelBoard, int x, int y, int label) {
    unsigned char *tempin;
    tempin = image->data;
    int currPosition = image->Width * y + x;
    // Base Case:
    if(x < 0 || y < 0 || x >= image->Width || y >= image->Height || labelBoard[currPosition] != 0 || tempin[currPosition] == 0) {
        return;
    }
    // Recursive Steps:
    labelBoard[currPosition] = label;
     // use 8-adjacent marking:
    for(int m = -1; m <= 1; m++) {
        for(int n = -1; n <= 1; n++) {
            DFS_MarkPixels(image, labelBoard, x+m, y+n, label);;
        }
    }
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

