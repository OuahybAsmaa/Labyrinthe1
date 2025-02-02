#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <limits.h>
#include <utility>
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
typedef pair<int, int> CellPosition;

// Directions possibles (haut, bas, gauche, droite)
const int Directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

// Fonction pour vérifier si une cellule est valide
bool IsCellValid(int row, int col, int totalRows, int totalCols, const vector<vector<int>>& gridMap) {
    return row >= 0 && row < totalRows && col >= 0 && col < totalCols && gridMap[row][col] != 1;
}

// Algorithme de Dijkstra
vector<CellPosition> FindPathDijkstra(const vector<vector<int>>& gridMap, CellPosition startCell, CellPosition endCell, vector<vector<int>>& distanceMatrix) {
    int totalRows = gridMap.size();
    int totalCols = gridMap[0].size();

    distanceMatrix.assign(totalRows, vector<int>(totalCols, INT_MAX));
    distanceMatrix[startCell.first][startCell.second] = 0;

    vector<vector<CellPosition>> previousCell(totalRows, vector<CellPosition>(totalCols, { -1, -1 }));

    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> priorityQueue;
    priorityQueue.push({ 0, startCell.first, startCell.second });

    while (!priorityQueue.empty()) {
        auto currentNode = priorityQueue.top();
        priorityQueue.pop();

        int currentDistance = std::get<0>(currentNode);
        int currentRow = std::get<1>(currentNode);
        int currentCol = std::get<2>(currentNode);

        cout << "Processing cell: (" << currentRow << ", " << currentCol << ")" << endl;

        if (currentRow == endCell.first && currentCol == endCell.second) {
            vector<CellPosition> path;
            for (CellPosition at = endCell; at != startCell; at = previousCell[at.first][at.second]) {
                path.push_back(at);
            }
            path.push_back(startCell);
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& direction : Directions) {
            int newRow = currentRow + direction[0];
            int newCol = currentCol + direction[1];

            if (IsCellValid(newRow, newCol, totalRows, totalCols, gridMap)) {
                int newDistance = distanceMatrix[currentRow][currentCol] + 1;

                if (newDistance < distanceMatrix[newRow][newCol]) {
                    distanceMatrix[newRow][newCol] = newDistance;
                    previousCell[newRow][newCol] = { currentRow, currentCol };
                    priorityQueue.push({ newDistance, newRow, newCol });
                    cout << "Adding cell: (" << newRow << ", " << newCol << ")" << endl;
                }
            }
        }
    }

    cout << "No path found!" << endl;
    return {}; // Aucun chemin trouvé
}

// Implémentation de BFS
vector<CellPosition> FindPathBFS(const vector<vector<int>>& gridMap, CellPosition startCell, CellPosition endCell, vector<vector<int>>& distanceMatrix) {
    int totalRows = gridMap.size();
    int totalCols = gridMap[0].size();

    queue<CellPosition> cellQueue;
    distanceMatrix.assign(totalRows, vector<int>(totalCols, -1));
    distanceMatrix[startCell.first][startCell.second] = 0;
    cellQueue.push(startCell);

    vector<vector<CellPosition>> previousCell(totalRows, vector<CellPosition>(totalCols, { -1, -1 }));

    while (!cellQueue.empty()) {
        int currentRow = cellQueue.front().first;
        int currentCol = cellQueue.front().second;
        cellQueue.pop();

        if (currentRow == endCell.first && currentCol == endCell.second) {
            vector<CellPosition> path;
            for (CellPosition at = endCell; at != startCell; at = previousCell[at.first][at.second]) {
                path.push_back(at);
            }
            path.push_back(startCell);
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& direction : Directions) {
            int newRow = currentRow + direction[0];
            int newCol = currentCol + direction[1];

            if (IsCellValid(newRow, newCol, totalRows, totalCols, gridMap) && distanceMatrix[newRow][newCol] == -1) {
                distanceMatrix[newRow][newCol] = distanceMatrix[currentRow][currentCol] + 1;
                previousCell[newRow][newCol] = { currentRow, currentCol };
                cellQueue.push({ newRow, newCol });
            }
        }
    }

    return {}; // Aucun chemin trouvé
}

