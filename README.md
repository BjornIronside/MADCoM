# MADCoM: Memetic Algorithm with Divide-and-Conquer Mutation for the Large-Scale Mixed Capacitated Arc Routing Problem (MCARP)

This repository houses the source code of MADCoM, Memetic Algorithm with Divide-and-Conquer Mutation (MADCoM).
MADCoM is based on the Unified Hybrid Genetic Search (UHGS) of [Vidal 2017](https://doi.org/10.1287/opre.2017.1595) and incorporates its source code, see https://github.com/vidalt/HGS-CARP.
It uses the Hierarchical Decomposition of [Tang et al. 2016](https://doi.org/10.1109/TCYB.2016.2590558) and the Route Cutting Off Decomposition of [Zhang et al. 2021](https://doi.org/10.1016/j.ins.2020.11.011) as the building blocks of its fast initialization and mutation operators.

## Usage
For CARP instances use:
> `madcom PATH_TO_INSTANCE -type 30`
 
For MCARP instances use:
> `madcom PATH_TO_INSTANCE -type 36`

 
