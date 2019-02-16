import random

def printMatrix(m, n, output):
    output.write(str(m) + " " + str(n) + "\n")
    
    for i in range(m):
        for j in range(n):
            output.write(str(random.randrange(10)) + " ")
        output.write("\n")

m = 5000
n = 2000

outputFile = open('fixture.txt', 'w')
printMatrix(m, n, outputFile)
printMatrix(n, m, outputFile)
outputFile.close()
