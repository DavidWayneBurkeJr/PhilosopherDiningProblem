#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PHIL_NO   5     /* the number of philosopher */
#define THINKING  1     /* think state */
#define HUNGRY    2     /* hungry state */
#define EATING    3     /* eat state */
#define CONTINUE  0     /* make the child process do its work */
#define TERMINATE 1     /* terminate the child process */
#define MAX       500   /* maximum number of iteration */


typedef struct message {
    int state;   /* current state */
    int index;   /* logical process id */
    int finish;  /* signal to continue or finish job */
} msg;

int host[2];            /* for host pipe */
int node[PHIL_NO][2];   /* for child node pipe */
int count[PHIL_NO];     /* save the number of eating state of each node */
int status[PHIL_NO];    /* array to save status of neighbors */

parent_process(void){
    printf("Parent process\n");
    int i;
    int j;
    int head = 0;       // increase index by one per iteration to be fair
    for (i = 0; i < MAX; i++){
        head = i;
        for (j = 0; j < PHIL_NO; j++){
            int currentPhil = (head + j) % 5; // get current node index
            printf("Current Phil is %d\n", currentPhil);
            msg m;
            close(node[currentPhil][1]);
            read(node[currentPhil][0], &m, sizeof(msg));    /* read from child node and save status in their */
            status[currentPhil] = m.state;                  /* index in the status array                     */
            if (m.state == HUNGRY){ //Check if state is hungry for current node
                if (status[(currentPhil - 1) % 5] != EATING && status[(currentPhil + 1) % 5] != EATING){
                    status[currentPhil] = EATING;  // check to the left and right of the node in status array
                    m.state = EATING;               //if  neither are eating, current node can eat
                    m.index = currentPhil;
                    m.finish = CONTINUE;
                    write(node[currentPhil][1], &m, sizeof(msg));
                }

            }
        }
        sleep(1);
        printf("Philosopher 1: %d, Philosopher 2: %d, Philosopher 3: %d, Philosopher 4: %d, Philosopher 5: %d\n", status[0],
                status[1], status[2], status[3], status[4]);
    }
    for (j = 0; j < PHIL_NO; j++){  // After done, set all to TERMINATE
        msg m;
        m.state = THINKING;
        m.index = j;
        m.finish = TERMINATE;
        write(node[j][1], &m, sizeof(msg));
        close(node[j][1]);
    }
    return 0;
}

child_process(int i){
    /* Initialization. Set msg to initial values and write it to parent */
    msg m;
    srand((unsigned) (time(NULL) ^ (getpid()<<16)));
    int r = rand() % 15;
    m.finish = CONTINUE;
    m.index = i;
    m.state = THINKING;
    do{
        write(node[m.index][1], &m, sizeof(msg));   /* Always write current status to parent in each iteration */
        sleep(r); // Thinking for random time
        close(node[m.index][0]);
        m.state = HUNGRY;   // Become hungry
        write(node[m.index][1], &m, sizeof(msg));   //Tell parent
        while (m.state != EATING){
            read(node[m.index][0], &m, sizeof(msg));    // Keep checking till parent tells child to eat
        }

        r = rand() % 15;
        sleep(r);   // Eat for a random amount of time
        m.state = THINKING; // Set state back to thinking
    }while (m.finish != TERMINATE);
    return 0;
}

main(void) {
    int i, pid[PHIL_NO];
    printf("Beginning\n");
    if (pipe(host) < 0) /* make pipe line for host */
    {
        perror("ERROR: Can't make pipe line for host node\n");
        exit(1);
    }else{
        printf("Host pipe made successfully\n");
    };
    for (i = 0; i < PHIL_NO; i++) /* make pipe lines for nodes */
    {
        if (pipe(node[i]) < 0) {
            perror("ERROR: Can't make pipe line for child node\n");
            exit(1);
        }else{
            printf("Child node made successfully\n");
        };
    }
    for (i = 0; i < PHIL_NO; i++) {
/* make 5 child nodes */
        if ((pid[i] = fork()) == 0) {
            child_process(i);
            exit(0);
        };
    }
    parent_process();
    return 0;
}