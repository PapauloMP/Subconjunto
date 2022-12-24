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

