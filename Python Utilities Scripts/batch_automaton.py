import main


def generate_batch_file(config_filename, batch_filename, log_filename):
    config_file = open(config_filename, 'r')
    solvers = config_file.readline().split()[1:]
    runs = int(config_file.readline().split()[-1])
    time_limit = int(config_file.readline().split()[-1])
    added_flags = f"-t {time_limit}"

    # Reading Instances
    instance_list = []
    instance_types = []
    config_file.readline()  # Instances header
    for line in config_file:
        instance_name = line.strip()
        instance_list.append(instance_name)
        if instance_name[:3] == 'Lpr' or instance_name[:4] == 'mval': # MCARP Instances
            instance_types.append(36)
        else:
            instance_types.append(30)

    output_file = open(batch_filename, 'w')
    print("ECHO OFF\n", file=output_file)
    for solver in solvers:
        print(f"::{solver}\nECHO {solver}\nECHO {solver} >> {log_filename}\n", file=output_file)
        for instance, ins_type in zip(instance_list, instance_types):
            for i in range(1, runs + 1):
                # Prints for progress
                line = f"ECHO %DATE% %TIME% - {instance[:-4]} Run {i}/{runs}"
                print(line, file=output_file)
                print(f"{line} >> {log_filename}", file=output_file)
                # Command to solve the instance
                if ins_type == 30:
                    ins_folder = 'CARP'
                else:
                    ins_folder = 'MCARP'
                print(f"{solver} \"{main.INSTANCES_FOLDER}/{ins_folder}/{instance}\" -type {ins_type} {added_flags}", file=output_file)
        print('\n', file=output_file)

    # Close files
    config_file.close()
    output_file.close()
    

if __name__ == '__main__':
    generate_batch_file(config_filename='automaton_config.txt',
                        batch_filename='../Program/automaton.bat',
                        log_filename='log.txt')
