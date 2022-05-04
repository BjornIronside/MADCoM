from main import INSTANCES_FOLDER
import re


def get_problem_type(instance):
    if len(instance.split('/')) == 1:
        instance_name = instance
    else:
        instance_name = instance.split('/')[-1]
    if instance_name[:3] == 'Lpr' or instance_name[:4] == 'mval':  # MCARP Instances
        return 'MCARP', 36
    else:
        return 'CARP', 30


def get_upper_bound(instance):
    if len(instance.split('/')) == 1:
        prob_name, ins_type = get_problem_type(instance)
        instance_path = f"{INSTANCES_FOLDER}/{prob_name}/{instance}"
    else:
        instance_path = instance
    with open(instance_path, 'r') as f:
        f.readline()
        line = f.readline()
        numbers = re.findall('[0-9]+', line)
        return int(max(numbers))


def calculate_gap(cost, ub):
    return cost / ub - 1


if __name__ == '__main__':
    # instance = 'Beijing-3.txt'
    instance = 'E15.dat'
    print(get_upper_bound(instance))
