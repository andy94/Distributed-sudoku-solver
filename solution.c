/****************************************************************************
* Tema  : 3 - APD                                                           *
* Autor : Andrei Ursache                                                    *
* Grupa : 332 CA                                                            *
* Data  : 30.12.2015                                                        *
*****************************************************************************/

#include "solution.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Initializeaza o solutie */
void init_solution(Solution **sol){
	int i;	
	*sol = (Solution*)malloc(sizeof(*sol));
	(*sol)->sol = (char**)malloc((sizeof(char*)*dimp));
	for(i = 0 ; i < dimp ; ++i){
		(*sol)->sol[i] = (char *)malloc(sizeof(char)*dimp);
	}
	(*sol)->sqr = (char**)malloc((sizeof(char*)*dim));
	for(i = 0 ; i < dim ; ++i){
		(*sol)->sqr[i] = (char *)malloc(sizeof(char)*dim);
	}
	(*sol)->next = NULL;
}

/* Dezaloca o solutie */
void free_solution(Solution *sol){
	int i;	
	for(i = 0 ; i < dimp ; ++i){
		free(sol->sol[i]);
	}
	free(sol->sol);
	for(i = 0 ; i < dim ; ++i){
		free(sol->sqr[i]);
	}
	free(sol->sqr);
}

/* Elimina urmatoarea solutie din lista*/
void delete_next_solution(Solution **sol){
	if( (*sol) != NULL && (*sol)->next != NULL){
		Solution *aux = (*sol)->next;
		(*sol)->next = ((*sol)->next)->next;
		free_solution(aux);
	}
}

/* Adauga la final o solutie */
void add_last_sol(Solution *solution, char sol[dimp][dimp], char sqr[dim][dim]){
	while(solution->next != NULL){
		solution = solution->next;
	}
	int i,j;
	for(i = 0 ; i < dimp ; ++i){
		for(j = 0 ; j < dimp ; ++j){
			solution->sol[i][j] = sol[i][j];
		}
	}
	for(i = 0 ; i < dim ; ++i){
		for(j = 0 ; j < dim ; ++j){
			solution->sqr[i][j] = sqr[i][j];
		}
	}
	init_solution(&(solution->next));
}

