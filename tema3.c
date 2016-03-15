/****************************************************************************
* Tema  : 3 - APD                                                           *
* Autor : Andrei Ursache                                                    *
* Grupa : 332 CA                                                            *
* Data  : 30.12.2015                                                        *
*****************************************************************************/

#include "mpi.h"

#include "solution.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Dimensiunile jocului si numarul de solutii */
int dim = 0, dimp = 0, dimpp=0, own_sol = 0;

/* Main ********************************************************************/
main(int argc, char *argv[]) {

	/* Declarations ********************************************************/
	
	int numtasks, rank, tag = 1;
	MPI_Status Stat;

	int i, j, k, m, n;
	int val;
	int parent=-2;

	FILE *topology_file, *sudoku_file, *output_file;

	/* MPI Initialization **************************************************/
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	char adj_matrix[numtasks][numtasks];
	char adj_matrix_received[numtasks][numtasks];
	char neighbors[numtasks];
	for(i = 0 ; i < numtasks ; ++i){
		neighbors[i]=0;
	}
	int nr=0;
	
	/* Check arguments */
	if(argc != 4){
		if(rank == 0){
			printf("Wrong Arguments!\nmpirun -np 10 ./tema3 <fisier_topologie");
			printf("> <fisier_intrare_sudoku> <fisier_output_sudoku>\n");
		 }
		 
		 MPI_Finalize();
		 return 0;
	}
	
	/* Read topology */
	topology_file = fopen(argv[1],"rt");
	if(topology_file == NULL){
		if(rank == 0){
			printf("Wrong <fisier_topologie>!\n");
		 }
		 MPI_Finalize();
		 return 0;
	}
	
	/* Citeste vecinii din fisier */
	read_neighbors_from_file(topology_file, rank, numtasks, neighbors, &nr);
	fclose(topology_file);
	
	/* Write info into specific process file */
	char proc_route_filename[NAME_MAX] = "proc_";
	char rank_name[NAME_MAX];
	sprintf(rank_name,"%d",rank);
	strcat(proc_route_filename,rank_name);
	
	FILE *proc_file = fopen(proc_route_filename,"w+");
	
	if(proc_file == NULL){
		 printf("Wrong <%s>!\n", proc_route_filename);
		 MPI_Finalize();
		 return 0;
	}
	
	fprintf(proc_file,"%d\n\n",rank);
	
	fprintf(proc_file,"[initial topology]:\n");
	for(i = 0 ; i < numtasks ; ++i){
		fprintf(proc_file, "%d ",neighbors[i]);
	}
	fprintf(proc_file,"\n\n");
	
	if(neighbors[rank]){
		neighbors[rank] = 0;
		nr--;
	}
	
	/* Sondaj & Ecou - Stabilire topologie ********************************/
	/* val = 1 -> sondaj
	 * val = 2 -> ecou */
	
	char routing_table[numtasks];
	char routing_table_received[numtasks];
	
	/* Init adj_matrix */
	for(m = 0 ; m < numtasks ; ++m){
		if(m == rank){
			for(n = 0 ; n < numtasks ; ++n){
				adj_matrix[m][n] = neighbors[n];
			}
		}
		else{
			for(n = 0 ; n < numtasks ; ++n){
				adj_matrix[m][n] = 0;
			}
		}
	}
		
	if(rank == 0){ // Root
		val = 1;
		for(i = 0 ; i < numtasks ; ++i){
			if(neighbors[i]){
				MPI_Send(&val, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			}
		}
		parent = -1;
		/* Init routing table */
		for(i = 0 ; i < numtasks ; ++i){
			routing_table[i] = -2;
		}
		
		routing_table[rank] = -1;
	}
	
	int len = nr;
	/* receive sondaj / ecou */
	for(k = 0 ; k < len; ++k){ 
			
		MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE , tag, MPI_COMM_WORLD,&Stat);
			
		if(val == 1){ // sondaj
			
			if(parent == -2){ // first parent
				
				parent = Stat.MPI_SOURCE;

				/* Send sondaj to kids */
				val = 1;
				for(i = 0 ; i < numtasks ; ++i){
					if(neighbors[i] && i != parent){
						MPI_Send(&val, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
					}
				}
				
				/* Init routing table */
				for(i = 0 ; i < numtasks ; ++i){
					routing_table[i] = parent;
				}
				routing_table[rank] = -1;
			}
			else { // bucla
				
				// Il pot sterge aici pentru ca deja a trimis catre celalalt,
				// deci il va sterge si el. De ce? pentru ca e al doilea tata,
				// mesajele se trimit doar la primul (adica inainte de asta)
				neighbors[Stat.MPI_SOURCE] = 0;
				nr--;
				adj_matrix[rank][Stat.MPI_SOURCE] = 0;
			}
		}
		else if(val == 2){ // ecou
 		
			/* Receive routing table and adj matrix */
			MPI_Recv(&routing_table_received, numtasks, MPI_CHAR, 
								Stat.MPI_SOURCE , tag, MPI_COMM_WORLD,&Stat);
			MPI_Recv(&adj_matrix_received, numtasks*numtasks, MPI_CHAR, 
								Stat.MPI_SOURCE , tag, MPI_COMM_WORLD,&Stat);
			
			for(j = 0 ; j < numtasks ; ++j){
				if(routing_table_received[j] != rank && 
					routing_table_received[j] != -1){
					
					routing_table[j] = Stat.MPI_SOURCE;
				}
			}
			routing_table[Stat.MPI_SOURCE] = Stat.MPI_SOURCE;
			
			for(m = 0 ; m < numtasks ; ++m){
				for(n = 0 ; n < numtasks ; ++n){
					adj_matrix[m][n] |= adj_matrix_received[m][n];
				}
			}
		}
	
	} // end receive sondaj / ecou
	
	
	if(rank != 0){
		val = 2;
		MPI_Send(&val, 1, MPI_INT, parent, tag, MPI_COMM_WORLD);
		
		/* Send routing table and adj matrix */
		MPI_Send(&routing_table, numtasks, MPI_CHAR, parent, tag, 
										MPI_COMM_WORLD);
		MPI_Send(&adj_matrix, numtasks*numtasks, MPI_CHAR, parent, tag, 
										MPI_COMM_WORLD);
	}


	
	/* Print Info node ****************************************************/
	
	fprintf(proc_file,"[actual topology]:\n");
	for(i = 0 ; i < numtasks ; ++i){
		fprintf(proc_file, "%d ",neighbors[i]);
	}
	fprintf(proc_file,"\n\n");
	
	fprintf(proc_file,"[nr neighbors]:\n");
	fprintf(proc_file,"%d\n",nr);
	
	fprintf(proc_file,"\n[parent]:\n");
	fprintf(proc_file,"%d\n",parent);
	
	if(rank == 0){
		fprintf(proc_file,"\n[adj matrix]:\n");
		for(m = 0 ; m < numtasks ; ++m){
			for(n = 0 ; n < numtasks ; ++n){
				fprintf(proc_file,"%d ",adj_matrix[m][n]);
			}
			fprintf(proc_file,"\n");
		}
		
	}
	
	fprintf(proc_file,"\n[routing table]:\n");
	for(i = 0; i < numtasks ; ++i){
		fprintf(proc_file,"%d\t->\t%d\n",i,routing_table[i]);
	}
	fprintf(proc_file,"\n");
	
	int num_nodes[numtasks];
	for(i = 0 ; i < numtasks ; ++i){
		int num = 0;
		for(j = 0 ; j < numtasks ; ++j){
			
			if(routing_table[j] == i){
				num++;
			}	
		}
		num_nodes[i]=num;
	}
	
	fprintf(proc_file,"[num nodes]:\n");
	for(i = 0; i < numtasks ; ++i){
		fprintf(proc_file,"%d ",num_nodes[i]);
	}
	fprintf(proc_file,"\n");
	
	fclose(proc_file);
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
		printf("Topology done ------------------- \n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* Read game configuration ********************************************/
	sudoku_file = fopen(argv[2],"rt");
	if(sudoku_file == NULL){
		if(rank == 0){
			printf("Wrong <fisier_intrare_sudoku>!\n");
		 }
		 MPI_Finalize();
		 return 0;
	}
	
	fscanf(sudoku_file, "%d", &dim);
	dimp = dim * dim;
	dimpp = dimp*dimp;
	
	if(dimp != numtasks){
		if(rank == 0){
			printf("Wrong numthreads!\n");
		 }
		 MPI_Finalize();
		 return 0;
	}
	
	char initial_sudoku_matrix[dimp][dimp];
	
	for(i = 0; i < dimp ; ++i){
		for(j = 0 ; j < dimp ; ++j){
			fscanf(sudoku_file, "%d", &val);
			initial_sudoku_matrix[i][j] = val;
		}
	}

	fclose(sudoku_file);
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
		printf("Read sudoku done ---------------- \n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* Impartire patrate ***************************************************/
	
	char squares_stat[dim][dim];
	/* 0 - ocupat sau nu stiu
	 * 1 - disponibil pentru subarborele curent - de aici imi iau si eu */
	
	int posi, posj; // pos in square_stat a patratului propriu
	
	if(rank == 0){
		for(i = 0 ; i < dim ; ++i){
			for(j = 0 ; j < dim ; ++j){
				squares_stat[i][j] = 1; // pentru  toate sunt libere
			}
		}
	}
	else{
		/* primeste status */
		MPI_Recv(&squares_stat, dimp, MPI_CHAR, parent, tag, MPI_COMM_WORLD,&Stat);
	}
	
	get_pos(squares_stat, &posi, &posj);
	squares_stat[posi][posj] = 0;
	
	int last_posi = posi;
	int last_posj = posj;
	
	for(i = 0 ; i < numtasks ; ++i){
		
		if(neighbors[i] && i != parent){
			char square_stat_branch[dim][dim];
			get_square_stat_branch(squares_stat,square_stat_branch, &last_posi, 
											&last_posj, num_nodes[i]);
			MPI_Send(&square_stat_branch, dimp, MPI_CHAR, i, tag,
											MPI_COMM_WORLD);
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
		printf("Done allocating ----------------- \n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	/* Determinare solutii patrat propriu **********************************/
	
	int indexi = posi *dim;
	int indexj = posj *dim;

	Solution *my_solution;
	init_solution(&my_solution);
	
	compute_all_solutions_for_my_square(my_solution, initial_sudoku_matrix, indexi, indexj);
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
		printf("Done my solutions --------------- \n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* Combine Solutions ***************************************************/
	
	int from;
	int sol_num;
	
	char sol_r[dimp][dimp];
	char sqr_r[dim][dim];
	
	int sol_nr = own_sol;
	Solution *solution;
	
	int kids_nr = nr-1;
	if(rank == 0){
		kids_nr++;
	}
	int nr_new;
	
	/* Primeste solutii de la copii */ 
	for(i = 0 ; i < kids_nr ; ++i){
		
		MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE , tag, MPI_COMM_WORLD, &Stat);
		from = Stat.MPI_SOURCE;

		sol_num=val;
		
		init_solution(&solution);
		sol_nr = 0;
		
		for(j = 0 ; j < sol_num; ++j){
			
			MPI_Recv(&sol_r, dimpp, MPI_CHAR, from , tag, MPI_COMM_WORLD, &Stat);
			MPI_Recv(&sqr_r, dimp, MPI_CHAR, from , tag, MPI_COMM_WORLD, &Stat);
			
			/* Combina cu ce am deja */
			combine(&my_solution,&solution, sol_r, sqr_r, &nr_new);
			sol_nr += nr_new;
		}
		my_solution = solution;
	}
	
	char sol_to_send[dimp][dimp];
	char sqr_to_send[dim][dim];
	
	/* Trimite solutii la parinte */
	if(rank != 0){
		MPI_Send(&sol_nr, 1, MPI_INT, parent, tag, MPI_COMM_WORLD);
		Solution *it = my_solution;
		for(i = 0 ; i < sol_nr ; ++i){
		
			for(m = 0 ; m < dimp ; ++m){
				for(n = 0 ; n < dimp ; ++n){
					sol_to_send[m][n] = (it->sol)[m][n];
				}
			}
			for(m = 0 ; m < dim ; ++m){
				for(n = 0 ; n < dim ; ++n){
					sqr_to_send[m][n] = (it->sqr)[m][n];
				}
			}
		
			MPI_Send(&sol_to_send, dimpp, MPI_CHAR, parent, tag, MPI_COMM_WORLD);
			MPI_Send(&sqr_to_send, dimp, MPI_CHAR, parent, tag, MPI_COMM_WORLD);
			it = it->next;
		}
	}
	
	/* Afisare solutii si scrie in fisier prima solutie */
	if(rank == 0){
		printf("\nSolution(s) founded: %d\n",sol_nr);
		print_sol(my_solution);
		
		output_file = fopen(argv[3],"w+");
		
		fprintf(output_file,"%d\n",dim);
		print_first_sol_to_file(output_file, my_solution);
		
		fclose(output_file);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
		printf("Done ALL ------------------------ \n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* MPI Finalize *********************************************************/
	MPI_Finalize();

	return 0;
}
