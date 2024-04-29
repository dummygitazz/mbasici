#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parser.h"
#include "compiler/compile.h"
#include "vm/vm.h"

int main(int argc, char *argv[]){
    int n_opt = 0, h_opt = 0; 
    char *file_name = NULL;
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            if(!strcmp(argv[i], "-n")){
                n_opt = 1;
            }else if(!strcmp(argv[i], "-h")){
                h_opt = 1;
            }else{
                printf("invalid option: %s\n", argv[i]);
                return 1;
            }
        }else{
            file_name = argv[i];
            break;
        }
    }
    FILE *file = fopen(file_name, "r");
    if(file == NULL){
        printf("error: '%s' doesn't exist\n", file_name);
        return 1;
    }
    if(!n_opt){
        char lines[10001][200] = {0};
        char buff[200];
        int max_ln = 1;
        while(fscanf(file, "%s", buff) != EOF){
            int ln = atoi(buff);
            fgets(buff, sizeof(buff), file);
            strcpy(lines[ln], buff);
            if(ln > max_ln) max_ln = ln;
        }

        fclose(file);
        file = tmpfile();
        if(file == NULL){
            printf("error: failed to create tmp file");
            return 1;
        }
        for(int i = 1; i <= max_ln; i++){
            if(strlen(lines[i])){
                fprintf(file, "%s", lines[i]);
            }else{
                fprintf(file, "\n");
            }
        }
        fseek(file, 0, SEEK_SET);
    }
    struct parser *p = parse(file);
    if(p->error){
        printf("EXITING");
        exit(1);
    }
    struct compiler *c = compile(p->program, p->lines);
    if(c->error){
        exit(1);
    }
    struct vm *vm = vm_create(c->instr);
    vm_exec(vm);
}