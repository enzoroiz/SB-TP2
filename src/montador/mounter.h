#define TABLE_SIZE 200
#define INSTRUCTION_SIZE 100
#define NULL_INT -1
#define NULL_CHAR '\0'
#define TRUE 1
#define FALSE 0

typedef struct Mounter {
	int symbolPC[TABLE_SIZE]; // Armazena o Program Counter
	char symbolLabel[TABLE_SIZE][INSTRUCTION_SIZE]; // Armazena a label
	int verbose; // Armazena se verboso
} Mounter;

void createMounter(int argc, char *argv[], Mounter *mounter); // Inicializa montador com argumentos passados por linha de comando

void createSymbolTable(Mounter *mounter, char input[]); // Executa o primeiro passo da tradução: Cria tabela de símbolos

int isLabel(char aux[]); // Verifica se instrução é ou não label

int isBreakLine(char aux[]); // Verifica se é quebra de linha ou linha vazia

int getNumberOfOperands(char aux[]); // Retorna o número de operandos da instrução passada por parâmetro

void translateProgram(Mounter *mounter, char input[], char output[]); // Executa o segundo passo da tradução: Tradução propriamente dita

int getLabelPC(Mounter *mounter, char label[], int PC); // Retorna o PC da label passada por parâmetro

void removeComments(char aux[]); // Remove os comentários se existentes

int getRegister(char reg[]); // Retorna o registrador em forma de número