#include "Mutation.h"

void Mutator::mutate(Individu *indi)
{
    // Copy pointer
    mutant = indi;

    // Mutation
    decompose();
    hierarchicalDecomposition();
    updateMutant();

    // Clean up for next mutation
    clearStructures();
    virtualTaskSet.clear();
}

void Mutator::generate(Individu *indi)
{
    // Copy pointer
    mutant = indi;
    // Form initial VT set consisting of all services
    nbVT = params->nbClients;
    for (int i{1}; i <= params->nbClients; i++)
    {
        virtualTaskSet.push_back(vector<int>(1,i));
    }
    // Generate Giant Tour
    hierarchicalDecomposition();
    updateMutant();

    // Clean up for next mutation
    clearStructures();
    virtualTaskSet.clear();
}

void Mutator::hierarchicalDecomposition()
{
    // Initializing the auxiliary structures
    initializeAuxStructures();
    int mostClustersLayer;
    while (nbVT > 1)
    {
        mostClustersLayer = min((int)ceil(beta * nbVT), maxClusters);
        nbClusters = (rand() % mostClustersLayer) + 1;

        if (nbClusters == 1)
        {
            // Putting all VTs in the same cluster
            for (int i{0}; i < nbVT; i++)
            {
                nearest[i] = 0;
            }
            groupVirtualTasks();
        }
        else
        {
            generateDistMatrix();
            cluster();
            groupVirtualTasks();
        }

        // Clearing for next iteration
        medoids.clear();
        removalLoss.clear();
    }
}

void Mutator::updateMutant()
{
    for (int i{0}; i < params->nbClients; i++)
    {
        mutant->chromT[1][i] = virtualTaskSet[0][i];
    }
}

