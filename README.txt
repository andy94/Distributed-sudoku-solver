 **********************************************************************
 * Tema        : 3 - APD                                              *
 * Autor       : Andrei Ursache                                       *
 * Grupa       : 332 CA                                               *
 * Data        : 30.12.2015                                           *
 **********************************************************************
 
 *  Aspecte generale **************************************************
 
    Tema este realizata in C folosind MPI.
    Fisierul Makefile contine o regula de build (compilarea in urma careia se 
obtine executabilul "tema3"), o regula de clean (se sterg executabilul toate 
fisierele specifice proceselor) si o regula de run (care va porni programul
cu parametrii "topo", "in" si "out", cu numarul de procese 4.
    Codul sursa este impartit in trei fisiere. In "tema3.c" se regaseste 
functia main. Imi cer scuze pentru dimensiunea acesteia. In "solution.c" 
impreuna cu "solution.h" se regasesc structuri si functii auxiliare.
    Mentionez ca fiecare nod comunica doar cu nodurile vecine din topologie
(topologie din care au fost eliminate lagaturile ce cauzau bucle). De asemenea,
pentru un imput corect, programul se incheie gracefully. Toate procesele se 
termina, iar cozile de comunicare sunt goale (se observa din algoritm).
    
    
 *  Ideea generala ****************************************************
 
    Programul primeste ca argument in linie de comanda numele celor 3 fisiere
mentionate in enunt. De asemenea numarul de procese trebuie sa fie egal cu 
patratul dimensiunii problemei (fiecare proces va rezolva un patrat din 
jocul de sudoku).
    Procesele citesc doar vecinii lor din fisier.
    Apoi, cu mesaje de sondaj cu ecou se descopera parintii, tabela de rutare si
matricea de adiacenta (partiala).
    Se imparte cate un patratel din joc fiecarui proces.
    Se genereaza toate solutiile posibile pentru acel patratel.
    Dupa aceasta, se asteapta si de la toti copii sa se primeasca solutiile 
pentru patratele lor. Se combina solutiile proprii cu cele primite si apoi se 
trimit parintelui.
    In final, dupa ce procesul radacina a primit de la toate procesele fiu, 
se afiseaza la consola solutiile si se scrie in fisierul de output diar 
prima solutie.


 *  Implementare ******************************************************
    
    I. Stabilirea topologiei
    Fiecare proces va citi din fisierul de topologie DOAR vecinii sai. In acest
moment el stie cu cine poate sa comunice, dar nu stie nimic legat de intreaga
topologie, tabela de rutare, bucle etc. 
    Pentru stabilirea topologiei si eliminarea buclelor se transmit mesaje de 
sondaj cu ecou. Primul mesaj de sonsaj primit de un nod este chiar de la 
parintele sau si il retine. Sondajele si ecourile se primesc pe un canal comun,
astfel incat sa poata fi detectate bucle (mesaje de sondaj "intarziate"). 
    Daca un proces primeste un mesaj de sondaj, dar are deja parinte, el pur si
simplu va sterge legatura catre acest "fals parinte". La randul sau si falsul 
parinte va sterge legatura catre el pentru ca mai intai sunt trimise toate 
mesajele de sondaj (nu exista in acest mod posibilitatea sa fie un mesaj de 
sondaj doar intr-un singur sens, in celalalt sa nu mai fi ajuns - vezi cod).
    Ca si mesaje de ecou, se transmite matrice de adiacenta construita pana in 
acel moment, precum si tabela de rutare. Asta reprezinta un ecou.
    Dupa aceasta etapa toate procesele au tabela de rutare completa (stie cate
noduri sunt pe fiecare ramura), iar procesul radacina va afisa si matricea de
adiacenta in fisier. Restul proceselor vor afisa pe prima linie numarul lor de 
ordine, iar dupa asta vor fi afisate informatiile cunoscute. Am ales un anumit 
format detaliat chiar in fisierele de output. Din enunt am inteles ca singura 
constrangere e ca pe prima linie sa fie id-ul. Sper ca nu e o problema ca am 
afisat mai mutle informatii.

    II. Impartirea patratelor
    Cu topologia stabilita, root-ul va imparti fiecarui copil un set de 
patrate indeajuns pentru toate nodurile de pe acea ramura. Fiecare nod, va 
prelua aceasta informatie si va imparti in acelasi mod la randul lui.
    Numarul de patrate necesare unei ramure este calculat folosind tabela de 
rutare. Impartirea lor se face de la radacina catre frunze.
    
    III. Generarea tuturor solutiilor pentru patratul propriu
    Avand fiecare nod patratelul sau din matrice, procesul isi genereaza toate
solutiile pentru el. Aici folosesc o restrangere a domeniului fiecarui element
liber pentru o generare mai rapida. In acest mod vor fi putine apeluri 
recursive.
    Solutiile sunt retinute intr-o structura speciala. O solutie este caracteri-
zata printr-o matrice de elemente de dim^2 x dim^2 (tabla de joc, cu elementele
umplute pentru patratelul respectiv) si o matrice de dimensiune dim x dim ce 
indica pentru ce patratele este solutia respectiva (1 sau 0 pentru fiecare 
patratel).

    IV. Combinarea si transmiterea solutiilor
    Fiecare proces asteapta sa primeasca solutii de la copii. Evident, frunzele
sar peste aceasta etapa si vor trimite direct solutiile pentru patratelele lor.
    Un proces este atentionat ca urmeaza sa primeasca solutiile unui copil daca
primeste de la acesta un int cu valoarea egala cu numarul de solutii pe care
il va primi. Apoi, primeste pe rand toate solutiile (cele doua matrice). Pentru
fiecare solutie primita, aceasta se "combina" cu toate pe care le are el deja.
Daca este corecta in continuare, atunci noile solutii se retin.    Astfel, dupa
fiecare set de solutii de la o ramura, procesul va avea un set de solutii 
completat (din ce in ce mai "plin").
    Dupa ce termina de primit solutiile si dupa ce le combina, le trimite pe 
toate parintelui. 
    
    V. Final
    Procesul root, dupa ce termina de primit de la toti copii si combina 
solutiile, va ramne cu solutiile complete pentru intreaga tabla de joc.
    Va scrie in fisier prima solutie, iar la consola le va aisa pe toate.
    Toate procesele se termina iar pe calnalele de comunicatie nu mai ramane
niciun mesaj.

 *   (!!!)   Alte detalii referitoare la implementarea temei se gasesc in 
             fisierele sursa.
            

    Andrei Ursache
    
