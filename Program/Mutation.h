#ifndef MUTATION_H
#define MUTATION_H

#include "Params.h"
#include "Individu.h"
#include <stdlib.h>
#include <vector>
using namespace std;

class Mutator
{
private:
public:
    // Parameters of mutation
    double beta;
    int maxClusters;

    // Pointer towards the parameters of the problem
    Params *params;

    // Pointer towards the individual to mutate
    Individu *mutant;

    // Generate an individual using Hierarchical Decomposition
    // Initial Virtual Task set is equal to the set of all services
    void generate(Individu *indi);
    
    // Mutate the individual
    void mutate(Individu *indi);

    // Virtual task set
    int nbVT;
    vector<vector<int>> virtualTaskSet;

    // Break the routes of an individual to generate a virtual task set
    void decompose();

    // Hierarchical Decomposition generates a giant tour
    void hierarchicalDecomposition();

    // Updates the giant tour chromosome of the individual being ,utated
    void updateMutant();

    // Clustering auxiliary data structures
    // Number of clusters
    int nbClusters;
    // Total deviation we want to minimize
    double td;
    // Distance matrix between virtual tasks
    vector<vector<double>> distMatrix;
    // Vector with the indices of the virtual tasks that are currently medoids
    vector<int> medoids;
    // Amount of total deviation lost if a medoid is removed
    vector<double> removalLoss;
    // Index of the medoid virtual task closest to each of the virtual tasks
    vector<int> nearest;
    // Distance to the nearest and second nearest medoids
    vector<double> distNearest;
    vector<double> distSecond;

    // Generate distance matrix
    void generateDistMatrix();

    // Cluster virtual task set
    void cluster();

    // Random sampling initialization of medoids
    void randomSampling();

    // Calculate initial total deviation
    void calculateInitialTD();

    // Update auxiliary structures and compute removal loss
    void updateAuxStructures();

    // Order virtual tasks in the same cluster according to Best Insertion Heuristic
    void groupVirtualTasks();

    // Initialize virtual task set and distance matrix
    void initializeAuxStructures();

    // Clears virtual task set and distance matrix
    void clearStructures();

    // Prints virtual task set for debugging purposes
    void printVTSet();
    void printDistMatrix();
    void printAuxStructures();

    // Constructor
    Mutator(Params *params);

    // Destructor
    ~Mutator(void);
};

#endif