typedef struct{
    char* keyword;
    int opt_i;
    int opt_o;
    char* output_file;
    int number_of_files;
    char** files;
} option_holder;



option_holder* createOptionHolder();

void printSelectedOptions(option_holder* options);
