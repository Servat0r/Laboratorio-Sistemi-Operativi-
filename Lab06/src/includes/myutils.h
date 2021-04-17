#if !defined(_MYUTILS_H)

#define _MYUTILS_H

#if !defined(_BOOL_TYPE)
#define _BOOL_TYPE
typedef enum {false, true} bool;
#endif

//Ritorna una stringa contenente la linea appena letta, o NULL altrimenti
char* mygetline(char* buffer, char* prompt, size_t n);

//Ritorna il vettore argv[] estratto da buffer e ne scrive la lunghezza in *argc
int getcmd(char* buffer, char* argv[], size_t maxsize);

//Controlla se filename == "exit"
bool checkExit(char* filename);

//Controlla se filename == "cd"
bool checkChdir(char* filename);

//Controlla se il processo corrente deve andare in background
bool checkBkgr(int argc, char* argv[]);

char* getfilename(char* pathname);

#endif
