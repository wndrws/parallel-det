from os import system, access, remove, W_OK
import numpy as np
import scipy.stats as st
import math
import argparse

def chooseAppropriateTimeUnit(timeInMicros):
    if (timeInMicros / 1000000 > 1):
        return "s"
    elif (timeInMicros/ 1000 > 1):
        return "ms"
    else:
        return "us"

def adjustToTimeUnit(timeInMicros, timeunit):
    if timeunit == "s":
        return timeInMicros / 1000000
    elif timeunit == "ms":
        return timeInMicros / 1000
    else:
        return timeInMicros


def formatProperly(mean, r, timeunit):
    mean = adjustToTimeUnit(mean, timeunit)
    r = adjustToTimeUnit(r, timeunit)
    precision = -round(math.log10(r)) + 1
    if precision < 0:
        roundedMean = int(round(mean, precision))
        roundedRadius = int(round(r, precision))
        return "{:d} ± {:d} {}".format(roundedMean, roundedRadius, timeunit)
    else:
        return "{:.{prec}f} ± {:.{prec}f} {}".format(mean, r, timeunit, prec=precision)

def findMeanEstimate(vals, p=0.9):
    N = len(vals)
    mean = np.mean(vals)
    tinv = st.t.ppf((1 + p)/2, N - 1)
    r = tinv*st.sem(vals)/math.sqrt(N)
    return (mean, r)

fileWithTotalTimes = "log-total-times.txt"
fileWithDetCalcTimes = "log-calc-times.txt"

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Statistics on determinant calculation")
    parser.add_argument("executable")
    parser.add_argument("fileWithMatrix")
    parser.add_argument("--runsCount", type=int, required=True)
    parser.add_argument("--cleanup", action="store_true")
    args = vars(parser.parse_args())
    
    if args["cleanup"] and access(fileWithTotalTimes, W_OK) and access(fileWithDetCalcTimes, W_OK):
        remove(fileWithTotalTimes)
        remove(fileWithDetCalcTimes)

    runsCount = args["runsCount"]
    executable = args["executable"]
    fileWithMatrix = args["fileWithMatrix"]
    print("Performing %d runs of \"%s\" using matrix in \"%s\" file..." % (runsCount, executable, fileWithMatrix))

    for i in range(runsCount):
        system("%s %s > NUL" % (executable, fileWithMatrix))
        print("\r  > Run progress: %d/%d" % (i + 1, runsCount), end="\r")
    
    with open(fileWithTotalTimes, "r") as logfile:
        totalTimesList = [int(t) for t in logfile.readlines()]

    with open(fileWithDetCalcTimes, "r") as logfile:
        calcTimesList = [int(t) for t in logfile.readlines()]

    totalMean, totalIntervalRadius = findMeanEstimate(totalTimesList)
    timeunit = chooseAppropriateTimeUnit(totalMean)
    print("\nResults:")
    print("  Full program run mean time        = %s" % formatProperly(totalMean, totalIntervalRadius, timeunit))
    print("  Determinant calculation mean time = %s" % formatProperly(*findMeanEstimate(calcTimesList), timeunit))