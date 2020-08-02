#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/stat.h>

#define MAXSIZE 256
#define FILENAME "sample4_in.txt"

int resources, customers, safe;
int *available, **maximum, **allocation, **need;

void *runThread(void);
void **fileRead(char *filename);


int main (int argc, char *argv[]) {

    if (argc < 2)
    {
        printf("4 parameters needed!\n");
        return -1;
    }
    // Intialize variables
    resources = argc - 1;
    safe = 0;
    char *userInput = malloc(sizeof(char) * MAXSIZE);
    // Intialize arrays
    available = malloc(sizeof(int) * resources);
    for (int i = 1; i < argc; i++)
    {
        available[i - 1] = atoi(argv[i]);
    }

    maximum = fileRead(FILENAME);

    allocation = malloc(sizeof(int *) * customers);
    need = malloc(sizeof(int *) * customers);
    for (int i = 0; i < customers; i++)
    {
        allocation[i] = malloc(sizeof(int) * resources);
        need[i] = malloc(sizeof(int) * resources);
    }

}

void *runThread(void *thread){
    int *tid = (int *)thread;
    printf("--> Customer/Thread %d\n", *tid);

    printf("Allocated resources:  ");

    for(int i = 0; i < resources; i++){
        printf("%d ", allocation[*tid][i]);
    }
    printf("\n");

    printf("Needed: ");

    for(int i = 0; i < resources; i++){
        printf("%d ", need[*tid][i]);
    }
    printf("\n");

    printf("Available:  ");

    for(int i = 0; i < resources; i++){
        printf("%d ", available[*tid][i]);
    }
    printf("\n");

    printf("Thread has started\n");
    sleep(1);
    printf("Thread has finished\n");
    sleep(1);
    printf("Thread is releasing resources\n");
    sleep(1);
    printf("New Available:  ");

    for (int i = 0; i < resources; i++){
        available[i] = available[i] + allocation[*tid][i];
        printf("%d ", available[i]);
    }

    printf("\n");
    pthread_exit(NULL);

}

int **fileRead(char *filename) {

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Error opening file, error code -1...Exiting.\n");
        return NULL;
    }

    struct stat st;
    fstat(fileno(fp), &st);
    char* fileContent = (char*)malloc(((int)st.st_size + 1) * sizeof(char));
    fileContent[0] = '\0';

    while (!feof(fp))
    {
        char line[100];
        if (fgets(line, 100, fp) != NULL)
        {
            strncat(fileContent, line, strlen(line));
        }
    }
    fclose(fp);

    char *fileDup = (char *)malloc((strlen(fileContent) + 1) * sizeof(char));
    char *cmd = NULL;
    strcpy(fileDup, fileContent);
    cmd = strtok(fileDup, "\r\n");

    while (cmd != NULL) // count customers
    {
        customers++;
        cmd = strtok(NULL, "\r\n");
    }

    strcpy(fileDup, fileContent);
    char *lines[customers];
    int i = 0;
    cmd = strtok(fileDup, "\r\n");

    while (cmd != NULL)
    {
        lines[i] = malloc(sizeof(cmd) * sizeof(char));
        strcpy(lines[i], cmd);
        i++;
        cmd = strtok(NULL, "\r\n");
    }

    int **new_max = malloc(sizeof(int *) * customers);

    for (int j = 0; j < customers; j++)
    {
        int *temp = malloc(sizeof(int) * resources);
        char *token = NULL;
        int x = 0;
        token = strtok(lines[j], ",");

        while (token != NULL)
        {
            temp[x] = atoi(token);
            x++;
            token = strtok(NULL, ",");
        }

        new_max[j] = temp;
    }
    return new_max;
}
int *checkSafeSeq() {
	int *sq = malloc(sizeof(int) * customers);

	int *wk = malloc(sizeof(int) * resources);

	int *fnsh = malloc(sizeof(int) * customers);

	int x;
	int y;

	for (x = 0; x < resources; x++) {
		wk[x] = available[x];
	}

	for (x = 0; x < customers; x++) {
		fnsh[x] = 0;
	}

	int cnt = 0;
	while (cnt < customers) {
		int fg = 0;
		for (x = 0; x < customers; x++) {
			if (fnsh[x] == 0) {
				int tmp = 1;
				y = 0;
				while (y < resources && tmp == 1) {
					if (need[x][y] > wk[y]) {
						tmp = 0;
					}
					y++;
				}

				if (tmp == 1) {
					sq[cnt] = x;
					fnsh[x] = 1;
					fg = 1;
					cnt++;
					for (int y = 0; y < resources; y++) {
						wk[y] = wk[y] + allocation[x][y];
					}
				}
			}
		}
		int q = 0;
		if (fg == 0) {
			for (q < customers; q++) {
				sq[q] = -1;
			}
			free(wk);
			free(fnsh);
			return sq;
		}
	}

	free(wk);
	free(fnsh);
	return sq;
}
