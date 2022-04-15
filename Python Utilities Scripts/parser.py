import main
import os
from datetime import datetime as dt
from datetime import timedelta as td

SOLVERS = {'madcom', 'madcom_hd', 'gencarp'}


def results_parser(instance_list, start_date=None, end_date=None):
    if not start_date:
        start_date = dt(2020, 1, 1)
    if not end_date:
        end_date = dt.now()

    for instance in instance_list:
        instance_name = instance.strip().split('.')[0]
        if instance_name[:3] == 'Lpr' or instance_name[:4] == 'mval':
            type_folder = 'MCARP'
        else:
            type_folder = 'CARP'
        solution_values = []
        # Parse through results of instance
        instance_results_folder = f"{main.RESULTS_FOLDER}/{type_folder}/{instance_name}"
        for result_filename in os.listdir(instance_results_folder):
            # Check if filename is a date format
            if result_filename[:3] != '202':
                continue
            results_date = dt.strptime(result_filename[:-4], "%Y-%m-%d %H_%M_%S")
            # If the results file is within the specified interval
            if start_date < results_date < end_date:
                f = open(f"{instance_results_folder}/{result_filename}")
                solution_values.append(f.readline().strip())
                f.close()
        print(f"{instance_name},{','.join(solution_values)}")


def logfile_parser(log_filename):
    instance_list = set()
    start_date = None
    end_date = None
    tolerance = td(minutes=10)
    with open(log_filename) as f:
        for line in f:
            spl = line.split()
            # Beginning of solver results
            if spl[0] in SOLVERS:
                if len(instance_list) and start_date is not None and end_date is not None:
                    results_parser(instance_list, start_date - tolerance, end_date + tolerance)
                    instance_list = set()
                    start_date = None
                    end_date = None
                print(line.strip())
            else:
                # Parse date from line
                run_date = dt.strptime(f"{spl[-6]} {spl[-5].split('.')[0]}", "%m/%d/%Y %H:%M:%S")
                if start_date is None:
                    start_date = run_date
                end_date = run_date

                # Parse instance name and add to instance list
                instance_list.add(spl[-3])
        # Last block of runs
        if len(instance_list) and start_date is not None and end_date is not None:
            results_parser(instance_list, start_date - tolerance, end_date + tolerance)
            instance_list = set()
            start_date = None
            end_date = None


if __name__ == '__main__':
    # instance_list = [
    #     'F1_g-6',
    #     'Beijing-3',
    #     'Beijing-5',
    #     # 'egl-g2-E',
    #     # 'Hefei-10',
    # ]
    # start_date = dt(2022, 2, 11, 18, 30)
    # end_date = dt.now()
    # results_parser(instance_list, start_date, end_date)

    logfile_parser('../Program/log.txt')
