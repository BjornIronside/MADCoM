from parsers import fetch_result
import subprocess
from datetime import datetime as dt
from main import INSTANCES_FOLDER, SOLVERS_FOLDER
from utils import get_problem_type

ALLOWED_ARGUMENTS = {'madcom': ('hdf', 'mutprob', 'gcprob', 'pcprob', 'trnsize', 'mu', 'lmbda', 'elite', 'div'),
                     'madcom_hd': ('hdf', 'mutprob', 'gcprob', 'pcprob', 'trnsize', 'mu', 'lmbda', 'elite', 'div'),
                     'gencarp': []}


def run_solver(solver, instance, time_limit=600, **arguments):
    # Check solver exists
    if solver not in ALLOWED_ARGUMENTS.keys():
        raise Exception('Unrecognized solver.')

    # Get instance name, full path and problem type
    if len(instance.split('/')) == 1:  # If instance is not a full path
        instance_name = instance
        prob_name, ins_type = get_problem_type(instance_name)
        instance_path = f"{INSTANCES_FOLDER}/{prob_name}/{instance}"
    else:
        instance_path = instance
        instance_name = instance.split('/')[-1]
        prob_name, ins_type = get_problem_type(instance_name)

    # Start building command
    command = f'{solver} "{instance_path}" -type {ins_type} -t {time_limit}'

    # Add command line parameters
    for arg, value in arguments.items():
        if arg not in ALLOWED_ARGUMENTS[solver]:
            raise Exception(f'Invalid argument {arg} for solver {solver}')
        command += f" -{arg} {value}"


    # Run solver
    date = dt.now()
    completed_process = subprocess.run(command, shell=True, cwd=SOLVERS_FOLDER, creationflags=subprocess.CREATE_NEW_CONSOLE)
    # completed_process = subprocess.run(command, shell=True, cwd=SOLVERS_FOLDER)

    # # Check if everything went well
    # if completed_process.returncode:
    #     raise Exception("Solver crashed.")

    # Retrieve and return result of the run
    result = fetch_result(instance_name, date, tol=time_limit / 2)
    # If run is successful, return the cost as an int, else repeat
    if result.isnumeric():
        return int(result)
    return run_solver(solver, instance, time_limit, **arguments)


def automaton_solver(solver, instance_list, time_limit, runs=1, **arguments):
    results = {}
    for instance in instance_list:
        instance_costs = []
        for i in range(runs):
            instance_costs.append(run_solver(solver, instance, time_limit, **arguments))
        results[instance] = instance_costs
    return results


if __name__ == '__main__':
    solver = 'madcom'
    instance = 'E15.dat'
    instance_list = ['Hefei-6.txt', 'Beijing-3.txt']
    time_limit = 30
    arguments = {'hdf': 0.5, 'mutprob': 0.3, 'gcprob': 0.05, 'pcprob': 0.15, 'trnsize': 25}

    result = run_solver(solver, instance, time_limit, **arguments)
    # result = automaton_solver(solver, instance_list, time_limit / 2, **arguments)
    print(result)
