#include "../Headers/Graph.h"
#include <iostream>
#include <stack>
#include <algorithm>
#include <list>
#include <cfloat>

using namespace std;

/**************************************************************************************************
 * Defining the Graph's methods
**************************************************************************************************/

// Constructor
Graph::Graph(bool directed, bool weighted_edge, bool weighted_node)
{

    this->order = 0;
    this->directed = directed;
    this->weighted_edge = weighted_edge;
    this->weighted_node = weighted_node;
    this->first_node = this->last_node = nullptr;
    this->number_edges = 0;
}

// Destructor
Graph::~Graph()
{

    Node *next_node = this->first_node;

    while (next_node != nullptr)
    {

        next_node->removeAllEdges();
        Node *aux_node = next_node->getNextNode();
        delete next_node;
        next_node = aux_node;
    }
}

// Getters
int Graph::getOrder()
{

    return this->order;
}
int Graph::getNumberEdges()
{

    return this->number_edges;
}

bool Graph::getDirected()
{

    return this->directed;
}

bool Graph::getWeightedEdge()
{

    return this->weighted_edge;
}

bool Graph::getWeightedNode()
{

    return this->weighted_node;
}

Node *Graph::getFirstNode()
{

    return this->first_node;
}

Node *Graph::getLastNode()
{

    return this->last_node;
}

// Other methods
/*
    The outdegree attribute of nodes is used as a counter for the number of edges in the graph.
    This allows the correct updating of the numbers of edges in the graph being directed or not.
*/
Node* Graph::insertNode(int id)
{
    auto node = new Node(id);

    if(this->getFirstNode() == nullptr){
        this->first_node = this->last_node = node;
    }
    else{
        this->getLastNode()->setNextNode(node);
        this->last_node = node;
    }
    this->order++;
    return node;
}

void Graph::insertEdge(int id, int target_id, float weight)
{
    auto source_node = this->getNode(id);
    auto target_node = this->getNode(target_id);
    if(source_node == nullptr){
        source_node = this->insertNode(id);
    }
    if(target_node == nullptr){
        target_node = this->insertNode(target_id);
    }

    if (!source_node->searchEdge(target_node->getId())){ ///evita multiarestas
        source_node->insertEdge(target_node->getId(), weight);
        if(directed)
            source_node->incrementOutDegree();
        else
            source_node->incrementInDegree();
        target_node->incrementInDegree();
        this->number_edges++;
    }
    if (!this->directed){ /// se não é direcionado adiciona uma outra aresta com sentido contrário
        if (!target_node->searchEdge(source_node->getId())) {
            target_node->insertEdge(source_node->getId(), weight);
        }
    }
}

void Graph::removeFirstNode()
{
    auto node = this->getFirstNode();
    this->first_node = node->getNextNode();

    if(this->last_node == node)
    {
        this->last_node = this->first_node;
    }
    delete node;
}
void Graph::removeLastNode()
{
    auto node = this->getFirstNode();

    if(node != last_node) {

        while (node->getNextNode() != last_node) {
            node = node->getNextNode();
        }
    }
    else{
        delete last_node;
        first_node = last_node = nullptr;
    }
    node->setNextNode(nullptr);
    delete last_node;
    last_node = node;
}
///------------------
void Graph::removeNode(int id)
{
    if (searchNode(id)) { //verifica se o nó está no grafo

        Node* targetNode = getNode(id);
        // Primeiro remove as arestas que apontam para o nó, para então excluí-lo
        for(Node* auxNode = getFirstNode(); auxNode != nullptr; auxNode = auxNode->getNextNode()){
            if(auxNode->removeEdge(id, getDirected(), targetNode)){
                number_edges--;
            }
        }

        auto node = this->getFirstNode();
        if (first_node->getId() == id) {
            this->number_edges -= first_node->getOutDegree();
            removeFirstNode();
        } else if (last_node->getId() == id) {
            this->number_edges -= last_node->getOutDegree();
            removeLastNode();
        } else {
            while (node->getNextNode() != nullptr) {

                if (node->getNextNode()->getId() == id) {
                    auto tobeRemoved = node->getNextNode();
                    node->setNextNode(tobeRemoved->getNextNode());
                    this->number_edges -= tobeRemoved->getOutDegree();
                    delete tobeRemoved;
                }
                node = node->getNextNode();
            }
        }

        this->order--;
    }
    else {
        cout << "Id invalido" << endl;
    }
}