void Mutator::decompose()
{
    int begin;
    int end;
    int mid;
    int nVehicles{params->nombreVehicules[1]};
    for (int i{nVehicles - mutant->nbRoutes}; i < nVehicles; i++)
    {
        virtualTaskSet.push_back(vector<int>());

        // Beginning and end of route
        begin = mutant->chromR[1][i];
        if (i == nVehicles - 1) // Final route
            end = mutant->chromT[1].size();
        else
            end = mutant->chromR[1][i + 1];

        // 10% chance of splitting the route into two virtual tasks if it has more than one service
        if (end - begin > 1 && rand() % 10 == 0)
        {
            if (end - begin == 1)
            {
                cout << begin << ' ' << end << mutant->nbRoutes << '\n';
                mutant->printGiantTour();
                mutant->printRoutes();
            }
            mid = (rand() % (end - begin - 1)) + begin + 1;
            // cout << mid << ' ';

            for (int j{begin}; j < mid; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);

            virtualTaskSet.push_back(vector<int>());

            for (int j{mid}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
        }
        // The whole route is a virtual task
        else
        {
            for (int j{begin}; j < end; j++)
            {
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
            }
        }
    }
    nbVT = (int)virtualTaskSet.size();
}

void Mutator::generateDistMatrix()
{
    double dist;
    for (int i{0}; i < nbVT; i++)
    {
        for (int j{0}; j < nbVT; j++)
        {
            if (i == j)
                continue;
            dist = 0.5 * (params->timeCost[virtualTaskSet[i].front()][virtualTaskSet[j].back()] +
                          params->timeCost[virtualTaskSet[j].front()][virtualTaskSet[i].back()]);

            distMatrix[i][j] = dist;
            distMatrix[j][i] = dist;
        }
    }
}

void Mutator::clearStructures()
{
    medoids.clear();
    removalLoss.clear();
    distMatrix.clear();
    nearest.clear();
    distNearest.clear();
    distSecond.clear();
}

void Mutator::cluster()
{
    //  Selecting the first medoids randomly
    randomSampling();
    for (int i{0}; i < nbClusters; i++)
    {
        removalLoss.push_back(0.0); // Initializing removal loss
        nearest[medoids[i]] = i;    // To identify the VTs that are medoids
    }

    // Calculate initial total deviation
    calculateInitialTD();

    // Updating the auxiliary structures
    updateAuxStructures();

    // Variables used during clustering
    bool improved{false};
    vector<double> deltaTD;
    double doj;
    double sharedAccumulator;
    int idxMin{0};
    double minDeltaTD;

    // Clustering loop
    while (true)
    {
        improved = false;
        for (int xc{0}; xc < nbVT; xc++)
        {
            // Check if xc is a medoid
            if (medoids[nearest[xc]] == xc)
            {
                continue;
            }
            deltaTD = removalLoss;
            sharedAccumulator = 0.0;
            for (int xo{0}; xo < nbVT; xo++)
            {
                doj = distMatrix[xo][xc];
                if (doj < distNearest[xo])
                {
                    sharedAccumulator += doj - distNearest[xo];
                    deltaTD[nearest[xo]] += distNearest[xo] - distSecond[xo];
                }
                else if (doj < distSecond[xo])
                {
                    deltaTD[nearest[xo]] += doj - distSecond[xo];
                }
            }

            // Choose best medoid
            minDeltaTD = 1.e20;
            for (int i{0}; i < nbClusters; i++)
            {
                if (deltaTD[i] < minDeltaTD)
                {
                    minDeltaTD = deltaTD[i];
                    idxMin = i;
                }
            }
            deltaTD[idxMin] += sharedAccumulator;
            if (deltaTD[idxMin] < 0)
            {
                // Swap roles of medoid idxMin and non-medoid xc
                medoids[idxMin] = xc;
                td += deltaTD[idxMin];
                // cout << "\nNew Medoid: " << xc << " Total Deviation: " << td;
                updateAuxStructures();
                improved = true;
            }
        }
        if (!improved)
            break;
    }
}

void Mutator::randomSampling()
{
    // Initialize medoids
    for (int i{0}; i < nbVT; i++)
        medoids.push_back(i);
    // Fisher-Yates shuffle of the first nbClusters VTs
    int numRandom{nbClusters};
    int left{nbVT};
    int r;
    int begin{0};
    while (numRandom--)
    {
        r = begin + (rand() % left);
        swap(medoids[begin], medoids[r]);
        ++begin;
        --left;
    }

    // Erase the remainder of the vector
    medoids.erase(medoids.begin() + nbClusters, medoids.end());
}

void Mutator::calculateInitialTD()
{
    td = 0;
    double min_dist{1.e20};
    double dist2Medoid;
    // Initialization Case
    if (nbVT == params->nbClients)
    {
        for (int xc{0}; xc < nbVT; xc++)
        {
            // Check if xc is a medoid
            if (medoids[nearest[xc]] == xc)
            {
                continue;
            }

            min_dist = 1.e20;
            for (int mi : medoids)
            {
                dist2Medoid = params->timeCost[xc+1][mi+1];
                if (dist2Medoid < min_dist)
                    min_dist = dist2Medoid;
            }
            td += min_dist;
        }
    }
    // Standard Case
    else
    {
        for (int xc{0}; xc < nbVT; xc++)
        {
            // Check if xc is a medoid
            if (medoids[nearest[xc]] == xc)
            {
                continue;
            }

            min_dist = 1.e20;
            for (int mi : medoids)
            {
                if (distMatrix[xc][mi] < min_dist)
                    min_dist = distMatrix[xc][mi];
            }
            td += min_dist;
        }
    }
}

void Mutator::initializeAuxStructures()
{
    // Temp vector for distance matrix between VTs
    vector<double> temp(nbVT, 0.0);

    // Initializing the auxiliary structures and distance matrix
    for (int i{0}; i < nbVT; i++)
    {
        distMatrix.push_back(temp);
        nearest.push_back(-1);
        distNearest.push_back(1.e20);
        distSecond.push_back(1.e20);
    }
}

void Mutator::updateAuxStructures()
{
    int i;
    double dist2Medoid;
    // Reset removal loss
    for (int j{0}; j < nbClusters; j++)
    {
        removalLoss[j] = 0;
    }

    // Initialization Case
    if (nbVT == params->nbClients)
    {
        for (int xo{0}; xo < nbVT; xo++)
        {
            distNearest[xo] = 1.e20;
            distSecond[xo] = 1.e20;
            i = 0;
            for (int mi : medoids)
            {
                dist2Medoid = params->timeCost[xo+1][mi+1];
                if (dist2Medoid < distSecond[xo])
                {
                    if (dist2Medoid < distNearest[xo])
                    {
                        // If mi becomes the closest medoid, then the previous closest medoid is now the second closest
                        distSecond[xo] = distNearest[xo];
                        distNearest[xo] = dist2Medoid;
                        nearest[xo] = i;
                    }
                    else
                    {
                        distSecond[xo] = dist2Medoid;
                    }
                }
                i++;
            }
            removalLoss[nearest[xo]] += distSecond[xo] - distNearest[xo];
        }
    }
    // Standard Case
    else
    {
        for (int xo{0}; xo < nbVT; xo++)
        {
            distNearest[xo] = 1.e20;
            distSecond[xo] = 1.e20;
            i = 0;
            for (int mi : medoids)
            {
                if (distMatrix[xo][mi] < distSecond[xo])
                {
                    if (distMatrix[xo][mi] < distNearest[xo])
                    {
                        // If mi becomes the closest medoid, then the previous closest medoid is now the second closest
                        distSecond[xo] = distNearest[xo];
                        distNearest[xo] = distMatrix[xo][mi];
                        nearest[xo] = i;
                    }
                    else
                    {
                        distSecond[xo] = distMatrix[xo][mi];
                    }
                }
                i++;
            }
            removalLoss[nearest[xo]] += distSecond[xo] - distNearest[xo];
        }
    }
}

void Mutator::groupVirtualTasks()
{
    vector<vector<int>> groupedVTIdxs;
    vector<vector<int>> newVirtualTaskSet;
    for (int i{0}; i < nbClusters; i++)
    {
        groupedVTIdxs.push_back(vector<int>());
    }
    for (int i{0}; i < nbVT; i++)
    {
        groupedVTIdxs[nearest[i]].push_back(i);
    }

    int i;
    int lastNode;
    double minDist;
    double dist;
    int bestVT{-1};
    int idxBestVT{-1};
    // Ordering the virtual tasks in each cluster according to the best insertion heuristic
    for (auto group : groupedVTIdxs)
    {
        // Initialize new virtual task
        newVirtualTaskSet.push_back(vector<int>());
        // Vector to check if a virtual task has been placed in the new virtual task
        int clusterSize{(int)group.size()};
        vector<bool> placed(clusterSize, false);

        // Start by finding the closest virtual task to the depot node
        lastNode = 0;
        // Every virtual task needs to be placed
        for (int j{0}; j < clusterSize; j++)
        {
            i = -1;
            minDist = 1.e20;
            for (int vtidx : group)
            {
                // If a virtual task has already been placed, skip the iteration
                i++;
                if (placed[i])
                    continue;
                // Distance from last service in new VT to the first service of the current VT
                dist = params->timeCost[lastNode][virtualTaskSet[vtidx].front()];
                if (dist < minDist)
                {
                    minDist = dist;
                    bestVT = vtidx;
                    idxBestVT = i;
                }
            }
            // Adding each service in the virtual task to the new virtual task
            for (auto t : virtualTaskSet[bestVT])
            {
                newVirtualTaskSet.back().push_back(t);
            }
            // The best VT is now placed
            placed[idxBestVT] = true;
            // Last node is now the last service of the newly added virtual task
            lastNode = virtualTaskSet[bestVT].back();
        }
    }
    virtualTaskSet = newVirtualTaskSet;
    nbVT = nbClusters;
}

void Mutator::printVTSet()
{
    cout << "\nVirtual Tasks:\n";
    for (auto vt : virtualTaskSet)
    {
        for (int t : vt)
        {
            cout << t << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

void Mutator::printDistMatrix()
{
    cout << "\nDistance Matrix:\n";
    for (auto row : distMatrix)
    {
        for (double d : row)
        {
            cout << d << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

void Mutator::printAuxStructures()
{
    cout << "\nMedoids:\n";
    for (auto i : medoids)
        cout << i << ' ';
    cout << "\nNearest Medoid:\n";
    for (auto i : nearest)
        cout << medoids[i] << ' ';
    cout << "\nDistance to Nearest:\n";
    for (auto i : distNearest)
        cout << i << ' ';
    cout << "\nDistance to Second:\n";
    for (auto i : distSecond)
        cout << i << ' ';
    cout << "\nRemoval Loss:\n";
    for (auto i : removalLoss)
        cout << i << ' ';
    cout << '\n';
}

Mutator::Mutator(Params *params) : params(params)
{
    maxClusters = max((int)sqrt(params->nbClients), 1);
    beta = params->beta;
}

Mutator::~Mutator()
{
    clearStructures();
}