#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<sys/inotify.h> // The inotify API provides a mechanism for monitoring filesystem events. for more infor check this out https://www.man7.org/linux/man-pages/man7/inotify.7.html#BUGS
#include <libnotify/notify.h> //Header file toshow notifications on desktop. For more info check 2nd section with C language https://wiki.archlinux.org/title/Desktop_notifications


#define EXIT_SUCCESS_ 0
#define EXIT_ERR_TOO_FEW_ARGS_ 1
#define EXIT_ERR_INIT_INOTIFY_ 2 
#define EXIT_ERR_AND_WATCH_ 3
#define EXIT_ERR_BASE_PATH_NULL_ 4
#define EXIT_ERR_READ_INOTIFY_ 5
#define EXIT_ERR_INIT_LIBNOTIFY_ 6

int IeventQueue_ = -1; // global file descriptor for inotify events
int IeventStatus_ = -1; //return value of inotify_add_watch(int fd, const char *pathname, uint32_t mask) stored

char* ProgramTitle_ = "WatchDog";

//---------signal handling to kill the daemon -------------------------
void signal_handler(int signal)
{
	int closeStatus_ = -1;
	printf("signal received, cleaning up....\n");

	closeStatus_ = inotify_rm_watch(IeventQueue_, IeventStatus_); //to remove item from inotify watchlist 
	if (closeStatus_ == -1)
	{
		fprintf(stderr, "error removing from watch queue \n");
	}
	close(IeventQueue_); 
	exit(EXIT_SUCCESS_);


}

int main(int argc, char** argv)
{
	bool libnotifyInitStatus_ = false;

	char* base_path_ = NULL;
	char* token_ = NULL; 
	char* delimiter_ = "/"; 
	char* notificationMessage_ = NULL; 
	NotifyNotification *notifyHandle_;

	char buffer[4096]; 
	int readLength_;
	const struct inotify_event* watchEvent_; 

	const uint32_t watchMask_ = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF; // we are using inotify_add_watch(int fd, const char *pathname, uint32_t mask); so here for mask we define various flags and or them meaning mask can be flag1 or flag2 or flag3 or .....

	//-------------------below line of code checks if the argument count is less than 2 then print error message.--------------------
	if (argc < 2)
	{
		exit(EXIT_ERR_TOO_FEW_ARGS_); 
		fprintf(stderr, "USAGE: WatchDog PATH\n");
	}


	//-------------------below line of code assigns a heap memory to base_path_ and copy file path in the base_path_------------------
	base_path_ = (char*)malloc(sizeof(char)*(strlen(argv[1]) + 1)); //define memory with base_path pointing to that memory
	strcpy(base_path_, argv[1]); //now base_path has the address of the file



	//-------------------below line of code converts the base_path_ to tokens by adding the delimiters like "/" For more infor refer- https://www.geeksforgeeks.org/string-tokenization-in-c/ -----------------------
	token_ = strtok(base_path_,delimiter_);//since the file path can be too long so we use strtok() function that tokkentizes the long strings. 
	while (token_)
	{
		base_path_ = token_;
		token_ = strtok(NULL,delimiter_); 
	}

	if (base_path_ == NULL)
	{
		fprintf(stderr, "Error getting base path!! \n");
		exit(EXIT_ERR_BASE_PATH_NULL_); 
	}

	libnotifyInitStatus_ = notify_init(ProgramTitle_);  //set that notification has to be done for which program
	if (!libnotifyInitStatus_)
	{
		fprintf(stderr,"Error initializting libnotify!! \n");
		exit(EXIT_ERR_INIT_LIBNOTIFY_); 
	}
	//-------------------below we check the status of global file descriptor for inotify ----------------------------------------------
	IeventQueue_ = inotify_init();
	if (IeventQueue_ == -1)
	{
		fprintf(stderr, "Error initializing inotify instance!! \n");
		exit(EXIT_ERR_INIT_INOTIFY_); 
	}

	//------------------ return value of inotify_add_watch(int fd, const char *pathname, uint32_t mask) is stored in variable -----------------------------
	IeventStatus_ = inotify_add_watch(IeventQueue_, argv[1], watchMask_);
	if (IeventStatus_ == -1)
	{
		fprintf(stderr, "Error adding file to watch instance!!");
		exit(EXIT_ERR_AND_WATCH_); 
	}

	//-------after all inotify and everything is corrrect, now register the signal -------------------------------------
	signal(SIGABRT, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);


	//------------------ Daemon starts at background--------------------------------
	while (true)
	{
		printf("Waiting for ievent.....\n");
		readLength_ = read(IeventQueue_, buffer, sizeof(buffer)); 

		if (readLength_ == -1)
		{
			fprintf(stderr, "Error reading from inotify instance!!");
			exit(EXIT_ERR_READ_INOTIFY_); 
		}

		//loop over all events of buffer
		for (char* bufferPointer_ = buffer; bufferPointer_ < buffer + readLength_; bufferPointer_ += sizeof(struct inotify_event) + watchEvent_->len)
		{
			notificationMessage_ = NULL;
			watchEvent_ = (const struct inotify_event*)bufferPointer_;

			if (watchEvent_->mask & IN_CREATE)
			{
				notificationMessage_ = "File created \n";
			}
			if (watchEvent_->mask & IN_DELETE)
			{
				notificationMessage_ = "File deleted \n";
			}
			if (watchEvent_->mask & IN_ACCESS)
			{
				notificationMessage_ = "File created \n";
			}
			if (watchEvent_->mask & IN_CLOSE_WRITE)
			{
				notificationMessage_ = "File written and closed \n";
			}
			if (watchEvent_->mask & IN_MODIFY)
			{
				notificationMessage_ = "File modiied \n";
			}
			if (watchEvent_->mask & IN_MOVE_SELF)
			{
				notificationMessage_ = "File moved \n";
			}

			if (notificationMessage_ == NULL) { continue; }

			notifyHandle_ = notify_notification_new(base_path_, notificationMessage_, "dialog-information");
			if (notifyHandle_ == NULL)
			{
				fprintf(stderr, "notification handle was null!! \n");
				continue;
			}
			notify_notification_show(notifyHandle_,NULL); 
		}
	}
}
