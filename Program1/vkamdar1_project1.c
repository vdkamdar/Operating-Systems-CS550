#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (argc == 4)
	{
		int H = atoi(argv[1]), C = atoi(argv[2]), tab = atoi(argv[3]), i;
		char height[10], children[10], tab_str[10];
		pid_t pid[C];

		for(i=0; i<tab; i++){
			printf("\t");
		}
		printf("(%d): Process starting\n", getpid());
                for(i=0; i<tab; i++){
                        printf("\t");
                }
		printf("(%d): Parent's id = (%d)\n", getpid(), getppid());
                for(i=0; i<tab; i++){
                        printf("\t");
                }
		printf("(%d): Height in the tree = (%d)\n", getpid(), H);
                for(i=0; i<tab; i++){
                        printf("\t");
                }
		printf("(%d): Creating (%d) children at height (%d)\n", getpid(), C, (H-1));

		if (H > 1){
			for(i=0; i<C; i++){
				pid[i]=fork();

				if (pid[i] == 0){
					snprintf(height, 10, "%d", H-1);
					snprintf(children, 10, "%d", C);
					snprintf(tab_str, 10, "%d", tab+1);
					if (execlp("./vkamdar1_project1", "./vkamdar1_project1", height, children, tab_str, (char *)NULL) < 0){
						fprintf(stderr, "\nexec failed\n");
					}
					exit(0);
				}
				else if (pid[i] < 0){
                                        fprintf(stderr, "\nfork failed\n");
                                        exit (1);
                                }
			}
			for (i=0; i<C; i++){
				wait(NULL);
			}
			printf("\n%d: Terminating at height %d\n", getpid(), H);
			exit(0);
		}
	}
	else{
		printf("\nPlease execute using this format: vkamdar1_project1 (height) (children) 0 \n");
	}
}
