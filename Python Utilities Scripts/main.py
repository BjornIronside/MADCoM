import os

RESULTS_FOLDER = "../Results"
INSTANCES_FOLDER = "../Instances"


def create_results_folders():
    problem_types = ('CARP', 'MCARP')
    for pt in problem_types:
        benchmarks_folder = f'{INSTANCES_FOLDER}/{pt}'
        for instance in os.listdir(benchmarks_folder):
            instance_name = instance.split('.')[0]
            if instance_name == 'README':
                continue
            instance_folder = f'{RESULTS_FOLDER}/{pt}/{instance_name}'
            if not os.path.isdir(instance_folder):
                os.mkdir(instance_folder)


if __name__ == '__main__':
    create_results_folders()
