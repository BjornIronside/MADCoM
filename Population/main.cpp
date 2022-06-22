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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "Genetic.h"
#include "commandline.h"
#include "Mutation.h"

using namespace std;

int main(int argc, char *argv[])
{

	Mutator *mutator;
	Population *population;
	Params *mesParametres;

	cout << endl;

	try
	{
		// Reading the commandline
		commandline c(argc, argv);

		if (!c.is_valid())
			throw string("Commandline could not be read, Usage : gencarp instance -type problemType [-t cpu-time] [-sol solutionPath]  [-s seed] [-veh nbVehicles] [-dep nbDepots]");

		// initialisation of the Parameters
		mesParametres = new Params(c, c.get_nbVeh(), false);

		// Running the algorithm
		mutator = new Mutator(mesParametres);
		clock_t popBegin = clock();
		population = new Population(mesParametres, mutator);
		clock_t popEnd = clock();
		clock_t popInitTime{popEnd - popBegin};
		cout << "Population initialization time: " << (double)popInitTime / (double)CLOCKS_PER_SEC << " s" << endl;

		delete population;
		delete mesParametres;
		cout << endl;
		return 0;
	}
	catch (const string &e)
	{
		cout << e << endl;
		cout << endl;
		return 0;
	}
}
