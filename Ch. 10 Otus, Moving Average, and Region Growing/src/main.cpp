#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#define pi acos(-1)

int boundaryCheck(int index_x, int index_y, int width, int height);
float calculate_sigma(int k, float mg, float ratio[]);
float sum(int k, float ratio[]);
void Otsu(Image *);
void Otsu_Partition(Image *);
void movingAverage(Image *);
void region_growing(Image *);
void DFS_MarkPixels(Image *, int *, int x, int y);
unsigned char *Otsu_Algorithm(unsigned char *, int width, int height);
Image *AverageFilter(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *, int flag);
void SavePNMImage(Image *, char *);

int main(int argc, char **argv)
{
    // Please adjust the input filename and path to suit your needs:
    char* file_in1 = (char*)"large_septagon_gaussian_noise_mean_0_std_50_added.pgm";
    char* file_in2 = (char*)"septagon_noisy_shaded.pgm";
    char* file_in3 = (char*)"spot_shaded_text_image.pgm";
    char* file_in4 = (char*)"defective_weld.pgm";
    char* file_in5 = (char*)"noisy_region.pgm";
    
    TestReadImage(file_in1, (char*)"", 1);
    TestReadImage(file_in2, (char*)"", 2);
    TestReadImage(file_in3, (char*)"", 3);
    TestReadImage(file_in4, (char*)"", 4);
    // TestReadImage(file_in5, (char*)"", 5);
    return(0);
}

int TestReadImage(char *file_in, char *file_out, int flag)
{
    Image *image, *smoothed;
    image = ReadPNMImage(file_in);
    
    //Topic 1:
    if(flag == 1) {
        Otsu(image);
        smoothed = AverageFilter(image);
        Otsu(smoothed);
    }
    
    //Topic 2:
    if(flag == 2) Otsu_Partition(image);
    
    //Topic 3:
    if(flag == 3) movingAverage(image);
    
    //Topic 4:
    if(flag == 4) region_growing(image);

    return 0;
}

// Algorithms Code:
void region_growing(Image *image) {
    unsigned char *tempin, *tempout1, *tempout2;
    Image *outimage1, *outimage2;
    int size = image->Width * image->Height;
    int checkBoard[size];
    float histogram[256];
    outimage1 = CreateNewImage(image, (char*)"#testing Function");
    outimage2 = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout1 = outimage1->data;
    tempout2 = outimage2->data;

    for(int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    for(int i = 0; i < size; i++) {
        int temp = tempin[i];
        histogram[temp] += 1;
    }
    
    int T = 256;
    int light_count = 0;
    while(light_count < (float)size * 0.004) {
        T--;
        light_count += histogram[T];
    }
    for(int i = 0; i < size; i++) {
        if(tempin[i] >= T) tempout1[i] = 255;
        else tempout1[i] = 0;
    }
    SavePNMImage(outimage1, (char*)"connected.pgm");
    
    for(int i = 0; i < size; i++) {
        if(tempout1[i] == 255) checkBoard[i] = 1;
        else checkBoard[i] = 2;
    }
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            DFS_MarkPixels(image, checkBoard, j, i);
        }
    }
        
    for(int i = 0; i < size; i++) {
        if(checkBoard[i] == 3) tempout2[i] = 255;
        else tempout2[i] = 0;
    }
    SavePNMImage(outimage2, (char*)"Region_growing.pgm");
}

void DFS_MarkPixels(Image *image, int *checkBoard, int x, int y) {
    unsigned char *tempin;
    tempin = image->data;
    int currPosition = image->Width * y + x;
    // Base Case:
    if(x < 0 || y < 0 || x >= image->Width || y >= image->Height || checkBoard[currPosition] != 1) {
        return;
    }
    // Recursive Steps:
    checkBoard[currPosition] = 3;
    // use 8-adjacent marking:
    for(int m = -1; m <= 1; m++) {
        for(int n = -1; n <= 1; n++) {
            if(checkBoard[image->Width * (y+n) + x+m] != 2) continue;
            if(tempin[image->Width * (y+n) + x+m] > 200) {
                checkBoard[image->Width * (y+n) + x+m] = 1;
                DFS_MarkPixels(image, checkBoard, x+m, y+n);
            }
            else checkBoard[image->Width * (y+n) + x+m] = 0;
        }
    }
}

void movingAverage(Image *image) {
    unsigned char *tempin, *tempout, *Z;
    Image *outimage;
    int size = image->Width * image->Height;
    float T[size];
    int T_inverse[size];
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    Z = (unsigned char *) malloc(size+20);

    for(int i = 0; i < 20; i++) {
        Z[i] = 0;
    }
    
    int index = 20;
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            Z[index++] = tempin[image->Width * i + j];
        }
        i++;
        for(int j = image->Width-1; j <= 0; j++){
            Z[index++] = tempin[image->Width * i + j];
        }
    }
    
    for(int i = 0; i < size; i++) {
        float sum = 0.0;
        for(int j = 0; j < 20; j++) {
            sum += Z[i-j+20];
        }
        T[i] = sum / 20.0 * 0.5;
    }
    
    index = 0;
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            T_inverse[index++] = T[image->Width * i + j];
        }
        i++;
        for(int j = image->Width-1; j <= 0; j++){
            T_inverse[index++] = T[image->Width * i + j];
        }
    }
    
    for(int i = 0; i < size; i++) {
        if(tempin[i] > T_inverse[i]) tempout[i] = 255;
        else tempout[i] = 0;
    }
    SavePNMImage(outimage, (char*)"Moving_Average.pgm");
}

