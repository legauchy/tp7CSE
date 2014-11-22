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
		file->count = 0;
		
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
		if(f->count - f->index < nbelem*taille ) {
			int return_read = read(f->fd, (void*)(f->buffer + f->count), TAILLE - f->count);
			if(return_read == -1) {//ERREUR
				return -1;
			}else if(return_read == 0  && f->count - f->index == 0) {//On a atteint EOF et on a plus rien a donner dans le buffer
				return 0;
			}
			f->count += return_read;
		}
		
		if(f->index + taille*nbelem < TAILLE) {
		
		} else {
		
		}
		
	}while(f->index == 0);	
	
	return elem_lu;
}

int flush(FICHIER *f) {

	return 0;
}

int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if(f->mode == 'E') {
		int decalage = 0;
		int taille_totale = nbelem * taille;
		
		while(taille_totale > TAILLE - f->index) {
			//Remplir le buffer a max
			memcpy(f->buffer+f->index, ((char *)p)+decalage, TAILLE - f->index);
			//Flush
			if(flush(f) == -1 ){
				return -1;
			}
			//Augmenter le decalage, restaurer l'index, reduire taille_totale
			taille_totale -= TAILLE - f->index;
			decalage += TAILLE - f->index;
			f->index = 0;
		}
		if(taille_totale < TAILLE - f->index) {//suffisament de place pour mettre dans le buffer d'écriture
			memcpy(f->buffer + f->index, p, taille_totale);
		
		}
		
	}
	
	return -1;
}






