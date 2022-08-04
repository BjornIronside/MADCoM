import main
import os
from itertools import product

TYPE_FOLDER = {
    30: 'CARP',
    36: 'MCARP'
}


def generate_batch_file(config_filename, batch_filename, log_filename):
    config_file = open(config_filename, 'r')
    solvers = config_file.readline().split()[1:]
    runs = int(config_file.readline().split()[-1])
    time_limit = int(config_file.readline().split()[-1])
    added_flags = f"-t {time_limit}"

    # Reading Parameters
    params = {}
    config_file.readline()
    line = config_file.readline()
    while line != "INSTANCES:\n":
        spl = line.split()
        params[spl[0]] = spl[1:]
        line = config_file.readline()
    param_flags = []
    for param_set in product(*params.values()):
        pflag = f''
        for f, p in zip(params.keys(), param_set):
            pflag += f"{f} {p} "
        param_flags.append(pflag)
    for pflag in param_flags:
        print(pflag)

    # Reading Instances
    all_instances = set(os.listdir('../Instances/CARP') + os.listdir('../Instances/MCARP'))
    instance_list = []
    instance_types = []
    for line in config_file:
        instance_name = line.strip()
        if instance_name not in all_instances:
            raise Exception(f"Instance \"{instance_name}\" not found. Please check if the instance name is written "
                            f"correctly.")
        instance_list.append(instance_name)
        if instance_name[:3] == 'Lpr' or instance_name[:4] == 'mval':  # MCARP Instances
            instance_types.append(36)
        else:
            instance_types.append(30)

    # Writing batch file
    output_file = open(batch_filename, 'w')
    print("ECHO OFF\n", file=output_file)
    for solver in solvers:
        for pflag in param_flags:
            print(f"::{solver} {pflag}\nECHO {solver} {pflag}\nECHO {solver} {pflag} >> {log_filename}\n", file=output_file)
            for instance, ins_type in zip(instance_list, instance_types):
                for i in range(1, runs + 1):
                    # Prints for progress
                    line = f"ECHO %DATE% %TIME% - {instance[:-4]} Run {i}/{runs}"
                    print(line, file=output_file)
                    print(f"{line} >> {log_filename}", file=output_file)

                    # Command to solve the instance
                    print(f"{solver} \"{main.INSTANCES_FOLDER}/{TYPE_FOLDER[ins_type]}/{instance}\" "
                          f"-type {ins_type} {added_flags} {pflag}", file=output_file)
            print('\n', file=output_file)

    # Close files
    config_file.close()
    output_file.close()


if __name__ == '__main__':
    generate_batch_file(
                        config_filename='automaton_config.txt',
                        # config_filename='test_config.txt',
                        # batch_filename='automaton.bat',
                        batch_filename='../Program/uhgs_mval_instances.bat',
                        # batch_filename='../Population/medoid_initialization.bat',
                        log_filename='uhgs_mval_instances_limited.txt'
                        # log_filename='../Population/medoid_initialization.txt'
    )
