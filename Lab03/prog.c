char opt;
char* optarg;
char optopt;

int getopt(int, char**, char*);

int printf(char*,...);


int main(int argc, char* argv[]){
	int opt;
	while ((opt = getopt(argc, argv, "n:m:h")) != -1) {
		switch(opt){
			case 'n' :
			case 'm' : printf("l'argomento di '-%c' è '%s'\n",opt,optarg); break;
			case 'h' : printf("'-h' non ha argomenti\n"); break;
			case ':' : printf("'-%c' richiede un argomento\n",optopt); break;
			case '?' : printf("'-%c' non è riconosciuta\n", optopt);
			default:;
			
		}
	}
	return 0;
}
