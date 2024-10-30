import subprocess 
import argparse
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file', type=str, required=True)
    parser.add_argument('-rootSolve', '--rootSolveMethod', choices=['LP', 'FC', 'AC4', 'AC3'], type=str, default='LP')
    parser.add_argument('-nodeSolve', '--nodeSolveMethod', choices=['LP', 'FC', 'AC4', 'AC3'], type=str, default='LP')
    parser.add_argument('-var', '--varChooser', choices=['random', 'smallest', 'max'], type=str, default='random')
    parser.add_argument('-val', '--valChooser', choices=['random', 'smallest', 'copy'], type=str, default='random')
    parser.add_argument('-t', '--timeLimit', type=str, default='-1')
    parser.add_argument('-seed', '--randomSeed', type=str, default='')
    parser.add_argument('-v', '--verbosity', choices=['0', '1'], type=str, default='1')
    parser.add_argument('-nSol', '--nbSolution', type=str, default='1')
    args = parser.parse_args()
    result = subprocess.Popen(['./run.exe', args.file,  args.rootSolveMethod, args.nodeSolveMethod,  args.varChooser,  args.valChooser,  args.verbosity, args.timeLimit, args.randomSeed, args.nbSolution, "0", "0"], stdout=subprocess.PIPE, text=True)

    for line in iter(result.stdout.readline, ''):
        line = line.replace('\r', '').replace('\n', '')
        print(line)