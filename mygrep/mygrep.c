#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "grep.h"

option_holder* createOptionHolder(){
    option_holder* options = malloc(sizeof(option_holder));
    options->keyword = NULL;
    options->opt_i = 0;
    options->opt_o = 0;
    options->output_file = NULL;
    options->files = NULL;
    return options;
}
void writeToFile(char* buff, char* output_file);

void printSelectedOptions(option_holder* options){

    printf("Keyword: %s\n", options->keyword);
    
    if(options->opt_i > 0){
        printf("Option i was selected \n");
    }
    
    if(options->opt_o > 0){
        printf("Option o was selected with output file: %s\n", options->output_file);
    }

    if(options->files == NULL){
        printf("Reading from standard input... \n");
    }else{
        printf("Reading from selected files...\n");
    }

}

int hasSubstring(char* buff, char* keyword, int opt_i){


    char string_buff[strlen(buff)] ;
    memcpy(string_buff, buff, strlen(buff));




    if(opt_i > 0){
        for(long unsigned int i = 0; i < strlen(keyword); i++){
            keyword[i] = tolower(keyword[i]);
        }

        for(long unsigned int i = 0; i < strlen(string_buff); i++){
            string_buff[i] = tolower(string_buff[i]);
        }
     }



    char* searchedString;
    searchedString = strstr(string_buff, keyword);

    return (searchedString != NULL);
}


void programOutput(char* buff, option_holder* options){

    if(hasSubstring(buff, options->keyword, options->opt_i)){
        if(options->opt_o > 0){
            writeToFile(buff, options->output_file);
        }else{
            printf("%s", buff);
        }
    }
}



void writeToFile(char* buff, char* output_file){
    FILE* fptr;

    fptr = fopen(output_file, "a+");

    if(fptr == NULL){
        printf("Error occurred, couldn't open file. \n");
        exit(EXIT_FAILURE);
    }else{
        fputs(buff, fptr);
    }

    fclose(fptr);
}




void readingFromSTDIN(option_holder* options){
        while(1){
            char buff[256];
            fgets(buff, sizeof(buff), stdin);
            
            programOutput(buff, options);
        }
}


void startMyGrep(option_holder* options){
    if(options->files == NULL){
        readingFromSTDIN(options);
    }
}




int main(int argc, char** argv){
    option_holder* option = createOptionHolder();

    int opt;
    while((opt = getopt(argc, argv, ":io:")) != -1){
        switch(opt){
            case 'i':
                option->opt_i++;
                break;
            case 'o':
                if(option->opt_o == 0){     
                    option->output_file = optarg;
                    option->opt_o++;
                }
                break;
            case '?':
                printf("Error occured! \n");
                return EXIT_FAILURE;
        }
    }

    option->keyword = argv[optind];


    if(option->keyword == NULL){
        printf("Error occured: The keyword must not be NULL!\n");
        return EXIT_FAILURE;
    }



    startMyGrep(option);

    free(option);
}