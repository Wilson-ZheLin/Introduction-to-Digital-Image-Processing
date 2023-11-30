#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

void Roberts(Image *);
void Prewitt(Image *);
void Sobel(Image *);
void Canny(Image *);
void LoG(Image *);
void GlobalThreshold(Image *);
int boundaryCheck(int index_x, int index_y, int width, int height);
Image *Threshold(Image *);
Image *Gaussian(Image *);
Image *Laplacian(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *, int flag);
void SavePNMImage(Image *, char *);

int main(int argc, char **argv)
{
    // Please adjust the input filename and path to suit your needs:
    char* file_in1 = (char*)"headCT-Vandy.pgm";
    char* file_in2 = (char*)"building_original.pgm";
    char* file_in3 = (char*)"noisy_fingerprint.pgm";
    char* file_in4 = (char*)"polymersomes.pgm";
    TestReadImage(file_in1, (char*)"", 1);
    //TestReadImage(file_in2, (char*)"", 1);
    //TestReadImage(file_in3, (char*)"", 1);
    TestReadImage(file_in1, (char*)"", 2);
    //TestReadImage(file_in3, (char*)"", 2);
    TestReadImage(file_in4, (char*)"", 3);
    //TestReadImage(file_in3, (char*)"", 3);
    return(0);
}

int TestReadImage(char *file_in, char *file_out, int flag)
{
    Image *image;
    image = ReadPNMImage(file_in);
    
    //Topic 1:
    if(flag == 1) {
        Roberts(image);
        Prewitt(image);
        Sobel(image);
    }
    //Topic 2:
    if(flag == 2) {
        Canny(image);
        LoG(image);
    }
    //Topic 3:
    if(flag == 3) GlobalThreshold(image);
    return 0;
}

// Algorithms Code:
void Roberts(Image *image) {
    unsigned char *tempin, *tempout;
    float temp1, temp2;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height-1; i++) {
        for(int j = 0; j < image->Width-1; j++) {
            temp1 = pow((float)tempin[image->Width * i + j] - (float)tempin[image->Width * (i+1) + j + 1], 2);
            temp2 = pow((float)tempin[image->Width * i + j + 1] - (float)tempin[image->Width * (i+1) + j], 2);
            tempout[image->Width * i + j] = (int)sqrt(temp1 + temp2);
        }
    }
    outimage = Threshold(outimage);
    SavePNMImage(outimage, (char*)"Roberts.pgm");
}

void Sobel(Image *image) {
    unsigned char *tempin, *tempout;
    int index, square[9];
    float temp1, temp2;
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
            temp1 = abs((float)square[2] + (float)square[5]*2 + (float)square[8] - (float)square[0] - (float)square[3]*2 - (float)square[6]);
            temp2 = abs((float)square[6] + (float)square[7]*2 + (float)square[8] - (float)square[0] - (float)square[1]*2 - (float)square[2]);
            tempout[image->Width * i + j] = (int)sqrt(pow(temp1, 2) + pow(temp2, 2));
        }
    }
    outimage = Threshold(outimage);
    SavePNMImage(outimage, (char*)"Sobel.pgm");
}

void Prewitt(Image *image) {
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
            temp1 = square[2] + square[5] + square[8] - square[0] - square[3] - square[6];
            temp2 = square[0] + square[1] + square[2] - square[6] - square[7] - square[8];
            tempout[image->Width * i + j] = abs(temp1) + abs(temp2);
        }
    }
    outimage = Threshold(outimage);
    SavePNMImage(outimage, (char*)"Prewitt.pgm");
}

Image *Threshold(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    int size = image->Width * image->Height, max = 0;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < size; i++) {
        if(tempin[i] > max) max = tempin[i];
    }
    int threshold = round((float)max * 0.33);
    
    for(int i = 0; i < size; i++) {
        if(tempin[i] >= threshold) tempout[i] = 255;
        else tempout[i] = 0;
    }
    return(outimage);
}

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
            
            for(int m = -1; m <= 1; m++) {
                for(int n = -1; n <= 1; n++) {
                    // use boundary check:
                    sum += boundaryCheck(j + n, i + m, image->Width, image->Height) ? tempin[image->Width * (i + m) + (j + n)] : 0;
                }
            }
            int temp = sum - 9 * tempin[image->Width * i + j];
            // handle excess values:
            if(temp > 255) temp = 255;
            if(temp < 0) temp = 0;
            tempout[image->Width * i + j] = temp;
        }
    }
    return (outimage);
}

