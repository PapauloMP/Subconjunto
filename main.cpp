#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <tuple>
#include <iomanip>
#include <chrono>
#include "Headers/Graph.h"
#include "Headers/Node.h"

using namespace std;

Graph* leitura(ifstream& input_file, int directed, int weightedEdge, int weightedNode){

    //Variáveis para auxiliar na criação dos nós no Grafo
    int idNodeSource;
    int idNodeTarget;
    int order;

    //Pegando a ordem do grafo
    input_file >> order;

    //Criando objeto grafo
    Graph* graph = new Graph(directed, weightedEdge, weightedNode);

    //Leitura de arquivo

    if(!graph->getWeightedEdge() && !graph->getWeightedNode()){

        while(input_file >> idNodeSource >> idNodeTarget) {
            graph->insertEdge(idNodeSource, idNodeTarget, 0);

        }

    }else if(graph->getWeightedEdge() && !graph->getWeightedNode() ){

        float edgeWeight;

        while(input_file >> idNodeSource >> idNodeTarget >> edgeWeight) {
            graph->insertEdge(idNodeSource, idNodeTarget, edgeWeight);

        }

    }else if(graph->getWeightedNode() && !graph->getWeightedEdge()){

        float nodeSourceWeight, nodeTargetWeight;

        while(input_file >> idNodeSource >> nodeSourceWeight >> idNodeTarget >> nodeTargetWeight) {
            graph->insertEdge(idNodeSource, idNodeTarget, 0);
            graph->getNode(idNodeSource)->setWeight(nodeSourceWeight);
            graph->getNode(idNodeTarget)->setWeight(nodeTargetWeight);

        }

    }else if(graph->getWeightedNode() && graph->getWeightedEdge()){

        float nodeSourceWeight, nodeTargetWeight, edgeWeight = 0;

        while(input_file >> idNodeSource >> nodeSourceWeight >> idNodeTarget >> nodeTargetWeight) {
            graph->insertEdge(idNodeSource, idNodeTarget, edgeWeight);
            graph->getNode(idNodeSource)->setWeight(nodeSourceWeight);
            graph->getNode(idNodeTarget)->setWeight(nodeTargetWeight);

        }

    }

    return graph;
}

Graph* leituraInstancia(ifstream& input_file, int directed, int weightedEdge, int weightedNode){

    //Variáveis para auxiliar na criação dos nós no Grafo
    int idNodeSource;
    int idNodeTarget;
    int order;
    //int numEdges;

    //Pegando a ordem do grafo
    input_file >> order; //>> numEdges;

    //Criando objeto grafo
    Graph* graph = new Graph(directed, weightedEdge, weightedNode);

    //Leitura de arquivo
    while(input_file >> idNodeSource >> idNodeTarget) {
        graph->insertEdge(idNodeSource, idNodeTarget, 0);

    }

    return graph;
}

int menu(){

    int selecao;

    cout << "MENU" << endl;
    cout << "----" << endl;
    cout << "[1] Algoritmo guloso" << endl;
    cout << "[2] Algoritmo guloso randomizado adaptativo" << endl;
    cout << "[3] Algoritmo guloso randomizado adaptativo reativo" << endl;
    cout << "[0] Sair" << endl;

    cin >> selecao;

    return selecao;

}

void selecionar(int selecao, Graph* graph, ofstream& output_file){

    chrono::steady_clock::time_point start;
    chrono::steady_clock::time_point end;
    chrono::steady_clock::duration elapsed;

    switch (selecao) {
        case 1: {
            start = chrono::steady_clock::now();

            auto solution = graph->greedy();

            end = chrono::steady_clock::now();
            elapsed  = end - start;

            graph->printDominatingSet(output_file, &solution, 1);

            break;
        }
        case 2: {
            float alpha[3] = {0.15, 0.30, 0.50};

            vector<Node*> solution;

            int option = 0;
            cout << "ESCOLHA O VALOR DE ALFA:" << endl;
            cout << "[1] - 0.15" << endl;
            cout << "[2] - 0.30" << endl;
            cout << "[3] - 0.50" << endl;

            cin >> option;

            start = chrono::steady_clock::now();

            switch(option){
                case 1: solution = graph->greedyRandom(alpha[0], 500); break;
                case 2: solution = graph->greedyRandom(alpha[1], 500); break;
                case 3: solution = graph->greedyRandom(alpha[2], 500); break;
            }

            end = chrono::steady_clock::now();
            elapsed  = end - start;

            graph->printDominatingSet(output_file, &solution, 2);

            break;
        }
        case 3: {
            start = chrono::steady_clock::now();

            float alpha[5] = {0.05, 0.10, 0.15, 0.30, 0.50};

            vector<Node*> solution;

            solution = graph->greedyRandomReactive(alpha, 5, 250, 2500);

            end = chrono::steady_clock::now();
            elapsed  = end - start;

            graph->printDominatingSet(output_file, &solution, 3);

            break;
        }

    }
    if(selecao >= 1 && selecao <= 3) {
        cout << endl << "TEMPO DE PROCESSAMENTO TOTAL: " << chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        cout << "ms" << endl;
    }

}

