#include <stdio.h>

int main(int argc, char **argv)
{
	extern int encrypt (const char *filename);
	extern int decrypt (const char *filename);
	extern int prglogo (void);

	char encryptFlag = 0;
	char decryptFlag = 0;

	if (argc < 2 || argc > 3) {
		prglogo ();
		printf ("Usage: %s <inputfile>\n"
			"=======================================\n"
			"%s [-e or -d] - e for enc\n"
			"d is for decrypting.\n\n"
			"but only putting two args will\n"
			"result in that file being encrypted.\n"
			"=======================================\n", argv[0],
			argv[0]);
		return 0;
	}	

	prglogo ();

	if (argc == 2) {
		printf ("Encrypting file %s...\n", argv[1]);
		if (encrypt (argv[1]) == 0) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else {
		if ((argv[1][0] == '-') && (argv[1][1] == 'e')) {
			encryptFlag = 1;
		}
		else {
			if ((argv[1][0] == '-') && (argv[1][1] == 'd')) {
				decryptFlag = 1;
			}
		}
	
		if (encryptFlag) {
			printf ("Encrypting file %s...\n", argv[2]);
			if (encrypt (argv[2]) == 0) {
				return 0;
			}
			else {
				return 1;
			}
		}
		if (decryptFlag) {
			printf ("Decrypting file %s...\n", argv[2]);
			if (decrypt (argv[2]) == 0) {
				return 0;
			}
			else {
				return 1;
			}
		}
	}

	return 0;
}

