/****************************************************************************
* Tema  : 3 - APD                                                           *
* Autor : Andrei Ursache                                                    *
* Grupa : 332 CA                                                            *
* Data  : 30.12.2015                                                        *
*****************************************************************************/

#ifndef _SOLUTION_H_
#define _SOLUTION_H_

#define LINE_IN_DIM 100
#define NAME_MAX 10

#include <stdio.h>

/* Dimensiuni joc */
extern int dim, dimp, dimpp, own_sol;

/* Structura specifica unei solutii 
 * Lista de solutii */
typedef struct Solution{
	/* Matricea cu valorile jocului */
	char **sol;
	
	/* O matrice de 0 si 1 de dim x dim
	 * care arata pentru ce patrate este solutia */
	char **sqr;
	
	/* Urmatoarea solutie */
	struct Solution *next;
}Solution;

/* Functii specifice unei solutii */
void init_solution(Solution **sol);
void free_solution(Solution *sol);
void delete_next_solution(Solution **sol);
void add_last_sol(Solution *solution, char sol[dimp][dimp], char sqr[dim][dim]);
void print_sol(Solution *solution);
void print_first_sol_to_file(FILE *out,Solution *sol);

/* Functii auxiliare */
void print_recv_sol(char sol[dimp][dimp], char sqr[dim][dim]);
void read_neighbors_from_file(FILE *topology_file, int rank, int numtasks, 
								char *neighbors, int *length);

/* Functii pentru determinarea solutiilor din joc */
int check(char square[dim][dim], int i,int j);
void get_next_pos(char square[dim][dim], int *i, int *j);
void generate_all_sols_domain(Solution *solution, 
				char initial_sudoku_matrix[dimp][dimp], char square[dim][dim], 
				int i, int j, char domain[dim][dim][dimp],int indexi, int indexj);
void compute_all_solutions_for_my_square(Solution *solution, 
				char initial_sudoku_matrix[dimp][dimp], int indexi, int indexj);
void get_pos(char squares_stat[dim][dim], int *posi, int *posj);
void get_square_stat_branch(char squares_stat[dim][dim],
				char square_stat_branch[dim][dim],int *last_posi,int *last_posj, 
				int num);
void merge_sol(char sol_r[dimp][dimp], char** sol, char sol_res[dimp][dimp], 
				char sqr_r[dim][dim], char** sqr, char sqr_res[dim][dim]);
int check_sol(char sol_res[dimp][dimp], char sqr_res[dim][dim]);
void combine(Solution **my_solution, Solution **solution, 
				char sol_r[dimp][dimp],char sqr_r[dim][dim], int *nr);

#endif