bool Graph::searchNode(int id)
{
    auto node = this->getFirstNode();

    while(node != nullptr)
    {
        if(node->getId() == id)
        {
            return true;
        }
        node = node->getNextNode();
    }

    return false;
}

Node *Graph::getNode(int id)
{
    auto node = this->getFirstNode();

    while(node != nullptr)
    {
        if(node->getId() == id)
        {
            return node;
        }
        node = node->getNextNode();
    }

    return node;
}
Graph* Graph::operator&(Graph *graph) {

    Graph* intersection = new Graph(this->getDirected(), this->getWeightedEdge(), this->getWeightedNode());

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        int node_id = node->getId();
        for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
            int target_id = edge->getTargetId();
            if(graph->searchNode(node_id) && graph->searchNode(target_id)) // verifica se há intersecção de nós
                if(graph->getNode(node_id)->searchEdge(target_id)) // verifica se há intersecção de arestras
                    intersection->insertEdge(node->getId(), edge->getTargetId(), edge->getWeight());
        }
    }

    return intersection;
}

Graph* Graph::operator+(Graph* graph) {

    Graph* sum = new Graph(this->getDirected(), this->getWeightedEdge(), this->getWeightedNode());

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
            sum->insertEdge(node->getId(), edge->getTargetId(), edge->getWeight());
        }
    }
    for(auto node = graph->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
            sum->insertEdge(node->getId(), edge->getTargetId(), edge->getWeight());
        }
    }
    return sum;
}

Graph* Graph::operator-(Graph* graph) {

    Graph* diff = new Graph(this->getDirected(), this->getWeightedEdge(), this->getWeightedNode());

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
            if(!graph->searchNode(node->getId()) && !graph->searchNode(edge->getTargetId()))
                diff->insertEdge(node->getId(), edge->getTargetId(), edge->getWeight());
        }
    }

    return diff;
}

