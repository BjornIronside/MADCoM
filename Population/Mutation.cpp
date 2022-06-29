#include "Mutation.h"

void Mutator::mutate(Individu *indi)
{
    // cout << "Mutation on Individual with Cost: " << indi->coutSol.evaluation << '\n';
    // Copy pointer
    mutant = indi;
    bool success{false};
    int attempts{0};

    while (!success && attempts < 10)
    {
        attempts += 1;
        // Mutation
        if (params->useRCO_decomposition)
            rcoDecompose();
        else
            randomDecompose();
        hierarchicalDecomposition();
        success = updateMutant();

        // if (!success)
        //   cout << "Failed mutation! Attempt nº" << attempts << ". Retrying...\n";

        // Clean up for next mutation
        clearStructures();
        virtualTaskSet.clear();
    }

    nbMutations += 1;
}

void Mutator::generate(Individu *indi)
{
    // Copy pointer
    mutant = indi;
    // Form initial VT set consisting of all services
    nbVT = params->nbClients;
    for (int i{1}; i <= params->nbClients; i++)
    {
        virtualTaskSet.push_back(vector<int>(1, i));
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
            // cout << "cluster";
            groupVirtualTasks();
            // cout << "grouped";
        }

        // Clearing for next iteration
        medoids.clear();
        removalLoss.clear();
    }
}

bool Mutator::updateMutant()
{
    bool successfulMutation{false};
    for (int i{0}; i < params->nbClients; i++)
    {
        if (mutant->chromT[1][i] != virtualTaskSet[0][i])
        {
            mutant->chromT[1][i] = virtualTaskSet[0][i];
            successfulMutation = true;
        }
    }
    return successfulMutation;
}

void Mutator::randomDecompose()
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
            mid = (rand() % (end - begin - 1)) + begin + 1;

            for (int j{begin}; j < mid; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);

            virtualTaskSet.push_back(vector<int>());

            for (int j{mid}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);

            nbRandomCuts += 1;
        }
        // The whole route is a virtual task
        else
        {
            for (int j{begin}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
        }
    }
    nbVT = (int)virtualTaskSet.size();
}

