#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

	// actualizar variable actual
	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;

	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	// si no hay plan, construye uno
	if (!hayPlan){
		hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
	}


	Action sigAccion;
	if (hayPlan && plan.size() > 0){ // hay un plan no vacio
		accion = plan.front();	 // tomo la sig accion del plan
		plan.erase(plan.begin());	 // eliminamos la acción del plan
	} else
		cout << "No se pudo encontrar un plan" << endl;

	
	return accion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
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
		return pathFinding_CostoUniforme(origen,un_objetivo,plan,mapaResultado);
		break;
	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
		cout << "No implementado aun\n";
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		cout << "No implementado aun\n";
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
	estado mejorPadre;
	list <estado> hijos;

};

struct ComparaEstadosA_Nodos
{
    bool operator()(const nodoA_ &b, const nodoA_ &c) const
    {
    
        estado a = b.st;
        estado n = c.st;
        if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
            (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas > n.zapatillas ) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.zapatillas == n.zapatillas and a.bikini > n.bikini ))
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

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;				  // Lista de Abiertos

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

void actualizaPathCost(nodoA_ & nodo, vector< vector< unsigned char> > mapaR, Action accion, const estado &destino, double gPadre){
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
	nodo.g = valor + gPadre;
	double x = nodo.st.fila - destino.fila;
	double y = nodo.st.columna - destino.columna;

	nodo.h = (int)sqrt(pow(x,2)+pow(y,2));

	

	

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
void actualizaNodosHijosCerrados(nodoA_ actual, nodoA_ &inicial, int diferencia, set<nodoA_, ComparaEstadosA_Nodos> &Cerrados, priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> &Abiertos, set<nodoA_,ComparaEstadosA_Nodos> &SetAbiertos) {
	int siz = actual.hijos.size();
	for(auto it=actual.hijos.begin(); it != actual.hijos.end(); it++){
		nodoA_ n; n.st = *it;
		auto cerrado = Cerrados.find(n);
		if (cerrado != Cerrados.end()){
			n.g = cerrado->g - diferencia; 
			n.h=cerrado->h;
			n.mejorPadre=inicial.st;
			n.hijos = cerrado->hijos;
			n.secuencia = cerrado->secuencia;

			Cerrados.erase(*cerrado);
			Cerrados.insert(n);
			
			inicial.hijos.push_back(n.st);

			actualizaNodosHijosCerrados(n, inicial, diferencia, Cerrados, Abiertos, SetAbiertos);
		} else {
			auto abierto= SetAbiertos.find(n);
			n.g = abierto->g - diferencia; 
			n.h=abierto->h;
			n.mejorPadre=inicial.st;
			n.hijos = abierto->hijos;
			n.secuencia = abierto->secuencia;
			
			actualizaNodoAbierto(Abiertos,SetAbiertos, *it);
			
			Abiertos.push(n);
			SetAbiertos.insert(n);
		}
	}

}


void insertaNodo(nodoA_ &hijo, nodoA_ &current, const estado &destino,  vector< vector< unsigned char> > mapaR, set<nodoA_, ComparaEstadosA_Nodos> &Cerrados, // Lista de Cerrados
	priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> &Abiertos, set<nodoA_,ComparaEstadosA_Nodos> &SetAbiertos, Action accion){
	// Generar descendiente de girar a la derecha 90 grados
		
		hijo.mejorPadre = current.st;
		list<estado> a;
		hijo.hijos = a;
		
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
		
		//Abiertos.push(hijo);
		set<nodoA_,ComparaEstadosA_Nodos>::iterator existeAbierto = SetAbiertos.find(hijo);
		set<nodoA_,ComparaEstadosA_Nodos>::iterator existeCerrado = Cerrados.find(hijo);	

		if (existeAbierto != SetAbiertos.end()){
			if (existeAbierto->g+existeAbierto->h > hijo.g+hijo.h)
				actualizaNodoAbierto(Abiertos,SetAbiertos,hijo.st);
			
		}else if (existeCerrado != Cerrados.end()){
			if (existeCerrado->g+existeCerrado->h > hijo.g+hijo.h){
				int diferencia = existeCerrado->g - hijo.g;
				//actualizaNodosHijosCerrados(*existeCerrado, hijo, diferencia, Cerrados, Abiertos, SetAbiertos);
				Cerrados.erase(*existeCerrado);
				Cerrados.insert(hijo);
			}
			
		} else{
			SetAbiertos.insert(hijo);
			Abiertos.push(hijo);
		}

		current.hijos.push_back(hijo.st);
}

void sacaNodo(nodoA_ n){
	cout << n.g << " " << n.h << "|" << n.st.fila << " "<< n.st.columna << " "<< n.st.orientacion << endl;


}



bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > mapaR){
	cout << "Calculando plan\n";
	bool salida = false;
	plan.clear();
	set<nodoA_, ComparaEstadosA_Nodos> Cerrados; // Lista de Cerrados
	priority_queue<nodoA_,vector<nodoA_>, ComparaDistanciaA_Nodos> Abiertos;			  // Lista de Abiertos
	set<nodoA_,ComparaEstadosA_Nodos> SetAbiertos;

	nodoA_ current;
	current.st = origen;
	current.secuencia.empty();
	int zz=0;



	Abiertos.push(current);
	SetAbiertos.insert(current);
	while (!Abiertos.empty()){
		
		SetAbiertos.erase(Abiertos.top());
		Abiertos.pop();


		cout << current.st.fila << " " << current.st.columna << " - " << current.st.orientacion << endl;

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
		//sacaNodo(hijoTurnR);

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
			current = Abiertos.top();
		}
	}

	
		cout << "No encontrado plan\n";
	





	return salida;
}


int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
