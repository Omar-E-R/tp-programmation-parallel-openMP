/*******************************************************
 * Omar EL RIFAI TP3-4
 * Pour tester avec la version parallelise avec N threads
 * il faut definir la constante lors de la compilation
 * grace a l'option -D :
 * gcc ELRIFAI_Omar_tp34.c -otest -DPARALLEL=N -fopenmp -Wall

********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#define NB_PIXELS 256

typedef struct color_pixel_struct {
    unsigned char r,g,b;
} color_pixel_type;

typedef struct color_image_struct
{
  int width, height;
  color_pixel_type * pixels;
} color_image_type;

typedef struct grey_image_struct
{
  int width, height;
  unsigned char * pixels;
} grey_image_type;


/**********************************************************************/

color_image_type * loadColorImage(char *filename){
  int i, width,height,max_value;
  char format[8];
  color_image_type * image;
  FILE * f = fopen(filename,"r");
  if (!f){
    fprintf(stderr,"Cannot open file %s...\n",filename);
    exit(-1);
  }
  fscanf(f,"%s\n",format);
  assert( (format[0]=='P' && format[1]=='3'));  // check P3 format
  while(fgetc(f)=='#') // commentaire
    {
      while(fgetc(f) != '\n'); // aller jusqu'a la fin de la ligne
    }
  fseek( f, -1, SEEK_CUR);
  fscanf(f,"%d %d\n", &width, &height);
  fscanf(f,"%d\n", &max_value);
  image = malloc(sizeof(color_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(color_pixel_type));
  assert(image->pixels != NULL);

  for(i=0 ; i<width*height ; i++){
      int r,g,b;
      fscanf(f,"%d %d %d", &r, &g, &b);
      image->pixels[i].r = (unsigned char) r;
      image->pixels[i].g = (unsigned char) g;
      image->pixels[i].b = (unsigned char) b;
    }
  fclose(f);
  return image;
}

/**********************************************************************/

grey_image_type * createGreyImage(int width, int height){
  grey_image_type * image = malloc(sizeof(grey_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(unsigned char));
  assert(image->pixels != NULL);
  return(image);
}

/**********************************************************************/

void saveGreyImage(char * filename, grey_image_type *image){
  int i;
  FILE * f = fopen(filename,"w");
  if (!f){
    fprintf(stderr,"Cannot open file %s...\n",filename);
    exit(-1);
  }
  fprintf(f,"P2\n%d %d\n255\n",image->width,image->height);
  for(i=0 ; i<image->width*image->height ; i++){
    fprintf(f,"%d\n",image->pixels[i]);
  }
  fclose(f);
}

/*******************************************************
 * Transforms a color image PPM to grey PGM
********************************************************/
grey_image_type* colorImageToGrey(color_image_type *colorImage){
  double t, start, stop;
  start = omp_get_wtime();
  int width=colorImage->width, height=colorImage->height;
  grey_image_type *greyImage=createGreyImage(width, height);
  int r, g, b;
#ifdef PARALLEL
#pragma omp parallel for private(r, g, b) num_threads(PARALLEL)
#endif
  for (int i = 0; i < width * height; i++){
    r=colorImage->pixels[i].r;
    g=colorImage->pixels[i].g;
    b=colorImage->pixels[i].b;
    greyImage->pixels[i] = (299 * r +587 * g + 114 * b) / 1000;
  }
  stop = omp_get_wtime();
  t = stop - start;
  printf("TEMPS-colorToGrey: %lf\n", t);
  return greyImage;
}
//Une petite fonction qui fait le calcul d'ambossage d'un pixel
char greyPixelToColor(unsigned char* pixels, int index, int width, int height){
  return 128 - 2*pixels[index - width -1 ] - pixels[index-width] - pixels[index -1]
   + pixels[index +1] + pixels[index +width] + 2*pixels[index + width +1];

}
/*******************************************************
 * La fonction d'embossage
 *******************************************************/
grey_image_type* greyImageToColor(grey_image_type* greyImage){
  double t, start, stop;
  start = omp_get_wtime();
  int width=greyImage->width, height=greyImage->height;
  grey_image_type* colorImage=createGreyImage(width, height);
#ifdef PARALLEL
#pragma omp parallel for num_threads(PARALLEL)
#endif
  for (int i = 0; i < width*height; i++){
    if(i < width || i % width == 0 || (i+1)% width ==0|| (height-1)*width <= i ){
      colorImage->pixels[i]=greyImage->pixels[i];
    }else{
      colorImage->pixels[i]= greyPixelToColor(greyImage->pixels, i, width, height);
    }
  }
  stop=omp_get_wtime();
  t = stop - start;
  printf("TEMPS-greyTocolor: %lf\n", t);
  return colorImage;
}

/*******************************************************
 * La fonction qui augmente le contraste d'une image
 *******************************************************/
grey_image_type* transform(grey_image_type* greyImage){
  double t, start, stop;
  start = omp_get_wtime();
  int width=greyImage->width, height=greyImage->height;
  grey_image_type* contrastImage=createGreyImage(width, height);
  int *H, *C;
  H=calloc(NB_PIXELS, sizeof(int));//POUR S'ASSURER QUE LES CASES DE MEMOIRES RESERVEES SONT INITIALISEES A ZERO
  C=calloc(NB_PIXELS, sizeof(int));
  unsigned char pixel=0;
#ifdef PARALLEL
//Pour gagner de l'acceleration reelle il vaut mieux ne pas paralleliser
//cette boucle car elle contient une section critiques
#pragma omp parallel for private(pixel) num_threads(PARALLEL)
#endif
  for (int i = 0; i < width*height; i++){
    pixel=greyImage->pixels[i];
#ifdef PARALLEL
 //CET INSTRUCTIONS DOIT ETRE ATOMIQUE, SINON PLUSIEURS ACCES ET MODIFICATION PEUVENT ETRE FAIRE EN MEME TEMPS
#pragma omp critical
{
#endif
    H[pixel]++;
#ifdef PARALLEL
    }
#endif
  }
#ifdef PARALLEL
//ON PEUT PARALLELISER CETTE BOUCLE CAR CHAQUE LES INSTRUCTION SONT INDEPENDANTE A CHAQUE ITERATIONS DE (i)
#pragma omp parallel for num_threads(PARALLEL) schedule(dynamic, 50)
#endif
  for (int i = 0; i < NB_PIXELS; i++){
    for (int j = 0; j <= i; j++){
      C[i]+=H[j];
    }
  }
#ifdef PARALLEL
#pragma omp parallel for private(pixel) num_threads(PARALLEL)
#endif
  for (int i = 0; i < width*height; i++){
    pixel=greyImage->pixels[i];
    contrastImage->pixels[i] =256 * C[pixel] / (width * height);
  }
  free(H);
  free(C);
  stop = omp_get_wtime();
  t = stop - start;
  printf("TEMPS-transform: %lf\n", t);
  return contrastImage;
}



int main(int argc, char* argv[]){

  saveGreyImage("image0_grey.pgm",colorImageToGrey(loadColorImage("image0.ppm")));
  saveGreyImage("image0_color.pgm", greyImageToColor(colorImageToGrey(loadColorImage("image0.ppm"))));
  saveGreyImage("image0_contraste.pgm", transform(colorImageToGrey(loadColorImage("image0.ppm"))));
  saveGreyImage("image2_grey.pgm",colorImageToGrey(loadColorImage("image2.ppm")));
  saveGreyImage("image2_color.pgm", greyImageToColor(colorImageToGrey(loadColorImage("image2.ppm"))));
  saveGreyImage("image2_contraste.pgm", transform(colorImageToGrey(loadColorImage("image2.ppm"))));


  return 0;
}