/*
DOSYA YAZMA ISLEMININ YAPILABILMESI ICIN
KOD DERLENDIKTEN SONRA TERMINAL(CMD) DE KODUN BULUNDUGU DIZININ ACILIP
 
 gcc -o 17011061 17011061.c
 ./17011061
 
KOMUTLARININ SIRASIYLA CALISTIRILMASI GEREKMEKTEDIR
 

KODDA YER ALAN FONKSIYONLARA VE YAPILMASI ISTENEN ISLEME GORE
YAPILMASI GEREKENLERE SATIR 264 TEN ITIBAREN DETAYLICA YER VERILMISTIR

-----------
 17011061 - TAHİR CAN ÖZER
 
 Not: Kodu derlerken macos üzerinde xcode kullandım.Farklı ideler ile test etme şansım olmadı.
*/


#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILTERSIZE 5
#define SIGMA 1.0
#define K  1

typedef struct Image {
    char pgmType[3];
    unsigned char** data;
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
} Image;

// YAPILAN DEFINE TANIMLAMALARINA GORE GAUS KERNELI OLUSTURAN FONKSIYON
void generateGausKernel(double **kernel){
    double x,y,sum = 0;
    int i, j;
    
    for (i = 0; i < FILTERSIZE; i++) {
        for (j = 0; j < FILTERSIZE; j++) {
            x = i - (FILTERSIZE - 1) / 2.0;
            y = j - (FILTERSIZE - 1) / 2.0;
            kernel[i][j] =K * exp(((pow(x, 2) + pow(y, 2)) / ((2 * pow(SIGMA, 2)))) *(-1));
            sum += kernel[i][j];
        }
    }
    for (i = 0; i < FILTERSIZE; i++) {
        for (j = 0; j < FILTERSIZE; j++) {
            kernel[i][j] /= sum;
        }
    }
}

// GAUS KERNELI ILE ORIJINAL GORUNTU MATRIX INE KONVULUSYON ISLEMI UYGULAYAN FONKSIYON
// ISLEM SONUCUNU **filterResult PARAMETRESI ILE ALIYORUZ
void applyGausFilter(Image* pgm, int **filterResult, double **kernel){
    
    int i,j,n,m;
    int size = FILTERSIZE/2;
    
    for(i=size; i < pgm->height -size; i++){
        for(j=size; j < pgm->width -size; j++){
            for(n = 0; n < FILTERSIZE; n++){
                for(m = 0; m < FILTERSIZE; m++){
                    filterResult[i][j] += kernel[n][m] * pgm->data[i-size+n][j-size+m];
                }
            }
            if(filterResult[i][j] > 255) {filterResult[i][j] = 255;}
            if(filterResult[i][j] < 0) {filterResult[i][j] = 0;}
        }
    }
}

//LAPLACIAN KERNELLERI ILE GORUNTU MATRIX INE KONVULUSYON ISLEMI UYGULAYAN FONKSIYON
//HER IKI KERNEL ICIN ISLEM AYNI ANDA UYGULANARAK SONUCLAR
//**firstFilterResult ve **secondFilterResult PARAMETLERI SAYESINDE ALINIR

void applyLaplacianFilter(Image* pgm, int **firstFilterResult,int **secondFilterResult, int fisrtKernel[3][3], int secondKernel[3][3]){
    
    int i,j,n,m;
    
    for(i=1; i < pgm->height -1; i++){
        for(j=1; j < pgm->width -1; j++){
            for(n = 0; n < 3; n++){
                for(m = 0; m < 3; m++){
                    firstFilterResult[i][j] += fisrtKernel[n][m] * pgm->data[i-1+n][j-1+m];
                    secondFilterResult[i][j] += secondKernel[n][m] * pgm->data[i-1+n][j-1+m];
                }
            }
            if(firstFilterResult[i][j] > 255) {firstFilterResult[i][j] = 255;}
            if(secondFilterResult[i][j] > 255) {secondFilterResult[i][j] = 255;}
            if(firstFilterResult[i][j] < 0) {firstFilterResult[i][j] = 0;}
            if(secondFilterResult[i][j] < 0) {secondFilterResult[i][j] = 0;}
        }
    }
    
}

