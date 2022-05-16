#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"





void actualizaSegunOrientacion(int &fil, int &col, int &orient);

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
	
	// nivel 4
	if (sensores.nivel == 4 && (sensores.colision || inicio)){
		inicio=false;
		
		ultimaAccion = actWHEREIS;
		return ultimaAccion;
	}

	if (sensores.nivel < 4 ){
		// actualizar variable actual
		actual.fila = sensores.posF;
		actual.columna = sensores.posC;
		actual.orientacion = sensores.sentido;
	} else {
		if (sensores.posF != -1){
			fil = sensores.posF;
			col = sensores.posC;
			orient = sensores.sentido;
		} else {
			switchAccion();
		}
		
		actual.fila = fil;
		actual.columna = col;
		actual.orientacion = orient;

		// if (sensores.superficie[2] == 'l' || sensores.superficie[2] == 'a'){
		// 	ultimaAccion=actIDLE;
		// 	return ultimaAccion;
		// }
		for (auto it = objetivos.begin(); it != objetivos.end(); it++)
		{

			if (actual.fila == it->fila && actual.columna == it->columna){
				bool encontrada=false;
				for (auto ot = listBateriasEncontradas.begin(); ot != listBateriasEncontradas.end() && !encontrada; ot++)
				{
					if (it->fila == ot->fila && it->columna == ot->columna)
						encontrada=true;
				}
				
				if (!encontrada)
					listBateriasEncontradas.push_back(*it);
				cout << listBateriasEncontradas.size() << endl;
				if (listBateriasEncontradas.size()>2){
					listBateriasEncontradas.clear();
				}
				hayPlan = false;
			}
		}
	}

	// cout << "Fila: " << actual.fila << endl;
	// cout << "Col : " << actual.columna << endl;
	// cout << "Ori : " << actual.orientacion << endl;

	// // Capturo los destinos
	// cout << "sensores.num_destinos : " << sensores.num_destinos << endl;

	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	char c = mapaResultado[actual.fila][actual.columna];

	if (c == 'K'){
		actual.bikini=true;
		actual.zapatillas=false;
	} else if (c == 'D'){
		actual.bikini=false;
		actual.zapatillas=true;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	if (sensores.nivel >= 3){
		
		actualizarVistaMapa( sensores, actual, mapaResultado);
		if(sensores.nivel == 4){
			actualizarVistaMapa(sensores, actual, mapaAux);
		}
		

		if (estoyAccionBasica){
			if (num_iteracion >= 120){
				estoyAccionBasica=false;
				num_iteracion=0;
			} else
				num_iteracion++;

			ultimaAccion = accionBasica(actual,mapaResultado,sensores);
			return ultimaAccion;

		}


		if (sensores.vida > 450 && sensores.bateria < VAL_MIN_BATERIA && sensores.bateria < 2*sensores.vida && sensores.bateria < VAL_MAX_BATERIA&& !buscaBateria){
			for (int i=0; i < mapaResultado.size() && !buscaBateria; i++){
				for (size_t j = 0; j < mapaResultado[0].size() && !buscaBateria; j++)
				{
					if (mapaResultado[i][j] == 'X'){
						bateria.fila=i;
						bateria.columna=j;

						listBaterias.push_back(bateria);
						
					}
				}
				
			}

			if (!listBaterias.empty()){
				int diferenciaMinima = 2e6;
				for (auto it= listBaterias.begin(); it!=listBaterias.end(); it++){
					int difActual= max(abs(actual.fila - it->fila), abs(actual.columna - it->columna));

					if (difActual<diferenciaMinima){
						bateria = *it;
					}
				}

				
				hayPlan = pathFinding_AlgoritmoA(actual, bateria, plan, mapaResultado);
				if (hayPlan ){
					buscaBateria=true;
				}
			}
		}



		if (buscaBateria){
			
			if (sensores.bateria < 2*sensores.vida && sensores.bateria < VAL_MAX_BATERIA && mapaResultado[actual.fila][actual.columna] == 'X'){
				ultimaAccion =  actIDLE;
				return ultimaAccion;
			} else if (mapaResultado[actual.fila][actual.columna] == 'X'){
				buscaBateria=false;
				hayPlan=false;
			}
		}
	}


	// si no hay plan, construye uno
	if (!hayPlan){
		hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan, sensores, accion);
	}
	

	if (hayPlan && plan.size() > 0){ // hay un plan no vacio
		accion = plan.front();	 // tomo la sig accion del plan
		if (accion == actFORWARD && (!pasoPosible(actual,sensores,2) ||  sensores.superficie[2] != '_')){
			if (buscaBateria){
				list<estado> l;
				l.push_back(bateria);
				if (sensores.superficie[2] != '_'){
					int f=fil, c=col; 
					actualizaSegunOrientacion(f,c,orient);
					unsigned char ant = mapaAux[f][c];
					mapaAux[f][c]='M';
					hayPlan = pathFinding_AlgoritmoA(actual, bateria, plan,mapaAux);
					mapaAux[f][c] = ant;
				}else
					hayPlan = pathFinding_AlgoritmoA(actual, bateria, plan, mapaResultado);
			
			} else {
				buscaSalida=true;

				hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan, sensores, accion);
			}
			if (hayPlan)
				accion = plan.front();
			else{
				estoyAccionBasica = true;
				ultimaAccion =  accionBasica(actual,mapaResultado,sensores);
				return ultimaAccion;
			}
		}


		if (hayPlan){
			plan.erase(plan.begin());	 // eliminamos la acción del plan
			ultimaAccion = accion;
		}
	} 

	if(plan.size() <= 0) {
		hayPlan=false;
		if (accion == actIDLE && sensores.nivel <=3){
			estoyAccionBasica = true;
			ultimaAccion =  accionBasica(actual,mapaResultado,sensores);
			return ultimaAccion;
		}
	}

	ultimaAccion = accion;
	return accion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, Sensores sensores, Action &accion)
{
	estado un_objetivo;
	switch (level)
	{
	case 0:
		cout << "Demo\n";
		
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Anchura(origen, un_objetivo, plan);
		break;
	case 2:
		cout << "Optimo en coste\n";
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_AlgoritmoA(origen,un_objetivo,plan,mapaResultado);
		break;
	case 3:
		//cout << "Reto 1: Descubrir el mapa\n";
        return pathFinding_DescubreMapa(origen,un_objetivo,plan,mapaResultado, sensores, accion);
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		return pathFinding_BuscaObjetivos(origen, objetivos, plan, mapaResultado, sensores);		
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


struct nodoA_
{
	estado st;
	list<Action> secuencia;
	int g = 0, h = 0;

//	estado mejorPadre;
//	list <estado> hijos;

};
int valorBool(bool zapatillas, bool bikini){
	int ret;

	if (!zapatillas && !bikini)
		ret=0;
	else if (!zapatillas && bikini)
		ret=1;
	else if (zapatillas && !bikini)
		ret=2;
	else
		ret=3;

	return ret;
}

struct ComparaEstadosA_Nodos
{
    bool operator()(const nodoA_ &b, const nodoA_ &c) const
    {
    
		int aBool = valorBool(b.st.zapatillas, b.st.bikini), nBool = valorBool(c.st.zapatillas, c.st.bikini);
		
        estado a = b.st;
        estado n = c.st;
        if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
            (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and aBool > nBool))
            return true;
        else
            return false;
    }
};
struct ComparaDistanciaA_Nodos
{
	bool operator()(const nodoA_ &a, const nodoA_ &n) const
	{
		double a_F = a.g+a.h,
			   n_F= n.g+n.h;
		
		if ((a_F > n_F)) 
			return true;
		else 
			return false;
	}
};





// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		
		return true;

		
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "I ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}
void ComportamientoJugador:: pintaPrecipicios(){
	for (size_t i = 0; i < mapaResultado.size(); i++)
	{
		for (size_t j = 0; j < mapaResultado.size(); j++)
		{
			if (i>2 && i < (mapaResultado.size()-3)){
				if (j>2 && j <mapaResultado.size()-3 ) j= (mapaResultado.size()-3);
			} 
			mapaResultado[i][j] = 'P';
		}	
	}
}
// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

void insertaNodoAnchura(nodo &nod, Action accion, set<estado, ComparaEstados> & Cerrados,queue<nodo> &Abiertos, set<estado,ComparaEstados> &SetAbiertos){
	if (Cerrados.find(nod.st) == Cerrados.end() &&SetAbiertos.find(nod.st) == SetAbiertos.end() )
		{
			nod.secuencia.push_back(accion);
			Abiertos.push(nod);
			SetAbiertos.insert(nod.st);
		}
}
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;	
	set<estado,ComparaEstados> SetAbiertos;			  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);
	SetAbiertos.insert(current.st);
	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{
		SetAbiertos.erase(Abiertos.front().st);
		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		insertaNodoAnchura(hijoTurnR,actTURN_R,Cerrados, Abiertos, SetAbiertos);

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		insertaNodoAnchura(hijoSEMITurnR,actSEMITURN_R,Cerrados, Abiertos, SetAbiertos);

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		insertaNodoAnchura(hijoTurnL,actTURN_L,Cerrados, Abiertos, SetAbiertos);

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		insertaNodoAnchura(hijoSEMITurnL,actSEMITURN_L,Cerrados, Abiertos, SetAbiertos);


		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			insertaNodoAnchura(hijoForward,actFORWARD,Cerrados, Abiertos, SetAbiertos);
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.front();
		}
	}
	
	
	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		
		return true;

		
	}
	else
	{
		cout << "No encontrado plan\n";
	}
	

	return false;
}

