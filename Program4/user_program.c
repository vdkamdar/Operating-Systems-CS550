#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#define SIZE 500
struct Buffer
{
	unsigned long address;
	long time;
};
int main()
{
	struct Buffer buf[SIZE];
	int i=0;
	int errorFlag = 0;
	int fp = open("/proc/Probe",O_RDONLY);
	errorFlag = read(fp,buf,1);
		printf("Page Fault Address           Time\n");
	for(i=0;i<SIZE;i++)
	{
		//printf("Read worked the Page fault address is 0x%lx caught at time %ld\n",buf[i].address,buf[i].time);
		printf("0x%lx           %ld\n",buf[i].address,buf[i].time);
	}
	close(fp);
	return 0;
}
