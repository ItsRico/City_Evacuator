#include <cstdlib>
#include <iostream> 

#include "evac.h"
#include "EvacRunner.h"

using namespace std;

int RatioCmp(const void *ptr, const void *ptr2)
{
    if(((EvacInfo*)ptr)->ratio > ((EvacInfo*)ptr2)->ratio)
        return -1;
    else
    if(((EvacInfo*)ptr)->ratio < ((EvacInfo*)ptr2)->ratio)
        return 1;
    else
        return 0;
}
 
Evac::Evac(City *citie, int numCitie, int numRoads) : numCities(numCitie)
{
    cities = new CityInfo[numCities];
    edge = new RoadEdge[numRoads* 3];
    usedRoads = new int[numRoads * 3]; 
    visitedIDs = new int[numCities];
    int id;
    int numEdges;
    int i = 0;

    while(i < numCities)
    {
        id = citie[i].ID;
        cities[id].evacPeople = 0;
        cities[id].maxPeople = citie[i].population;
        cities[id].numEdges = numEdges = citie[i].roadCount;
        cities[id].edgeID = new unsigned int[numEdges];
        int roadID = 0;
        for(int j = 0; j < numEdges; j++)
        {
            cities[id].edgeID[j] = roadID = citie[i].roads[j].ID;
            edge[roadID].destCityID = citie[i].roads[j].destinationCityID;
            edge[roadID].capacity =  citie[i].roads[j].peoplePerHour;
        } 
        i++;
    }
} // Evac()  Constructs our graph based on passed in parameters

void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{
    hour = 1;
    routeCount = 0;
    int evacQ[numCities];
    front = 0;
    back = 0;
    EvacInfo evacCities[numEvacs];
    int id;
    int i = 0;

    while(i < numEvacs)
    {
        id = evacIDs[i];
        
        cities[id].known = true;
        cities[id].deadEnd = false;
        cities[id].evacuatedCity = true;
        
        cities[id].depth = 1;
        evacQ[back] = id;
        evacCities[i].ID = id;
        back++;
        i++;
    } //    finds our augmented paths for our evac city

    bool peopleRemaining = true;
    visitedSize = 0;

    do
    { 
        int offset = back, ID, destCityID;   // allows function to be called more than once 

        for(int i = 0; i < back; i++)       //function -> initialize false?
            cities[evacQ[i]].deadEnd = false; 

        while(front < offset)
        {
            ID = evacQ[front++];
            
            for(int j = 0; j < cities[ID].getNumEdges() ;)  //increments if 
            {
                destCityID = edge[cities[ID].edgeID[j]].destCityID;
                
                if(cities[destCityID].getDepth() != 0)
                {
                    if(cities[destCityID].getDepth() < hour - 1)
                    {
                         cities[ID].numEdges--;
                         cities[ID].edgeID[j] = (unsigned) cities[ID].edgeID[cities[ID].numEdges];
                         cities[ID].edgeID[cities[ID].numEdges] = (unsigned) cities[ID].edgeID[j];
                    }
                    else
                    {
                        j++;
                        continue;
                    }
                } 
                else if(!cities[destCityID].known)
                {
                    evacQ[back++] = destCityID;        
                    cities[ID].depth = hour + 1;
                    cities[destCityID].known = true;
                    continue;
                }
                else 
                {
                  j++;
                  continue;
                }
            }
        } 

        int flow = 0;
        int i = 0;
        if(peopleRemaining)
        {
              
              peopleRemaining = false; 
              
              while(i < numEvacs)
              {
    
                  int ID = evacCities[i].ID;
                  if(cities[ID].evacPeople < cities[ID].getMaxPeople())
                  {
        
                      peopleRemaining = true; // change later
                      flow = 0;
                      
                      //while(j < cities[ID].getNumEdges())
                      for(int j = 0; j < cities[ID].getNumEdges(); j++)
                      {

                          if(edge[cities[ID].edgeID[j]].capacity < cities[edge[cities[ID].edgeID[j]].destCityID].maxPeople)
                          {
                                flow += edge[cities[ID].edgeID[j]].capacity; 
                                continue;
                          }

                          flow += cities[edge[cities[ID].edgeID[j]].destCityID].getMaxPeople();
                         
                      } // calculate each edge capacity
                      
                      evacCities[i].ratio = (cities[ID].maxPeople - cities[ID].evacPeople) / flow;  // calc -> ratio
                  
                  } // not done
                  i++;
              } // for remaining cities

              if(peopleRemaining) 
                  qsort(evacCities, numEvacs, sizeof(EvacInfo), RatioCmp); // sort by max ratio, lets us pick our next 

        }

        usedCount = 0;
        for(int i = 0; i < numEvacs; i++)
        {
            int cityID = evacCities[i].ID;
            int total = 0;
            int difference = 0;
            int needed = cities[cityID].maxPeople - cities[cityID].evacPeople;

            while(visitedSize > 0) // mark backroad not visited
            {
              visitedSize -= 1;
                cities[visitedIDs[visitedSize]].visited = false;
                
            }
            
              visitedIDs[visitedSize++] = cityID;
              visitedSize += 1;
              cities[cityID].visited = true;

              int edgeID; 
              int maxFlow;
            
            for(int i = 0; i < cities[cityID].getNumEdges() && total < needed; i++)
            {
                edgeID = cities[cityID].edgeID[i];
                maxFlow = edge[edgeID].capacity - edge[edgeID].active;
                difference = needed - total;

                if(maxFlow > difference)     
                    maxFlow = difference;
                
                maxFlow = dfs(edge[edgeID].destCityID, maxFlow, cityID);
                edge[edgeID].active += maxFlow;
                total += maxFlow;
                if(maxFlow > 0)
                    usedRoads[usedCount++] = edgeID;
            
            }  
      
            cities[cityID].evacPeople += total;
        
        } 
   

        i = 0;
        
        while(i < numCities)
        {
          cities[i].visited =0;
          i++;
        }
 
        i = 0;
        while(i < usedCount)
        {
            
            evacRoutes[routeCount].roadID = usedRoads[i];
            evacRoutes[routeCount].numPeople = edge[usedRoads[i]].active;
            evacRoutes[routeCount].time = hour;
            routeCount += 1;
            edge[usedRoads[i]].active = 0;
            cities[edge[usedRoads[i]].destCityID].visited = 0;
            i++;
        }
          hour++;
    }while(peopleRemaining);

} // eacuate

