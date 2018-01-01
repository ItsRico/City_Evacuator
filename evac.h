#ifndef evacH
#define evacH
#include "EvacRunner.h"

class CityInfo
{
    public:
      bool known;
      bool deadEnd;
      bool evacuatedCity; 
      unsigned int *edgeID; 
      int maxPeople;
      int evacPeople;
      int numEdges;
      int depth;
      bool visited;
      CityInfo():known(false), deadEnd(true), evacuatedCity(false), evacPeople(0), visited(false) {}

  int getNumEdges() {return numEdges;};
  int getDepth(){return depth;};
  int getMaxPeople() {return maxPeople;};
};// graph representation 

class RoadEdge
{
    public:
      int destCityID;
      int capacity;
      int active;
      RoadEdge():active(0) {}
}; 

class EvacInfo
{
    public:
      int ID;
      float ratio;
};  //info we will save about our evacuation routes for future routes 


class Evac
{
    CityInfo *cities;
    RoadEdge *edge;
    EvacInfo *evacCities;

    int numCities;
    int hour;

    int *evacQ, back, front;
    int *visitedIDs, visitedSize;
    int *usedRoads, usedCount;
  
    public:
        int dfs(int cityID, int needed, int sourceCityID);
        Evac(City *cities, int numCities, int numRoads);
        void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes, int &routeCount); 
};  


#endif
