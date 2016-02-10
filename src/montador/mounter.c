#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mounter.h"

void createMounter(int argc, char *argv[], Mounter *mounter){
	int i, j;
	
	// Initialize VM mode
	if((argc != 4) || (argv[3][0] == 's')){
		mounter->verbose = FALSE;
	} else {
		mounter->verbose = TRUE;
	}

	// Initialize symbol tables
	for(i=0; i<TABLE_SIZE; i++){
		mounter -> symbolPC[i] = NULL_INT;
		for(j=0; j<INSTRUCTION_SIZE; j++){
			mounter -> symbolLabel[i][j] = NULL_CHAR;
		}
	}
}

int getNumberOfOperands(char operation[]){
	// Instructions with 2 operands
    if( !strcasecmp(operation, "COPY") || !strcasecmp(operation, "LOAD") ||
        !strcasecmp(operation, "STORE") || !strcasecmp(operation, "ADD") ||
        !strcasecmp(operation, "SUB") || !strcasecmp(operation, "AND") || 
        !strcasecmp(operation, "OR")  || !strcasecmp(operation, "XOR")){
        return 2;
    } else if( 
    	// Instruction with 1 operand
    	!strcasecmp(operation, "READ") || !strcasecmp(operation, "WRITE") ||
    	!strcasecmp(operation, "NOT") || !strcasecmp(operation, "PUSH") ||
        !strcasecmp(operation, "NEG") || !strcasecmp(operation, "JMP") ||
        !strcasecmp(operation, "JZ") || !strcasecmp(operation, "JNZ") ||
        !strcasecmp(operation, "JN") || !strcasecmp(operation, "JNN")  ||
        !strcasecmp(operation, "POP") || !strcasecmp(operation, "CALL") ){
        return 1;
    } else {
    	// HALT or RET -- No operand
        return 0;
    }
}

int isLabel(char aux[]){
	int lastCharPosition = strlen(aux) - 1;

	// Check last char = ':' to check if is label
    if(aux[lastCharPosition] == ':'){
        aux[lastCharPosition] = '\0';
        return TRUE;
    } else {
        return FALSE;
    }
}

int isBreakLine(char aux[]){
	// Verifica se instrução é ENDMACRO
    if(!strcasecmp(aux, "\n") || !strcasecmp(aux, "")){
        return TRUE;
    } else {
        return FALSE;
    }	
}

void removeComments(char aux[]){
    int i;
    // Remove comments from lines
    for (i=0; i<INSTRUCTION_SIZE; i++){
    	if(aux[i] == ';'){
    	    aux[i] = '\n';
    	    aux[i+1] = '\0';
    	    break;
    	}
    }
}

int getRegister(char reg[]){
	// Remove o R da string de registrador
	char * aux;
	aux = strtok (reg, "R");

	// Retorna o registrador como número
	return atoi(aux);
}

int getLabelPC(Mounter *mounter, char label[], int PC){
    int i;

    // Try to find corresponding label and return
    for (i=0; i<TABLE_SIZE; i++){
    	if(!strcasecmp(mounter->symbolLabel[i], label)){
    	    return mounter->symbolPC[i] - PC;
    	}
    }

    return atoi(label);
}

void createSymbolTable(Mounter *mounter, char input[]){
	FILE *inputProgram;
	char line[INSTRUCTION_SIZE], aux[INSTRUCTION_SIZE];
	int PC=0, ILC=0;

	// Open file to read and get first line
	inputProgram = fopen(input, "r");
	
	// Read file until reach its end
	while((!feof(inputProgram)) && (ILC < TABLE_SIZE) ){
		// Get the line
		PC++;
        fgets(line, sizeof(line), inputProgram);
        removeComments(line);

		// Check if any label is found
    	strcpy(aux, "");
    	sscanf(line,"%s", aux);

    	if(isBreakLine(aux)){
    		PC--;
			continue;
		}

		if(isLabel(aux)){
			// Set Label and PC to symbol table
			strcpy(mounter -> symbolLabel[ILC], aux);
			mounter -> symbolPC[ILC] = PC;
			sscanf(line,"%*s %s", aux);

			// Print label and PC if verbose
			if(mounter->verbose){
				printf("Label: %s \t- PC: %d\n", mounter->symbolLabel[ILC], mounter->symbolPC[ILC]);
			}

			// Increment ILC
			ILC++;
		}

		// Increment PC and read another line
		PC = PC + getNumberOfOperands(aux);
	}

	fclose(inputProgram);
}