void Otsu_Partition(Image *image) {
    unsigned char *tempin, *tempout;
    int y = (float)image->Height / 2.0 + 1;
    int x = (float)image->Width / 3.0 + 1;
    int size_partition = x * y;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    unsigned char inPart1[size_partition], inPart2[size_partition], inPart3[size_partition], inPart4[size_partition], inPart5[size_partition], inPart6[size_partition];
    unsigned char *outPart1, *outPart2, *outPart3, *outPart4, *outPart5, *outPart6;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            if(j - 2*x >= 0 && i - y >= 0) {
                inPart6[x * (i-y) + (j-2*x)] = tempin[image->Width * i + j];
            }
            else if(j - x < 0 && i - y < 0) {
                inPart1[x * i + j] = tempin[image->Width * i + j];
            }
            else if(j - 2*x >= 0 && i - y < 0) {
                inPart3[x * i + (j-2*x)] = tempin[image->Width * i + j];
            }
            else if(j - x < 0 && i - y >= 0) {
                inPart4[x * (i-y) + j] = tempin[image->Width * i + j];
            }
            else if(j >= x && j < 2*x && i - y >= 0) {
                inPart5[x * (i-y) + (j-x)] = tempin[image->Width * i + j];
            }
            else if(j >= x && j < 2*x && i - y < 0) {
                inPart2[x * i + (j-x)] = tempin[image->Width * i + j];
            }
        }
    }
    
    outPart1 = Otsu_Algorithm(inPart1, x, y);
    outPart2 = Otsu_Algorithm(inPart2, x, y);
    outPart3 = Otsu_Algorithm(inPart3, x, y);
    outPart4 = Otsu_Algorithm(inPart4, x, y);
    outPart5 = Otsu_Algorithm(inPart5, x, y);
    outPart6 = Otsu_Algorithm(inPart6, x, y);
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            if(j - 2*x >= 0 && i - y >= 0) {
                tempout[image->Width * i + j] = outPart6[x * (i-y) + (j-2*x)];
            }
            else if(j - x < 0 && i - y < 0) {
                tempout[image->Width * i + j] = outPart1[x * i + j];
            }
            else if(j - 2*x >= 0 && i - y < 0) {
                tempout[image->Width * i + j] = outPart3[x * i + (j-2*x)];
            }
            else if(j - x < 0 && i - y >= 0) {
                tempout[image->Width * i + j] = outPart4[x * (i-y) + j];
            }
            else if(j >= x && j < 2*x && i - y >= 0) {
                tempout[image->Width * i + j] = outPart5[x * (i-y) + (j-x)];
            }
            else if(j >= x && j < 2*x && i - y < 0) {
                tempout[image->Width * i + j] = outPart2[x * i + (j-x)];
            }
        }
    }
    SavePNMImage(outimage, (char*)"Otsu_Partition.pgm");
}

unsigned char* Otsu_Algorithm(unsigned char *tempin, int width, int height) {
    unsigned char *tempout;
    int size = width * height;
    tempout = (unsigned char *) malloc(size);
    float histogram[256], ratio[256], sigma[256];
    
    for(int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    for(int i = 0; i < size; i++) {
        int temp = tempin[i];
        histogram[temp] += 1;
    }
    for(int i = 0; i < 256; i++) {
        ratio[i] = histogram[i] / (float)size;
    }
    
    float mg = sum(256, ratio);
    
    for(int i = 0; i < 256; i++) {
        sigma[i] = calculate_sigma(i, mg, ratio);
    }
    
    float max = sigma[0];
    int max_count = 1, k = 0;
    for(int i = 1; i < 256; i++) {
        if(abs(sigma[i] - max) < 1e-10) {
            k += i;
            max_count++;
        }
        else if(sigma[i] > max) {
            max = sigma[i];
            max_count = 1;
            k = i;
        }
    }
    float k_final = (float)k / (float)max_count;
    
    for(int i = 0; i < size; i++) {
        if(tempin[i] <= k_final) tempout[i] = 0;
        else tempout[i] = 255;
    }
    return tempout;
}

void Otsu(Image *image) {
    unsigned char *tempin, *tempout, *out;
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    out = outimage->data;
    tempout = Otsu_Algorithm(tempin, image->Width, image->Height);
    
    for(int i = 0; i < image->Width * image->Height; i++) {
        out[i] = tempout[i];
    }
    SavePNMImage(outimage, (char*)"Otsu.pgm");
}

float calculate_sigma(int k, float mg, float ratio[]){
    float p1k = 0.0;
    for(int i = 0; i < k; i++) {
        p1k += ratio[i];
    }
    float mk = sum(k, ratio);
    if(p1k < 1e-10 || (1 - p1k) < 1e-10) return 0.0;
    else return pow(mg * p1k - mk, 2) / (p1k * (1 - p1k));
}

float sum(int k, float ratio[]) {
    float sum = 0.0;
    for(int i = 0; i < k; i++) {
        sum += i * ratio[i];
    }
    return sum;
}

Image *AverageFilter(Image *image) {
    unsigned char *tempin, *tempout, Sudoku[25];
    Image *outimage;
    outimage = CreateNewImage(image, (char*)"#testing Function");
    tempin = image->data;
    tempout = outimage->data;
    
    for(int i = 0; i < image->Height; i++) {
        for(int j = 0; j < image->Width; j++){
            int num = 0;
            for(int x = -2; x <= 2; x++) {
                for(int y = -2; y <= 2; y++) {
                    Sudoku[num++] = boundaryCheck(j + y, i + x, image->Width, image->Height) ? tempin[image->Width * (i + x) + (j + y)] : 0;
                }
            }
            float sum = 0.0;
            for(int k = 0; k < 25; k++) {
                sum += Sudoku[k];
            }
            int avg = round(sum / 25.0);
            tempout[image->Width * i + j] = avg;
        }
    }
    return outimage;
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

