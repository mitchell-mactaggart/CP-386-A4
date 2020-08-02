 /*
Name: Hassan Siddiqui
Email: sidd1850@mylaurier.ca
Student ID: 170691850
Github Username: Sidd1850

Name: Mitchell MacTaggart
Email: mact6000@mylaurier.ca
Student ID: 160886000
GitHub Username: mitchell-macTaggart
*/
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
int *available, **maximum, **allocation, **need, *sq;

void runProgram();
void *runThread(void *thread);
int **fileRead(char *filename);
int *checkSafeSeq();


int main (int argc, char *argv[]) {

    if (argc < 2)
    {
        printf("4 parameters needed!\n");
        return -1;
    }

    // Intialize variables
    resources = argc - 1;
    safe = 0;

    // Intialize arrays
    available = malloc(sizeof(int) * resources);
    for (int i = 1; i < argc; i++)
    {
        available[i - 1] = atoi(argv[i]);
    }

	//getting maximum value from readme file
    maximum = fileRead(FILENAME);
	//declaring allocation variables

    allocation = malloc(sizeof(int *) * customers);
    need = malloc(sizeof(int *) * customers);
    for (int i = 0; i < customers; i++)
    {
        allocation[i] = malloc(sizeof(int) * resources);
        need[i] = malloc(sizeof(int) * resources);
    }

	//starting output and printing each variable
    printf("Amount of Customers: %d\n", customers);
    printf("Available resources: ");
    for(int i = 0; i < resources; i++){
        printf("%d ", available[i]);
    }
    printf("\n");

    printf("Maximum resources:\n");
    for (int i = 0; i < customers; i++){
        for (int j = 0; j < resources; j++){
            printf("%d", maximum[i][j]);
            if (j < resources - 1)
                printf(" ");
        }
        printf("\n");
    }

	//calling the run program function to allow us to run the different aspects of the program
    runProgram();
    return 0;
}

