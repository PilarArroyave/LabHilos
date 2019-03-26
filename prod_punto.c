#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

//declaracion de métodos
void obtenerVector(int *vector, FILE *archivo);
int contarElementos(FILE *archivo);
int prodPunto(int *vector1, int *vector2, int tamano);
int prodPuntoHilos(int *vector1, int *vector2, int tam, int numeroHilos);
void *multiplicar(void *argv);
double timeval_diff(struct timeval *a, struct timeval *b);

//declaracion de estructuras
typedef struct dato {
   int  *vect1;
   int  *vect2;
   int  offset;
   int  tam;
   int idHilo;
} data;  

//declaracion de variables globles
int result = 0;
pthread_mutex_t banderaResult;


int main(int argc, char *argv[]){
    pthread_mutex_init(&banderaResult,NULL);
    // Comprobar que se haya ingresado el parámetro (PID)
    if (argc<=3){
       printf("Debes ingresar los archivos de los vectores y la cantidad de hilos respectivamente...\n");
       return 1;
    }
    
    char vector1[20], vector2[20]; 
    int tamHilos = strtol(argv[3], NULL, 10);

    if(tamHilos == 2 || tamHilos == 4 || tamHilos == 8 || tamHilos == 16){

    }
    else
    {
        printf("Error en el tamaño de los hilos debe ser alguno de estos valores [ 2, 4, 8, 16 ]\n");
        return 1;
    }
    
    FILE *inFile1, *inFile2;
    strncpy(vector1, argv[1], strlen(argv[1]) + 1);
    strncpy(vector2, argv[2], strlen(argv[2]) + 1);
    inFile1 = fopen(vector1,"r");
    inFile2 = fopen(vector2,"r");

    // Comprobar que los archivos se han abierto
    if(inFile1 == NULL){
        printf("Error al abrir el archivo %s\n",vector1);
        return -1;
    }
    if(inFile2 == NULL){
        printf("Error al abrir el archivo %s\n",vector2);
        return -1;
    }
    
    //Inician las operaciones para el producto punto    
    int tvec1, tvec2;
    tvec1 = contarElementos(inFile1);
    rewind(inFile1);
    tvec2 = contarElementos(inFile2);
    rewind(inFile2);
    if (tvec1 != tvec2){
        printf("Los vectores tienen tamaños diferentes. No se puede realizar el producto punto entre ellos \n");
        return -1;
    }
    int vec1[tvec1], vec2[tvec2], pp, pp2;
    obtenerVector(vec1, inFile1);
    obtenerVector(vec2, inFile2);
    //descomentar
    pp = prodPunto(vec1,vec2,tvec1);
    pp2 = prodPuntoHilos(vec1,vec2,tvec1, tamHilos);
    printf("El producto punto de los vectores ingresados es: %d\n", pp);
    printf("El producto punto de los vectores ingresados es: %d\n", pp2);
    fclose(inFile1);
    fclose(inFile2);
    //showInfoProccess(vector1, inFile1);    
}


int contarElementos(FILE *archivo){
    int cont = 0;
    char c;
    do{
        c = fgetc(archivo);
        if(c == '\n'){
            cont++;
        }
    }while(c != EOF);
    return cont;
}


void obtenerVector(int *vector, FILE *archivo){
    char nro[20], ch[1], c;
    int cont = 0;
    while((fgets(nro, 201, archivo)!=NULL)) {
      		vector[cont]=atoi(nro);	
      		cont++;
   	}
}   


int prodPunto(int *vector1, int *vector2, int tamano){
    struct timeval t_ini, t_fin;
    double secs;
    int resultado = 0;
    gettimeofday(&t_ini, NULL);
    for(int i = 0; i < tamano; i++){
        resultado = resultado + (vector1[i]*vector2[i]);
    }
    gettimeofday(&t_fin, NULL);
    secs = timeval_diff(&t_fin, &t_ini);
    printf("Tiempo de ejecucion sin hilos %.16g ms\n", secs * 1000.0);
    return resultado;
}


int prodPuntoHilos(int *vector1, int *vector2, int tam, int numeroHilos){
    
    //validamos que el numero de hilos es diferente de cero
    if(numeroHilos <= 0) return -1;
    if(numeroHilos > tam) {
        numeroHilos = tam;
        printf("como el numero de hilos es mayor que el numero de componentes del vector entonces se toman el número de componentes domo el número de hilos...\n");
    }
    pthread_t hilos[numeroHilos];
    struct timeval t_ini, t_fin;
    double secs;
    int subTamano = tam/numeroHilos;
    int residuo = (int) ceil((double) (tam % numeroHilos) / (double) numeroHilos);
    int acumulado = 0;
    
     result = 0;

    gettimeofday(&t_ini, NULL);
    for(int i = 0; i < numeroHilos; i++)
    {
        int tamVec = subTamano;
        acumulado += tamVec;
        //le suma la adicion al subtamaño
        if((acumulado + (numeroHilos - (i+1)) * subTamano) < tam){
            tamVec += residuo;
            acumulado += residuo;
        }
        data d;
        d.tam = tamVec;
        d.offset = acumulado - tamVec;
        d.vect1 = vector1;
        d.vect2 = vector2;
        d.idHilo = i;
        pthread_create(&hilos[i], NULL, multiplicar, (void *)&d);
        pthread_join(hilos[i],NULL);
    }
    gettimeofday(&t_fin, NULL);
    secs = timeval_diff(&t_fin, &t_ini);
    printf("Tiempo de ejecucion con %d hilos %.16g ms\n", numeroHilos, secs * 1000.0);
    return result;
}


void *multiplicar(void *args){
    data *d = (data *) args;
    int producto = 0;
    for(int i = d->offset; i < (d->offset + d->tam); i++)
    {
        producto += d->vect1[i] * d->vect2[i];
        //printf("id: %d       vec1: %d - vec2: %d - prod: %d\n",d->idHilo, d->vect1[i], d->vect2[i], producto);
        
    }
    pthread_mutex_lock(&banderaResult);
        result += producto;
    pthread_mutex_unlock(&banderaResult);
    //printf("hola \ttamaño: %d  -  offset: %d \n", d->tam, d->offset);
}

/* retorna "a - b" en segundos */
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}