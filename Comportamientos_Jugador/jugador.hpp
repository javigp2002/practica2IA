#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool bikini = false;
  bool zapatillas =false;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      resetMapaPotencial();
      pintaPrecipicios();
      resetMapaRecorrido();
      iniciaMapaAux();
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      resetMapaPotencial();
      resetMapaRecorrido();
      pintaPrecipicios();
      iniciaMapaAux();

    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}



  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan, planVacio;
    bool hayPlan = false, inicio=true;
    const int FIL_POTENCIAL = mapaResultado.size();
    const int COL_POTENCIAL = mapaResultado[0].size(), VAL_MAX_SALIDA = 1e5;
    const int MAX_MAPA = mapaResultado.size();
    int num_iteracion=0;
    bool buscaSalida=false,  sigueMuro = false;
    int fil,col, orient;
    Action ultimaAccion;

    std::vector< std::vector<int> > mapaPotencial, mapaRecorrido; 
    std::vector< std::vector<unsigned char> > mapaAux;
    estado bateria;
    list<estado>listBaterias, listBateriasEncontradas;
    bool buscaBateria=false, estoyAccionBasica=false;
    const int VAL_MAX_BATERIA = 3000;
    const int VAL_MIN_BATERIA = 1250;


    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, Sensores sensores, Action &accion);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AlgoritmoA(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapaR);
    bool pathFinding_DescubreMapa(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapaR, Sensores sensores, Action &accion);
    bool pathFinding_BuscaObjetivos(const estado &origen, const list<estado> &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapaR, Sensores sensores);

    void actualizaMapaPotencialMinimo(int fil_obj, int col_obj,  int &minimo,  pair<int,int> &objetivo, estado est);
    pair<int,int> inicializaMapaPotencialMinimo(estado est);
    void resetMapaPotencial();
    void resetMapaRecorrido();
    void iniciaMapaAux();

    void PintaPlan(list<Action> plan);
    void pintaPrecipicios();
    bool HayObstaculoDelante(estado &st);
    void actualizarVistaMapa(Sensores sensores,  const estado &actual,  vector< vector< unsigned char> > & mapa);

    Action accionPorDefecto(const estado &origen,  std::vector< std::vector< unsigned char> > mapaR, Sensores sensores);
    Action accionBasica(const estado &origen,  std::vector< std::vector< unsigned char> > mapaR, Sensores sensores);

    bool pasoPosible (const estado &origen, Sensores sensores, int sen);
    void switchAccion();
    estado cercanoLista(list<estado> lista,int fila, int columna);

    pair<int,int> rayos(const estado &origen);
};

#endif
