#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "grep.h"


/**

    Allocate memory for default option_holder struct. 
    @return option_holder struct initialized 
*/
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

/**

    The following function takes the option_holder struct as input
    and prints the selected options such as keyword or 
    if option i was selected to the terminal.

    @param options option_holder struct 

*/
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
        printf("Reading from selected files... \n");
    }

}

/**

    The following function allocates memory to the option_holder struct
    in order to save the files to the array.

    @param number_of_files the number of files that need to be put into the array
    @param array_of_files an array of strings that needs to be stored in options
    @param options the option_holder struct 

*/
void allocateFileArrayMemory(int number_of_files, char** array_of_files, option_holder* options){
    if(number_of_files >= 1){
        options->files = malloc(sizeof(options->files) * number_of_files);
        
        for(int i = 0; i < number_of_files; i++){
            options->files[i] = array_of_files[i];
        }
    }
}


/**

    The following function uses an input string and formats it to lower case.
    The input string is not altered a copy is made.  

    @param string any string
    @return a copy of the input string which is now in lower case 

 */
char *copyToLowercase(char* string){
    int size = strlen(string);
    char *copy = malloc(size + 1); //allocating memory and taking null terminator into consideration 

    if(!copy) return NULL;

    for(int i = 0; i < size; i++){
        copy[i] = tolower(string[i]);
    }

    copy[size] = '\0'; //adding null terminator 

    return copy;
}

/**

    The following returns an integer > 0 if the keyword
    was found in the input string.
    
    @param buff input string
    @param keyword keyword to search in buff 
    @param opt_i opt_i > 0 -> case insensitive search 
*/
int hasSubstring(char* buff, char* keyword, int opt_i){
    if(opt_i > 0){
        char* lower_buff = copyToLowercase(buff);
        char* lower_keyword = copyToLowercase(keyword);
        
        char* searchedString;
        searchedString = strstr(lower_buff, lower_keyword);

        return (searchedString != NULL);
     }

    char* searchedString;
    searchedString = strstr(buff, keyword);

    return (searchedString != NULL);
}

/**

    The following function is called when the program is executed.
    If there is an output file specified it writes it to the file.
    If not then the stdout is used. 

    @param buff input string (either user input or file input)
    @param options the option_holder struct 

*/
void programOutput(char* buff, option_holder* options){

    if(hasSubstring(buff, options->keyword, options->opt_i)){
        if(options->opt_o > 0){
            writeToFile(buff, options->output_file);
        }else{
            printf("%s", buff);
        }
    }
}

/**

    Appending output to file. If file isn't found it is created. 

    @param buff String that is appended to the output file
    @param output_filw path to the output file. 

*/
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

/**

    The following function reads from standard input (terminal).
    This might need some rework to allow any length of data. 

    @param options option_holder struct 

*/
void readingFromSTDIN(option_holder* options){
        char buff[256];
        while(fgets(buff, sizeof(buff), stdin) != NULL){ 
            programOutput(buff, options);
        }
}

/**

    The following function is called to start the grep
    and using options to determine how the program behaves.

    @param options option_holder struct
*/
void startMyGrep(option_holder* options){
    if(options->files == NULL){
        readingFromSTDIN(options);
    }
}

int main(int argc, char** argv){
    option_holder* option = createOptionHolder(); //initializing option_holder


    //Parsing options
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

    //Parsing keywords
    option->keyword = argv[optind];
    optind++;

    if(option->keyword == NULL){
        printf("Error occured: The keyword must not be NULL!\n");
        return EXIT_FAILURE;
    }

    //Parsing files if there are any 
    int number_of_files = argc - optind;

    if(number_of_files > 0){
        char *files[number_of_files];
        for(int i = 0; optind < argc; optind++){
            files[i] = argv[optind];
            i++;
        }
        
        allocateFileArrayMemory(number_of_files, files, option);
    }
    
    startMyGrep(option);
    free(option);

    return EXIT_SUCCESS;
}