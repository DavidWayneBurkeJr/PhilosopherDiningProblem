#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

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


parent_process(){
    printf("Parent process");
    int i;
    int j;
    msg m;
    for (i = 1; i < MAX; i++){
        for (j = 0; j < PHIL_NO; j++){
            read(node[j][0], &m, sizeof(msg));
            if (m.state == HUNGRY){

            }else if (m.state == THINKING){

            }
        }
    }
    for (j = 0; j < PHIL_NO; j++){
        m.finish = TERMINATE;
        write(node[j][1], &m, sizeof(msg));
    }
}

child_process(int i){
    msg m;
    int r = rand() % 15;
    m.state = CONTINUE;
    do{
        read(host[0], &m, sizeof(msg));
        close(host[0]);
        printf("Philosopher %d is %d", i, m.state);
        sleep(r);
        m.index = i;
        m.state = HUNGRY;
        printf("Philosopher %d is %d", i, m.state);
        write(host[1], &m, sizeof(msg));
        close(host[0]);
        while (m.state != EATING){
            read(host[0], &m, sizeof(msg));
        }
        printf("Philosopher %d is %d", i, m.state);
        close(host[0]);
        r = rand() % 15;
        sleep(r);
        m.state = THINKING;
        write(host[0], &m, sizeof(msg));
    }while (m.finish != TERMINATE);
}

main() {
    int i, pid[PHIL_NO];
    printf("Beginning");
    // init(); /* initialization */
    if (pipe(host) < 0) /* make pipe line for host */
    {
        perror("ERROR: Can't make pipe line for host node\n");
        exit(1);
    };
    for (i = 0; i < PHIL_NO; i++) /* make pipe lines for nodes */
    {
        if (pipe(node[i]) < 0) {
            perror("ERROR: Can't make pipe line for child node\n");
            exit(1);
        };
    }
    for (i = 0; i < PHIL_NO; i++) {
/* make 5 child nodes */
        if ((pid[i] = fork()) == 0) {
            child_process(i);
            printf("Made %d", i);
            exit(0);
        };
    }
    parent_process();
}