void Canny(Image *image) {
    unsigned char *tempin, *tempout, *tempout1, *tempout2;
    Image *outimage, *outimage1, *outimage2;
    int index, square[9];
    float temp1, temp2, angel[image->Width * image->Height];
    
    outimage = CreateNewImage(image, (char*)"#testing function");
    outimage1 = CreateNewImage(image, (char*)"#testing function");
    outimage2 = CreateNewImage(image, (char*)"#testing function");
    tempout = outimage->data;
    tempout1 = outimage1->data;
    tempout2 = outimage2->data;
    
    // Step 1: Gaussian Blur:
    image = Gaussian(image);
    tempin = image->data;
    SavePNMImage(image, (char*)"Gaussian.pgm");

    // Step 2: Sobel
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
            temp1 = abs((float)square[2] + (float)square[5]*2 + (float)square[8] - (float)square[0] - (float)square[3]*2 - (float)square[6]);
            temp2 = abs((float)square[6] + (float)square[7]*2 + (float)square[8] - (float)square[0] - (float)square[1]*2 - (float)square[2]);
            tempout[image->Width * i + j] = (int)sqrt(pow(temp1, 2) + pow(temp2, 2));
            
            if(temp1 == 0.0 || temp2 == 0.0) angel[image->Width * i + j] = 0;
            else {
                float theta = atan2(temp1, temp2) * (180.0 / pi);
                if((theta <= 22.5 && theta >= -22.5) || (theta <= -157.5) || (theta >= 157.5)) angel[image->Width * i + j] = 1; // "-"
                else if((theta > 22.5 && theta <= 67.5) || (theta > -157.5 && theta <= -112.5)) angel[image->Width * i + j] = 2; // "/"
                else if((theta > 67.5 && theta <= 112.5) || (theta >= -112.5 && theta < -67.5)) angel[image->Width * i + j] = 3; // "|"
                else if((theta >= -67.5 && theta < -22.5) || (theta > 112.5 && theta < 157.5)) angel[image->Width * i + j] = 4; // "\"
            }
        }
    }
    SavePNMImage(outimage, (char*)"Canny_2.pgm");
    
    // Step 3: Non-Maximum Supression:
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int currPosition = image->Width * i + j;
            if(angel[currPosition] == 0) tempout1[currPosition] = 0;
            else if(angel[currPosition] == 1) {
                if(tempout[currPosition] >= tempout[currPosition-1] && tempout[currPosition] >= tempout[currPosition+1]) tempout1[currPosition] = tempout[currPosition];
                else tempout1[currPosition] = 0;
            }
            else if(angel[currPosition] == 2) {
                if(tempout[currPosition] >= tempout[image->Width * (i-1) + j + 1] && tempout[currPosition] >= tempout[image->Width * (i+1) + j - 1]) tempout1[currPosition] = tempout[currPosition];
                else tempout1[currPosition] = 0;
            }
            else if(angel[currPosition] == 3) {
                if(tempout[currPosition] >= tempout[image->Width * (i-1) + j] && tempout[currPosition] >= tempout[image->Width * (i+1) + j]) tempout1[currPosition] = tempout[currPosition];
                else tempout1[currPosition] = 0;
            }
            else if(angel[currPosition] == 4) {
                if(tempout[currPosition] >= tempout[image->Width * (i-1) + j - 1] && tempout[currPosition] >= tempout[image->Width * (i+1) + j + 1]) tempout1[currPosition] = tempout[currPosition];
                else tempout1[currPosition] = 0;
            }
        }
    }
    SavePNMImage(outimage1, (char*)"Canny_3.pgm");
    
    // Step 4: Detection with double threshold algorithm:
    int label_table[image->Width * image->Height], label_update[image->Width * image->Height];
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int currPosition = image->Width * i + j;
            if(tempout1[currPosition] > 120) {
                label_table[currPosition] = 1;
                label_update[currPosition] = 1;
            }
            else if(tempout1[currPosition] < 50) {
                label_table[currPosition] = 0;
                label_update[currPosition] = 0;
            }
            else {
                label_table[currPosition] = 2;
                label_update[currPosition] = 2;
            }
        }
    }
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int currPosition = image->Width * i + j;
            
            if(label_table[currPosition] == 2) {
                int sum = 0;
                for(int m = -1; m <= 1; m++) {
                    for(int n = -1; n <= 1; n++) {
                        sum += label_table[image->Width * (i + m) + (j + n)];
                        if(label_table[image->Width * (i + m) + (j + n)] == 1) {
                            label_update[currPosition] = 1;
                        }
                    }
                }
                if(sum == 2) label_update[currPosition] = 0;
            }
        }
    }
    // output the result image:
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            int currPosition = image->Width * i + j;
            if(label_update[currPosition] == 1) tempout2[currPosition] = 255;
            else tempout2[currPosition] = 0;
        }
    }
    SavePNMImage(outimage2, (char*)"Canny.pgm");
}