struct ComparaNodos
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

void actualizaPathCost(nodoA_ & nodo, vector< vector< unsigned char> > &mapaR, Action accion, const estado &destino, double gPadre){
	int valor, fil = nodo.st.fila, col = nodo.st.columna;

	bool agua = mapaR[fil][col] == 'A';
	bool bosque = mapaR[fil][col] == 'B';
	bool tierra = mapaR[fil][col] == 'T', bikini = nodo.st.bikini, zapatillas = nodo.st.zapatillas;
	switch(accion){
		case actIDLE:
			break;
		case actFORWARD:
			if (agua){
				if (bikini)
					valor = 10;
				else
					valor = 200;

			} else if (bosque) {
				if (zapatillas)
					valor = 15;
				else	
					valor = 100;
			} else if (tierra) {
				valor = 2;
			} else 
				valor = 1;
			break;
		case actTURN_L:
		case actTURN_R:
			if (agua){
				if (bikini)
					valor = 5;
				else
					valor = 500;

			} else if (bosque) {
				if (zapatillas)
					valor = 1;
				else	
					valor = 3;
			} else if (tierra) {
				valor = 2;
			} else 
				valor = 1;
			break;
		
		case actSEMITURN_R:
		case actSEMITURN_L:
			if (agua){
				if (bikini)
					valor = 2;
				else
					valor = 300;

			} else if (bosque) {
				if (zapatillas)
					valor = 1;
				else	
					valor = 2;
			} else if (tierra) {
				valor = 1;
			} else 
				valor = 1;
			break;
		default:
			break;
		

	}

	if (mapaR[fil][col] == '?')
		valor = 1;

	if (mapaR[fil][col] == 'P' || mapaR[fil][col] == 'M' )
		valor = 3e7;

	nodo.g = valor + gPadre;
	double x = nodo.st.fila - destino.fila;
	double y = nodo.st.columna - destino.columna;

	nodo.h = (int)max(abs(x), abs(y));
}

