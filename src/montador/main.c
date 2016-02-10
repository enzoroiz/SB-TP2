#include <stdio.h>
#include <stdlib.h>
#include "mounter.h"

int main(int argc, char *argv[]){
	Mounter mounter;
	createMounter(argc, argv, &mounter);
	createSymbolTable(&mounter, argv[1]);
	translateProgram(&mounter, argv[1], argv[2]);
    return 0;
}