//SOBEL KERNELLERI ILE GORUNTU MATRIX INE KONVULUSYON ISLEMI UYGULAYAN FONKSIYON
// X,Y VE HER IKI YONDEKI DEGISIM SONUCLARI
//**gxResult , **gyResult ve **sobelResult PARAMETRELERI SAYESINDE ALINIR
void applySobelFilter(Image* pgm, int **gxResult,int **gyResult,int **sobelResult,int gxKernel[3][3], int gyKernel[3][3]){
    
    int i,j,n,m;
    
    for(i=1; i < pgm->height -1; i++){
        for(j=1; j < pgm->width -1; j++){
            for(n = 0; n < 3; n++){
                for(m = 0; m < 3; m++){
                    gxResult[i][j] += gxKernel[n][m] * pgm->data[i-1+n][j-1+m];
                    gyResult[i][j] += gyKernel[n][m] * pgm->data[i-1+n][j-1+m];
                }
            }
            gxResult[i][j] = abs(gxResult[i][j]);
            gyResult[i][j] = abs(gyResult[i][j]);
            sobelResult[i][j] = sqrt(pow(gxResult[i][j],2)+pow(gyResult[i][j], 2));
            if(sobelResult[i][j] > 255) {sobelResult[i][j] = 255;}
     //       if(sobelResult[i][j] < 0) {sobelResult[i][j] = 0;} // 0 dan kucuk olma ihtimali yok
            if(gxResult[i][j] > 255) {gxResult[i][j] = 255;}
            if(gyResult[i][j] > 255) {gyResult[i][j] = 255;}
        }
    }
}

//file okuma islemi icin hazir olarak alindi
//image dosyasini okumak icin kullanildi
void ignoreComments(FILE* fp)
{
    int ch;
    char line[100];
    
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }else{
        fseek(fp, -1, SEEK_CUR);
    }
}


