**LINUX DAEMON FOR SINGLE THREADED APPLICATION**
 
=>	I’ve named this code “**WatchDog.c**”.

=>	This is the code for Linux daemon/ background service written in C.

=>	This code runs in background and creates Linux notification for specific file if it were created, deleted or modified.

=>	To run this file, make sure that “libnotify development package” is already installed on the system. I’ve primarily used UBUNTU distro. If its not installed then install using this command “**sudo apt-get install libnotify-dev**”.

=>	For inotify api, refer this link =>  (https://tinyurl.com/3rytjp2n)

=>	To create desktop notifications refer this link => (https://tinyurl.com/2p9xaabj)

=>	For strtok() function refer this link => (https://tinyurl.com/46nkzrke)

=>  For more steps information refer this link => https://www.youtube.com/watch?v=9nDYYc_7sKs


================================================================================

**STEPS TO RUN THIS CODE**

=>	First lets make any random file, lets say “paji.txt” on the same location where the WatchDog.c is located.

=>	Now run this command in terminal “gcc -o WatchDog WatchDog.c $(pkg-config --cflags --libs libnotify)”.

=>	After this run ./WatchDog paji.txt       i.e. specify the file that you want to keep watch on.

=>	Now if I modify and save this paji.txt file, I will get the instant notification. See below example-  

=>	After doing all the changes, if we want to close the paji.txt file, just close it. Again we will get its notification. The daemons @ background will keep running, so I have sent signal to close the daemon and clean the watch list queue, so that Daemon , on new file, makes new notifications and not just continue with old ones. 

=>  See notification being generated above at top for any changes being made in out example file paji.txt.

![image](https://github.com/rish1602/WatchDog/assets/130556604/52017c6e-2c91-4b8e-84e4-8a13297c2615)


![image](https://github.com/rish1602/WatchDog/assets/130556604/a4429ecf-6a10-4260-ae3e-f7f6fe09a321)
