/*  ---------------------------------------------------------------------- //
    Hybrid Genetic Search for Arc Routing Problems -- HGS-CARP
    Copyright (C) 2016 Thibaut VIDAL

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  ---------------------------------------------------------------------- */

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
using namespace std;

class commandline
{
private:
    // say if the commandline is valid
    bool command_ok;

    // CPU time allowed
    int cpu_time;

    // seed
    int seed;

    // instance type, if given (see definitions in Params.h)
    int type;

    // nbVehicles, if given (for the MM-kWRPP, the fleet size has to be specified)
    int nbVeh;

    // nbDepots, if given (used to generate the MDCARP instances)
    int nbDep;

    // Fraction of initial population to generate using Hierarchical Decomposition
    double fractionHD;

    // Mutation Probability
    double mutationProb;

    // Cutting Probabilities
    double goodCutProb;
    double poorCutProb;

    // Mutation Tournament Size
    int mutTournSize;

    // Population Parameters
    int mu;
    int lambda;
    int nElite;
    int nDiver;

    // Results folder
    string resultsFolder;

    // instance path
    string instance_path;

    // instance name
    string instance_name;

    // output path
    string output_name;

    // BKS path (used to replace it if a better solution is found)
    string BKS_name;

    // progress file path
    string progress_file_name;

    // simple setters
    void SetDefaultOutput(string to_parse);

public:
    // constructor
    commandline(int argc, char *argv[]);

    // destructor
    ~commandline();

    // Getters
    string get_path_to_instance();
    string get_path_to_solution();
    string get_path_to_BKS();
    string get_path_to_progress_file();
    string get_instance_name();
    int get_cpu_time();
    int get_type();
    int get_nbVeh();
    int get_nbDep();
    int get_seed();
    double get_fractionHD();
    double get_mutationProb();
    double get_goodCutProb();
    double get_poorCutProb();
    int get_mutTournSize();
    int get_mu();
    int get_lambda();
    int get_nElite();
    int get_nDiver();

    // say if the commandline is valid
    bool is_valid();
};
#endif
