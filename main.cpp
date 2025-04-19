#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <unistd.h>
#include <cfloat>
#include <iomanip>

using namespace std;

// Define Vertex structure
struct Vertex {
    int index;
    float key;
    int pi;
    Vertex() : key(numeric_limits<float>::max()), pi(-1) {}
};
//Node Structure Define
struct Node {
    int index;
    int u;
    int v;
    float w;
    Node* next;
    Node(int idx, int x, int y, float weight, Node* nxt) : index(idx), u(x), v(y), w(weight), next(nxt) {}
};

// MinHeap for priority queue operations in Dijkstras Algorithm.
class MinHeap {
private:
    vector<Vertex*> heapArray;
    vector<int> position; //index of vertices inside heap
    int cap;
    int size;

    void swap(int i, int j) {
        Vertex* temp = heapArray[i];
        heapArray[i] = heapArray[j];
        heapArray[j] = temp;
        position[heapArray[i]->index] = i;
        position[heapArray[j]->index] = j;
    }
//Heapify UP
    void heapifyUp(int i) {
        while (i != 0 && heapArray[(i - 1) / 2]->key > heapArray[i]->key) {
            swap(i, (i - 1) / 2);
            i = (i - 1) / 2;
        }
    }
//Heapify DOWN
    void heapifyDown(int i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < size && heapArray[left]->key < heapArray[smallest]->key)
            smallest = left;
        if (right < size && heapArray[right]->key < heapArray[smallest]->key)
            smallest = right;
        if (smallest != i) {
            swap(i, smallest);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap(int capacity) : cap(capacity), size(0) {
        heapArray.resize(capacity);
        position.resize(capacity, -1);
    }

    void insert(Vertex* v) {
        if (size == cap) {
            cout << "Heap is full" << endl;
            return;
        }
        heapArray[size] = v;
        position[v->index] = size;
        heapifyUp(size);
        size++;
    }

    Vertex* extractMin() {
        if (size == 0){
            return nullptr;
        }
        Vertex* minVertex = heapArray[0];
        heapArray[0] = heapArray[size - 1];
        position[heapArray[0]->index] = 0;
        size--;
        heapifyDown(0);
        position[minVertex->index] = -1;
        return minVertex;
    }

    void decreaseKey(Vertex* vertex, float newKey) {
        int i = position[vertex->index];
        if (vertex->key > newKey) {
            vertex->key = newKey;
            heapifyUp(i);
        }
    }

    bool isEmpty() const {
        return size == 0;
    }
};

// Function to build the AdjacencyList
vector<Node*> buildAdjacencyList(const string& filename, bool isDirected, int flag, int vertCount) {
    vector<Node*> adjList(vertCount, nullptr);
    ifstream file(filename);
    string line;

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << "Current working directory: " << cwd << endl;
    } else {
        perror("getcwd() error");
    }

    cout << "Opening file: " << filename << endl;

    if (!file.is_open()) {
        cout << "Error: File not found: " << filename << endl;
        return adjList;
    }

    while (getline(file, line)) {
        istringstream iss(line);
        int index, u, v;
        float w;
        if(!(iss >> index >> u >> v >> w)){
            //cout << "Error reading the line: " << line << endl; //For testing purposes
            continue;
        }
        cout << "Read edge: " << u << " -> " << v << " (weight: " << w << ")" << endl;
        Node* node = new Node(index, u, v, w, nullptr);
        if (flag == 1) {  // Insert at front
            node->next = adjList[u-1];
            adjList[u - 1] = node;
        } else {  // Insert at end
            Node** ptr = &adjList[u - 1];
            while (*ptr) ptr = &((*ptr)->next);
            *ptr = node;
        }
        if (!isDirected) { //Basically when == UD or ud (not D or d)
            Node* node = new Node(index, v, u, w, nullptr);
            if (flag == 1) {
                node->next = adjList[v - 1];
                adjList[v - 1] = node;
            } else {
                Node** ptr = &adjList[v - 1];
                while (*ptr) ptr = &((*ptr)->next);
                *ptr = node;
            }
        }
    }
    file.close();
    return adjList;
}

//Print Adjacency List
void printAdjList(const vector<Node*>& adjList) {
    for (size_t i = 0; i < adjList.size(); ++i) {
        cout << "ADJ[" << i+1 << "]:";
        for (Node* node = adjList[i]; node; node = node->next) {
            cout << "-->[" << node->u << " " << node->v << ": " << fixed << setprecision(2) << node->w << "]";
            }
        cout << endl;
    }
}

//clear Adjacency List
void clearAdjList(vector<Node*>& adjList) {
    for (size_t i = 0; i < adjList.size(); ++i) {
        Node* node = adjList[i];
        while (node) {
            Node* temp = node;
            node = node->next;
            delete temp;
        }
    }
}

// Relax which is used by Dijkstras algorithm
void relax(Vertex& u, Vertex& v, float weight) {
    if (v.key > u.key + weight) {
        v.key = u.key + weight;
        v.pi = u.index;
    }
}

