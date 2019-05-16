// Copyright 2019 Luca Istrate, Danut Matei
#include "./aegraph.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

std::string strip(std::string s) {
    // deletes whitespace from the beginning and end of the string
    s.erase(0, s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    return s;
}

std::pair<std::string, std::string> split_first(std::string s,
                                                char delimiter = ',') {
    // returns a pair that contains: <first_cut, rest_of_graph>

    int numOpen = 0;

    int len = s.size();
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == delimiter && numOpen == 0)
            return std::make_pair(strip(s.substr(0, i)),
                                  strip(s.substr(i + 1, len)));
        if (c == '[') numOpen += 1;
        if (c == ']') numOpen -= 1;
    }

    return {strip(s), std::string()};
}

std::vector<std::string> split_level(std::string s, char delimiter = ',') {
    // splits 's' into separate entities (atoms, subgraphs)

    std::vector<std::string> result;
    auto snd = s;
    while (true) {
        auto p = split_first(snd, delimiter);
        auto fst = p.first;
        snd = p.second;

        result.push_back(fst);

        if (snd.empty()) return result;
    }
}

int AEGraph::num_subgraphs() const { return subgraphs.size(); }

int AEGraph::num_atoms() const { return atoms.size(); }

int AEGraph::size() const { return num_atoms() + num_subgraphs(); }

bool AEGraph::operator<(const AEGraph& other) const {
    return this->repr() < other.repr();
}

bool AEGraph::operator==(const AEGraph& other) const {
    return this->repr() == other.repr();
}

bool AEGraph::operator!=(const AEGraph& other) const {
    return this->repr() != other.repr();
}

AEGraph AEGraph::operator[](const int index) const {
    // offers an easier way of accessing the nested graphs
    if (index < num_subgraphs()) {
        return subgraphs[index];
    }

    if (index < num_subgraphs() + num_atoms()) {
        return AEGraph('(' + atoms[index - num_subgraphs()] + ')');
    }

    return AEGraph("()");
}

std::ostream& operator<<(std::ostream& out, const AEGraph& g) {
    out << g.repr();
    return out;
}

AEGraph::AEGraph(std::string representation) {
    // constructor that creates an AEGraph structure from a
    // serialized representation
    char left_sep = representation[0];
    char right_sep = representation[representation.size() - 1];

    assert((left_sep == '(' && right_sep == ')') ||
           (left_sep == '[' && right_sep == ']'));

    // if the left separator is '(' then the AEGraph is the entire
    // sheet of assertion
    if (left_sep == '(') {
        is_SA = true;
    } else {
        is_SA = false;
    }

    // eliminate the first pair of [] or ()
    representation = representation.substr(1, representation.size() - 2);

    // split the graph into separate elements
    auto v = split_level(representation);
    // add them to the corresponding vector
    for (auto s : v) {
        if (s[0] != '[') {
            atoms.push_back(s);
        } else {
            subgraphs.push_back(AEGraph(s));
        }
    }

    // also internally sort the new graph
    this->sort();
}

std::string AEGraph::repr() const {
    // returns the serialized representation of the AEGraph

    std::string left, right;
    if (is_SA) {
        left = '(';
        right = ')';
    } else {
        left = '[';
        right = ']';
    }

    std::string result;
    for (auto subgraph : subgraphs) {
        result += subgraph.repr() + ", ";
    }

    int len = atoms.size();
    if (len != 0) {
        for (int i = 0; i < len - 1; i++) {
            result += atoms[i] + ", ";
        }
        result += atoms[len - 1];
    } else {
        if (subgraphs.size() != 0)
            return left + result.substr(0, result.size() - 2) + right;
    }

    return left + result + right;
}

void AEGraph::sort() {
    std::sort(atoms.begin(), atoms.end());

    for (auto& sg : subgraphs) {
        sg.sort();
    }

    std::sort(subgraphs.begin(), subgraphs.end());
}