void actualizaNodoAbierto (priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> &Abiertos, set<nodoA_,ComparaEstadosA_Nodos> &SetAbiertos, estado status){
	stack<nodoA_> pila;
	
	while (Abiertos.top().st.fila != status.fila && Abiertos.top().st.columna != status.columna && Abiertos.top().st.orientacion != status.orientacion) {
		pila.push(Abiertos.top());
		Abiertos.pop();
	}
	
	SetAbiertos.erase(Abiertos.top());
	Abiertos.pop(); // quitamos el que deseamos
	

	while (!pila.empty()){
		Abiertos.push(pila.top());
		pila.pop();
	}
	
	
}

/*
void actualizaNodosHijosCerrados(nodoA_ &actual, int diferencia, set<nodoA_, ComparaEstadosA_Nodos> &Cerrados, priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> &Abiertos, set<nodoA_,ComparaEstadosA_Nodos> &SetAbiertos) {
	int siz = actual.hijos.size();
	
	for(auto it=actual.hijos.begin(); it != actual.hijos.end(); it++){
		nodoA_ hijoActual ;
		hijoActual.st= *it;

		auto cerrado = Cerrados.find(hijoActual);
		if (cerrado != Cerrados.end()){
			hijoActual = *cerrado;
			actualizaNodosHijosCerrados(hijoActual,diferencia,Cerrados,Abiertos, SetAbiertos);
			
			Cerrados.erase(hijoActual);
			hijoActual.g-=diferencia;
			Cerrados.insert(hijoActual);
		} else {
			auto abierto = SetAbiertos.find(hijoActual);
			if (abierto != SetAbiertos.end()){
				hijoActual = *abierto;
				//actualizaNodoAbierto(Abiertos,SetAbiertos,hijoActual.st);
				SetAbiertos.erase(hijoActual);
				hijoActual.g -= diferencia;
				
				SetAbiertos.insert(hijoActual);
				Abiertos.push(hijoActual);

			}
		}
	}

}
*/


void insertaNodo(nodoA_ &hijo, nodoA_ &current, const estado &destino,  vector< vector< unsigned char> >& mapaR, set<nodoA_, ComparaEstadosA_Nodos> &Cerrados, // Lista de Cerrados
	priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> &Abiertos, set<nodoA_,ComparaEstadosA_Nodos> &SetAbiertos, Action accion){
		

	hijo.secuencia.push_back(accion);

	char c = mapaR[hijo.st.fila][hijo.st.columna];
	if (c == 'K'){
		hijo.st.bikini=true;
		hijo.st.zapatillas=false;
	} else if (c == 'D'){
		hijo.st.bikini=false;
		hijo.st.zapatillas=true;
	}
	

	// actualiza h y g
	actualizaPathCost(hijo,mapaR,accion, destino,current.g);
	

	set<nodoA_,ComparaEstadosA_Nodos>::iterator existeAbierto = SetAbiertos.find(hijo);
	set<nodoA_,ComparaEstadosA_Nodos>::iterator existeCerrado = Cerrados.find(hijo);	

	
	if (existeAbierto != SetAbiertos.end()){
		if (existeAbierto->g+existeAbierto->h > hijo.g+hijo.h){			
			SetAbiertos.erase(*existeAbierto);
			Abiertos.push(hijo);
			
			SetAbiertos.insert(hijo);
		}
	}else if (existeCerrado != Cerrados.end()){		
	} else{
		SetAbiertos.insert(hijo);
		Abiertos.push(hijo);
	}

}

void sacaNodo(nodoA_ n){
	cout << n.g << " " << n.h << "|" << n.st.fila << " "<< n.st.columna << " "<< n.st.orientacion << endl;
}



