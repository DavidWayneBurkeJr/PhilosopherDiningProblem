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


parent_process(void){
    printf("Parent process\n");
    int i;
    int j;
    msg m;
    for (i = 1; i < MAX; i++){
        for (j = 0; j < PHIL_NO; j++){
            read(node[j][0], &m, sizeof(msg));
            if (m.state == HUNGRY){
                printf("Philosopher %d is HUNGRY\n", j);

            }else if (m.state == THINKING){
                printf("Philosopher %d is THINKING\n", j);
            }
        }
    }
    for (j = 0; j < PHIL_NO; j++){
        m.state = THINKING;
        m.index = j;
        m.finish = TERMINATE;
        write(node[j][1], &m, sizeof(msg));
        close(node[j][1]);
    }
    return 0;
}

child_process(int i){
    msg m;
    srand((unsigned) (time(NULL) ^ (getpid()<<16)));
    int r = rand() % 15;
    printf("%d\n", r);
    m.finish = CONTINUE;
    m.index = i;
    m.state = THINKING;
    write(node[m.index][1], &m, sizeof(msg));
    do{
        read(node[m.index][0], &m, sizeof(msg));
        close(node[m.index][0]);
        printf("Philosopher %d is %d\n", i, m.state);
        sleep(r);
        m.state = HUNGRY;
        printf("Philosopher %d is %d\n", i, m.state);
        write(node[m.index][1], &m, sizeof(msg));
        close(node[m.index][1]);
        while (m.state != EATING){
            read(node[m.index][0], &m, sizeof(msg));
        }
        close(node[m.index][0]);
        m.state = EATING;

        printf("Philosopher %d is %d\n", i, m.state);
        r = rand() % 15;
        sleep(r);
        m.state = THINKING;
        write(node[m.index][1], &m, sizeof(msg));
        close(node[m.index][1]);
    }while (m.finish != TERMINATE);
    return 0;
}

main(void) {
    int i, pid[PHIL_NO];
    printf("Beginning\n");
    // init(); /* initialization */
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
            printf("Made %d\n", i);
            child_process(i);
            exit(0);
        };
    }
    parent_process();
    return 0;
}