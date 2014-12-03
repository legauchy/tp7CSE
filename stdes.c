#include "stdes.h"

#define TAILLE 1024

// Structure de fichier
struct _ES_FICHIER {
	int fd;		// descripteur de fichier
	char mode;	// ouvert en lecture ou écriture
	char buffer[TAILLE];	// buffer d'écriture/lecture
	int index;	// indice de ce qui est lisible dans le buffer
	int count;	// indice de ce que l'on a déjà lu dans le buffer
};

/*struct _ES8_CELL {
	FICHIER *f;
	cell succ;
};*/

// Initialise la structure de fichier
FICHIER* ouvrir(char *nom, char mode) {
	FICHIER *file = malloc(sizeof(FICHIER));
	// Ouvre dans le bon mode le descripteur de fichier
	if (mode == 'E') {
		file->fd = open(nom, O_WRONLY);
		
	} else if (mode == 'L') {
		file->fd = open(nom, O_RDONLY);
		
	} else {
	
		return NULL;
	}
	
	if (file->fd != -1) {
		file->mode = mode;
		file->index = 0;
		file->count = 0;
		
		return file;
	}
	
	return NULL;
}

void closeAll() {
	
}

// Vide le buffer du fichier
int flush(FICHIER *f, int taille) {
	int somme = 0;
	while(somme != taille) {
		int ret = 0;
		ret = write(f->fd,  f->buffer + somme, taille - somme);
		if (ret == -1 && somme != 0) {
			return somme;
		} else if(ret == -1) {
			return -1;
		}
		somme += ret;
	}
	
	return somme;
}

// Ferme le fichier
int fermer(FICHIER *f) {
	if (f->mode == 'E') {
		flush(f, f->index);
	}
	
	return close(f->fd);
}

// Lit 'nbelem' éléments de taille 'taille' et écrit à l'adresse donnée 'p'
int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if (f->mode != 'L') {
		return -1;
	}
	int taille_totale_a_lire = nbelem * taille;
	int nb_elem_lu = 0;	// Taille lue retournée par l'appel à 'read'
	int decalage = 0;
	
	while (taille_totale_a_lire > f->index - f->count) {	// Tant qu'il faut lire plus que ce qui est disponible dans le buffer
		// Si le buffer est vide, on le remplit en lisant le fichier
		if (f->index - f->count == 0) {
			nb_elem_lu = read(f->fd, f->buffer, TAILLE);
			if (nb_elem_lu == -1) {	// Erreur
				return -1;
			} else if (nb_elem_lu == 0) {	// Fin de fichier
				return 0;
			}
			f->index = nb_elem_lu;
			f->count = 0;
		// Sinon on copie ce qui est lisible dans le buffer à l'adresse p en prenant en compte ce qui à pu déjà être lu auparavant
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
	
	// Retourne le nombre d'élément lus
	return decalage;
}

// Ecrit 'nbelem' éléments de taille 'taille' à l'adresse de 'p'
int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
	if (f->mode != 'E') {
		return -1;
	}
	int taille_totale_a_ecrire = nbelem * taille;
	int nb_elem_ecrit = 0;
	int decalage = 0;
	
	while(taille_totale_a_ecrire > TAILLE - f->index) {		// Tant que la place libre dans le buffer est plus petite que ce que l'on veut écrire
		// On rempli le buffer a son maximum avant de flush
		memcpy(f->buffer + f->index, ((char*) p) + decalage, TAILLE - f->index);
		int ret = flush(f, TAILLE);		// Flush du buffer
		if(ret == -1 && nb_elem_ecrit == 0) {	// Erreur
			return -1;
		} else if (ret == -1) {	// Erreur mais avec copie d'une partie du buffer
			return nb_elem_ecrit;
		}
		// Augmenter le decalage, restaurer l'index, reduire taille_totale_a_ecrire
		nb_elem_ecrit += ret;
		taille_totale_a_ecrire -= TAILLE - f->index;
		decalage += TAILLE - f->index;
		f->index = 0;
	}
	
	if (taille_totale_a_ecrire <= TAILLE - f->index) {	// Suffisament de place pour mettre dans le buffer d'écriture
		memcpy(f->buffer + f->index, ((char *)p)+decalage, taille_totale_a_ecrire);
		f->index += taille_totale_a_ecrire;
		nb_elem_ecrit += taille_totale_a_ecrire;
	}
	
	// Retourne le nombre d'élément lus
	return nb_elem_ecrit;
}

