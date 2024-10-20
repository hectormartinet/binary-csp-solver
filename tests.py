import subprocess 
import os
 
if __name__ == "__main__":
    all_files = os.listdir('./Tests')
    print("Launch tests:")
    success_tests = 0
    for i, file in enumerate(all_files):
        print(file)
        result = subprocess.run(['./run.exe', './Tests/' + file,  'LP',  'random',  'random',  '0'], stdout=subprocess.PIPE, text=True)
        success_tests+=1
        # for line in iter(result.stdout.readline, ''):
        #     line = line.replace('\r', '').replace('\n', '')
        #     print(line) 
    print(str(success_tests) + "/" + str(len(all_files)) + " successful tests")