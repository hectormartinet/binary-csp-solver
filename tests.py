import subprocess 
import os
 
if __name__ == "__main__":
    fichier = open("./Tests/tests_instructions.txt", "r")
    tests = fichier.read()
    fichier.close()
    all_tests = tests.split("\n")
    success_tests = 0
    print("Launch tests:")
    errors = []
    for test in all_tests[1:]:
        parameters = test.split(" ")
        file = parameters[0]
        print(file)
        result = subprocess.Popen(['./run.exe', './Tests/' + file,  parameters[1],  parameters[2],  parameters[3],  '0', parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]], stdout=subprocess.PIPE, text=True, stderr=subprocess.PIPE)
        # for l in result.stdout:
        #     print(l)
        hasError = False
        for err in result.stderr:
            errors.append([file, err])
            hasError = True
        if not hasError:
            success_tests+=1
    print(str(success_tests) + "/" + str(len(all_tests)-1) + " successful tests\n")
    if (len(errors)):
        print("Errors:")
        for error in errors:
            print(error[0] + ": " + error[1])
