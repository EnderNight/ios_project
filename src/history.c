
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void history();

int main(void){
    history();
    return 0;
}
void history(){
    int i;
    FILE* fd=NULL;
    char size[4096];
    int numHist=0;
    char *Hist1;
    // We put all the pointer to null in order to have no duplicate problems  
    Hist1 = malloc(4096);
    fd= fopen("history.txt","w+");
    if (NULL == fd){
            printf("\n fopen() Error!!\n"); 
            free(Hist1);
            exit(1);
        } 
    while(1){
        //We need to read the input from the shell
        if(fgets(size, 4096, stdin) == NULL){
            fclose(fd);
            free(Hist1);
            exit(1);
        }
        //We check if the user has entered a command
        if(strcmp(size, "") !=0){

            if ((Hist1 = strdup(size)) != NULL)
            {
                if (Hist1[0] != '\0'){
                    fprintf( fd, "History command  %d: %s", numHist, Hist1);
                    numHist++;
                }
            }else{
                fprintf(stderr, "The user has not entwred a format correct command -- failed\n");
            }
        }
        if (numHist>9){
            numHist=0;
            free(Hist1);
            exit(0);
        }
    }      	
}