bool ComportamientoJugador::pathFinding_AlgoritmoA(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapaR){
	cout << "Calculando plan\n";
	bool salida = false;
	plan.clear();
	set<nodoA_, ComparaEstadosA_Nodos> Cerrados; // Lista de Cerrados
	priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> Abiertos;			  // Lista de Abiertos
	set<nodoA_,ComparaEstadosA_Nodos> SetAbiertos;

	nodoA_ current;
	current.st = origen;
	current.secuencia.empty();


	Abiertos.push(current);
	SetAbiertos.insert(current);
	while (!Abiertos.empty()){
		SetAbiertos.erase(Abiertos.top());
		Abiertos.pop();


		if (current.st.fila == destino.fila and current.st.columna == destino.columna)
		{
			cout << "Cargando el plan\n";
			plan = current.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);
			// ver el plan en el mapa
			VisualizaPlan(origen, plan);
			
			return true;
		}

		// Generar descendiente de girar a la derecha 90 grados
		nodoA_ hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		insertaNodo(hijoTurnR,current,destino,mapaR,Cerrados,Abiertos,SetAbiertos,actTURN_R);
		

		// Generar descendiente de girar a la derecha 45 grados
		nodoA_ hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		insertaNodo(hijoSEMITurnR,current,destino,mapaR,Cerrados,Abiertos,SetAbiertos, actSEMITURN_R);
		
		// Generar descendiente de girar a la derecha 90 grados
		nodoA_ hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		insertaNodo(hijoTurnL,current,destino,mapaR,Cerrados,Abiertos,SetAbiertos,actTURN_L);


		// Generar descendiente de girar a la izquierda 45 grados
		nodoA_ hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		insertaNodo(hijoSEMITurnL,current,destino,mapaR,Cerrados,Abiertos,SetAbiertos, actSEMITURN_L);
		
		// Generar descendiente de avanzar
		nodoA_ hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			insertaNodo(hijoForward,current,destino,mapaR,Cerrados,Abiertos,SetAbiertos, actFORWARD);
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			Cerrados.insert(current);

			if (SetAbiertos.find(Abiertos.top()) == SetAbiertos.end() && !Abiertos.empty())
				Abiertos.pop();
			if (!Abiertos.empty())
				current = Abiertos.top();
		}
	}
	

	return salida;
}

