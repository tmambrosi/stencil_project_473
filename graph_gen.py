import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os
# from collections import namedtuple

dir = "figs/"

if not os.path.exists(dir):
    os.mkdir(dir)

num_threads = 4

def plot_graph(y_type, y_label):
    # total time v num threads
    ax = plt.gca()

    pth_data_0.plot(ax = ax, kind = 'line', y = y_type, x = 'Number_of_Threads', ylabel = y_label, xlabel='Number of Threads', xticks=(np.arange(1, num_threads+1, step=1)))
    pth_data_1.plot(ax = ax, kind = 'line', y = y_type, x = 'Number_of_Threads')
    pth_data_2.plot(ax = ax, kind = 'line', y = y_type, x = 'Number_of_Threads')
    pth_data_3.plot(ax = ax, kind = 'line', y = y_type, x = 'Number_of_Threads')

    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label=f"PTH: {y_type} v Number of Threads")
    plt.savefig(f"{dir}pth_{y_type}.png")

    ax.cla()  

    bx = plt.gca()

    mpi_data_0.plot(ax = bx, kind = 'line', y = y_type, ylabel = y_label, x = 'Number_of_Threads', xticks=(np.arange(1, num_threads+1, step=1)))
    mpi_data_1.plot(ax = bx, kind = 'line', y = y_type, x = 'Number_of_Threads')
    mpi_data_2.plot(ax = bx, kind = 'line', y = y_type, x = 'Number_of_Threads')
    mpi_data_3.plot(ax = bx, kind = 'line', y = y_type, x = 'Number_of_Threads')

    #plt.legend(['5x5', '10x10', '20x20'])
    plt.legend(['5x5', '10x10', '20x20', '40x40'])
    plt.title(label=f"MPI: {y_type} v Number of Threads")
    plt.savefig(f"{dir}mpi_{y_type}.png")

    bx.cla() 

def do_speedup(dataframe):
    dataframe['SpeedUp'] = dataframe.iat[0,0]/dataframe['Total_Time']

def do_compspeedup(dataframe):
    dataframe['Comp_SpeedUp'] = dataframe.iat[1,0]/dataframe['Computation_Time']

def do_efficiency(dataframe):
    dataframe['Efficiency'] = dataframe['SpeedUp']/dataframe['Number_of_Threads']

def do_compefficiency(dataframe):
    dataframe['Comp_Efficiency'] = dataframe['Comp_SpeedUp']/dataframe['Number_of_Threads']

def my_reader(file_name):
    df = pd.read_csv(file_name, sep=" ", header=None)
    df.columns = ['Total_Time', 'Computation_Time', 'Number_of_Threads', 'rows', 'cols']

    do_speedup(df)
    do_compspeedup(df)
    do_efficiency(df)
    do_compefficiency(df)

    return df

pth_data_0 = my_reader('pth_data_500.txt')
pth_data_1 = my_reader('pth_data_1000.txt')
pth_data_2 = my_reader('pth_data_2000.txt')
pth_data_3 = my_reader('pth_data_4000.txt')

mpi_data_0 = my_reader('mpi_data_500.txt')
mpi_data_1 = my_reader('mpi_data_1000.txt')
mpi_data_2 = my_reader('mpi_data_2000.txt')
mpi_data_3 = my_reader('mpi_data_4000.txt')

plot_graph('Total_Time', 'Time (sec)')
plot_graph('Computation_Time','Time (sec)')
plot_graph('SpeedUp', 'Speedup')
plot_graph('Comp_SpeedUp', 'Speedup')
plot_graph('Efficiency','Efficiency')
plot_graph('Comp_Efficiency','Efficiency')

