#!/bin/bash

DIRECTORIO="Resultados"
output=$DIRECTORIO/resultados

#./practica2SG mapa semilla nivel fila columna sentido

#Crea el directorio si no existe
if [ ! -d "./$DIRECTORIO" ];then
mkdir $DIRECTORIO;fi

#Crea los ficheros resultados
if [[ -e $output.dat || -L $output.dat ]] ; then
    i=1
    while [[ -e $output-$i.dat || -L $output-$i.dat ]] ; do
        let i++
    done
    output=$output-$i
fi

nivel=4

echo -n ""> $output.dat

echo -e "\nMapa: ./mapas/mapa30.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
   ./practica2SG mapas/mapa30.map 1 4 4 4 6 3 3 5 10 16 14 18 10 21 4 7 4 24 4 12 3 6 15 25 18 8 19 15 14 23 15 26 8 10 24 19 26 25 7 16 11 22 15 20 15 22 19 10 20 4 13 26 24 9 6 26 10 17 19 25 13 24 20 26 19 12 18 8 23 9 13 6 5 8 16 12 5 3 14 11 22 11 8 6 17 7 4 21 3 23 4 15 5 7 23 21 19 4 15 6 13 24 17 6 26 5 4 24 10 16 17 13 20 22 9 26 22 22 24 14 4 24 26 7 18 6 21 9 9 18 6 19 15 16 21 3 14 13 10 25 13 17 24 7 20 14 14 9 21 5 18 20 20 19 10 5 18 18 21 9 22 20 19 7 15 26 20 10 17 17 19 8 23 8 9 9 5 20 3 20 11 6 23 16 8 26 14 17 4 8 25 14 13 25 14 8 5 20 21 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa50.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa50.map 1 4 28 25 3 39 45 9 16 38 13 27 23 31 18 45 31 35 7 12 6 40 7 20 6 10 25 41 30 14 31 26 24 38 26 38 20 44 14 17 40 45 3 4 9 33 44 17 3 3 11 42 13 26 18 38 25 33 26 46 46 36 14 36 31 17 34 8 22 44 41 16 11 44 17 29 32 42 21 46 19 40 34 45 24 46 7 44 32 21 30 14 39 15 22 11 9 13 27 20 8 45 5 6 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa75.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa75.map 1 4 11 47 0 59 68 3 32 14 25 57 20 41 35 47 27 38 71 68 47 53 10 18 10 60 11 16 38 62 45 22 47 39 37 57 39 57 30 66 21 26 60 68 5 6 14 49 66 26 4 4 17 63 19 40 28 57 38 50 39 69 69 54 21 55 47 26 51 12 33 66 62 24 17 67 26 51 71 44 49 64 32 69 29 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa100.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa100.map 1 4 63 31 3 3 3 79 91 19 33 76 27 55 47 62 36 51 95 91 63 71 14 24 13 80 15 21 51 83 61 29 63 52 49 77 52 76 40 89 28 35 80 91 6 94 52 8 19 66 89 34 6 6 23 85 26 53 37 76 51 66 53 3 43| tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/pinkworld.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG mapas/pinkworld.map 1 4 68 14 1 3 3 41 35 47 27 38 71 68 47 18 10 60 11 30 9 66 21 68 5 71 39 6 14 49 66 4 17 40 28 50 39 69 69 12 33 66 62 67 26 51 71 44 49 64 32 69 29 68 36 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/islas.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG mapas/islas.map 1 4 47 53 2 74 47 46 42 71 56 83 52 58 65 85 43 92 39 81 68 91 48 21 95 92 14 88 64 43 61 28 78 30 44 22 18 27 55 41 16 90 10 12 49 76 68 38 74 | tail -n1 >> $output.dat
done


echo -e "\nMapa: ./mapas/marymonte.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG mapas/marymonte.map 1 4 66 38 0 59 64 41 35 12 65 71 68 47 53 10 45 69 8 6 37 68 59 36 48 11 13 70 70 8 67 59 10 34 70 33 4 66 70 37 22 46 71 70 18 | tail -n1 >> $output.dat
done



echo -e "\nMapa: ./mapas/medieval.map" >> $output.dat
for ((  c=4; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat

    ./practica2SG mapas/medieval.map 1 4 3 3 2 9 20 13 48 9 87 84 39 91 19 33 76 87 39 57 29 84 53 96 96 63 3 88 54 32 44 19 75 29 95 96 48 21 33 87 41 9 20 13 48 9 87 84 39 91 19 33 76 87 39 | tail -n1 >> $output.dat
done




    