void Mutator::rcoDecompose()
{
    // mutant->printRoutes();
    double avgTaskRank{calculateAverageTaskRank()};

    // Decomposing the routes of the solution using RCO
    int begin;
    int end;
    int nVehicles{params->nombreVehicules[1]};
    vector<int> goodLinks;
    vector<int> poorLinks;
    int goodCut{-1};
    int poorCut{-1};

    // Iterate through the routes of the solution
    for (int i{nVehicles - mutant->nbRoutes}; i < nVehicles; i++)
    {
        virtualTaskSet.push_back(vector<int>());

        // Beginning and end of route
        begin = mutant->chromR[1][i];
        if (i == nVehicles - 1) // Final route
            end = mutant->chromT[1].size();
        else
            end = mutant->chromR[1][i + 1];

        if (end - begin <= 1) // Route has only one service, cutting is impossible
        {
            for (int j{begin}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
            continue;
        }

        // Classify each link as good or poor
        for (int j{begin}; j < end - 1; j++)
        {
            if (taskRankMatrix[mutant->chromT[1][j]][mutant->chromT[1][j + 1]] < avgTaskRank)
                goodLinks.push_back(j);
            else
                poorLinks.push_back(j);
        }
        // Randomly decide whether to cut a good or poor link
        if (!goodLinks.empty() && dis01(mt) < goodLinkCutProb)
            goodCut = goodLinks[rand() % goodLinks.size()];
        if (!poorLinks.empty() && dis01(mt) < poorLinkCutProb)
            poorCut = poorLinks[rand() % poorLinks.size()];

        // Cut the route into virtual tasks at the selected links
        if (goodCut > -1) // A good link is going to be cut
        {
            nbGoodCuts += 1;
            if (poorCut > -1) // Cut a good and poor link
            {
                nbPoorCuts += 1;
                int a{min(goodCut, poorCut)};
                int b{max(goodCut, poorCut)};

                for (int j{begin}; j <= a; j++)
                    virtualTaskSet.back().push_back(mutant->chromT[1][j]);

                virtualTaskSet.push_back(vector<int>());

                for (int j{a + 1}; j <= b; j++)
                    virtualTaskSet.back().push_back(mutant->chromT[1][j]);

                virtualTaskSet.push_back(vector<int>());

                for (int j{b + 1}; j < end; j++)
                    virtualTaskSet.back().push_back(mutant->chromT[1][j]);
            }
            else // Cut only at the good link
            {
                for (int j{begin}; j <= goodCut; j++)
                    virtualTaskSet.back().push_back(mutant->chromT[1][j]);

                virtualTaskSet.push_back(vector<int>());

                for (int j{goodCut + 1}; j < end; j++)
                    virtualTaskSet.back().push_back(mutant->chromT[1][j]);
            }
        }
        else if (poorCut > -1) // Cut only at the poor link
        {
            nbPoorCuts += 1;
            for (int j{begin}; j <= poorCut; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);

            virtualTaskSet.push_back(vector<int>());

            for (int j{poorCut + 1}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
        }
        else // Don't cut, i.e. the whole route is a virtual task
        {
            for (int j{begin}; j < end; j++)
                virtualTaskSet.back().push_back(mutant->chromT[1][j]);
        }

        // Reset aux variables
        goodCut = -1;
        poorCut = -1;
        goodLinks.clear();
        poorLinks.clear();
    }
    nbVT = (int)virtualTaskSet.size();
    // printVTSet();
}

double Mutator::calculateAverageTaskRank()
{
    int begin;
    int end;
    int nVehicles{params->nombreVehicules[1]};
    double sumRanks{0.0};

    for (int i{nVehicles - mutant->nbRoutes}; i < nVehicles; i++)
    {
        // Beginning and end of route
        begin = mutant->chromR[1][i];
        if (i == nVehicles - 1) // Final route
            end = mutant->chromT[1].size();
        else
            end = mutant->chromR[1][i + 1];

        for (int j{begin}; j < end - 1; j++)
            sumRanks += taskRankMatrix[mutant->chromT[1][j]][mutant->chromT[1][j + 1]];
    }
    return sumRanks / (params->nbClients - mutant->nbRoutes);
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
    bool generating{nbVT == params->nbClients};

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
                if (generating)
                    doj = params->timeCost[xo + 1][xc + 1];
                else
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
        if (!improved || td <= 0)
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
                dist2Medoid = params->timeCost[xc + 1][mi + 1];
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
    // double tdCheck{0};
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
            distNearest[xo] = 1.e30;
            distSecond[xo] = 1.e30;
            i = 0;
            for (int mi : medoids)
            {
                dist2Medoid = params->timeCost[xo + 1][mi + 1];
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
            // tdCheck += distNearest[xo];
            removalLoss[nearest[xo]] += distSecond[xo] - distNearest[xo];
        }
        // cout << ' ' << tdCheck << ' ';
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
    int lastService;
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
        bestVT = selectFirstVT(group, placed);
        // Adding each service in the virtual task to the new virtual task
        for (auto t : virtualTaskSet[bestVT])
        {
            newVirtualTaskSet.back().push_back(t);
        }
        // save the last service of the selected VT;
        lastService = virtualTaskSet[bestVT].back();

        // Every virtual task needs to be placed
        for (int j{0}; j < clusterSize - 1; j++)
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
                dist = params->timeCost[lastService][virtualTaskSet[vtidx].front()];
                if (dist < minDist)
                {
                    minDist = dist;
                    bestVT = vtidx;
                    idxBestVT = i;
                }
            }
            // Check if a virtual task was selected to be added
            if (minDist != 1.e20)
            {
                // Adding each service in the virtual task to the new virtual task
                for (auto t : virtualTaskSet[bestVT])
                {
                    newVirtualTaskSet.back().push_back(t);
                }
                // The best VT is now placed
                placed[idxBestVT] = true;
                // lastService is now the last service of the newly added virtual task
                lastService = virtualTaskSet[bestVT].back();
            }
        }
    }
    virtualTaskSet = newVirtualTaskSet;
    nbVT = nbClusters;
}

