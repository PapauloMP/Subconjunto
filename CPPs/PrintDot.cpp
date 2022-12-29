//
// Created by Rafael on 08/10/2022.
//
#include "../Headers/Graph.h"
#include <iostream>
#include <stack>
#include <iomanip>

string getEdgeShape(bool isDirected) {
    if (isDirected)
        return " -> ";
    else
        return " -- ";
}

string getGraphType(bool directed, bool isStrict){
    string graphType = directed ? "digraph" : "graph";
    string strictName = isStrict ? "strict" : "";
    return strictName + " " + graphType;
}

string getWeightLabel(bool isWeighted, float weight) {
    stringstream stream;
    stream << fixed << setprecision(2) << weight;
    string weightStr = isWeighted ? " [label=" + stream.str() + "]" : "";
    return weightStr;
}

void Graph::printGraph(ofstream& outputFile) {
    string edgeShape = getEdgeShape(this->getDirected());

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
            string weightStr = getWeightLabel(this->getWeightedEdge(), edge->getWeight());
            outputFile << "\t\t" << node->getId() << edgeShape << this->getNode(edge->getTargetId())->getId();
            outputFile << weightStr << ";" << endl;
        }
    }
}

void Graph::printIntersection(ofstream& outputFile){
    string graphType = getGraphType(directed, true);
    outputFile << graphType << " intersecção{" << endl;
    outputFile << "\tsubgraph cluster_0{" << endl;
    outputFile << "\t\tlabel=\"Grafo intersecção\";" << endl;
    printGraph(outputFile);
    outputFile << "\t}" << endl;
    outputFile << "}" << endl;
}

void Graph::printSum(ofstream& outputFile){
    string graphType = getGraphType(directed, true);
    outputFile << graphType << " soma{" << endl;
    outputFile << "\tsubgraph cluster_0{" << endl;
    outputFile << "\t\tlabel=\"Grafo união\";" << endl;
    printGraph(outputFile);
    outputFile << "\t}" << endl;
    outputFile << "}" << endl;
}

void Graph::printDifference(ofstream &outputFile) {
    string graphType = getGraphType(directed, true);
    outputFile << graphType << " diferença{" << endl;
    outputFile << "\tsubgraph cluster_0{" << endl;
    outputFile << "\t\tlabel=\"Grafo diferença\";" << endl;
    printGraph(outputFile);
    outputFile << "\t}" << endl;
    outputFile << "}" << endl;
}

void Graph::printPert(ofstream &outputFile, std::stack<Node*>& pertPath){
    if(pertPath.empty()){
        cout << "Caminho vazio" << endl;
        return;
    }
    string edgeAttribute = "[color=red,penwidth=3.0]";
    string graphType = getGraphType(directed, true);
    outputFile << graphType << " pert{" << endl;
    outputFile << "\trankdir = \"LR\";" << endl;
    outputFile << "\tsubgraph cluster_0{" << endl;
    outputFile << "\t\tlabel=\"Grafo PERT (CPM)\";" << endl;
    printGraph(outputFile);

    cout << "Caminho crítico:" << endl;
    while(!pertPath.empty()){
        cout << pertPath.top()->getId() << "->";
        Node* n = pertPath.top();
        pertPath.pop();

        if(!pertPath.empty()) {
            outputFile << "\t\t" << n->getId() << "->" << pertPath.top()->getId() << edgeAttribute << endl;
        }
    }
    cout << "||" << endl;

    outputFile << "\t}" << endl;
    outputFile << "}" << endl;
}

void Graph::printDominatingSet(ofstream &outputFile, std::vector<Node*>* solution, int option){
    string graphType = getGraphType(directed, true);
    string name;
    string clusterName;
    switch(option){
        case 1: {
            name = " greedy{";
            clusterName = "Algoritmo guloso";
        break;
        }
        case 2: {
            name = " greedyRandomized{";
            clusterName = "Algoritmo guloso randomizado";
        break;
        }
        case 3: {
            name = " greedyRandomizedReactive{";
            clusterName = "Algoritmo guloso randomizado reativo";
        break;
        }
    }
    outputFile  << graphType << name << endl;
    outputFile  << "\t" << "subgraph cluster_0{" << endl;
    outputFile  << "\t\tlabel=\"" << clusterName <<"\";" << endl;
    outputFile  << "\t\tnode [style=filled];" << endl;
    outputFile  << "\t\tnode [colorscheme=rdylgn5];" << endl;

    vector<Node*>::iterator it;
    float cost = 0;
    cout << "Subconjunto dominante: ";

    for (it = solution->begin(); it != solution->end(); it++) {
        outputFile << "\t\t" << (*it)->getId() << "[fillcolor=1];" << endl;
        cost += (*it)->getWeight();
        cout << (*it)->getId() << ", ";
        for (Edge* auxEdge = (*it)->getFirstEdge(); auxEdge != nullptr; auxEdge = auxEdge->getNextEdge()) {
            outputFile << "\t\t" << (*it)->getId()<< "--" << auxEdge->getTargetId() << ";" << endl;
        }
    }
    cout << endl;
    outputFile << "\"CUSTO = " << cost << "\"[fillcolor=5][shape=note];" << endl;
    outputFile << "\t}" << endl;
    outputFile << "}" << endl;
}


