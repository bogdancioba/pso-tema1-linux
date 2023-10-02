# Biblioteca Custom

Acest document descrie o serie de functii custom pentru manipularea fisierelor, inspirate de functiile standard din stdio.h. Fiecare functie este descrisa pe scurt mai jos:

## SO_FILE
Structura SO_FILE este echivalentul custom pentru FILE din biblioteca standard C. Contine metadate despre un fisier deschis, cum ar fi descriptorul de fisier, bufferul si pozitia curenta în buffer.


## so_fopen
Deschide un fisier si returneaza un pointer la o structura SO_FILE. Putem deschide cu modurile :
​	r ​- deschide fișierul pentru citire. Dacă fișierul nu există, ​funcția eșuează.
​	r+ ​- deschide fișierul pentru citire și scriere. Dacă fișierul nu există, funcția eșuează.
​	w ​- deschide fișierul pentru scriere. Dacă fișierul nu există, este creat. Dacă fișierul există, este trunchiat la dimensiune 0
​	w+ ​- deschide fișierul pentru citire și scriere. Dacă fișierul nu există, este creat. Dacă fișierul există, este trunchiat la dimensiune 0
​	a ​- deschide fișierul în modul ​append ​- scriere la sfârșitul fișierului. Dacă fișierul ​nu există, este creat.
	​a+​ - deschide fișierul ​în modul ​append+read​. Dacă fișierul ​nu există, ​este creat.


## so_fclose
Inchide un fisier deschis anterior cu so_fopen si elibereaza memoria alocata pentru structura SO_FILE.

## so_fgetc
Citeste un caracter dintr-un fisier deschis .

## so_fputc
Scrie un caracter într-un fisier deschis.

## so_fread
Citeste date din fisier într-un buffer specificat.

## so_fwrite
Scrie date dintr-un buffer specificat în fisier.

## so_fseek
Modifica pozitia curenta de citire/scriere în fisier.

## so_ftell
Returneaza pozitia curenta de citire/scriere în fisier.

## so_fflush
Datele din buffer sunt scrise în fisier. Intoarce 0 in caz de succes sau ​SO_EOF​ in caz de eroare.

## so_fileno
Returneaza descriptorul de fisier pentru un fisier deschis.

## so_feof
Verifica daca s-a ajuns la sfarsitul unui fisier.și eliberează memoria ocupată de structura ​SO_FILE

## so_ferror
Verifica daca s-a produs o eroare la ultima operatie de citire/scriere.

## so_popen
Deschide un proces, care va executa comanda specificată de parametrul ​command. Funcția întoarce o structură ​SO_FILE​ pe care apoi se pot face operațiile uzuale de citire/​scriere,​ ca și cum ar fi un fișier obișnuit.

## so_pclose
Inchide un proces deschis cu so_popen și eliberează memoria ocupată de structura ​SO_FILE

------------
Pentru a crea biblioteca, trebuie sa rulati comanda 'make' in folderul cu codul sursa, headerul si makefileul.