int Mutator::selectFirstVT(vector<int> &group, vector<bool> &placed)
{
    int bestVT{-1};
    int idxBestVT{-1};
    int i{-1};
    double minDist{1.e20};
    double dist{0};

    // First is the Closest to depot
    if (params->selectionMethod == 0 || nbClusters == 1)
    {
        for (int vtidx : group)
        {
            // Distance from the depot to the first service of the current VT
            dist = params->timeCost[0][virtualTaskSet[vtidx].front()];
            i++;
            // Find the closest
            if (dist < minDist)
            {
                minDist = dist;
                bestVT = vtidx;
                idxBestVT = i;
            }
        }
    }
    // First is the Medoid
    else if (params->selectionMethod == 1)
    {
        bool found{false};
        // Faster to compute by using distNearest == 0, must ensure all distances are greater than zero
        for (int vtidx : group)
        {
            i++;
            for (int med : medoids)
            {
                if (vtidx == med)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                bestVT = vtidx;
                idxBestVT = i;
                break;
            }
        }
    }
    // First is the closest to another medoid
    else if (params->selectionMethod == 2)
    {
        for (int vtidx : group)
        {
            i++;
            // Distance is the distance to the second closest medoid
            dist = distSecond[vtidx];
            // Find the closest
            if (dist < minDist)
            {
                minDist = dist;
                bestVT = vtidx;
                idxBestVT = i;
            }
        }
    }
    // Say that the first VT is now placed
    placed[idxBestVT] = true;
    // return the index to the best VT
    return bestVT;
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

void Mutator::printTaskRankMatrix()
{
    cout << "\nDistance Matrix:\n";
    for (auto row : taskRankMatrix)
    {
        for (int d : row)
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

void Mutator::computeTaskRankMatrix()
{
    int taskMatSize{params->nbClients + params->nbDepots};
    vector<int> temp(taskMatSize, 1);

    for (int task = 0; task < taskMatSize; task++)
    {
        taskRankMatrix.push_back(temp);
        // Argsort the row of timeCost
        vector<int> idx(taskMatSize);
        iota(idx.begin(), idx.end(), 0); // Initialize vector with 1,2,3,...
        stable_sort(idx.begin(), idx.end(), [task, this](int i1, int i2)
                    { return params->timeCost[task][i1] < params->timeCost[task][i2]; }); // Sorted indexes

        int currentRank{1};
        taskRankMatrix[task][idx[0]] = currentRank++;

        for (int i{1}; i < taskMatSize; ++i)
        {
            if (params->timeCost[task][idx[i]] == params->timeCost[task][idx[i - 1]])
            {
                taskRankMatrix[task][idx[i]] = taskRankMatrix[task][idx[i - 1]];
                ++currentRank;
            }
            else
                taskRankMatrix[task][idx[i]] = currentRank++;
        }
    }
}

Mutator::Mutator(Params *params) : params(params)
{
    cout << "Initializing mutator\n";
    if (params->hdVariant == 0)
        maxClusters = params->nbClients;
    else if (params->hdVariant == 1)
        maxClusters = max((int)sqrt(params->nbClients), 1);
    else if (params->hdVariant == 2)
        maxClusters = 10;
    cout << "Maximum Clusters: " << maxClusters << endl;
    beta = params->beta;
    goodLinkCutProb = params->goodLinkCutProb;
    poorLinkCutProb = params->poorLinkCutProb;
    if (params->useRCO_decomposition)
        computeTaskRankMatrix();

    // Initializing random distribution
    random_device rd;
    mt = mt19937();
    dis01 = uniform_real_distribution<>(0.0, 1.0);

    // Initialize Traces
    nbRandomCuts = 0;
    nbPoorCuts = 0;
    nbGoodCuts = 0;
    nbMutations = 0;
}

Mutator::~Mutator()
{
    clearStructures();
}