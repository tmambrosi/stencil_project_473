import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from collections import namedtuple


# tanner - i will fix these when we get more project details
def plot_total_times():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'yellow')


    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label="Compute Times")
    plt.show()

def plot_compute_times():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'Total compute time',color = 'yellow')


    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label="Compute Times")
    plt.show()

def plot_speedup():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'SpeedUp',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'SpeedUp',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'SpeedUp',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'SpeedUp',color = 'yellow')

    
    plt.legend(['5x5', '10x10', '20x20', '40x40'])  
    plt.title(label="Speedup Overall")
    plt.show()
    
def plot_mmspeedup():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'MM_SpeedUp',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'MM_SpeedUp',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'MM_SpeedUp',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'MM_SpeedUp',color = 'yellow')    
   
    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label="Speedup Computation")
    plt.show()

def plot_efficiency():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency',color = 'yellow')    
   
    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label="Efficiency")
    plt.show()

def plot_efficiency_comp():
    ax = plt.gca()

    df1.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency Computation',color = 'red', xlabel = 'Number of Threads', ylabel = 'Time (sec)')
    df2.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency Computation',color = 'blue')
    df3.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency Computation',color = 'green')
    df4.plot(ax = ax, kind = 'line', use_index = True, y = 'Efficiency Computation',color = 'yellow')    
   
    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label="Efficiency Computation")
    plt.show()
    
def do_speedup(dataframe):
    dataframe['SpeedUp'] = dataframe['Total Time']/dataframe['Computation Time']

def do_mmspeedup(dataframe):
    dataframe['Stencil SpeedUp'] = dataframe.loc[1, "Total Time"]/dataframe['Total Time']

def do_efficiency(dataframe):
    dataframe['Efficiency'] = dataframe['SpeedUp']/dataframe['Number of Threads']

def do_mmefficiency(dataframe):
    dataframe['Efficiency Computation'] = dataframe['Stencil SpeedUp']/dataframe['Number of Threads']


num_experiments = 2
max_threads = 16

sizes = [20]


pth_data_20 = pd.read_csv(f'pth_data_20.txt', sep=" ", header=None)
pth_data_20.columns = ["Total Time", "Computation Time", "Number of Threads", "rows", "cols"]

#mpi_data = pd.read_csv('pth_data.txt', sep=" ", header=None)

do_speedup(pth_data_20)
do_mmspeedup(pth_data_20)
do_efficiency(pth_data_20)
do_mmefficiency(pth_data_20)


ax = plt.gca()


    