void ComportamientoJugador::actualizarVistaMapa(Sensores sensores, const estado &actual,  vector< vector< unsigned char> > & mapa){
int new_fil = actual.fila, new_col = actual.columna;
	int contador = 1, contador_ant = 0;

	mapa[new_fil][new_col] = sensores.terreno[0];

	if (actual.orientacion % 2 == 0){
		for (int i = 0; i < 3; i++){
			switch (actual.orientacion){
				case 0: new_col--; new_fil--; break;
				case 2: new_col++; new_fil--; break;
				case 4: new_col--; new_fil++; break;
				case 6: new_col--; new_fil--; break;
			}

			// se cuenta diferente en sur y oeste (de 3-1, 8-4, 16-9)
			if (actual.orientacion == 4 || actual.orientacion == 6){
				contador = contador_ant + 3+2*i;
				contador_ant = contador;
			}


			for (int j = 0; j < 3+2*i; j++) {
				if (actual.orientacion==0 || actual.orientacion == 4){ // norte,sur -->columnas
					mapa[new_fil][new_col+j] = sensores.terreno[contador];
				}else{ // este oeste --> filas
					mapa[new_fil+j][new_col] = sensores.terreno[contador];
				}
				if (actual.orientacion == 0 || actual.orientacion == 2) //norte este
					contador++;
				else  // sur oeste
					contador--;	
				
	
			}
		}
	} else {
		int filas=0, columnas=0;

		if (actual.orientacion == 1){
			mapa[new_fil][new_col] = sensores.terreno[0]; mapa[new_fil][new_col+1] = sensores.terreno[3]; mapa[new_fil][new_col+2] = sensores.terreno[8]; mapa[new_fil][new_col+3] = sensores.terreno[15];
			new_fil--;
			mapa[new_fil][new_col] = sensores.terreno[1]; mapa[new_fil][new_col+1] = sensores.terreno[2]; mapa[new_fil][new_col+2] = sensores.terreno[7]; mapa[new_fil][new_col+3] = sensores.terreno[14];
			new_fil--;
			mapa[new_fil][new_col] = sensores.terreno[4]; mapa[new_fil][new_col+1] = sensores.terreno[5]; mapa[new_fil][new_col+2] = sensores.terreno[6]; mapa[new_fil][new_col+3] = sensores.terreno[13];
			new_fil--;
			mapa[new_fil][new_col] = sensores.terreno[9]; mapa[new_fil][new_col+1] = sensores.terreno[10]; mapa[new_fil][new_col+2] = sensores.terreno[11]; mapa[new_fil][new_col+3] = sensores.terreno[12];
		} else if (actual.orientacion == 7){
			mapa[new_fil][new_col] = sensores.terreno[0]; mapa[new_fil-1][new_col ] = sensores.terreno[3]; mapa[new_fil-2][new_col ] = sensores.terreno[8]; mapa[new_fil-3][new_col ] = sensores.terreno[15];
			new_col--;
			mapa[new_fil][new_col] = sensores.terreno[1]; mapa[new_fil-1][new_col ] = sensores.terreno[2]; mapa[new_fil-2][new_col ] = sensores.terreno[7]; mapa[new_fil-3][new_col ] = sensores.terreno[14];
			new_col--;
			mapa[new_fil][new_col] = sensores.terreno[4]; mapa[new_fil-1][new_col ] = sensores.terreno[5]; mapa[new_fil-2][new_col ] = sensores.terreno[6]; mapa[new_fil-3][new_col ] = sensores.terreno[13];
			new_col--;
			mapa[new_fil][new_col] = sensores.terreno[9]; mapa[new_fil-1][new_col ] = sensores.terreno[10]; mapa[new_fil-2][new_col ] = sensores.terreno[11]; mapa[new_fil-3][new_col ] = sensores.terreno[12];
		} else if (actual.orientacion == 5){
			mapa[new_fil][new_col] = sensores.terreno[0]; mapa[new_fil][new_col-1] = sensores.terreno[3]; mapa[new_fil][new_col-2] = sensores.terreno[8]; mapa[new_fil][new_col-3] = sensores.terreno[15];
			new_fil++;
			mapa[new_fil][new_col] = sensores.terreno[1]; mapa[new_fil][new_col-1] = sensores.terreno[2]; mapa[new_fil][new_col-2] = sensores.terreno[7]; mapa[new_fil][new_col-3] = sensores.terreno[14];
			new_fil++;
			mapa[new_fil][new_col] = sensores.terreno[4]; mapa[new_fil][new_col-1] = sensores.terreno[5]; mapa[new_fil][new_col-2] = sensores.terreno[6]; mapa[new_fil][new_col-3] = sensores.terreno[13];
			new_fil++;
			mapa[new_fil][new_col] = sensores.terreno[9]; mapa[new_fil][new_col-1] = sensores.terreno[10]; mapa[new_fil][new_col-2] = sensores.terreno[11]; mapa[new_fil][new_col-3] = sensores.terreno[12];
		} else {
			mapa[new_fil][new_col] = sensores.terreno[0]; mapa[new_fil+1][new_col ] = sensores.terreno[3]; mapa[new_fil+2][new_col ] = sensores.terreno[8]; mapa[new_fil+3][new_col ] = sensores.terreno[15];
			new_col++;
			mapa[new_fil][new_col] = sensores.terreno[1]; mapa[new_fil+1][new_col ] = sensores.terreno[2]; mapa[new_fil+2][new_col ] = sensores.terreno[7]; mapa[new_fil+3][new_col ] = sensores.terreno[14];
			new_col++;
			mapa[new_fil][new_col] = sensores.terreno[4]; mapa[new_fil+1][new_col ] = sensores.terreno[5]; mapa[new_fil+2][new_col ] = sensores.terreno[6]; mapa[new_fil+3][new_col ] = sensores.terreno[13];
			new_col++;
			mapa[new_fil][new_col] = sensores.terreno[9]; mapa[new_fil+1][new_col ] = sensores.terreno[10]; mapa[new_fil+2][new_col ] = sensores.terreno[11]; mapa[new_fil+3][new_col ] = sensores.terreno[12];
		}

		
	}
}

void ComportamientoJugador::resetMapaPotencial(){
	vector<int> aux_Potencial(COL_POTENCIAL, VAL_MAX_SALIDA);
	for (size_t i = 0; i < FIL_POTENCIAL; i++)
	{
		mapaPotencial.push_back(aux_Potencial);
	} 
}

void ComportamientoJugador::resetMapaRecorrido(){
	vector<int> aux_Potencial(COL_POTENCIAL, -1);
	for (size_t i = 0; i < FIL_POTENCIAL; i++)
	{
		mapaRecorrido.push_back(aux_Potencial);
	} 
}