// Implémentation de DFS (Depth-First Search)
vector<CellPosition> FindPathDFS(const vector<vector<int>>& gridMap, CellPosition startCell, CellPosition endCell, vector<vector<int>>& distanceMatrix) {
    int totalRows = gridMap.size();
    int totalCols = gridMap[0].size();

    stack<CellPosition> cellStack;
    distanceMatrix.assign(totalRows, vector<int>(totalCols, -1));
    distanceMatrix[startCell.first][startCell.second] = 0;
    cellStack.push(startCell);

    vector<vector<CellPosition>> previousCell(totalRows, vector<CellPosition>(totalCols, { -1, -1 }));

    while (!cellStack.empty()) {
        int currentRow = cellStack.top().first;
        int currentCol = cellStack.top().second;
        cellStack.pop();

        if (currentRow == endCell.first && currentCol == endCell.second) {
            vector<CellPosition> path;
            for (CellPosition at = endCell; at != startCell; at = previousCell[at.first][at.second]) {
                path.push_back(at);
            }
            path.push_back(startCell);
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& direction : Directions) {
            int newRow = currentRow + direction[0];
            int newCol = currentCol + direction[1];

            if (IsCellValid(newRow, newCol, totalRows, totalCols, gridMap) && distanceMatrix[newRow][newCol] == -1) {
                distanceMatrix[newRow][newCol] = distanceMatrix[currentRow][currentCol] + 1;
                previousCell[newRow][newCol] = { currentRow, currentCol };
                cellStack.push({ newRow, newCol });
            }
        }
    }

    return {}; // Aucun chemin trouvé
}

// Point d'entrée principal
int main() {
    httplib::Server server;

    server.Post("/pathfinding", [](const httplib::Request& request, httplib::Response& response) {
        cout << "Received POST request to /pathfinding" << endl;

        try {
            // Parse JSON request
            auto jsonData = json::parse(request.body);
            cout << "Received data: " << jsonData.dump(2) << endl; // Log des données reçues

            // Extraire les données
            vector<vector<int>> gridMap = jsonData["grid"];
            CellPosition startCell = { jsonData["start"]["row"], jsonData["start"]["col"] };
            CellPosition endCell = { jsonData["end"]["row"], jsonData["end"]["col"] };
            string algorithm = jsonData["algorithm"];

            if (gridMap.empty() || gridMap[0].empty()) {
                response.status = 400;
                response.set_content("{\"error\": \"Invalid grid data\"}", "application/json");
                return;
            }

            // Exécuter l'algorithme
            vector<CellPosition> path;
            vector<vector<int>> distanceMatrix;
            if (algorithm == "dijkstra") {
                path = FindPathDijkstra(gridMap, startCell, endCell, distanceMatrix);
            }
            else if (algorithm == "bfs") {
                path = FindPathBFS(gridMap, startCell, endCell, distanceMatrix);
            }
            else if (algorithm == "dfs") {
                path = FindPathDFS(gridMap, startCell, endCell, distanceMatrix);
            }
            else {
                response.status = 400;
                response.set_content("{\"error\": \"Unsupported algorithm\"}", "application/json");
                return;
            }

            // Log du chemin trouvé
            cout << "Path found: " << path.size() << " steps" << endl;
            for (const auto& cell : path) {
                cout << "(" << cell.first << ", " << cell.second << ") ";
            }
            cout << endl;

            // Préparer la réponse
            json responseJson = json::object();
            json pathJson = json::array();
            for (const auto& cell : path) {
                pathJson.push_back({ {"row", cell.first}, {"col", cell.second} });
            }
            responseJson["path"] = pathJson;
            responseJson["distances"] = distanceMatrix;
            response.set_content(responseJson.dump(), "application/json");
        }
        catch (const exception& error) {
            cerr << "Error: " << error.what() << endl;
            response.status = 500;
            response.set_content("{\"error\": \"Internal server error\"}", "application/json");
        }
        });

    cout << "Server is listening on localhost:8080" << endl;
    server.listen("localhost", 8080);

    return 0;
}