std::stack<Node*> Graph::pert() {
    stack<Node*> solution;

    if(!this->getDirected()){
        cout << "ERRO: grafo nao direcionado!" << endl;
        return solution;
    }

    bool hasCycle = true;
    auto alfa = new float[this->getOrder()];
    bool* inserted = new bool[this->getOrder()];

    for (int i = 0; i < this->getOrder(); ++i)
        inserted[i] = false;

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        if(node->getInDegree() == 0){       //identifica e coloca os nós iniciais (alfa = 0)
            alfa[node->getId()] = 0;
            inserted[node->getId()] = true;
            if(hasCycle) {
                hasCycle = DFS(node, inserted);
            }
        }
    }
    if(hasCycle) {
        cout << "ERRO: grafo cíclico!" << endl;
        return solution;
    }
    else
        for (int i = 0; i < this->getOrder(); ++i)
            inserted[i] = false;

    ///CALCULANDO ALFA

    bool finish = false;

    while(!finish) {
        finish = true;

        for (auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
            //para cada nó ainda não inserido no vetor verificar se todos os seus predecessores ja foram inseridos
            //se sim, inserí-lo em alfa com o valor calculado.
            if (!inserted[node->getId()]) {
                finish = false;
                int cont = 0;
                float maxAlfa = 0;
                float edgeWeight;

                for (auto auxNode = this->getFirstNode(); auxNode != nullptr; auxNode = auxNode->getNextNode()) {
                    //identificando os predecessores e checando se estão inseridos na lista
                    if (inserted[auxNode->getId()]) {
                        for (auto edge = auxNode->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
                            if (edge->getTargetId() == node->getId()) {
                                edgeWeight = edge->getWeight();
                                if(maxAlfa < (alfa[auxNode->getId()] + edgeWeight)) {
                                    maxAlfa = alfa[auxNode->getId()] + edgeWeight;
                                }
                                cont++;
                            }
                        }
                    }
                }
                if (cont == node->getInDegree()) {
                    alfa[node->getId()] = maxAlfa;
                    inserted[node->getId()] = true;
                }
            }
        }
    }

    ///CALCULANDO BETA

    auto beta = new float[this->getOrder()];
    auto delta = new float[this->getOrder()];
    float cost = 0;

    for (int i = 0; i < this->getOrder(); ++i) {
        inserted[i] = false;
        if(alfa[i] > cost)
            cost = alfa[i];
    }

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        if(node->getOutDegree() == 0){                                   //identifica e coloca os nós terminais
            beta[node->getId()] = alfa[node->getId()];                  // (beta == 0) na solução
            inserted[node->getId()] = true;
            solution.push(node);
            delta[node->getId()] = 0;
        }
    }

    finish = false;
    while(!finish){
        finish = true;
        for (auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
            //para cada nó ainda não inserido no vetor verificar se todos os seus sucessores ja foram inseridos
            //se sim, inserí-lo no vetor com o valor de alfa calculado.
            if (!inserted[node->getId()]) {
                finish = false;
                int cont = 0;
                auto minBeta = cost;
                float edgeWeight;

                //identificando os sucessores e checando se estão inseridos na lista
                for (auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
                    if (inserted[edge->getTargetId()]) {
                        edgeWeight = edge->getWeight();
                        if (minBeta > (beta[edge->getTargetId()] - edgeWeight)) {
                            minBeta = beta[edge->getTargetId()] - edgeWeight;
                        }
                        cont++;
                    }
                }

                if (cont == node->getOutDegree()) {
                    beta[node->getId()] = minBeta;
                    delta[node->getId()] = beta[node->getId()] - alfa[node->getId()];
                    if(delta[node->getId()] == 0){
                        solution.push(node);
                    }
                    inserted[node->getId()] = true;
                }

            }
        }
    }

    cout << "Custo (tempo): " << cost << endl;

    delete [] alfa;
    delete [] beta;
    delete [] delta;
    delete [] inserted;

    return solution;
}

bool Graph::DFS(Node* node, bool* marked){ //função usada para verificar ciclo
    bool flag;
    for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
        if(!marked[edge->getTargetId()]){
            marked[edge->getTargetId()] = true;
            flag = DFS(this->getNode(edge->getTargetId()), marked);
            if (flag)
                return true;
            marked[edge->getTargetId()] = false;
        }
        else{
            return true;
        }
    }
    return false;
}


void Graph::topologicalSort(){

    if(!this->getDirected()){
        cout << "ERRO: grafo nao direcionado!" << endl;
        return;
    }

    vector<Node*> initialNodes;
    stack<Node*> pertResult;
    bool* closed = new bool[this->getOrder()];
    bool* marked = new bool[this->getOrder()];
    bool flag;

    for(auto node = this->getFirstNode(); node != nullptr; node = node->getNextNode()) {
        if(node->getInDegree() == 0)
            initialNodes.push_back(node);
    }
    if(initialNodes.empty()){
        cout << "ERRO: grafo cíclico!" << endl;
        return;
    }

    for (int i = 0; i < this->getOrder(); ++i)
        closed[i] = false;


    for (auto node : initialNodes) {
        flag = topologicalSortRec(node, marked, closed, pertResult);
        if(flag) {
            cout << "ERRO: grafo cíclico!" << endl;
            return;
        }
    }

    cout << "Ordem de execução:" << endl;
    if(!pertResult.empty()) {
        cout << pertResult.top()->getId();
        pertResult.pop();
    }
    while(!pertResult.empty()){
        cout << "->" << pertResult.top()->getId();
        pertResult.pop();
    }
    cout << endl;

    delete [] closed;
    delete [] marked;
}

bool Graph::topologicalSortRec(Node* node, bool* marked, bool* closed, stack<Node*>& stack){
    bool flag;
    for(auto edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
        if(!marked[edge->getTargetId()]){
            marked[edge->getTargetId()] = true;
            flag = topologicalSortRec(this->getNode(edge->getTargetId()), marked, closed, stack);
            if (flag)
                return true;
            marked[edge->getTargetId()] = false;
        }
        else{
            return true;
        }
    }
    if(!closed[node->getId()]) {
        closed[node->getId()] = true;
        stack.push(node);
    }
    return false;
}

