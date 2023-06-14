#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <unistd.h> /* for sleep() */
#include <stdbool.h>



// typedefs
typedef struct ST_PCB {
    int arrival_time;
    char name[9];
    int duration;
    struct ST_PCB * next;
} T_PCB;


// forward declarations
T_PCB * read_tasks(void);
void show_tasks_lin(T_PCB * head);
void show_tasks_circ(T_PCB * head);
T_PCB * sort_tasks_on_arrival(T_PCB * head);


// main function
int main(void) {
    int time_quantum;

    // serial list of newly arrived tasks
    // used to enforce last-come, first-served
    T_PCB * new_queue = NULL;
    // circular list of active tasks
    // used to implement Round-Robin
    T_PCB * ready_queue = NULL;
    // extra state needed to switch tasks
    // from new_queue to ready_queue when they're started
    T_PCB * ready_queue_prev = NULL;

    FILE *fh_outputfile;

    // show an emtpy list
    show_tasks_lin(new_queue);

    // read new tasks into the new_queue
    new_queue = read_tasks();

    // show the tasks
    show_tasks_lin(new_queue);

    // sort the tasks on arrival time
    new_queue = sort_tasks_on_arrival(new_queue);

    // show the tasks
    show_tasks_lin(new_queue);

    // open the OUTPUT file and validate the return value
    // TODO

    // the new_queue now contains tasks in their arrival order.
    // This is a linear linked list used to implement the last-come, first-served logic.
    // When a task from the new_queue is scheduled, it is afterwards moved to the ready_queue.
    // This ready_queue is a circular linked list, used to implement the fair round-robin logic.

/* MODIFY BELOW HERE --------------------------------------------------------*/
    if(strlen(new_queue->name)==3){
        printf("Error: %s",new_queue->name);
        const char *errormsg;
        if(strcmp(new_queue->name,"FOE") == 0){
            errormsg = "Could not open file!";
        }else if(strcmp(new_queue->name,"0TS") == 0){
            errormsg = "Cannot schedule 0 tasks!";
        }else if(strcmp(new_queue->name,"NAT") == 0){
            errormsg = "Cannot have a negative arrival time!";
        }else if(strcmp(new_queue->name,"INL") == 0){
            errormsg = "Invalid task name length!";
        }else if(strcmp(new_queue->name,"NDU") == 0){
            errormsg = "Cannot have a duration < 1!";
        }
        fh_outputfile = fopen("output.txt","w");
        fputs("error\n",fh_outputfile);
        fputs(errormsg,fh_outputfile);
        fclose(fh_outputfile);
        free(new_queue);
        exit(0);
    }

    fh_outputfile = fopen("output.txt","w");

    time_quantum = 0;
    T_PCB * node;
    T_PCB * tmp;

    int len = 0;
    node = new_queue;
    while(node != NULL){
        len++;
        node = node->next;
    }
    T_PCB * toEncounter[len];
    node = new_queue;
    for (int i = 0; i < len; i++) {
        toEncounter[i] = node;
        node = node->next;
    }

    while (time_quantum<50){
        if(time_quantum<10){
            printf("0%d - ",time_quantum);
            fprintf_s(fh_outputfile,"0%d - ",time_quantum);
        }
        else{
            printf("%d - ",time_quantum);
            fprintf_s(fh_outputfile,"%d - ",time_quantum);
        }

        //check for new arrivals
        bool scheduled = true;
        while (scheduled==true && new_queue != NULL){
            scheduled = false;
            if(new_queue->arrival_time == time_quantum){
                scheduled = true;
                tmp = new_queue->next;

                if(ready_queue == NULL){ ready_queue = new_queue;}
                if(ready_queue_prev == NULL){ready_queue_prev = new_queue;}

                new_queue->next = ready_queue;
                ready_queue_prev->next = new_queue;
                ready_queue = new_queue;

                new_queue = tmp;
            }
        }


        //Run Scheduled Task and go to next
        if(ready_queue != NULL) {
            printf("%s", ready_queue->name);
            fprintf_s(fh_outputfile, "%s", ready_queue->name);

            //only way to ensure (new) for multiple arrivals on 1 time slice
            for (int i = 0; i < len; i++) {
                if(ready_queue == toEncounter[i]){
                    printf(" (new)");
                    fprintf_s(fh_outputfile," (new)");
                    toEncounter[i] = NULL;
                }
            }

            ready_queue->duration--;

            if(ready_queue->duration <= 0){ //task done, remove from list
                printf("   task is done ");
                if(ready_queue->next == ready_queue){//task was the only one, make list empty
                    free(ready_queue);
                    ready_queue = NULL;
                }
                else {//task was not the only one, relink
                    ready_queue_prev->next = ready_queue->next;
                    free(ready_queue);
                    ready_queue = ready_queue_prev->next;
                }
            }else {
                ready_queue_prev = ready_queue;
                ready_queue = ready_queue->next;
            }
        }else{
            printf("idle");
            fprintf_s(fh_outputfile,"idle");
        }
        printf("\n");
        fprintf_s(fh_outputfile,"\n");
        time_quantum++;
    }
    fclose(fh_outputfile);
/* MODIFY ABOVE HERE --------------------------------------------------------*/

    return 0;
}