// Retourne un entier sous forme d'une chaine de caractère
// Trouvé sur internet
char* itoa(int val) {
	if(val == 0) {
		return "0";
	}
	static char buf[32] = {0};
	int i = 30;
	for(;val && i; --i, val /= 10){
		buf[i] = "0123456789"[val % 10];
	}
	
	return &buf[i+1];
}

// Ecrit dans le fichier les paramètres passés
int fecriref(FICHIER *f, char* format, ...) {
	if (f->mode != 'E') {
		return -1;
	}
	va_list va;
	va_start(va, format);
	int i;
	// Parcourt la chaine 'format' jusqu'à la fin
	for(i = 0 ; format[i] != '\0'; i++) {
		char c = format[i];
		// Si c'est un caractère spécial, on écrit ce qui est passé en paramètre
		if(c == '%') {
			i++;
			c = format[i];
			if(c == 'c') {
				char tmp = (char)va_arg(va, int);
				ecrire(&tmp, 1, 1, f);
			} else if (c == 's') {
				char* tmp = va_arg(va, char*);
				ecrire(tmp, 1, strlen(tmp), f);
			} else if (c == 'd') {
				char* tmp = itoa(va_arg(va, int));
				ecrire(tmp, 1, strlen(tmp), f);
			} else {
				return -1;
			}
			
		// Sinon on écrit la suite de la chaine dans le fichier
		} else {
			ecrire(format+i, 1, 1, f);
		}
		
	}
	va_end(va);
	
	return 0;
}


int fliref(FICHIER *f, char* format, ...) {
	if (f->mode != 'L') {
		return -1;
	}
	va_list va;
	va_start(va, format);
	int i;
	int ret = 0;
	int total_lu = 0;
	
	for(i = 0 ; format[i] != '\0'; i++) {
		char c = format[i];
		if(c == '%') {
			i++;
			c = format[i];
			if(c == 'c') {
				char * tmp = va_arg(va, char*);
				ret = lire(tmp,1,1,f);
				if(ret == -1) {
					return ret;
				}else{
					total_lu += ret;
				}
			} else if (c == 's') {
				char* tmp = va_arg(va, char*);
				do {
					ret = lire(tmp,1,1,f);
					if (ret <= 0) {
						return -1;
					}
					printf("[tmp] %c %i\n", *tmp, ret);
					
					if(tmp[0] == ' ' || tmp[0] == '\n' || tmp[0] == EOF) {
						i++;
						if (*tmp != format[i] && ret != 0 && i < strlen(format)) {
							printf("Bad format j'attend %i j'ai %i \n",format[i],*tmp);
							return -1;
						}
						tmp[0] = '\0';
					}
					
					total_lu += ret;
					
					tmp++;
				} while(tmp[-1] != '\0');
			} else if (c == 'd') {
				
				int* tmp = va_arg(va, int*);
				char nb[30];
				int j = 0;
				char* digit = malloc(sizeof(char));
				do {
					ret = lire(digit, 1, 1, f);
					if (ret <= 0) {
						return -1;
					}
					printf("[digit] %c\n",*digit);
					if ((int)*digit >= 48 && (int)*digit <= 57) {
						nb[j] = *digit;
						j++;
					}else{
						nb[j]='\0';
						j++;
						if(*digit == ' ' || *digit == '\n' || *digit == EOF) {
							i++;
							if (*digit != format[i] && ret != 0 && i < strlen(format)) {
								printf("Je passe ici \n");
								printf("Bad format j'attend %i j'ai %i \n",format[i],*tmp);
								return -1;
							}
						}else {
							printf("Je passe la \n");
							f->count--;
						}	
					}
				} while(nb[j-1] != '\0');
				
				*tmp = atoi(nb);
				free(digit);
			} else {
				return -1;
			}
			
		} else {
			char * tmp = malloc(sizeof(char));
			ret = lire(tmp, 1, 1, f);
			if(*tmp != format[i] && ret != 0) {
				printf("Bad format j'attend %c j'ai %c \n",format[i],*tmp);
				return -1;
			}
			free(tmp);
		}
	}
	
	return total_lu;
}