/*
pair<int,int> ComportamientoJugador::inicializaMapaPotencial(estado est){
	char c;
 	int maximo = 0 ;
	pair<int,int> objetivo;
	stack<pair<int,int>> valores; 
	for (int i = 0; i < FIL_POTENCIAL; i++){
		for	(int j = 0; j < COL_POTENCIAL; j++){
			unsigned char c_actual = mapaResultado[i][j];
			int valor;
			
			if (c_actual == '?'){
				valores.push({i,j});
				valor=0;
			}
			else if(c_actual != 'M' && c_actual != 'P' ){
				valor=0;
			} else
				valor = VAL_MAX_SALIDA;

			mapaPotencial[i][j] = valor;
		}
	}

	int siz=valores.size();
	for (size_t i = 0; i < siz; i++)
	{
		actualizaMapaPotencial(valores.top().first,valores.top().second, maximo, objetivo, est);
		valores.pop();
	}
// for (size_t i = 0; i < FIL_POTENCIAL; i++)		
// 				{
// 					cout << endl;
// 					for (size_t j = 0; j < COL_POTENCIAL; j++)
// 					{
// 						cout << setw (6)<< mapaPotencial[i][j] << " ";
// 					}
					
// 				}
// 				cout << endl;
	return objetivo;

}

void ComportamientoJugador::actualizaMapaPotencial(int fil_obj, int col_obj, int &maximo, pair<int,int> &objetivo, estado est){
	bool actualizoMapa, puedoActualizar;
	for (size_t i = 0; i < FIL_POTENCIAL; i++)
	{
		for (size_t j = 0; j < COL_POTENCIAL; j++)
		{
			puedoActualizar = true;

			actualizoMapa= !(i == fil_obj && j == col_obj)  && mapaResultado[i][j] != '?' && mapaResultado[i][j] != 'P' && mapaResultado[i][j] != 'M';
			if ( mapaResultado[i][j] == 'A' && !est.bikini) puedoActualizar = false;
			if ( mapaResultado[i][j] == 'B' && !est.zapatillas) puedoActualizar=false;
			
			if (actualizoMapa && puedoActualizar){
			
				int horizontal = i-fil_obj;
				int vertical  =j-col_obj;
				mapaPotencial[i][j] += max(abs(horizontal), abs(vertical));
				
				if (mapaPotencial[i][j] > maximo ){
					maximo =  mapaPotencial[i][j];
					objetivo.first = i;
					objetivo.second = j;
				}
			}
		}		
	}
}
*/


pair<int,int> ComportamientoJugador::inicializaMapaPotencialMinimo(estado est){
	char c;
 	int minimo = 1e5+1 ;
	pair<int,int> objetivo;
	stack<pair<int,int>> valores; 
	for (int i = 0; i < FIL_POTENCIAL; i++){
		for	(int j = 0; j < COL_POTENCIAL; j++){
			unsigned char c_actual = mapaResultado[i][j];
			int valor;
			
			if (c_actual == '?'){
				valores.push({i,j});
				valor=0;
				// objetivo.first = i;
				// objetivo.second=j;
			}else if (c_actual == 'A' && !est.bikini) {
				valor = 20000;
			} else if (c_actual == 'B' && !est.zapatillas){
				valor = 10000;
			}
			else if(c_actual != 'M' && c_actual != 'P' && mapaRecorrido[i][j] <0){
				valor=0;
			} else
				valor = VAL_MAX_SALIDA;

			mapaPotencial[i][j] = valor;
		}
	}

	int siz=valores.size();
	for (size_t i = 0; i < siz; i++)
	{
		actualizaMapaPotencialMinimo(valores.top().first,valores.top().second, minimo, objetivo, est);
		valores.pop();
	}
for (size_t i = 0; i < FIL_POTENCIAL; i++)		
				{
					cout << endl;
					for (size_t j = 0; j < COL_POTENCIAL; j++)
					{
						cout << setw (8)<< mapaPotencial[i][j] << "\t";
					}
					
				}
				cout << endl;
	return objetivo;

}

void ComportamientoJugador::actualizaMapaPotencialMinimo(int fil_obj, int col_obj, int &minimo, pair<int,int> &objetivo, estado est){
	bool actualizoMapa, puedoActualizar;
	for (size_t i = 0; i < FIL_POTENCIAL; i++)
	{
		for (size_t j = 0; j < COL_POTENCIAL; j++)
		{
			puedoActualizar = true;

			actualizoMapa= !(i == fil_obj && j == col_obj)  && mapaResultado[i][j] != '?' && mapaResultado[i][j] != 'P' && mapaResultado[i][j] != 'M';
			if ( mapaResultado[i][j] == 'A' && !est.bikini) puedoActualizar = false;
			if ( mapaResultado[i][j] == 'B' && !est.zapatillas) puedoActualizar=false;
			
			if (actualizoMapa && puedoActualizar && mapaRecorrido[i][j]<0){
			
				int horizontal = i-fil_obj;
				int vertical  =j-col_obj;
				mapaPotencial[i][j] += abs(horizontal) + abs(vertical);
				
				if (mapaPotencial[i][j] < minimo ){
					minimo =  mapaPotencial[i][j];
					objetivo.first = i;
					objetivo.second = j;
				}
			}
		}
		
	}
	
}





bool ComportamientoJugador::pasoPosible (const estado &origen, Sensores sensores, int sen){
	bool puedo_pasar = sensores.terreno[sen] != 'M' && sensores.terreno[sen] != 'P' ; 
	if (origen.zapatillas)
		return puedo_pasar && sensores.terreno[sen] != 'A' ;
	else if (origen.bikini)
		return puedo_pasar && sensores.terreno[sen] != 'B';
	else
		return puedo_pasar &&sensores.terreno[sen] != 'A'&& sensores.terreno[sen] != 'B';
}