/**
 * @brief      Swap 2 tasks in a linked list
 *
 * @param      task1,task2  the tasks to be swapped
 *
 * @return     returns the first element of the swapped elements
 */
T_PCB * swap(T_PCB * task1, T_PCB * task2) {
    T_PCB * tmp = task2->next;
    task2->next = task1;
    task1->next = tmp;
    return task2;
}


/**
 * @brief      Sort tasks in the new queue on arrival time
 *
 * @param      head  The head of the list to be sorted
 *
 * @return     The head of the sorted list
 */

T_PCB * sort_tasks_on_arrival(T_PCB * head) {
    //get length of list
    int len = 0;
    T_PCB *node = head;
    while(node != NULL){
        len++;
        node = node->next;
    }

    T_PCB ** orignalHead = &head;
    T_PCB ** sortHead;
    for (int i = 0; i < len; i++) {
        sortHead = orignalHead;
        for (int j = 0; j < len-1; j++) {
            if((*sortHead)->arrival_time>(*sortHead)->next->arrival_time){
                T_PCB * task = *sortHead;
                *sortHead = swap(task,task->next);
            }
            sortHead = &(*sortHead)->next;
        }
    }
    return *orignalHead;
}


/**
 * @brief      Shows the tasks in a non-circular list
 *
 * @param      head  The head of the non-circular list
 */
void show_tasks_lin(T_PCB * head) {
    T_PCB * ptr;
    int ID = 0;

    if(head == NULL ){
        printf("Linear list is empty\n");
    } else {
        ptr = head;
        printf("Printing linear list\n");
        printf("  ID |    TASK  |  duration | arrival time\n");
        printf("  ---+----------+-----------+-----------\n");

        while(ptr != NULL) {
            printf("  %2d | %s | %8d  | %5d\n", ID, ptr->name, ptr->duration, ptr->arrival_time);
            ptr = ptr->next;
            ID++;
        }
        printf("\n");
    }
}

/**
 * @brief      Shows the tasks in a circular list
 *
 * @param      head  An entry into the circular list
 */
void show_tasks_circ(T_PCB * head) {
    T_PCB * ptr;
    int ID = 0;

    if(head == NULL ){
        printf("Cirulcar list is empty\n");
    } else {

        ptr = head->next;
        while(ptr->next != head){
            ptr = ptr->next;
        }

        printf("Printing circular list\n");
        printf("  ID |    TASK  |  duration | arrival time\n");
        printf("  ---+----------+-----------+-----------\n");

        while(head != ptr) {
            printf("  %2d | %s | %8d  | %5d\n", ID, head->name, head->duration,\
       head->arrival_time);
            head = head->next;
            ID++;
        }
        printf("  %2d | %s | %8d  | %5d\n", ID, head->name, head->duration, head->arrival_time);
        printf("\n");
    }
}


/**
 * @brief      Reads tasks from file
 *
 * @return     Pointer to the linked list of PCB's
 */
T_PCB * read_tasks(void) {
    FILE *f;
    char buffer[10];
    char* ptr;
    unsigned int nrOfTasks;

    int arrival_time;
    char name[9];
    int duration;

    // open the INPUT "tasks.txt" file and validate the return value
    f = fopen("tasks.txt","r");
    if(f == NULL){
        T_PCB* error = malloc(sizeof(T_PCB));
        strcpy(error->name,"FEO");
        return error;
    }
    // the first line determines the number of jobs in the file
    fgets(buffer,100,f);
    nrOfTasks = strtol(buffer,&ptr,10);
    if(nrOfTasks == 0){
        T_PCB* error = malloc(sizeof(T_PCB));
        strcpy(error->name,"0TS");
        return error;
    }

    //Declare array of PCB
    T_PCB* PCBptrs[nrOfTasks];

    // read all the jobs and create a PCB for each
    for (int i = 0; i < nrOfTasks; i++) {
        fgets(buffer,100,f);
        arrival_time = strtol(buffer,&ptr,10);
        if(arrival_time < 0){
            T_PCB* error = malloc(sizeof(T_PCB));
            strcpy(error->name,"NAT");
            return error;
        }

        fgets(buffer,100,f);
        //strip the /n if present
        size_t s = strlen(buffer);
        if (s && (buffer[s-1] == '\n')) buffer[--s] = 0;

        if(s != 8){
            T_PCB* error = malloc(sizeof(T_PCB));
            strcpy(error->name,"INL");
            return error;
        }
        strcpy(name,buffer);

        fgets(buffer,100,f);
        duration = strtol(buffer,&ptr,10);
        if(duration <= 0){
            T_PCB* error = malloc(sizeof(T_PCB));
            strcpy(error->name,"NDU");
            return error;
        }
        //All inputs valid -> Malloc
        PCBptrs[i] = malloc(sizeof(T_PCB));
        PCBptrs[i]->arrival_time = arrival_time;
        strcpy(PCBptrs[i]->name,name);
        PCBptrs[i]->duration = duration;
        PCBptrs[i]->next = NULL;
    }
    fclose(f);
    // All PCB's end up in a -linear- linked list
    for (int i = 0; i < nrOfTasks-1; i++) {
        PCBptrs[i]->next = PCBptrs[i+1];
    }
    // return the head to the newly created linear linked list
    return PCBptrs[0];
}
