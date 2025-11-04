#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


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

void readingFromSTDIN(int opt_i){
        while(1){
            char buff[256];
            fgets(buff, sizeof(buff), stdin);
        
            char* searchedString;
            searchedString = strstr(buff, options->keyword);
        
            if(searchedString != NULL){
                printf("%s", buff);
            }
        }
}


void startMyGrep(option_holder* options){
    if(options->files == NULL){
        
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