/// -----AUXILIARES PARA OS ALGORITMOS GULOSOS-----

struct compareNodes{
    bool operator()(Node* node1, Node* node2) {
        return node1->getInDegree() < node2->getInDegree();
    }
};

bool compareNode(Node* const x, Node* const y){
    return x->getInDegree() > y->getInDegree();
}

std::priority_queue<Node*, std::vector<Node*>, compareNodes> orderCandidates2(Graph* graph){
    std::priority_queue<Node*, std::vector<Node*>, compareNodes> candidates;
    for (Node* node = graph->getFirstNode(); node != nullptr; node = node->getNextNode()){
        candidates.push(node);
    }
    return candidates;
}

std::vector<Node*> orderCandidates(Graph* graph){
    std::vector<Node*> candidates;
    for (Node* node = graph->getFirstNode(); node != nullptr; node = node->getNextNode()){
        candidates.push_back(node);
    }
    std::sort(candidates.begin(), candidates.end(), compareNode);
    return candidates;
}

///-----ALGORITMO GULOSO-----

std::vector<Node*> Graph::greedy() {

    auto LC = orderCandidates(this);
    std::vector<Node*> solution;

    bool* reached = new bool[this->order];
    for (int i = 0; i < this->order; ++i) {
        reached[i] = false;
    }

    int cont = 0;
    float quality = 0; //soma dos pesos dos nós na solução
    Node* bestNode;

    int i = 0;
    while(cont != this->order){
        bestNode = LC.at(i);
        solution.push_back(bestNode);
        quality += bestNode->getWeight();


        if(!reached[bestNode->getId()]) {
            cont++;
            reached[bestNode->getId()] = true;
        }

        for (Edge* edge = bestNode->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()){
            if(!reached[edge->getTargetId()]) {
                cont++;
                reached[edge->getTargetId()] = true;
            }
        }
        i++;
    }

    std::cout << "QUALIDADE: " << quality << std::endl;

    return solution;
}

///-----ALGORITMO GULOSO RANDOMIZADO-----

int getRandomIndex(float alpha, int maxRange){
    int range = (int)((float)maxRange*alpha);
    if (range == 0)
        return 0;
    else
        return rand() % range;
}

std::vector<Node*> Graph::greedyRandom(float alpha, int numIter){
    auto ordered = orderCandidates(this);
    bool* reached = new bool[this->order];
    Node* bestNode;
    std::vector<Node*> bestSolution;
    float bestQuality = 0;

    for (int i = 0; i < numIter; i++) {
        int cont = 0;
        float quality = 0;
        auto LC = ordered;
        std::vector<Node*> solution;

        for (int j = 0; j < this->order; j++) {
            reached[j] = false;
        }
        int index;
        while (cont != this->order) {
            index = getRandomIndex(alpha, (int)LC.size());
            bestNode = LC.at(index);


            solution.push_back(bestNode);
            quality += bestNode->getWeight();

            if (!reached[bestNode->getId()]) {
                cont++;
                reached[bestNode->getId()] = true;
            }

            for (Edge *edge = bestNode->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
                if (!reached[edge->getTargetId()]) {
                    cont++;
                    reached[edge->getTargetId()] = true;
                }
            }

            LC.erase(LC.cbegin() + index);
            //i++;
        }

        if(quality < bestQuality || bestQuality == 0) {
            bestSolution = solution;
            bestQuality = quality;
        }
    }

    std::cout << "QUALIDADE: " << bestQuality << std::endl;

    return bestSolution;
}

///-----ALGORITMO GULOSO RANDOMIZADO REATIVO-----

void updateProbabilities(float *probabilities, float* alphaAverage, float qualityBestSol, int numAlfas){
    float sumProbabilities = 0;
    for (int i = 0; i < numAlfas; ++i) {
        float q = qualityBestSol / alphaAverage[i]; // Qi = (F(S*)/Ai)
        probabilities[i] = q;
        sumProbabilities += probabilities[i];
    }
    for (int i = 0; i < numAlfas; ++i) {
        probabilities[i] = probabilities[i] / sumProbabilities;  // Pi = Qi/SUM1->m(Qj)
    }
}

