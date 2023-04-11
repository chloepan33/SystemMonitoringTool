# **System Monitoring Tool**

---

A simple command line program written in C that reports different metrics of system utilization, able to run the different queries of the system *concurrently*. The program provides information about basic system information, CPU utilization, memory utilization, and users’ information.

## **Getting Started**

---

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### **Prerequisites**

You need a C compiler to build the program. GCC is recommended for this purpose. The program should be run on a Linux operating system.

### **Installing**

1. Clone the repository to your local machine
    
    ```
    git clone <https://github.com/chloepan33/SystemMonitoringTool.git>
    
    ```
    
2. Using make file to compile the program
    
    ```
    make
    ```
    
3. Run the program
    
    ```
    ./sys_monitoring_tool
    ```
    

### Usage

Run the program directly

```
./sys_monitoring_tool
```

The program will output the following information:

```
----------------------------
Nbr of samples: 10 -- every 1 secs
Memory usage: 1476 kilobytes
3.69 GB / 8.14 GB  -- 6.70 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.70 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.70 GB / 18.73 GB
3.69 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.70 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.70 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
3.70 GB / 8.14 GB  -- 6.71 GB / 18.73 GB
----------------------------
### Sessions/users ### 
zha10849 pts/102 tmux(538474).%0
wangz725 pts/1031 99.229.78.191
jookendr pts/1040 70.55.37.244
huan2534 pts/1069 184.147.220.161
jianganq pts/1084 69.166.116.217
----------------------------
Number of cores: 3
CPU usage: 1.68%
----------------------------
### System Information ### 
System Name:  Linux
Machine Name:  mathlab
Version:  #154-Ubuntu SMP Thu Jan 5 17:03:22 UTC 2023
Release:  5.4.0-137-generic
Architecture: x86_64
----------------------------
```

With ***10*** samples and sampled every ***1*** second, by default.

The following command line arguments are available for use

- `-system`, which will only display system usage information
- `-user`, which will only display user usage information
- `-graphics`, which will provide graphical output
- `-sequential`, which will show information sequentially without requiring a screen refresh
- `-samples=N` , which allows a value ***N*** to be specified to indicate how many times statistics will be collected
- `-tdelay=T`, which specifies the frequency of sampling in ***T*** seconds

The program also takes positive integers as arguments.

For example, by running the program

```
./sys_monitoring_tool 5 2

```

The sample size will now be 5 and sampled every 2 seconds.

If user input 3 integers or more, the program will display “`To many input integers!`” error message.

If only 1 integer been entered, only the sample size will change to the input value while the sampling frequency remains unchanged as 1 sec.

Most of the command line arguments could be combined.

For example `./sys_monitoring_tool --system --graphics --samples=5`, will result in displaying only the system information in graphical form with 5 samples.

However, `--user`  combined with any of `--system`, `--graphics` will be considered as invalid.

The program will display “`Command combination invalid`” error message.

Any other user input will lead to “`Invalid command line arguments`” error message.

---

### More information

The program displaying information including user usage and system usage.

For user usage, it displays all current login users’ information including its username, device name and host name for remote login.

For system usage, it displays total utilization of the CPU and memory.

Memory information are displayed in unit of GB, including total physical memory, used physical memory, total virtual memory and used virtual memory.

- total physical memory = totalram
- used physical memory = (totalram - freeram) - (bufferram + cachedram + SReclaimable)
- total virtual memory = totalram + total swap
- used virtual memory = used physical memory + totalswap - freeswap

Calculated by reading file /proc/meminfo.

For graphical representations.

- for CPU utilization: “`|||`” are used to represent positive percentage increase
- for memory utilization: showing the variation of memory used
    - if memory increase but less then 0.01 GB, “`|o`” will be displayed, otherwise if memory increase “`######*`” will be shown
    - if memory decrease but less then 0.01 GB, “`|@`” will be displayed, otherwise if memory decrease “`:::::@`” will be shown

When user send control-c signal, the program will ask if user really want to quit the program.

As for control-z signal, the program will ignore it and perform nothing.

---

### How do I solve the problem

To make the code work in a concurrent fashion, I first divided the functions into three main parts: memory usage, CPU usage, and user information. For each of these parts, I created a corresponding C file that can be run independently, takes command line arguments, and reports user/memory/CPU information ten times per second by default.

Next, to achieve concurrency, I utilized `fork()` to create three child processes. Each process is responsible for sampling user/memory/CPU information independently by using `execvp()` to call the other C files and pass arguments. To enable communication between the parent and child processes, I called `pipe()` three times to create three pipes for each child process to write to and for the parent process to read from.

Using `dup2()`, I redirected the standard output of the child process to the corresponding pipe write file descriptor. The parent process can then read from these pipes and print out each iteration to the standard output (i.e., the terminal screen).

In summary, my approach to making the code work concurrently involved dividing the functions into independent parts, creating child processes to run each part, and using pipes to enable communication between the child and parent processes.

---

# Function Overview in sys_monitoring_tool.c

All documentation for functions are included within the code file.

### **`ctrlc_handler(int sig)`**

This function is a handler for the **`control c`** signal. It asks the user if they really want to quit the program. If the user enters **`y`** or **`Y`**, the program terminates. If the user enters anything else, the control-c signal is reset to this handler.

### **`ctrlz_handler(int sig)`**

This function is a handler for the **`control z`** signal. It does nothing and ignores the signal.

### **`set_signals()`**

This function sets the signals for **`control c`** and **`control z`**, and performs error checking.

### **`saveCursorPosition()`**

This function saves the current cursor position.

### **`restoreCursorPosition()`**

This function restores the most recently saved cursor position.

### **`read_output(FILE *read_file)`**

This function reads output written to the given result file. It reads from the pipe line by line and prints the output, until it reads a special string indicating that one iteration is done.

### **`ShowMemoryUsage()`**

This function displays the memory used by the current program in units of kilobytes. If it fails to get memory usage, it shows an error message.

### **`ShowSystemInfo()`**

This function displays the system information, including system name, machine name, version, release, and architecture. If it fails to get system information, it shows an error message.

### **`RunStats(int *fd, char *file, char *argv[])`**

This function creates a child process for running an independent C program. It creates a pipe and a child process, and redirects the standard input and output to the pipe. The child process is responsible for running the C program and writing the output to the pipe. The parent process reads the output from the pipe and prints it to the console.