Action ComportamientoJugador::accionPorDefecto(const estado &origen,  std::vector< std::vector< unsigned char> > mapaR, Sensores sensores){
	Action accion;
	int f=origen.fila,c=origen.columna;
	int fil_aux_mas_tres = f, col_aux_mas_tres = c;
	int fil_aux_delante = f, col_aux_delante = c;

	switch (origen.orientacion){
		case 0: f--; fil_aux_mas_tres = f-3; fil_aux_delante = fil_aux_mas_tres; col_aux_mas_tres = c+3;  break;
		case 1:
		case 3:
		case 5:
		case 7:
			return actSEMITURN_R;
			break;
			
		case 2: c++; col_aux_mas_tres = c+3; col_aux_delante = col_aux_mas_tres; fil_aux_mas_tres = f+3; break;
		case 4: f++; fil_aux_mas_tres = f+3; fil_aux_delante = fil_aux_mas_tres; col_aux_mas_tres = c-3;  break;
		case 6: c--; col_aux_mas_tres = c-3; col_aux_delante = col_aux_mas_tres; fil_aux_mas_tres = f-3;  break;
	}
	bool gira= false;
	bool noSeSaleDeMapa1 = fil_aux_mas_tres >=0 && fil_aux_mas_tres < MAX_MAPA && col_aux_mas_tres >= 0 && col_aux_mas_tres < MAX_MAPA;
	bool noSeSaleDeMapa2 = fil_aux_delante >=0 && fil_aux_delante < MAX_MAPA && col_aux_delante >= 0 && col_aux_delante < MAX_MAPA;
	
	if ((noSeSaleDeMapa1 && mapaResultado[fil_aux_mas_tres][col_aux_mas_tres] != '?') && (noSeSaleDeMapa2 && mapaResultado[fil_aux_delante][col_aux_delante] != '?') )
			gira = true;

	
	if (pasoPosible(origen,sensores,2) && !gira){
		accion = actFORWARD;
		num_iteracion=0;
	} else if (pasoPosible(origen,sensores,3) && !gira) {
		sigueMuro = true;
		accion = actSEMITURN_R;

	} else {
		accion = actTURN_R;
		num_iteracion++;
		if (num_iteracion >= 4){ //ha dado una vuelta completa
			buscaSalida=true;
			num_iteracion=0;
			// la nueva accion sera dar la vuelta para que vuelva por donde vino
			accion=actTURN_L;
		
		}

	} 

	return accion;
}


Action ComportamientoJugador::accionBasica(const estado &origen,  std::vector< std::vector< unsigned char> > mapaR, Sensores sensores){
	Action accion;
	
	estado es = origen;
	if (!HayObstaculoDelante(es)){
		accion = actFORWARD;
	} else if (pasoPosible(origen,sensores,3)) {
		sigueMuro = true;
		accion = actSEMITURN_R;

	} else {
		accion = actTURN_R;

	} 

	return accion;
}


bool ComportamientoJugador::pathFinding_DescubreMapa(const estado &origen, const estado &destino, list<Action> &plan, 
 		std::vector< std::vector< unsigned char> >& mapaR, Sensores sensores,  Action &accion){

	if (buscaSalida){
		cout << "buscando salida"<<endl;  
		// pair<int,int> objetivo = inicializaMapaPotencialMinimo(origen);
		
		// estado destino; 
		

		// if (objetivo.first == origen.fila && origen.columna == objetivo.second){
		// 	objetivo = inicializaMapaPotencial(origen);
			
		// }
		// destino.fila= objetivo.first;
		// destino.columna = objetivo.second;
		// pathFinding_AlgoritmoA(origen, destino, plan, mapaR);
		estado destino1; 
		pair<int,int> objetivo = rayos(origen);
		destino1.fila= objetivo.first;
		destino1.columna = objetivo.second;
		
		buscaSalida=false;

		return pathFinding_AlgoritmoA(origen, destino1, plan, mapaR);
		
		
 		
	} else if (sigueMuro) {
		if ( pasoPosible(origen,sensores,1)){
			sigueMuro = false;
			accion = actSEMITURN_L;
		}else if (pasoPosible(origen,sensores,2)){
			accion = actFORWARD;
		} else {
			accion = actSEMITURN_R;
			sigueMuro = false;
		}
	} else{                                             
		accion = accionPorDefecto(origen,mapaResultado,sensores);
	}
	

	return false;
}