int getAlpha(float* probabilities,int numAlphas){
    int alpha = -1;
    float random;
    float infLimit = 0;
    float upperLimit = 0;

    random = (float)(rand() % 100);

    for (int i = 0; i < numAlphas; ++i) {
        upperLimit += probabilities[i];

        //verifica se random pertence ao intervalo de determinado alfa
        if(random >= infLimit*100 && random <= upperLimit*100) {
            alpha = i;
            break;
        }
        else
            infLimit += probabilities[i];
    }

    return alpha;
}

std::vector<Node *> Graph::greedyRandomReactive(float* alpha, int alphaSize, int block, int numIter) {
    auto ordered = orderCandidates(this);
    bool* reached = new bool[this->order];
    Node* bestNode;
    std::vector<Node*> bestSolution;
    float bestQuality = 0;
    int bestAlpha;

    float probabilities[alphaSize]; //vetor de probabilidades
    float alphaAverage[alphaSize]; //vetor de qualidade média para cada alfa

    //auxiliares para a média
    float sumQualities[alphaSize]; //somatório das qualidades das soluções
    int usesForAlpha[alphaSize]; //armazena o número de vezes que o alfa foi usado
    for (int i = 0; i < alphaSize; i++) {
        alphaAverage[i] = 1;
        probabilities[i] = 1/(float)alphaSize;
        usesForAlpha[i] = 0;
        sumQualities[i] = 0;
    }
    int alphaIndex = 0;

    for (int i = 0; i < numIter; i++) {

        if(i % 10 == 0 && i != 0){
            updateProbabilities(probabilities, alphaAverage, bestQuality, alphaSize);
            cout << "PROBABILIDADES: " << endl;
            for (int j = 0; j < alphaSize; ++j) {
                cout << "Probabilidade[" << j << "]: " << probabilities[j] << endl;
            }
        }

        int cont = 0;
        float quality = 0;
        auto LC = ordered;
        std::vector<Node*> solution;

        for (int j = 0; j < this->order; j++) {
            reached[j] = false;
        }

        int index;
        while (cont != this->order) {
            alphaIndex = getAlpha(probabilities, alphaSize);
            usesForAlpha[alphaIndex]++;
            index = getRandomIndex(alpha[alphaIndex], (int)LC.size());
            bestNode = LC.at(index);


            solution.push_back(bestNode);
            quality += bestNode->getWeight();

            if (!reached[bestNode->getId()]) {
                cont++;
                reached[bestNode->getId()] = true;
            }

            for (Edge *edge = bestNode->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {
                if (!reached[edge->getTargetId()]) {
                    cont++;
                    reached[edge->getTargetId()] = true;
                }
            }

            sumQualities[alphaIndex] += quality*quality;
            alphaAverage[alphaIndex] = sumQualities[alphaIndex] / (float)usesForAlpha[alphaIndex];

            LC.erase(LC.cbegin() + index);
            //i++;
        }

        if(quality < bestQuality || bestQuality == 0) {
            bestSolution = solution;
            bestQuality = quality;
            bestAlpha = alphaIndex;
        }
    }

    std::cout << "--MELHOR SOLUÇÃO--" << bestQuality << std::endl;
    std::cout << "QUALIDADE: " << bestQuality << std::endl;
    std::cout << "ALFA: " << alpha[bestAlpha] << std::endl;

    return bestSolution;
}


/*
//Function that verifies if there is a path between two nodes
bool Graph::depthFirstSearch(int initialId, int targetId){
    
}


void Graph::breadthFirstSearch(ofstream &output_file){
    
}


Graph *Graph::getComplement(){
    
}

    

//A function that returns a subjacent of a directed graph, which is a graph which the arcs have opposite directions to the original graph
Graph* Graph::getSubjacent(){
    
}

bool Graph::connectedGraph(){
    
}



bool Graph::hasCircuit(){
    
}



float** Graph::floydMarshall(){
    
}

   

float* Graph::dijkstra(int id){
    
}

*/