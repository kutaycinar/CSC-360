# Unix Shell Interpreter

This project implements a simple shell program similar to the Unix shell bash or the command prompt in Windows.

The shell will support execution of programs, the ability to change directories and background execution.

## Part I – Basic execution

Using fork() and execvp() this shell implements the ability for the user to execute arbitrary commands.

For example, if the user types:
```
shell> ls -l /usr/bin
```
This shell executes the ls command with the parameters ```-l``` and ```/usr/bin```, which results in the contents of the directory ```/usr/bin``` being displayed on the screen.

## Part II – Changing Directories

Using the functions getcwd() and chdir() this program has the functionality so that users can change directories using the command cd, and print the current working directory using the 
command pwd.

The cd command takes one argument that is the directory to change into. The special parameter .. indicates that the current directory should move “up” one directory. That is, if the current directory is ```/home/kutaycinar/foo``` and the user types **cd ..** the new directory will be ```/home/kutaycinar```.

The pwd command takes no parameters.

Additionally, the shell prompt is changed from what's shown in part 1 to include the current working directory:

```/home/kutaycinar/csc360>```

## Part III – Background Execution

This shell implements a simplfiied version of background processing that supports a fixed number (in this case 5) of processes executing in the background.

If the user types: bg cat foo.txt the shell starts the command cat with the argument foo.txt in the background. That is, the program will execute and the shell will also continue to execute.

The command bglist will display a listing of all the commands currently executing in 
the background, similar to:
```
0: /home/kutaycinar/a1/foo
1: /home/kutaycinar/a1/foo
Total Background jobs: 2
```
In this case, there are 2 background jobs, both running the program foo.

The command bgkill 1 will send the TERM signal to job 1 to terminate that job.

## Part IV – Suspending and Resuming background jobs

Many shells allow the user to suspend and resume jobs that are running. This shell implement a simplified version of this functionality which allows the user to suspend and resume background jobs only.

The command _stop_ will suspend a job that is currently executing in the background by sending it the SIGSTOP signal.

The command _start_ will resume a job that is currently stopped by sending it the SIGCONT signal.

Both the _start_ and the _stop_ command take a single parameter which is the job number to start or stop respectively. The program outputs an error message if the user tries 
to stop a job that is currently stopped or start a job which is currently running.

The bglist command is updated to include a new field which is the status of the current background processes, where R indicates running and S indicates stopped:
```
0[R]: /home/jason/a1/foo
1[S]: /home/jason/a1/foo
Total Background jobs: 2
```