void translateProgram(Mounter *mounter, char input[], char output[]){
	FILE *inputProgram, *outputProgram;
	char line[INSTRUCTION_SIZE], aux[INSTRUCTION_SIZE], *colon;
	int PC=1, reg;

	// Open a file to read and another to write
	inputProgram = fopen(input, "r");
    outputProgram = fopen(output, "w+");

    // Get first line from input
    
    
    // Read file until reach its end
    while(!feof(inputProgram)){
    	// Get line without comments
    	fgets(line, sizeof(line), inputProgram);
    	removeComments(line);

    	// Get instruction/label
    	strcpy(aux, "");
    	sscanf(line,"%s", aux);

    	if(isBreakLine(aux)){
			continue;
		}
    	
    	// Check if label
    	if(isLabel(aux)){
    		colon = strstr(line, ":") + 1;
            strcpy(line, colon);
            sscanf(line, "%s", aux);
    	}

    	// Compare to find the right instruction
    	// then print to file the instruction number and its operands

    	// LOAD
    	if(!strcasecmp(aux, "LOAD")){
			fprintf(outputProgram, "01\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Memory
			sscanf(line, "%*s %*s %s", aux);
            fprintf(outputProgram, "%02d\n", getLabelPC(mounter, aux, PC));
		}

		// STORE
		if(!strcasecmp(aux, "STORE")){
			fprintf(outputProgram, "02\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Memory
			sscanf(line, "%*s %*s %s", aux);
            fprintf(outputProgram, "%02d\n", getLabelPC(mounter, aux, PC));
		}

		// READ
		if(!strcasecmp(aux, "READ")){
            fprintf(outputProgram, "03\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
			reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
        }

        // WRITE
        if(!strcasecmp(aux, "WRITE")){
			fprintf(outputProgram, "04\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
			reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// COPY
		if(!strcasecmp(aux, "COPY")){
            fprintf(outputProgram, "05\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// NEG
		if(!strcasecmp(aux,"NEG")){
			fprintf(outputProgram, "06\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// SUB
		if(!strcasecmp(aux, "SUB")){
            fprintf(outputProgram, "07\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);


			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// ADD
		if(!strcasecmp(aux, "ADD")){
            fprintf(outputProgram, "08\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// AND
		if(!strcasecmp(aux, "AND")){
            fprintf(outputProgram, "09\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// OR
		if(!strcasecmp(aux, "OR")){
            fprintf(outputProgram, "10\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// XOR
		if(!strcasecmp(aux, "XOR")){
            fprintf(outputProgram, "11\n");
			PC = PC + 3;

			// Register
			sscanf(line, "%*s %s %*s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);

			// Register
            sscanf(line,"%*s %*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// NOT
		if(!strcasecmp(aux,"NOT")){
			fprintf(outputProgram, "12\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// JMP
		if(!strcasecmp(aux,"JMP")){
			fprintf(outputProgram, "13\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n",  getLabelPC(mounter, aux, PC));
		}

		// JZ
		if(!strcasecmp(aux,"JZ")){
			fprintf(outputProgram, "14\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n",  getLabelPC(mounter, aux, PC));
		}

		// JNZ
		if(!strcasecmp(aux,"JNZ")){
			fprintf(outputProgram, "15\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n",  getLabelPC(mounter, aux, PC));
		}

		// JN
		if(!strcasecmp(aux,"JN")){
			fprintf(outputProgram, "16\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n",  getLabelPC(mounter, aux, PC));
		}

		// JNN
		if(!strcasecmp(aux,"JNN")){
			fprintf(outputProgram, "17\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n",  getLabelPC(mounter, aux, PC));
		}

		// PUSH
		if(!strcasecmp(aux,"PUSH")){
			fprintf(outputProgram, "18\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// POP
		if(!strcasecmp(aux,"POP")){
			fprintf(outputProgram, "19\n");
			PC = PC + 2;

			// Register
			sscanf(line, "%*s %s", aux);
            reg = getRegister(aux);
			fprintf(outputProgram, "%02d\n", reg);
		}

		// CALL
		if(!strcasecmp(aux, "CALL")){
            fprintf(outputProgram, "20\n");
			PC = PC + 2;

			// Memory
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n", getLabelPC(mounter, aux, PC));
        }

        // RET
        if(!strcasecmp(aux, "RET")){
			fprintf(outputProgram, "21\n");
			PC++;
		}

		// HALT
        if(!strcasecmp(aux, "HALT")){
			fprintf(outputProgram, "22\n");
			PC++;
		}

    	// WORD
    	if(!strcasecmp(aux, "WORD")){
			sscanf(line, "%*s %s", aux);
			fprintf(outputProgram, "%02d\n", atoi(aux));
			PC++;
		}

		// END
        if(!strcasecmp(aux, "END")){
           break;
        }
    }

    fclose(inputProgram);
    fclose(outputProgram);
}