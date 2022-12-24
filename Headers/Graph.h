/**************************************************************************************************
 * Implementation of the TAD Graph
**************************************************************************************************/

#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED
#include "Node.h"
#include <fstream>
#include <stack>
#include <queue>
#include <list>

using namespace std;

class Graph{

    //Atributes
    private:
        int order;
        int number_edges;
        bool directed;
        bool weighted_edge;
        bool weighted_node;
        Node* first_node;
        Node* last_node;

    public:
        //Constructor
        Graph(bool directed, bool weighted_edge, bool weighted_node);
        //Destructor
        ~Graph();
        //Getters
        int getOrder();
        int getNumberEdges();
        bool getDirected();
        bool getWeightedEdge();
        bool getWeightedNode();
        Node* getFirstNode();
        Node* getLastNode();
        //Other methods
        Node* insertNode(int id);
        void insertEdge(int id, int target_id, float weight);
        void removeNode(int id);
        void removeFirstNode();
        void removeLastNode();
        bool searchNode(int id);
        Node* getNode(int id);
        void incrementNumberEdges(){ this->number_edges++; }

        //functions
        void printGraph(ofstream& outputFile);
        Graph* operator&(Graph* graph); //GRAFO INTERSECÇÃO
        Graph* operator+(Graph* graph); //GRAFO UNIÃO
        Graph* operator-(Graph* graph); //GRAFO DIFERENÇA
        std::stack<Node*> pert();       //PERT
        void printIntersection(ofstream &outputFile);
        void printSum(ofstream &outputFile);
        void printDifference(ofstream &outputFile);
        void printPert(ofstream &outputFile, std::stack<Node*>& pertPath);
        void topologicalSort();
        void printDominatingSet(ofstream &outputFile);
        std::vector<Node*> greedy();
        std::vector<Node*> greedyRandom(float alfa, int numIter);
        std::vector<Node*> greedyRandomReactive(float* alfa, int alfaSize, int block, int numIter);


        /*
        bool depthFirstSearch(int initialId,int targetId);
        void breadthFirstSearch(ofstream& output_file);
        Graph* getComplement();
        Graph* getSubjacent();
        bool hasCircuit();
        bool connectedGraph();
        float** floydMarshall();
        float* dijkstra(int id);*/



    private:
        //Auxiliar methods
        bool DFS(Node* node, bool* marked);
        bool topologicalSortRec(Node* node, bool* marked, bool* closed, stack<Node*>& stack);
};

#endif // GRAPH_H_INCLUDED