int Evac::dfs(int cityID, int remainder, int sourceCityID)
{
    if(!cities[cityID].visited)
    {
      cities[cityID].visited = true;
      visitedIDs[visitedSize++] = cityID;
      visitedSize += 1;
    }
    else
      return 0;

    int sum = 0; 
    int difference = 0;

    if(!cities[cityID].evacuatedCity) // if not marked evac then return amount 
    {
        difference = cities[cityID].maxPeople - cities[cityID].evacPeople;

        if(remainder > difference)
        {
            sum = difference;
            cities[cityID].evacPeople = cities[cityID].getMaxPeople();
        }
        else 
        {
            cities[cityID].evacPeople += remainder;
        }

        if(remainder <= difference)
        {
          return remainder;
        }
    } 
    if(cities[cityID].deadEnd)
    {
        return sum;
    }

    int currentMax;
    for(int i = 0; i < cities[cityID].getNumEdges() && sum < remainder; i++)
    {
        if(edge[cities[cityID].edgeID[i]].destCityID == sourceCityID)
            continue;  
        
        currentMax = edge[cities[cityID].edgeID[i]].capacity - edge[cities[cityID].edgeID[i]].active;
        if(currentMax > (remainder - sum))
          currentMax  = remainder - sum;
        
        currentMax = dfs(edge[cities[cityID].edgeID[i]].destCityID, currentMax, cityID);
        edge[cities[cityID].edgeID[i]].active += currentMax;
        sum += currentMax;

        if(currentMax > 0)
            usedRoads[usedCount++] = cities[cityID].edgeID[i];    
    }
    return sum;
} 
