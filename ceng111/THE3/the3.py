from math import exp
def forward_pass(Network, X):
    Networkk = Network[:]
    x = X[:]
    while len(Networkk) > 0:
        if Networkk[0][0][:6] == "linear":
            Weights = Networkk[0][1]
            l = len(Weights)
            f = len(Weights[0])
            a = []
            for i in range(l):
                b = 0
                for j in range(f):
                    c = 0
                    c = Weights[i][j] * x[j]
                    b += c
                a += [b]
            x = a
        elif Networkk[0][:4] == "relu":
            for i in range(len(x)):
                if x[i] < 0:
                    x[i] = 0
        elif Networkk[0][:7] == "sigmoid":
            for i in range(len(x)):
                if x[i] < -700 or abs(x[i] + 700) == 0.000001:
                    x[i] = 0
                elif x[i] > 700 or abs(x[i] - 700) == 0.000001:
                    x[i] = 1
                elif (x[i] > -700) and (x[i] < 700):
                    x[i] = 1 / ( 1 + exp((-x[i])))
        Networkk.pop(0)
    return x


