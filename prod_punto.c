#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

void obtenerVector(int *vector, FILE *archivo);
int contarElementos(FILE *archivo);
int prodPunto(int *vector1, int *vector2, int tamano);

int main(int argc, char *argv[]){
    // Comprobar que se haya ingresado el parámetro (PID)
    if (argc<=3){
       printf("Debes ingresar los archivos de los vectores y la cantidad de hilos...\n");
       return 1;
    }    
    char vector1[20], vector2[20]; 
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
    int vec1[tvec1], vec2[tvec2], pp;
    obtenerVector(vec1, inFile1);
    obtenerVector(vec2, inFile2);
    pp = prodPunto(vec1,vec2,tvec1);
    printf("El producto punto de los vectores ingresados es: %d\n", pp);
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
    return cont + 1;
}

void obtenerVector(int *vector, FILE *archivo){
    char nro[5], ch[1], c;
    int cont = 0;
    nro[0] = '\0';
    while ((c = fgetc(archivo)) != EOF){
        ch[0]=c;
        if(c == '\n'){
            vector[cont] = atoi(nro);
            cont++;
            strcpy(nro, "");
        }else{
            strcat(nro, ch);
        }
    }
    vector[cont] = atoi(nro);
}   

int prodPunto(int *vector1, int *vector2, int tamano){
    int resultado = 0;
    for(int i = 0; i < tamano; i++){
        resultado = resultado + (vector1[i]*vector2[i]);
    }
    return resultado;
}