void runProgram(){
	char *uip = malloc(sizeof(char) * MAXSIZE);
	// Loop until "exit" is inputted
	while (1) {
		printf("Enter Command('RQ','RL','*','RUN','exit'): ");
		fgets(uip, MAXSIZE, stdin);
		if (strlen(uip) > 0 && uip[strlen(uip) - 1] == '\n') {
			uip[strlen(uip) - 1] = '\0';
		}
		if (strstr(uip, "RUN")) // This is to check if a User enters RUN
				{
			//Calls the safety sequence program
			//This part is to allow the program to run
			sq = checkSafeSeq();
			if (safe == 1) {
				for (int x = 0; x < customers; x++) {
					int thread = sq[x];
					pthread_t threadId;
					pthread_attr_t attr;
					pthread_attr_init(&attr);
					pthread_create(&threadId, &attr, runThread,
							(void *) &thread);
					pthread_join(threadId, NULL);
				}
			} else {
				printf("Warning: In a unsafe state, fix before running.\n");
			}
		} else if (strstr(uip, "*")) // This is here to check if the user inputs *
				{
			//This is to allow the program to print all variables like

			//Available resources are printed
			printf("Available Resources:\n");
			for (int x = 0; x < resources; x++) {
				printf("%d ", available[x]);
			}
			printf("\n");

			//Maximum resources are printed
			printf("Maxmium Resources:\n");
			for (int x = 0; x < customers; x++) {
				for (int y = 0; y < resources; y++) {
					printf("%d", maximum[x][y]);
					if (y < resources - 1)
						printf(" ");
				}
				printf("\n");
			}

			//Allocated resources are printed
			printf("Allocated Resources:\n");
			for (int x = 0; x < customers; x++) {
				for (int y = 0; y < resources; y++) {
					printf("%d", allocation[x][y]);
					if (y < resources - 1)
						printf(" ");
				}
				printf("\n");
			}

			//needed resources are printed
			printf("Needed Resources:\n");
			for (int x = 0; x < customers; x++) {
				for (int y = 0; y < resources; y++) {
					printf("%d", need[x][y]);
					if (y < resources - 1)
						printf(" ");
				}
				printf("\n");
			}
		} else if (strstr(uip, "RQ")) // This is to check if the user enters RQ
				{
			//This bit allows the program to allocate resources
			int *sizeArray = malloc(sizeof(int) * (resources + 1));
			char *cusID = NULL;
			cusID = strtok(uip, " ");
			int ct = 0;
			while (cusID != NULL) {
				if (ct > 0) {
					sizeArray[ct - 1] = atoi(cusID);
				}
				cusID = strtok(NULL, " ");
				ct++;
			}
			//Variable initialization and forloops for processings
			int customerAmount = sizeArray[0];
			if (customerAmount < customers && ct == resources + 2) {
				for (int x = 0; x < resources; x++) {
					allocation[customerAmount][x] = sizeArray[x + 1];
					need[customerAmount][x] = maximum[customerAmount][x] - allocation[customerAmount][x];
					if (need[customerAmount][x] < 0) {
						need[customerAmount][x] = 0;
					}
				}
				//This is a fail safe and checks for any irregularities in the users inputs
			} else {
				if (customerAmount >= customers) {
					printf("Thread out of bounds, please try again.\n");
				} else {
					printf("Incorrect amount of parameter, please try again.\n");
				}
			}
			free(sizeArray);
			//calls the safety program again to see if it returns safe or unsafe
			sq = checkSafeSeq();
			printf("Request satisfied.\n");
			if (sq[0] == -1) {
				safe = 0;
				printf("Warning: In a unsafe state, fix before running.\n");
			} else {
				safe = 1;
				printf("In a Safe State.\n");
			}
		} else if (strstr(uip, "RL")) // checks to see if user entered RL
				{
			//The purpose of this bit is to deallotcate resources
			int *sizeArray = malloc(sizeof(int) * (resources + 1));
			char *cusID = NULL;
			cusID = strtok(uip, " ");
			int ct = 0;

			while (cusID != NULL) {
				if (ct > 0) {
					sizeArray[ct - 1] = atoi(cusID);
				}
				cusID = strtok(NULL, " ");
				ct++;
			}

			int customerAmount = sizeArray[0];
			// Resources are getting deallocated here
			if (customerAmount < customers && ct == resources + 2) {
				for (int x = 0; x < resources; x++) {
					if (sizeArray[x + 1] <= allocation[customerAmount][x]) {
						allocation[customerAmount][x] -= sizeArray[x + 1];
						need[customerAmount][x] = maximum[customerAmount][x]
								- allocation[customerAmount][x];
					} else {
						printf("Too many resources than allocated.\n");
						break;
					}
				}
			} else {
				if (customerAmount >= customers) {
					printf("Thread out of bounds, please try again.\n");
				} else {
					printf("Incorrect amount of parameter, please try again.\n");
				}
			}
			free(sizeArray);
			sq = checkSafeSeq();
			printf("Request satisfied.\n");
			if (sq[0] == -1) {
				safe = 0;
				printf("Warning: In a unsafe state, fix before running.\n");
			} else {
				safe = 1;
				printf("In a Safe State.\n");
			}
		} else if (strstr(uip, "exit")) // Checks to see if the user enters EXIT
				//This will exit the code
				{
			free(maximum);
			free(allocation);
			free(need);
			free(available);
			free(sq);
			break;
		} else // user enters invaild input
		{
			printf("\"%s\" Invalid input, valid inputs['RQ','RL','*','Run','exit'].\n", uip);
		}
	}
}

//This is the main run function where the bankers algorithm can proceed with requests and processing
void *runThread(void *thread){

    int *threadId = (int *)thread;
    printf("--> Customer/Thread %d\n", *threadId);

    printf("Allocated resources:  ");

    for(int i = 0; i < resources; i++){
        printf("%d ", allocation[*threadId][i]);
    }
    printf("\n");

    printf("Needed: ");

    for(int i = 0; i < resources; i++){
        printf("%d ", need[*threadId][i]);
    }
    printf("\n");

    printf("Available:  ");

    for(int i = 0; i < resources; i++){
        printf("%d ", available[i]);
    }
    printf("\n");
	//Shows the updates on the algorithm

    printf("Thread has started\n");
    sleep(1);
    printf("Thread has finished\n");
    sleep(1);
    printf("Thread is releasing resources\n");
    sleep(1);
    printf("New Available:  ");

    for (int i = 0; i < resources; i++){
        available[i] = available[i] + allocation[*threadId][i];
        printf("%d ", available[i]);
    }

    printf("\n");
    pthread_exit(NULL);

}
//This function opens the sample document that has all the customer data in, it then
//reads said file and processes max value for further processing in the program

int **fileRead(char *filename) {

    FILE *fp = fopen(filename, "r");
	//Checks to see if the file was opened or not and if not then an error message is printed
    if (!fp)
    {
        printf("Error opening file, error code -1...Exiting.\n");
        return NULL;
    }

    struct stat st;
    fstat(fileno(fp), &st);
    char* fileContent = (char*)malloc(((int)st.st_size + 1) * sizeof(char));
    fileContent[0] = '\0';
//Checking file content
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
    //initalize varibles
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
		if (fg == 0) {
			for (int q = 0; q < customers; q++) {
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