bool AEGraph::contains(const std::string other) const {
    // checks if an atom is in a graph
    if (find(atoms.begin(), atoms.end(), other) != atoms.end()) return true;

    for (const auto& sg : subgraphs)
        if (sg.contains(other)) return true;

    return false;
}

bool AEGraph::contains(const AEGraph& other) const {
    // checks if a subgraph is in a graph
    if (find(subgraphs.begin(), subgraphs.end(), other) != subgraphs.end())
        return true;

    for (const auto& sg : subgraphs)
        if (sg.contains(other)) return true;

    return false;
}

std::vector<std::vector<int>> AEGraph::get_paths_to(
    const std::string other) const {
    // returns all paths in the tree that lead to an atom like <other>
    std::vector<std::vector<int>> paths;

    int len_atoms = num_atoms();
    int len_subgraphs = num_subgraphs();

    for (int i = 0; i < len_atoms; i++) {
        if (atoms[i] == other && size() > 1) {
            paths.push_back({i + len_subgraphs});
        }
    }

    for (int i = 0; i < len_subgraphs; i++) {
        if (subgraphs[i].contains(other)) {
            auto r = subgraphs[i].get_paths_to(other);
            for (auto& v : r) v.insert(v.begin(), i);
            copy(r.begin(), r.end(), back_inserter(paths));
        }
    }

    return paths;
}

std::vector<std::vector<int>> AEGraph::get_paths_to(
    const AEGraph& other) const {
    // returns all paths in the tree that lead to a subgraph like <other>
    std::vector<std::vector<int>> paths;
    int len_subgraphs = num_subgraphs();

    for (int i = 0; i < len_subgraphs; i++) {
        if (subgraphs[i] == other && size() > 1) {
            paths.push_back({i});
        } else {
            auto r = subgraphs[i].get_paths_to(other);
            for (auto& v : r) v.insert(v.begin(), i);
            copy(r.begin(), r.end(), back_inserter(paths));
        }
    }

    return paths;
}

/*  Recursive function to find the double cuts
    up - the parent node of the current node
    curr - the current node
    cp - current path
    path - all the paths found so far */
void findCuts(const AEGraph* up, const AEGraph* curr, std::vector<int>& cp,
              std::vector<std::vector<int>>& paths) {
    if (up != nullptr) {
        if (up->num_subgraphs() == 1 && up->num_atoms() == 0) {
            // We need to eliminate the last element of the current path
            // to find the path to the current element
            std::vector<int> temp = cp;
            temp.pop_back();
            paths.push_back(temp);
        }
    }

    for (int it = 0; it < curr->subgraphs.size(); it++) {
        // For every child we need another path based on its parents path
        std::vector<int> temp = cp;
        temp.push_back(it);
        findCuts(curr, &(curr->subgraphs[it]), temp, paths);
    }
}

// Task 1
std::vector<std::vector<int>> AEGraph::possible_double_cuts() const {
    std::vector<std::vector<int>> paths = {};

    for (int it = 0; it < num_subgraphs(); it++) {
        std::vector<int> cp = {it};
        findCuts(nullptr, &(subgraphs[it]), cp, paths);
    }

    return paths;
}

/* Search in the nodes then do the cut
    parent - the parent of the node
    curr - current node
    where - the current path (where to cut)
    iChild - what children is the current node to its parent */
void recursiveCuts(AEGraph* parent, AEGraph* curr, std::vector<int>& where,
                   int iChild) {
    if (where.size() == 0) {
        AEGraph toAdd = curr->subgraphs[0];
        parent->subgraphs.erase(parent->subgraphs.begin() + iChild);

        for (auto& it : toAdd.atoms) {
            parent->atoms.push_back(it);
        }

        for (auto& it : toAdd.subgraphs) {
            parent->subgraphs.push_back(it);
        }

        return;
    } else {
        int index = where.front();
        where.erase(where.begin());
        recursiveCuts(curr, &curr->subgraphs[index], where, index);
    }
}

// Task 2
AEGraph AEGraph::double_cut(std::vector<int> where) const {
    AEGraph toModify(repr());
    recursiveCuts(nullptr, &toModify, where, NULL);
    return toModify;
}

