#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>

#define BUFFER_LENGTH 9000
static char user_buffer[BUFFER_LENGTH];

int main(){
	int ret_val, fd;

	fd = open("/dev/process_list", O_RDONLY);             // Open the device with read only access

	if (fd < 0){
		perror("Failed to open the device process_list!");
		return errno;
	}

	ret_val = read(fd, user_buffer, BUFFER_LENGTH);        // Read the response

	if (ret_val < 0){
		perror("Failed to read the message from the process_list");
		return errno;
	}

	printf("\n%s\n", user_buffer);			// display the buffer contents

	close(fd);

	printf("End of the program\n");
	return 0;
}
