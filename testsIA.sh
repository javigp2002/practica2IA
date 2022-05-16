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

nivel=3

echo -n ""> $output.dat

echo "Mapa: ./mapas/cuadricula.map" >> $output.dat

echo -e -n "Nivel $c: " >> $output.dat
for (( c=3; c<=$nivel; c+=1 ))
do
    ./practica2SG ./mapas/cuadricula.map 0 $c 78 79 0 | tail -n1 >> $output.dat
done


echo -e "\nMapa: ./mapas/mapa30.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa30.map 0 $c 15 20 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa50.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa50.map 0 $c 20 20 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa75.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa75.map 0 $c 14 37 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/mapa100.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa100.map 0 $c 78 79 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/100b.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa100b.map 0 $c 15 20 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/100c.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/mapa100c.map 0 $c 15 20 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/scape.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/scape.map 0 $c 14 18 2 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/vertigo.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/vertigo.map 0 $c 15 20 0 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/islas.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
	echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/islas.map 0 $c 84 39 6 | tail -n1 >> $output.dat
done


echo -e "\nMapa: ./mapas/marymonte.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/marymonte.map 0 $c 68 14 2 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/pinkworld.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/pinkworld.map 0 $c 68 14 2 | tail -n1 >> $output.dat
done

echo -e "\nMapa: ./mapas/medieval.map" >> $output.dat
for ((  c=3; c<=$nivel; c+=1 ))
do
    echo -e -n "Nivel $c: " >> $output.dat
    ./practica2SG ./mapas/medieval.map 0 $c 68 14 2 | tail -n1 >> $output.dat
done
