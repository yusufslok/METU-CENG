#include "the6.h"

// do not add extra libraries here
int find_structure(std::vector< std::vector<std::pair<int,int> > > &bond_energies, std::vector< std::vector<std::pair<int,int> > > &lowest_energy_structure) {
    int num_vertices = bond_energies.size();
    std::vector<bool> in_tree(num_vertices, false);
    std::vector<int> parent(num_vertices, -1);
    std::vector<int> key(num_vertices, INT_MAX);
    int start_vertex = 0;
    key[start_vertex] = 0;

    for (int i = 0; i < num_vertices - 1; ++i) {
        int min_key_vertex = -1;
        for (int v = 0; v < num_vertices; ++v) {
            if (!in_tree[v] && (min_key_vertex == -1 || key[v] < key[min_key_vertex])) {
                min_key_vertex = v;
            }
        }

        in_tree[min_key_vertex] = true;
        for (int j=0; j< bond_energies[min_key_vertex].size();j++) {
            int adjacent_vertex = bond_energies[min_key_vertex][j].first;
            int weight = bond_energies[min_key_vertex][j].second;
            if (!in_tree[adjacent_vertex] && weight < key[adjacent_vertex]) {
                parent[adjacent_vertex] = min_key_vertex;
                key[adjacent_vertex] = weight;
            }
        }
    }

    lowest_energy_structure.resize(num_vertices);
    int total_bond_energy = 0;

    for (int v = 0; v < num_vertices; ++v) {
        if (parent[v] != -1) {
            std::pair<int,int> temp1(parent[v], key[v]);
            std::pair<int,int> temp2(v, key[v]);
            lowest_energy_structure[v].push_back(temp1);
            lowest_energy_structure[parent[v]].push_back(temp2);
            total_bond_energy += key[v];
        }
    }

    return total_bond_energy;
}


void dfs(int current, std::vector<std::vector<std::pair<int, int>>>& molecule_structure,
         std::vector<int>& visited, std::vector<int>& current_chain,
         std::vector<int>& longest_chain, int& max_length) {
    
    visited[current] = 1;
    current_chain.push_back(current);

    for (int j=0; j< molecule_structure[current].size();j++) {
        int next_vertex = molecule_structure[current][j].first;
        if (!visited[next_vertex]) {
            dfs(next_vertex, molecule_structure, visited, current_chain, longest_chain, max_length);
        }
    }

    if (current_chain.size() > max_length) {
        max_length = current_chain.size();
        longest_chain = current_chain;
    }

    current_chain.pop_back();
    visited[current] = 0;
}

int find_longest_chain(std::vector<std::vector<std::pair<int, int>>>& molecule_structure,
                       std::vector<int>& chain) {
    int num_vertices = molecule_structure.size();
    std::vector<int> visited(num_vertices, 0);
    std::vector<int> current_chain, longest_chain;
    int max_length = 0;

    for (int vertex = 0; vertex < num_vertices; ++vertex) {
        if (!visited[vertex]) {
            dfs(vertex, molecule_structure, visited, current_chain, longest_chain, max_length);
        }
    }
    chain = longest_chain;

    return max_length;
}