// Dijkstras algorithm to find shortest path from single source
void dijkstra(vector<Vertex>& vertices, const vector<Node*>& adjList, int source) {
    vertices[source - 1].key = 0;
    MinHeap pq(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        pq.insert(&vertices[i]);
    }
    while (!pq.isEmpty()) {
        Vertex* u = pq.extractMin();
        for (Node* node = adjList[u->index]; node; node = node->next) {
            Vertex& v = vertices[node->v - 1];
            float newKey = u->key + node->w;
            if (v.key > newKey) {
                relax(*u, v, node->w);
                pq.decreaseKey(&v, newKey);
            }
        }
    }
}

//prints the path from source to destination
void printPath(const vector<Vertex>& vertices, int source, int dest) {
    if (vertices[dest - 1].pi == -1) {
        cout << "No path from " << source << " to " << dest << endl;
        return;
    }
    stack<int> path;
    for (int at = dest; at != source; at = vertices[at - 1].pi) {
        path.push(at);
    }
    path.push(source);
    cout << "Path from " << source << " to " << dest << ": ";
    while (!path.empty()) {
        cout << path.top() << (path.size() > 1 ? " -> " : "");
        path.pop();
    }
    cout << endl;
}

//prints length of the path from source to destination
void printLength(const vector<Vertex>& vertices, int source, int dest) {
    if (vertices[dest-1].pi == -1 || vertices[dest-1].key == std::numeric_limits<float>::max()) {
        cout << "No path from " << source << " to " << dest << endl;
    } else {
        cout << "Length from " << source << " to " << dest << ": " << vertices[dest].key << endl;
    }
}

// Main function, handles menu and user choices
int main() {
    string com, filename;
    char directChar;
    bool directed;
    int flag, vertCount, sourcing, dest;
    vector<Node*> adjList;
    vector<Vertex> vertices;
    int lastSource = -1;
    int lastDest = -1;

    while (true) {
        cout << "Graph Pathfinding Tool - Dijkstra's Algorithm by Andre Ashworth" << endl;
        cout << "1. Read graph from file (enter filename, D or UD, + flag)" << endl;
        cout << "2. Print ADJ" << endl;
        cout << "3. Single Source, enter starting node number" << endl;
        cout << "4. Single Pair, enter starting and ending node numbers" << endl;
        cout << "5. Print Path, enter starting and ending node numbers" << endl;
        cout << "6. Print Length, enter the starting and ending node numbers" << endl;
        cout << "7. Print Path, enter the starting and ending node numbers" << endl;
        cout << "8. Stop" << endl;
        cout << "Enter your choice between (1-8): ";
        cin >> com;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        try {
            int choice = stoi(com);
            switch (choice) {
                case 1:
                    cout << "Enter filename, D or UD, flag of (1 or 2) (Ex: network01.txt D 2): ";
                    cin >> filename >> directChar >> flag;
                    cout << "Enter number of vertices: ";
                    cin >> vertCount;
                    directed = (directChar = 'D' || directChar == 'd');
                    adjList = buildAdjacencyList(filename, directed, flag, vertCount);
                    vertices.resize(vertCount);
                    for (int i = 0; i < vertCount; i++) {
                        vertices[i].index = i;
                    }
                    lastSource = -1;
                    lastDest = -1;
                    break;
                case 2:
                    printAdjList(adjList);
                    break;
                case 3:
                    cout << "Enter the starting node number: ";
                    cin >> sourcing;
                    dijkstra(vertices, adjList, sourcing);
                    lastSource = sourcing;
                    lastDest = -1;
                    break;
                case 4:
                    cout << "Enter the starting and ending node numbers: ";
                    cin >> sourcing >> dest;
                    dijkstra(vertices, adjList, sourcing);
                    printPath(vertices, sourcing, dest);
                    lastSource = sourcing;
                    lastDest = dest;
                    break;
                case 5:
                    cout << "Enter the starting and ending node numbers for path: ";
                    cin >> sourcing >> dest;
                    if(lastSource == sourcing){
                        if(lastDest == dest){
                        printPath(vertices, sourcing, dest);
                        }else{
                            cout << "There is no path from " << sourcing << " to " << dest << "." << endl;
                        }
                    }else{
                        cout << "There is no path from " << sourcing << " to " << dest << "." << endl;
                    }
                    break;
                case 6:
                    cout << "Enter the starting and ending node numbers for length: ";
                    cin >> sourcing >> dest;
                    if(lastSource == sourcing){
                        if(lastDest == dest){
                        printLength(vertices, sourcing, dest);
                        }else{
                            cout << "There is no path from " << sourcing << " to " << dest << "." << endl;
                        }
                    }else{
                        cout << "There is no path from " << sourcing << " to " << dest << "." << endl;
                    }
                    break;
                case 7:
                    cout << "Enter the starting and ending node numbers for path: ";
                    cin >> sourcing >> dest;
                    if(lastSource == sourcing){
                        printPath(vertices, sourcing, dest);
                    }else{
                        cout << "No path from " << sourcing << " to " << dest << endl;
                    }
                    break;
                case 8:
                    cout << "Program Stopping." << endl;
                    clearAdjList(adjList); // Cleanup before exiting
                    return 0;
                default:
                    cout << "Enter a choice (1-8)." << endl;
                    break;
            }
        } catch (const invalid_argument& e) {
            cout << "Invalid Input, try again" << endl;
        }
    }
    return 0;
}