// Task 3
std::vector<std::vector<int>> AEGraph::possible_erasures(int level) const {
    std::vector<int> possiblePaths;
    std::vector<std::vector<int>> possibleErasures;
    std::string representation;

    return possibleErasures;
}

// Task 4
AEGraph AEGraph::erase(std::vector<int> where) const { 
    
    return AEGraph("()"); 
}

/*  A get_paths function that actually works well
    (while still not showing paths to single children, that can't be eliminated)
    So, it is optimised for the deiterations function */
void betterGetPaths(const AEGraph* curr, const std::string query,
                    std::vector<int>& cp,
                    std::vector<std::vector<int>>& paths) {
    for (int it = 0; it < curr->size(); it++) {
        int k = curr->num_subgraphs();
        if (it < k) {
            if (curr->subgraphs[it].repr() == query && curr->size() > 1) {
                // If we found it, we do not need to search it's children
                std::vector<int> temp = cp;
                temp.push_back(it);
                paths.push_back(temp);
            } else {
                // Search in its children
                std::vector<int> temp = cp;
                temp.push_back(it);
                betterGetPaths(&curr->subgraphs[it], query, temp, paths);
            }
        } else {
            if (curr->atoms[it - k] == query && curr->size() > 1) {
                std::vector<int> temp = cp;
                temp.push_back(it);
                paths.push_back(temp);
            }
        }
    }
}

/*  Search for places where deiterations can be done
    curr - the current subgraph we are in
    cp - the path to the current node
    paths - all the paths found so far */
void findDeiterations(const AEGraph* curr, std::vector<int>& cp,
                      std::vector<std::vector<int>>& paths) {
    int k = curr->num_subgraphs();
    std::vector<int> next;

    for (int i = 0; i < curr->size(); i++) {
        std::string query;

        // If it is a subgraph
        if (i < k) {
            query = curr->subgraphs[i].repr();
        } else {
            // If it is a leaf
            query = curr->atoms[i - k];
        }

        // Search for the element in every subgraph
        // (except him, if it is a subgraph)
        for (int j = 0; j < k; j++) {
            if (j != i) {
                std::vector<std::vector<int>> foundPaths;
                std::vector<int> aux = cp;
                aux.push_back(j);
                betterGetPaths(&curr->subgraphs[j], query, aux, foundPaths);
                for (auto& it : foundPaths) {
                    paths.push_back(it);
                }

                // If it found no path, we can continue
                // the operation in that subgraph
                if (foundPaths.empty()) {
                    next.push_back(j);
                }
            }
        }
    }

    // Continue the search for deiterations in the available subgraphs
    for (auto& it : next) {
        std::vector<int> temp = cp;
        temp.push_back(it);
        findDeiterations(&curr->subgraphs[it], temp, paths);
    }
}

// Task 5
std::vector<std::vector<int>> AEGraph::possible_deiterations() const {
    // 20p
    std::vector<std::vector<int>> paths;
    std::vector<int> aux;
    findDeiterations(this, aux, paths);
    return paths;
}

/*  Search for the element that need to be deiterated
    curr - current node
    the current path (where to deiterate)
 */
void recursiveDeiteration(AEGraph* curr, std::vector<int>& where) {
    // If we reached the final child location
    if (where.size() == 1) {
        int k = curr->subgraphs.size();
        if (where[0] < k) {
            curr->subgraphs.erase(curr->subgraphs.begin() + where[0]);
        } else {
            // Because the first k elements are the subgraphs, we need
            // to substract that from the index to find the desired atom
            curr->atoms.erase(curr->atoms.begin() + where[0] - k);
        }
        return;
    } else {
        // Search in the next child
        int index = where.front();
        where.erase(where.begin());
        recursiveDeiteration(&curr->subgraphs[index], where);
    }
}

// Task 6
AEGraph AEGraph::deiterate(std::vector<int> where) const {
    AEGraph toModify(repr());
    recursiveDeiteration(&toModify, where);
    return toModify;
}