//image dosyasinin okunup Image structure ina atamanin yapildigi fonksiyon
bool openImage(Image* pgm,const char* filename){
    
    int i,j;
    FILE* pgmfile = fopen(filename, "rb");
    
    if (pgmfile == NULL) {
        printf("Dosya bulunmuyor\n");
        return false;
    }
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%s",pgm->pgmType);
        
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%d %d",&(pgm->width),&(pgm->height));
    ignoreComments(pgmfile);
    //   fscanf(pgmfile, "%d", &(pgm->maxValue));
    ignoreComments(pgmfile);
    pgm->data = malloc(pgm->height * sizeof(unsigned char*));
    
    if (pgm->pgmType[1] == '5') {   //binary
        
        fgetc(pgmfile);
        
        for (i = 0;i < pgm->height; i++) {
            
            pgm->data[i] = malloc(pgm->width * sizeof(unsigned char));
            
            if (pgm->data[i] == NULL) {
                fprintf(stderr,"malloc failed\n");
                exit(1);
            }
            fread(pgm->data[i], sizeof(unsigned char),pgm->width, pgmfile);
        }
    }else if (pgm->pgmType[1] == '2'){        //ascii tipi icin gerekli
        
        for (int i = 0; i < pgm->height; ++i)
        {
            pgm->data[i] = (unsigned char*)malloc(pgm->width * sizeof(unsigned char*));
            for (j = 0; j < pgm->width; ++j)
            fscanf (pgmfile, "%d", &pgm->data[i][j]);
        }
    }else{
        fprintf(stderr, "Hatali dosya tipi!\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(pgmfile);
    
    return true;
}

//ISLEM SONUCU OLUSAN GORUNTULERIN DOSYAYA YAZILMASINI SAGLAYAN FONKSIYON
void writeFilteredImage(Image* pgm,int **filterResult, char filename[30]){
    
    int i,j;
    int temp = 0;
    
    FILE* pgmimg;
 //   pgmimg = fopen("3x3_S1_FruitGS.pgm", "wb");
    pgmimg = fopen(filename, "wb");
    
    fprintf(pgmimg, "P2\n");
    
    fprintf(pgmimg, "%d %d\n", pgm->width, pgm->height);
    
    fprintf(pgmimg, "255\n");
    
    for (i = 0; i < pgm->height; i++) {
        for (j = 0; j < pgm->width; j++) {
            temp = filterResult[i][j];
            
            fprintf(pgmimg, "%d ", temp);
        }
        fprintf(pgmimg, "\n");
    }
    fclose(pgmimg);
}


int main(int argc, char const* argv[])
{
    int i,j;
    int sobelXKernel[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
    int sobelYKernel[3][3] = {{1,2,1}, {0,0,0}, {-1,-2,-1}};
    int firstLaplacianKernel[3][3] = {{0,-1,0}, {-1,4,-1}, {0,-1,0}};
    int secondLaplacianKernel[3][3] = {{-1,-1,-1}, {-1,8,-1}, {-1,-1,-1}};

    Image* pgm = malloc(sizeof(Image));
    const char* filePath;
    
    if (argc == 2)
        filePath = argv[1];
    else
        // !!!!!  IMAGE PATH INI " " ICINE GIRINIZ
        
    filePath = "";

    openImage(pgm, filePath);

    double** gausKernel =(double **)calloc((FILTERSIZE) , sizeof(double *));
    int** gausFilterResult =(int **)calloc((pgm->height) , sizeof(int *));
    int** firstLablacianResult =(int **)calloc((pgm->height) , sizeof(int *));
    int** secondLablacianResult =(int **)calloc((pgm->height) , sizeof(int *));
    int** gxResult =(int **)calloc((pgm->height) , sizeof(int *));
    int** gyResult =(int **)calloc((pgm->height) , sizeof(int *));
    int** sobelResult =(int **)calloc((pgm->height) , sizeof(int *));

    for(i = 0; i < FILTERSIZE; i++){
        gausKernel[i] = (double *)calloc((FILTERSIZE) , sizeof(double));
    }
    
    for (i = 0; i < pgm->height; i++) {
        gausFilterResult[i] = (int *)calloc((pgm->width) , sizeof(int));
        firstLablacianResult[i] = (int *)calloc((pgm->width) , sizeof(int));
        secondLablacianResult[i] = (int *)calloc((pgm->width) , sizeof(int));
        gxResult[i] = (int *)calloc((pgm->width) , sizeof(int));
        gyResult[i] = (int *)calloc((pgm->width) , sizeof(int));
        sobelResult[i] = (int *)calloc((pgm->width) , sizeof(int));
    }
    
/*
 Yapmak istediginiz isleme gore ilgili fonksiyonu yorum satirindan kaldiriniz. Bkz satir :294
 
 -----Fonksiyon aciklamalari-----
 
 1)generateGausKernel(gausKernel);
    *DEFINE da tanimlanan parametrelere gore bir gaus kerneli olusturur.
    *Kerneli gausKernel double pointer i ile doner
 
 2)applyGausFilter(pgm, gausFilterResult, gausKernel);
    *Verilen image dosyasina gausKerneli ile konvulusyon uygular
    *Sonucu gausFilterResult double pointer i sayesinde doner
 
 3)applySobelFilter(pgm, gxResult,gyResult,sobelResult,sobelXKernel, sobelYKernel);
    *Verilen image dosyasina sobel kernellerini uygular
    *Sonuclari ayri ayri olarak gxResult , gyResult, SobelResult double pointer lari doner
 
 
 4)applyLaplacianFilter(pgm, firstLablacianResult, secondLablacianResult, firstLaplacianKernel, secondLaplacianKernel);
    *Verilen image dosyasina Laplacian kernellerini uygular
    *Sonuclarini ayri ayri olarak firstLablacianResult , secondLablacianResult double pointer lari doner
 
*/
    
    
   generateGausKernel(gausKernel);
   applyGausFilter(pgm, gausFilterResult, gausKernel);
//   applySobelFilter(pgm, gxResult,gyResult,sobelResult,sobelXKernel, sobelYKernel);
//   applyLaplacianFilter(pgm, firstLablacianResult, secondLablacianResult, firstLaplacianKernel, secondLaplacianKernel);
    

/*
 filename : Dosyanin kayit edilecegi adini tirnak icine giriniz
 
 Kolaylik olmasi acisindan birden fazla filename hazir olarak olusturulmustur.
*/
    
   char filenameGaus[30] = "LenaGaus_5x5_S1.pgm";
//   char filenameGx[20] = "7x7_S3_Gx_Lena.pgm";
//   char filenameGy[20]= "7x7_S3_Gy_Lena.pgm";
//   char filenameSobel[22] = "7x7_S3_Both_Lena.pgm";
//   char filenameLap1[30] = "7x7_S4_FirstLap_Lena.pgm";
//   char filenameLap2[30] = "7x7_S4_SecondLap_Lena.pgm";

/*
 Yazilacak dosyaya-dosyalara ait parametleri iceren fonksiyonu yorum satirindan kaldiriniz.
 Kolaylik olmasi acisindan birden fazla writeFilteredImage fonksiyonu hazir olarak olusturulmustur.
 */

    
    writeFilteredImage(pgm, gausFilterResult, filenameGaus);
//    writeFilteredImage(pgm, gxResult, filenameGx);
//    writeFilteredImage(pgm, gyResult, filenameGy);
//    writeFilteredImage(pgm, sobelResult, filenameSobel);
//    writeFilteredImage(pgm, firstLablacianResult, filenameLap1);
//    writeFilteredImage(pgm, secondLablacianResult, filenameLap2);

    
//OLUSTURULAN GAUS KERNELINI GORUNTULEMEK ICIN 329-334 SATIRLARINI YORUM SATIRINDAN KALDIRINIZ
  /*
    for (i = 0; i < FILTERSIZE; i++) {
        for (j = 0; j < FILTERSIZE; j++) {
           printf("%f ",gausKernel[i][j]);
        }
        printf("\n");
    }
   */
    
    for(i=0; i < FILTERSIZE; i++){
        free(gausKernel[i]);
    }
    
    for( i = 0; i < pgm->height; i++) {
        free(gausFilterResult[i]);
        free(firstLablacianResult[i]);
        free(secondLablacianResult[i]);
        free(gxResult[i]);
        free(gyResult[i]);
        free(sobelResult[i]);
    }
    free(gausKernel);
    free(gausFilterResult);
    free(firstLablacianResult);
    free(secondLablacianResult);
    free(gxResult);
    free(gyResult);
    free(sobelResult);
    return 0;
}
