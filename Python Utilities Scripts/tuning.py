import cpp_wrapper
from utils import calculate_gap, get_upper_bound
from functools import partial
from main import RESULTS_FOLDER

import time

from bayes_opt import BayesianOptimization
from bayes_opt.logger import JSONLogger, ScreenLogger
from bayes_opt.event import Events
from bayes_opt.util import load_logs


def evaluate_config(mu, offspring_ratio, elite_fraction, div_fraction, mutprob, gcprob, cutprob_ratio, trn_fraction,
                    solver, instances_dict, time_limit, runs):
    # Translate Tuning variables to parameters of solver
    parameters = get_parameters(mu, offspring_ratio, elite_fraction, div_fraction, mutprob, gcprob, cutprob_ratio,
                                trn_fraction)

    # Run solver with input parameters in the instance list
    results = cpp_wrapper.automaton_solver(solver, instances_dict.keys(), time_limit, see_traces=False, runs=runs,
                                           **parameters)

    # Calculate fitness = average gap across all runs and instances
    fitness = 0
    for inst, instance_costs in results.items():
        fitness -= sum([calculate_gap(cost, instances_dict[inst]) for cost in instance_costs]) / runs * 100
    fitness /= len(instances_dict)
    return fitness


def get_parameters(mu, offspring_ratio, elite_fraction, div_fraction, mutprob, gcprob, cutprob_ratio, trn_fraction):
    mu = round(mu)
    parameters = {
        'mu': mu,
        'lmbda': round(mu * offspring_ratio),
        'elite': max(1, round(mu * elite_fraction)),
        'div': max(1, round(mu * div_fraction)),
        'mutprob': mutprob,
        'gcprob': gcprob,
        'pcprob': min(1, gcprob * cutprob_ratio),
        'trnsize': min(mu, max(1, round(mu * trn_fraction)))
    }
    return parameters


def tune_solver(solver, instances, n_iter, time_limit=30 * 60, runs=1, verbose=2, save_logs=True):
    # Build instance dict with upper bounds of instances
    instance_dict = {inst: get_upper_bound(inst) for inst in instances}

    # Packing extra arguments into the evaluation function
    f_eval = partial(evaluate_config, solver=solver, instances_dict=instance_dict, time_limit=time_limit, runs=runs)

    # Parameter bounds
    pbounds = {
        'mu': (9.5, 100.499),  # To ensure equal probability across all values
        'offspring_ratio': (0.5, 2),
        'elite_fraction': (0, 0.5),
        'div_fraction': (0, 0.25),
        'mutprob': (0, 0.5),
        'gcprob': (0, 0.25),
        'cutprob_ratio': (1, 4),
        'trn_fraction': (0, 1),
    }
    # Build optimizer
    optimizer = BayesianOptimization(
        f=f_eval,
        pbounds=pbounds,
        verbose=verbose,
    )

    if save_logs:
        logger = JSONLogger(path=f"{RESULTS_FOLDER}/Calibration/{time.ctime().replace(':', '_')}.json")
        optimizer.subscribe(Events.OPTIMIZATION_STEP, logger)
        screen_printer = ScreenLogger(verbose=verbose)
        screen_printer._default_cell_size = 15
        optimizer.subscribe(Events.OPTIMIZATION_STEP, screen_printer)
        optimizer.subscribe(Events.OPTIMIZATION_START, screen_printer)
        optimizer.subscribe(Events.OPTIMIZATION_END, screen_printer)

    print(f'Start time: {time.ctime()}')
    optimizer.maximize(init_points=10, n_iter=n_iter)
    print(optimizer.max)
    return optimizer


def resume_tuning(log_file, solver, instances, n_iter, time_limit=30 * 60, runs=1, verbose=2):
    # Build instance dict with upper bounds of instances
    instance_dict = {inst: get_upper_bound(inst) for inst in instances}

    # Packing extra arguments into the evaluation function
    f_eval = partial(evaluate_config, solver=solver, instances_dict=instance_dict, time_limit=time_limit, runs=runs)

    # Parameter bounds
    pbounds = {
        'mu': (9.5, 100.499),  # To ensure equal probability across all values
        'offspring_ratio': (0.5, 2),
        'elite_fraction': (0, 0.5),
        'div_fraction': (0, 0.25),
        'mutprob': (0, 0.5),
        'gcprob': (0, 0.25),
        'cutprob_ratio': (1, 4),
        'trn_fraction': (0, 1),
    }
    # Build optimizer
    optimizer = BayesianOptimization(
        f=f_eval,
        pbounds=pbounds,
        verbose=verbose,
    )
    # Load previous points from log file
    load_logs(optimizer, logs=[log_file])

    logger = JSONLogger(path=log_file)
    optimizer.subscribe(Events.OPTIMIZATION_STEP, logger)
    screen_printer = ScreenLogger(verbose=verbose)
    screen_printer._default_cell_size = 15
    optimizer.subscribe(Events.OPTIMIZATION_STEP, screen_printer)
    optimizer.subscribe(Events.OPTIMIZATION_START, screen_printer)
    optimizer.subscribe(Events.OPTIMIZATION_END, screen_printer)

    print(f'Start time: {time.ctime()}')
    optimizer.maximize(n_iter=n_iter)
    print(optimizer.max)
    return optimizer


if __name__ == "__main__":
    solver = 'madcom'
    instances = ['egl-g2-D.dat', 'Hefei-6.txt', 'Beijing-2.txt']
    # instances = ['E15.dat', 'egl-e1-A.dat']
    n_iter = 50
    time_limit = 60
    optimizer = tune_solver(solver, instances, n_iter, time_limit=time_limit, verbose=2, save_logs=True)
    # log_file = "../Results/Calibration/Thu May  5 19_45_49 2022.json"
    # optimizer = resume_tuning(log_file, solver, instances, n_iter, time_limit=time_limit,
    #                           verbose=2)
