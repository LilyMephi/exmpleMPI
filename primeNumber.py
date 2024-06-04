import matplotlib.pyplot as plt
import numpy as np

x = np.loadtxt("logistic.txt",delimiter=',',dtype=float)
points = 1000*50
r =[]
l = 2.8

for i in range(1000*50):
    if( i%50 == 0):
        l += 0.001200
    r.append(l)
    

n =[]
r = np.around(r, decimals = 6)

for tmp in x:
    temp = np.around(tmp, decimals = 15)
    unique,counts = np.unique(temp, return_counts=True)
    n.append(len(unique))

plt.scatter(r,x,s = 0.5)

#plt.plot(r,n,c = "green")
plt.show()