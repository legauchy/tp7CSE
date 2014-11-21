#include "stdes.h"

#define TAILLE 1024

struct _ES_FICHIER {
	int fd;
	char mode;
	char buffer[TAILLE];
	int index;
	int count;
};

FICHIER* ouvrir(char *nom, char mode) {
	FICHIER* file = malloc(sizeof(FICHIER));
	if(mode == 'E') {
		file->fd = open(nom,O_WRONLY);
		
	}else if (mode == 'L') {
		file->fd = open(nom,O_RDONLY);
	} else{
	
		return NULL;
	}
	
	if(file->fd != -1) {
		file->mode = mode;
		file->index = 0;
		file->fin = 0;
		
		return file;
	}
	
	return NULL;
}

int fermer(FICHIER* f) {
	if(f->mode == 'E') {
		/** 
		TODO flush
		 */
	}
	
	return close(f->fd);
}

int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if(f->mode != 'L') {
		return -1;
	}
	int elem_lu = 0;
	
	do  {
		if(count - index < nbelem*taile ) {
			int return_read = read(f->fd, (void*)(f->buffer + count), TAILLE - count);
			if(return_read == -1) {//ERREUR
				return -1;
			}else if(return_read == 0  && count - index == 0) {//On a atteint EOF et on a plus rien a donner dans le buffer
				return 0;
			}
			count += return_read;
		}
		
		if(index + taille*nbelem < TAILLE) {
		
		} else {
		
		}
		
	}while(f->index == 0);	
	
	return elem_lu;
}

int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if(f->mode == 'E') {
		int i;
		for(i = 0 ; i < taille * nbelem ; i++) {
			f->buffer[f->index] = *((char*)p);
			p++;
			f->index ++; 
		}
		
		
	}
	
	return -1;
}