/* Afiseaza solutia */
void print_sol(Solution *solution){
	int nr = own_sol;
	while(solution->next != NULL){
		printf("Solution %d:\n",own_sol-nr+1);
		nr--;
		int i,j;
		for(i = 0 ; i < dimp ; ++i){
			for(j = 0 ; j < dimp ; ++j){
				printf("%d ", solution->sol[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		
		solution = solution->next;
	}
}

/* Afiseaza o solutie */
void print_recv_sol(char sol[dimp][dimp], char sqr[dim][dim]){
		int i,j;
		for(i = 0 ; i < dimp ; ++i){
			for(j = 0 ; j < dimp ; ++j){
				printf("%d ",sol[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		for(i = 0 ; i < dim ; ++i){
			for(j = 0 ; j < dim ; ++j){
				printf("%d ", sqr[i][j]);
			}
			printf("\n");
		}
		printf("\n");
}

/* Afiseaza prima solutie in fisier */
void print_first_sol_to_file(FILE *out,Solution *sol){
	int i, j;
	
	for(i = 0 ; i < dimp ; ++i){
		for(j = 0 ; j < dimp ; ++j){
			
			fprintf(out, "%d ", sol->sol[i][j]);
		}
		fprintf(out, "\n");
	}

}

/* Verifica daca valoarea lui (i,j) se ai afla in patrat */
int check(char square[dim][dim], int i,int j){
	int n, m;
	for(m = 0 ; m < dim ; ++m){
		for(n = 0 ; n < dim; ++n){
			if((m != i || n != j) && square[m][n] == square[i][j]){
				return 0;
			}
		}
	}
	return 1;
}

/* Calculeaza urmatoarea pozitie */
void get_next_pos(char square[dim][dim], int *i, int *j){
	
	if(*i >= dim){
		*i = -1;
		*j = -1;
		return;
	}
	
	for(;*j<dim;++(*j)){
		if(square[*i][*j] == 0){
			return;
		}
	}
	
	(*i)++;
	for (; *i < dim ; ++(*i)){
		for(*j = 0 ; *j < dim ; ++(*j)){
			if(square[*i][*j] == 0){
				return;
			}
		}
	}
	*i = -1;
	*j = -1;
}

/* Genereaza toate solutiile avand pentru fiecare variabila un domeniu */
void generate_all_sols_domain(Solution *solution, 
	char initial_sudoku_matrix[dimp][dimp], char square[dim][dim], int i, int j, 
	char domain[dim][dim][dimp],int indexi, int indexj){
									
	if(i == -1 && j == -1){ // Finished
		int m,n;
		char sol[dimp][dimp];
		char sqr[dim][dim];
		
		for(m = 0 ; m < dimp ; ++m){
			for(n = 0 ; n < dimp ; ++n){
				sol[m][n] = initial_sudoku_matrix[m][n];
			}
		}
		
		for(m = 0 ; m < dim ; ++m){
			for(n = 0 ; n < dim ; ++n){
				sol[m+indexi][n+indexj] = square[m][n];
			}
		}
		
		
		for(m = 0 ; m < dim ; ++m){
			for(n = 0 ; n < dim ; ++n){
				sqr[m][n] = 0;
			}
		}
		
		sqr[indexi/dim][indexj/dim] = 1;
		
		/* Adauga solutia in lista */
		add_last_sol(solution, sol, sqr);
		
		own_sol++;
	}

	else{ // Daca mai sunt locuri libere 
	
		int new_i = i;
		int new_j = j;
	
		/* Calculeaza urmatorii indecsi */
		new_j++;
		if(new_j == dim){
			new_j = 0;
			new_i++;
		}
		get_next_pos(square, &new_i,&new_j);
		
		int val;
		if(square[i][j] == 0){ // spatiu liber
			for(val = 1 ; val <= dimp ; ++val){
				if(domain[i][j][val-1]==0){ // am gasit o valoare din domeniu
				
					square[i][j] = val;
					if(check(square,i,j)){ // daca e ok valoarea
						/* Genereaza psibilitatile */
						generate_all_sols_domain(solution,initial_sudoku_matrix, 
							square,new_i,new_j, domain,indexi,indexj);
					}
				}
			}
			square[i][j] = 0;
		}
		else{ // daca e setat locul deja
			generate_all_sols_domain(solution,initial_sudoku_matrix, square,
				new_i,new_j, domain,indexi,indexj);
		}	
	}
}


/* Genereaza toate solutiile partiale pentru patratul meu */
void compute_all_solutions_for_my_square(Solution *solution, 
			char initial_sudoku_matrix[dimp][dimp], int indexi, int indexj){

	char domain[dim][dim][dimp];
	int i,j,k,l;
	
	for(i = 0 ; i < dim ; i++){
		for(j = 0 ; j < dim ; ++j){
			for(k = 0 ; k < dimp ; k ++){
				domain[i][j][k] = 0;
			}
		}
	}
	
	int nr = 0;
	
	
	/* Restrange domeniul */
	for(i = indexi ; i < indexi + dim ; ++i){
		for( j = indexj ; j < indexj +dim ; ++j){ //pentru fiecare element liber
			
			if(initial_sudoku_matrix[i][j] == 0){
			
				nr++;				
				for(k = 0 ; k < dimp ; ++k){ // linie
					if(initial_sudoku_matrix[i][k] != 0){
						domain[i - indexi][j - indexj][initial_sudoku_matrix[i][k]-1] = 1;
					}
				}
				for(k = 0 ; k < dimp ; ++k){ // coloana
					if(initial_sudoku_matrix[k][j] != 0){
						domain[i - indexi][j - indexj][initial_sudoku_matrix[k][j]-1] = 1;
					}
				}
			}
			else{ // daca e numar fixat
				for(k = 0 ; k < dim ; ++k){
					for(l = 0 ; l < dim ; ++l){
					domain[k][l][initial_sudoku_matrix[i][j]-1] = 1;
					}
				}
			}
		}
	}

	char square[dim][dim];
	for(i = 0 ; i < dim ; ++i){
		for(j = 0 ; j < dim ; ++j){
			
			square[i][j] = initial_sudoku_matrix[indexi+i][indexj+j];
			
		}
	}
	
	/* Acum ai domeniul, genereaza solutiile */
	generate_all_sols_domain(solution,initial_sudoku_matrix, square, 0, 0, 
							domain,indexi,indexj);
}

/* Citeste vecinii din fisier */
void read_neighbors_from_file(FILE *topology_file, int rank, int numtasks, 
								char *neighbors, int *length){
	int current = -1;
	char *line = NULL;
	size_t len;
	
	while(current != rank){
		if(fscanf(topology_file, "%d", &current) <= 0){
			return;
		}
		
		if(current == rank){
			fseek(topology_file, 3, SEEK_CUR);
			getline(&line, &len, topology_file);
			char s[4] = " \n";
	   		char *token = strtok(line, s);
	   		while( token != NULL ){
				neighbors[(char) atoi(token)] = 1;
				(*length)++;
				token = strtok(NULL, s);
			}
		}
		else{
			getline(&line, &len, topology_file);
		}
	}
}

/* Calculeaza pozitia in patrat */
void get_pos(char squares_stat[dim][dim], int *posi, int *posj){
	int i,j;
	*posi = *posj = -1;
	for (i = 0 ; i < dim ; ++i){
		for(j = 0 ; j < dim ; ++j){
			if(squares_stat[i][j] == 1){
				*posi = i;
				*posj = j;
				return;
			}
		}
	}
}

/* Aloca spatiu pentru o cale */
void get_square_stat_branch(char squares_stat[dim][dim],char square_stat_branch[dim][dim],int *last_posi,int *last_posj, int num){
	
	int i,j;
	for(i = 0 ; i < dim ; ++i){
		for(j = 0 ; j < dim ; ++j){
			square_stat_branch[i][j] = 0;
		}
	}
	(*last_posj)++;
	if(*last_posj == dim){
		(*last_posj) = 0;
		(*last_posi)++;
	}
	if(num > 0)
	for(i = *last_posi ; i< dim ; ++i){
		for(j = (i== *last_posi ) ? *last_posj :0 ; j < dim ; ++j){
			
			square_stat_branch[i][j] = 1;
			
			num--;
			
			if(num == 0){
				*last_posi = i;
				*last_posj = j;
				return;
			}
		}
	}
}

/* Combina doua soluti */
void merge_sol(char sol_r[dimp][dimp], char** sol, char sol_res[dimp][dimp], 
				char sqr_r[dim][dim], char** sqr, char sqr_res[dim][dim]){

	int i, j, m, n;
	
	for(i = 0 ; i < dim ; ++i){
		for(j = 0 ; j < dim ; ++j){
		
			if(sqr[i][j]){ // daca e in prima 
				for(m = 0; m < dim ; ++m){
					for(n = 0 ; n < dim ; ++n){
						sol_res[i*dim + m][j*dim + n] = sol[i*dim + m][j*dim + n];
					}
				}
				
				sqr_res[i][j] = 1;
			}
			else if(sqr_r[i][j]){ // daca e in a doua 
				for(m = 0 ; m < dim ; ++m){
					for(n = 0 ; n < dim ; ++n){
						sol_res[i*dim + m][j*dim + n] = sol_r[i*dim + m][j*dim + n];
					}
				}
			
				sqr_res[i][j] = 1;
			}
			else{ // daca e "neatins"
				for(m = 0; m < dim ; ++m){
					for(n = 0 ; n < dim ; ++n){
						sol_res[i*dim + m][j*dim + n] = sol[i*dim + m][j*dim + n];
					}
				}			
				sqr_res[i][j] = 0;
			}
		}
	}
}

/* Verifica daca e ok solutia */
int check_sol(char sol_res[dimp][dimp], char sqr_res[dim][dim]){

	int i, j, k;
	int vals[dimp];
	
	/* Pe linii */	
	for(i = 0 ; i < dimp ; ++i){
		for(k = 0 ; k < dimp ; ++k){
			vals[k] = 0;
		}
		for(j = 0 ; j < dimp ; ++j){
			if(sol_res[i][j] != 0){
				if(vals[sol_res[i][j]-1]){
					return 0;
				}
				else{
					vals[sol_res[i][j]-1] = 1;
				}
			}
		}
	}
	
	/* Pe coloane */
	for(j = 0 ; j < dimp ; ++j){
		for(k = 0 ; k < dimp ; ++k){
			vals[k] = 0;
		}
		for(i = 0 ; i < dimp ; ++i){
			if(sol_res[i][j] != 0){
				if(vals[sol_res[i][j]-1]){
					return 0;
				}
				else{
					vals[sol_res[i][j]-1] = 1;
				}
			}
		}
	}
	
	/* In interiorul patratului e deja ok */
	
	return 1;
}

/* Combina o solutie primita cu cele existente */
void combine(Solution **my_solution, Solution **solution, char sol_r[dimp][dimp],char sqr_r[dim][dim], int *nr){
	
	Solution *itsol = *my_solution;
	
	*nr = 0;

	char sol_res[dimp][dimp];
	char sqr_res[dim][dim];
	
	char **sol;
	char **sqr;
	
	/* Pentru fiecare solutie existenta in parte */
	while(itsol->next != NULL){
	
		sol = itsol->sol;
		sqr = itsol->sqr;
		
		/* Combin-o cu cea primita */
		merge_sol(sol_r,sol,sol_res,sqr_r,sqr,sqr_res);
		
		if(check_sol(sol_res,sqr_res) == 1){
			/* Daca e ok, formeaza una noua */
			add_last_sol(*solution, sol_res, sqr_res);
			(*nr)++;
		}
		itsol = itsol->next;
	}
}

