#############################################################################
# Tema  : 3 - APD                                                           #
# Autor : Andrei Ursache                                                    #
# Grupa : 332 CA                                                            #
# Data  : 30.12.2015                                                        #
#############################################################################

build: tema3.c solution.c
	mpicc tema3.c solution.c -o tema3

run: build
	mpirun -np 16 tema3 topo4 in out

clean:
	rm -f tema3 &>/dev/null
	rm -f proc_* &>/dev/null
