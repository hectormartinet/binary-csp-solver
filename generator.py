import csv
import os

i=-1
deja_vu = [0]*81
with open("C://Users//Horat//Downloads//archive//sudoku_cluewise.csv") as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
    for row in spamreader:
        i+=1
        if (i==0): continue
        [instance, solution, indices] = row[0].split(",")
        indices = int(indices)
        if deja_vu[indices]: continue
        deja_vu[indices] = 1
        f = open("instances//sudoku//sudoku_"+str(indices)+".txt", "a")
        f.write(instance)
        f.close()