pair<int,int> ComportamientoJugador::rayos(const estado &origen){
	pair<int,int> objetivoSur,objetivoEste,objetivoNorte,objetivoOeste, objetivoNoreste, objetivoNoroeste, objetivoSureste, objetivoSuroeste;
	int fila=origen.fila, columna=origen.columna;
	int suma =0, sumaSur=0, sumaEste=0, sumaNorte=0, sumaOeste=0, sumaNoreste=0, sumaNoroeste=0, sumaSureste=0, sumaSuroeste=0;
	
	while (fila < FIL_POTENCIAL-3){
		fila++;
		if (mapaResultado[fila][columna] == '?'){
			sumaSur++;
			objetivoSur.first = fila;
			objetivoSur.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;

	while (columna < COL_POTENCIAL-3){
		columna++;
		if (mapaResultado[fila][columna] == '?'){
			sumaEste++;
			objetivoEste.first = fila;
			objetivoEste.second = columna;
		}
	}
	fila=origen.fila, columna=origen.columna;
	while (fila >2){
		fila--;
		if (mapaResultado[fila][columna] == '?'){
			sumaNorte++;
			objetivoNorte.first = fila;
			objetivoNorte.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;
	while (columna > 2){
		columna--;
		if (mapaResultado[fila][columna] == '?'){
			sumaOeste++;
			objetivoOeste.first = fila;
			objetivoOeste.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;

	/*--------------------------------*/

	while (columna < COL_POTENCIAL-3 && fila>2){
		fila--; columna++;
		if (mapaResultado[fila][columna] == '?'){
			sumaNoreste++;
			objetivoNoreste.first = fila;
			objetivoNoreste.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;

	while (columna >2 && fila>2){
		fila--; columna--;
		if (mapaResultado[fila][columna] == '?'){
			sumaNoroeste++;
			objetivoNoroeste.first = fila;
			objetivoNoroeste.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;

	while (columna < COL_POTENCIAL-3 && fila <FIL_POTENCIAL-3){
		fila++; columna++;
		if (mapaResultado[fila][columna] == '?'){
			sumaSureste++;
			objetivoSureste.first = fila;
			objetivoSureste.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;

	while (columna >2 && fila <FIL_POTENCIAL-3){
		fila++; columna--;
		if (mapaResultado[fila][columna] == '?'){
			sumaSuroeste++;
			objetivoSuroeste.first = fila;
			objetivoSuroeste.second = columna;
		}
		
	}
	fila=origen.fila, columna=origen.columna;
	

	int maximo = max(max(sumaSur,sumaEste), max(sumaNorte,sumaOeste));

	if (maximo <= 3){
		maximo = max ( max(max(sumaSureste,sumaSuroeste), max(sumaNoreste,sumaNoroeste)), max(max(sumaSur,sumaEste), max(sumaNorte,sumaOeste)));
	}

	if (maximo == sumaSur)
		return objetivoSur;
	else if (maximo == sumaEste)
		return objetivoEste;
	else if(maximo == sumaNorte)
		return objetivoNorte;
	else if (maximo == sumaOeste)
		return objetivoOeste;
	else if (maximo == sumaNoreste)
		return objetivoNoreste;
	else if (maximo == sumaNoroeste)
		return objetivoNoroeste;
	else if (maximo == sumaSureste)
		return objetivoSureste;
	else
		return objetivoSuroeste;
	
}

bool estaEnLista(list<estado> lista, estado es){
	bool esta=false;
	for (auto it = lista.begin(); it != lista.end() && !esta; it++)
	{
		if (it->fila == es.fila && it->columna == es.columna)
			esta=true;
	}
	return esta;	
}

estado ComportamientoJugador::cercanoLista(list<estado> lista, int fila, int columna){
	int diferenciaMinima = 2e6;
	for (auto it= lista.begin(); it!=lista.end(); it++){
		if (!estaEnLista(listBateriasEncontradas, *it)){
			int difActual= max(abs(fila - it->fila), abs(columna - it->columna));

			if (difActual<diferenciaMinima){
				bateria = *it;
				diferenciaMinima=difActual;
			}
		}
	}
	return bateria;
}

void ComportamientoJugador::switchAccion (){
	switch (ultimaAccion)
		{
		case actFORWARD:
			actualizaSegunOrientacion(fil,col,orient);
			
			break;
		
		case actSEMITURN_R:
			orient = (orient+1)%8;
			break;
		case actTURN_R:
			orient = (orient+2)%8;
			break;
		
		case actTURN_L:
			orient=(orient+6)%8;
			break;
	
		case actSEMITURN_L:
			orient = (orient+7)%8;
			break;
		default:
			break;
	}

}

void actualizaSegunOrientacion(int &fil, int &col, int &orient){
	switch (orient){
		case 0:
			fil--;
			break;
		case 1:
			fil--;
			col++;
			break;
		case 2:
			col++;
			break;
		case 3:
			fil++;
			col++;
			break;
		case 4:
			fil++;
			break;
		case 5:
			fil++;
			col--;
			break;
		case 6:
			col--;
			break;
		case 7:
			fil--;
			col--;
			break;
	}	
}

bool ComportamientoJugador::pathFinding_BuscaObjetivos(const estado &origen, const list<estado> &destinos, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapaR, Sensores sensores){
	bool salida=false;
	estado objetivoCercano = cercanoLista(destinos, actual.fila, actual.columna);

	if (sensores.superficie[2] != '_'){
		int f=fil, c=col; 
		actualizaSegunOrientacion(f,c,orient);
		unsigned char ant = mapaAux[f][c];
		mapaAux[f][c]='M';
		hayPlan = pathFinding_AlgoritmoA(origen, objetivoCercano, plan,mapaAux);
		mapaAux[f][c] = ant;
	} else
		hayPlan = pathFinding_AlgoritmoA(origen, objetivoCercano, plan,mapaR);
	
	return hayPlan;
}

void ComportamientoJugador::iniciaMapaAux(){
	vector<unsigned char> aux_Potencial(mapaResultado.size(), '?');
	for (size_t i = 0; i < mapaResultado[0].size(); i++)
	{
		mapaAux.push_back(aux_Potencial);
	} 
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}


