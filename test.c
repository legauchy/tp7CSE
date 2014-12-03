#include "stdes.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FICHIER *f1;
	FICHIER *f2;
	char ch1, ch2;
	char c1[10], c2[10];
	int i1, i2;
	
	if (argc != 3)
		exit(-1);
	
	f1 = ouvrir (argv[1], 'L');
	if (f1 == NULL)
		exit (-1);
	
	f2 = ouvrir (argv[2], 'E');
	if (f2 == NULL)
		exit (-1);
	
	/*while (lire (&c, 1, 1, f1) == 1) {
		ecrire (&c, 1, 1, f2);
	}*/
	
	while (fliref(f1,"%c%d %s %c%d", &ch1, &i1, &c1, &ch2, &i2) > 0) {
		printf("%c %d %s %c %d", ch1, i1, c1, ch2, i2);
		//ecrire (&c, 1, 1, f2);
	}
    /*ecrire("SA MARCHE!\n",1,11,f2);
	fecriref(f2, "SA %d MARCHE!",450);*/
	
	fermer (f1);
	fermer (f2);
	
	return 0;
}
