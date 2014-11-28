#include "stdes.h"

#define TAILLE 64

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

int flush(FICHIER *f, int taille) {
	int somme = 0;
	while(somme != taille) {
		int ret = 0;
		ret = write( f->fd,  f->buffer + somme, taille - somme);
		if(ret == -1) {
			return ret;
		}
		somme += ret;
	}
	
	return somme;
}

int fermer(FICHIER* f) {
	if(f->mode == 'E') {
		flush(f, f->index);
	}
	
	return close(f->fd);
}

int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if(f->mode != 'L') {
		return -1;
	}
	int taille_totale_a_lire = nbelem * taille;
	int nb_elem_lu = 0;
	int decalage = 0;
	
	while (taille_totale_a_lire > f->index - f->count) {
		// /!\ Il faut lire et mettre dans le buffer avant la premiere lecture
		if (f->index - f->count == 0) {
			nb_elem_lu = read(f->fd, f->buffer, TAILLE);
			if (nb_elem_lu == -1) {	// Erreur
				return -1;
			} else if (nb_elem_lu == 0) {	// Fin de fichier
				return 0;
			}
			f->index = nb_elem_lu;
			f->count = 0;
		} else {
			memcpy(p+decalage, f->buffer + f->count, f->index - f->count);
			taille_totale_a_lire -= f->index - f->count;
			decalage += f->index - f->count;
			f->count = f->index;
		}
	}
	
	if (taille_totale_a_lire <= f->index - f->count) {
		memcpy(p, f->buffer + f->count, taille_totale_a_lire);
		f->count += taille_totale_a_lire;
		decalage += taille_totale_a_lire;
	}
	
	return decalage;
}

int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if(f->mode == 'E') {
		int decalage = 0;
		int taille_totale = nbelem * taille;
		
		while(taille_totale > TAILLE - f->index) {
			//Remplir le buffer a max
			memcpy(f->buffer+f->index, ((char *)p)+decalage, TAILLE - f->index);
			//Flush
			if(flush(f, TAILLE) == -1 ){
				return -1;
			}
			//Augmenter le decalage, restaurer l'index, reduire taille_totale
			taille_totale -= TAILLE - f->index;
			decalage += TAILLE - f->index;
			f->index = 0;
		}
		if(taille_totale <= TAILLE - f->index) {//suffisament de place pour mettre dans le buffer d'écriture
			memcpy(f->buffer + f->index, p, taille_totale);		// /!\ demander si memcpy merde
			f->index += taille_totale;
		}
		
		return nbelem * taille; // !!!! Penser a demander
	}
	
	return -1;
}






