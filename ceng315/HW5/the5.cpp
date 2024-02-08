#include "the5.h"


bool contains(const std::vector<int>& vec, int value) {
    for (int element : vec) {
        if (element == value) {
            return true;
        }
    }
    return false;
}

void findCyclicDependencies(int node, const std::vector<std::vector<int>>& dependencyMatrix,
                            std::vector<bool>& visited, std::vector<int>& currentPath,
                            std::vector<std::vector<int>>& cyclicDependencies);

void mergeCyclicDependencies(std::vector<std::vector<int>>& cyclicDependencies);

void run(const std::vector<std::vector<int>>& dependencyMatrix, bool& isCompilable,
         std::vector<int>& compileOrder, std::vector<std::vector<int>>& cyclicDependencies) {
    int n = dependencyMatrix.size();
    isCompilable = true;
    compileOrder.clear();
    cyclicDependencies.clear();

    std::vector<int> inDegree(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dependencyMatrix[i][j] == 1) {
                inDegree[j]++;
            }
        }
    }

    std::vector<int> queue;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            queue.push_back(i);
        }
    }

    while (!queue.empty()) {
        int current = -1;
        for (int i : queue) {
            if (inDegree[i] == 0) {
                if (current == -1 || i < current) {
                    current = i;
                }
            }
        }

        if (current == -1) {
            isCompilable = false;
            break;
        }
        int indexToRemove = -1;
        for (size_t i = 0; i < queue.size(); ++i) {
            if (queue[i] == current) {
                indexToRemove = static_cast<int>(i);
                break;
            }
        }

        if (indexToRemove != -1) {
            queue.erase(queue.begin() + indexToRemove);
        }

        compileOrder.push_back(current);

        for (int i = 0; i < n; ++i) {
            if (dependencyMatrix[current][i] == 1) {
                inDegree[i]--;
                if (inDegree[i] == 0) {
                    queue.push_back(i);
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (inDegree[i] > 0) {
            isCompilable = false;
            break;
        }
    }

    if (!isCompilable) {
        std::vector<bool> visited(n, false);
        std::vector<int> currentPath;

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                currentPath.clear();
                findCyclicDependencies(i, dependencyMatrix, visited, currentPath, cyclicDependencies);
            }
        }

        mergeCyclicDependencies(cyclicDependencies);
    }
}


void findCyclicDependencies(int node, const std::vector<std::vector<int>>& dependencyMatrix,
                            std::vector<bool>& visited, std::vector<int>& currentPath,
                            std::vector<std::vector<int>>& cyclicDependencies) {
    visited[node] = true;
    currentPath.push_back(node);

    for (int i = 0; i < dependencyMatrix.size(); ++i) {
        if (dependencyMatrix[node][i] == 1) {
            if (!visited[i]) {
                findCyclicDependencies(i, dependencyMatrix, visited, currentPath, cyclicDependencies);
            } else if (contains(currentPath, i)) {
                auto start = currentPath.begin();
                while (*start != i) {
                    ++start;
                }
                cyclicDependencies.push_back(std::vector<int>(start, currentPath.end()));
            }
        }
    }

    currentPath.pop_back();
    visited[node] = false;
}

void mergeCyclicDependencies(std::vector<std::vector<int>>& cyclicDependencies) {
    bool merged = true;

    while (merged) {
        merged = false;

        for (size_t i = 0; i < cyclicDependencies.size(); ++i) {
            for (size_t j = i + 1; j < cyclicDependencies.size(); ++j) {
                for (int element : cyclicDependencies[i]) {
                    if (contains(cyclicDependencies[j], element)) {
                        std::vector<int> mergedCycle;
                        mergedCycle.reserve(cyclicDependencies[i].size() + cyclicDependencies[j].size());
                        mergedCycle.insert(mergedCycle.end(), cyclicDependencies[i].begin(), cyclicDependencies[i].end());
                        mergedCycle.insert(mergedCycle.end(), cyclicDependencies[j].begin(), cyclicDependencies[j].end());

                        std::vector<int> uniqueElements;
                        for (int elem : mergedCycle) {
                            bool found = false;
                            for (int uniqueElem : uniqueElements) {
                                if (elem == uniqueElem) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                uniqueElements.push_back(elem);
                            }
                        }

                        cyclicDependencies[i] = uniqueElements;
                        cyclicDependencies.erase(cyclicDependencies.begin() + j);

                        merged = true;
                        break;
                    }
                }

                if (merged) {
                    break;
                }
            }

            if (merged) {
                break;
            }
        }
    }
}