void LoG(Image *image) {
    unsigned char *tempin, *tempout, *tempout1;
    Image *outimage, *outimage1;
    int index, square[9];
    
    outimage = CreateNewImage(image, (char*)"#testing function");
    outimage1 = CreateNewImage(image, (char*)"#testing function");
    tempout1 = outimage1->data;
    
    // Step 1: Gaussian Blur:
    image = Gaussian(image);
    tempin = image->data;
    
    // Step 2: Laplacian:
    outimage = Laplacian(image);
    tempout = outimage->data;
    SavePNMImage(outimage, (char*)"Laplacian.pgm");
    
    // Step 3: Find zero crossings:
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
            if(square[4] > 80 && (abs(square[8]-square[0]) > 60 || abs(square[7]-square[1]) > 60 || abs(square[6]-square[2]) > 60 || abs(square[5]-square[3]) > 60))  {
                tempout1[image->Width * i + j] = 200;
            }
            else tempout1[image->Width * i + j] = 0;
            
        }
    }
    SavePNMImage(outimage1, (char*)"LoG.pgm");
}


Image *Gaussian(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    int index, mask[25];
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++) {
            index = 0;
            // record the values in the 5x5 square:
            for(int m = -2; m <= 2; m++) {
                for(int n = -2; n <= 2; n++) {
                    // use boundary check:
                    mask[index++] = boundaryCheck(j + n, i + m, image->Width, image->Height) ? tempin[image->Width * (i + m) + (j + n)] : 0;
                }
            }
            float temp = 41*mask[12] + 16*(mask[6]+mask[8]+mask[16]+mask[18]) + 26*(mask[7]+mask[11]+mask[13]+mask[17]) + 7*(mask[2]+mask[10]+mask[14]+mask[22]) + 4*(mask[1]+mask[3]+mask[5]+mask[9]+mask[15]+mask[19]+mask[21]+mask[23]) + (mask[0]+mask[4]+mask[20]+mask[24]);
            tempout[image->Width * i + j] = (int)(temp / 273.0);
        }
    }
    return(outimage);
}

void GlobalThreshold(Image *image) {
    unsigned char *tempin, *tempout;
    Image *outimage;
    float T = 128; // provide an initial T
    int size = image->Width * image->Height, times = 0;
    outimage = CreateNewImage(image, (char*)"#testing function");
    tempin = image->data;
    tempout = outimage->data;
    
    while(times < 30) {
        float count1 = 0, count2 = 0, sum1 = 0, sum2 = 0;
        times++;
        
        for(int i = 0; i < size; i++) {
            if(tempin[i] >= T) {
                sum2 += tempin[i];
                count2++;
            }
            else {
                sum1 += tempin[i];
                count1++;
            }
        }
        float average1 = sum1 / count1;
        float average2 = sum2 / count2;
        T = (average1 + average2) / 2.0;// update the T1
    }
    // output the image:
    for(int i = 0; i < size; i++) {
        if(tempin[i] >= T) tempout[i] = 255;
        else tempout[i] = 0;
    }
    SavePNMImage(outimage, (char *)"Global Threshold.pgm");
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