int mainMenu(ofstream& output_file, Graph* graph){

    int selecao = 1;

    while(selecao != 0){
        //system("clear");
        selecao = menu();

        if(output_file.is_open())
            selecionar(selecao, graph, output_file);

        else
            cout << "Unable to open the output_file" << endl;

        output_file << endl;

    }

    return 0;
}

Graph* leituraSubconjuntoDominante(ifstream& input_file, int directed,  int weightedEdge, int weightedNode){
    if(directed || !weightedNode){
        cout << "ERRO: grafo nao satisfaz as condicoes!" << endl;
        return nullptr;
    }

    string discard;
    getline(input_file, discard); // descartando "NumberOfNodes:".

    string auxOrder;
    getline(input_file, auxOrder);
    int order = stoi(auxOrder);

    for (int i = 0; i <= order; i++) {
        getline(input_file, discard); // descartando toda a seção "Positions".
    }
    getline(input_file, discard); // descartando "***WEIGHTS***"

    Graph* graph = new Graph(directed, weightedEdge, weightedNode);

    string auxWeight;
    float weight;
    for (int i = 0; i < order; i++) {
        graph->insertNode(i);
        getline(input_file, auxWeight);
        weight = stof(auxWeight);
        graph->getLastNode()->setWeight(weight);
    }

    getline(input_file, discard); //descartando "***CONNECTIONS***".

    Node* node = graph->getFirstNode();
    Node* target_node;
    for (int i = 0; i < order; i++) {
        //como o grafo não é direcionado, a matriz é simétrica,
        //logo basta apenas percorre os campos abaixo ou acima da diagonal principal,
        //no caso, abaixo.
        for (int j = 0; j <= i; j++) {
            getline(input_file, discard, ' ');
            if(discard == "1"){
                //função de insertEdge do grafo realiza uma busca sequencial no grafo pelo id do nó,
                //no entanto, como já temos os nós essa função foi evitada, sendo substituida
                //pelos passos abaixo
                node->insertEdge(j, 1); //todas as arestas apresentam peso 1.
                graph->incrementNumberEdges();
                //InDegree usado como grau do vértice para grafos nao direcionados
                node->incrementInDegree();

                target_node = graph->getNode(j);
                target_node->insertEdge(i, 1);
                target_node->incrementInDegree();
            }
        }
        getline(input_file, discard);
        node = node->getNextNode();
    }

    return graph;
}

void imprimeMatriz(ofstream& output_file, Graph* graph){
    output_file << "ORDER: " << graph->getOrder() << endl;

    output_file << "MATRIZ DE ADJACENCIAS:" << endl;
    for(Node* auxNode = graph->getFirstNode(); auxNode != nullptr; auxNode = auxNode->getNextNode()){
        for (int j = 0; j < graph->getOrder(); ++j) {
            if(auxNode->hasEdgeBetween(j) == nullptr){
                output_file << "0 ";
            }
            else
                output_file << "1 ";
        }
        output_file << endl;
    }

}

int main(int argc, char const *argv[]) {

    //Verificação se todos os parâmetros do programa foram entrados
    if (argc != 6) {

        cout << "ERROR: Expecting: ./<program_name> <input_file> <output_file> <directed> <weighted_edge> <weighted_node> " << endl;
        return 1;

    }

    string program_name(argv[0]);
    string input_file_name(argv[1]);

    string instance;


    //Abrindo arquivo de entrada
    ifstream input_file;
    ofstream output_file;
    input_file.open(input_file_name, ios::in);
    output_file.open(argv[2], ios::out | ios::trunc);



    Graph* graph;

    if(input_file.is_open()){
        graph = leituraSubconjuntoDominante(input_file, stoi(argv[3]), stoi(argv[4]), stoi(argv[5]));

    }else {
        cout << "Unable to open " << argv[1];
        abort();
    }
    //imprimeMatriz(output_file, graph);
    srand(time(nullptr));
    mainMenu(output_file,graph);

    //Fechando arquivo de entrada
    input_file.close();

    //Fechando arquivo de saída
    output_file.close();

    return 0;
}

