#define _GNU_SOURCE
#include <sys/syscall.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#define task_max 100
#define INT_MAX 1 << 30
typedef struct Task{
    char name[36];
    pid_t pid;
    int start_time;
    int exec_time;
}Task;
char type[10]; /// global
double start_time[task_max];////////////
Task task[task_max];
int task_num;
int record[task_max] = {};
// void find_pro(){
//     int remain[task_max];
//     int record[task_max];
//     int choose = 0;
//     remain[0] = task[0].exec_time;
//     record[0] = 1;
//     for(int i = 1; i < task_num; i++){
//         remain[choose] -= task[i].start_time-task[i-1].start_time;
//         if(remain[choose] == 0){
//             // find other
//         }
//         remain[i] = task[i].exec_time;
//         int next = find_min(i);
//         if(next != choose){
//             if(remain[choose] > 0){
//                 record[next] = record[choose] + 1;
//             }
//         }
//     }
// }
int find_min(int remain[], int c){
    int min2 = INT_MAX, index = -1;
    for(int i = 0; i < c; i++){
        if(remain[i] > 0 && remain[i] < min2){
            min2 = remain[i], index = i;
            // printf("min2 = %d\n", min2);
        }
    }
    return index;
}
int min(int a, int b){
    if(a < b) return a;
    return b;
}
void find_pro(){
    int remain[task_max] = {};
    int choose = -1;
    int r_count = 0;
    int t = 0;
    int re_count = task_num+1;
    while(1){
        if(r_count < task_num){
            while(task[r_count].start_time <= t && r_count < task_num){
                remain[r_count] = task[r_count].exec_time;
                r_count++;
            }
        }
        if(r_count == 0) {
		t+=task[0].start_time;
		continue;

	}
        int next = find_min(remain, r_count);
        // printf("next = %d\n", next);
        if(next < 0){
            if(r_count >= task_num) break;
            choose = -1;
            continue;
        }
        // if(next != choose && choose >= 0){
        //     if(remain[choose] <= 0){
        //         record[next] = record[choose];
        //         record[choose] += 1;
        //     }else{
        //         record[next] = record[choose] + 1;
        //     }
        // }
        // printf("t = %d\n", t);
        // for(int i = 0; i < task_num; i++){
        //     printf("remain[%d] = %d record = %d\n", i, remain[i], record[i]);
        // }
        // printf("next = %d, choose = %d\n", next, choose);
        choose = next;
        int t1 = remain[choose];
        int t2 = (r_count < task_num && strcmp(type,"PSJF")==0 )? task[r_count].start_time - t:INT_MAX;
        int step = min(t1, t2);
        if((remain[choose] - step) <= 0){
            remain[choose] = -1;
            record[choose] = re_count--;
        }else{
            remain[choose] -= step;
        }
        t += step;
    }
    
    return;
}
int main(){
    // int t = -1;
    // t = sched_getscheduler(0);
    // switch(t){
    //     case SCHED_FIFO: printf("FIFO\n"); break;
    //     case SCHED_OTHER: printf("O\n"); break;
    //     case SCHED_RR: printf("R\n"); break;
    //     default: printf("no\n");
    // } 
    // printf("t = %d\n", t);
    scanf("%s", type);
    struct sched_param param;
    param.sched_priority = 99;
    int policy = SCHED_FIFO;
    // if(strcmp(type, "FIFO") == 0) policy = SCHED_RR;
    // else if(strcmp(type, "RR") == 0) policy = SCHED_RR;
    // else if(strcmp(type, "SJF") == 0) policy = SCHED_FIFO;
    // else if(strcmp(type, "PSJF") == 0) policy = SCHED_OTHER;
    // else {
    //     printf("Invalid Algo.\n");
    //     exit(1);
    // }
    if(sched_setscheduler(0, policy, &param) == -1){
        printf("error\n");
        exit(1);
    }
    param.sched_priority = 99;
    if(sched_setparam(0, &param) == -1){
        printf("error2\n");
        exit(1);
    }
    // printf("success!\n");
    scanf("%d", &task_num);
    for(int i = 0; i < task_num; i++){
        scanf("%s%d%d", task[i].name, &task[i].start_time, &task[i].exec_time);
    }
    // sort the list and find priority
    if(strcmp(type, "FIFO") == 0 || strcmp(type, "RR") == 0){
        for(int i = 0; i < task_num-1; i++){				////////////////bubble sort???
            for(int j = i+1; j < task_num; j++){
                if(task[j].start_time < task[i].start_time){
                    Task temtask = task[j];
                    task[j] = task[i];
                    task[i] = temtask;
                }
            }
        }
    }else if(strcmp(type, "SJF") == 0 || strcmp(type, "PSJF") == 0){
        for(int i = 0; i < task_num-1; i++){
            for(int j = i+1 ; j < task_num; j++){
                if(task[j].start_time < task[i].start_time ||(
                    task[j].start_time == task[i].start_time && task[j].exec_time < task[i].exec_time)){
                    Task temtask = task[j];
                    task[j] = task[i];
                    task[i] = temtask;
                }
            }
        }
    }
    if(strcmp(type, "PSJF") == 0 || strcmp(type,"SJF")==0 ){
       // printf("start\n");
        find_pro();
       // printf("end\n");
    }
    // set cpu
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(getpid(), sizeof(set), &set);
    int count = 98;
    int tunit = 0;
    // start
    for(int k = 0; k < task_num; k++){
        if(k == 0){
            for(int j = 0; j < task[k].start_time; j++){
                volatile unsigned i; for(i = 0; i < 1000000UL; i++);
            }
        }else{
            for(int j = 0; j < task[k].start_time-task[k-1].start_time; j++){
                volatile unsigned i; for(i = 0; i < 1000000UL; i++);
            }
        }
	struct timeval st;//////////
        gettimeofday(&st, NULL);
	start_time[k]=st.tv_sec+st.tv_usec*0.000001;
        task[k].pid = fork();
        if(task[k].pid == 0){
            // child part
       printf("%s pid=%d\n",task[k].name,getpid());     
            struct timeval et;///////////
            CPU_ZERO(&set);
            CPU_SET(1, &set);
            sched_setaffinity(task[k].pid, sizeof(set), &set);
            for(int j = 0; j < task[k].exec_time; j++){
                volatile unsigned i; 
                for(i = 0; i < 1000000UL; i++){
                }
                // RR timeout (bugs: cant iterate more than exec_time/500 times)
                if(strcmp(type, "RR") == 0){
                    if(j > 0 && j%500 == 0){
                        struct sched_param p;
                        sched_getparam(0, &p);
                        p.sched_priority -= 1;
                        sched_setparam(0, &p);
                    }
                }
            }
            gettimeofday(&et, NULL);
	//printf("pid=%d start=%f end=%f\n", getpid(),start_time[k], et.tv_sec+et.tv_usec*0.000001);
	int start_time_int=start_time[k];
	int start_time_fraction=1000000000*(start_time[k]-start_time_int);
	int end_time_int=et.tv_sec+et.tv_usec*0.000001;
	int end_time_fraction=1000000000*((et.tv_sec+et.tv_usec*0.000001)-end_time_int);
	

	syscall(351, getpid(),start_time_int,start_time_fraction, end_time_int,end_time_fraction);
//	printf("pid %d %d.%d %d.%d\n", getpid(),start_time_int,start_time_fraction, end_time_int,end_time_fraction);
            exit(1);
        }else{
            // parent part
            struct sched_param temparam;
            //set the priority
            CPU_ZERO(&set);
            CPU_SET(1, &set);
            sched_setaffinity(task[k].pid, sizeof(set), &set);
            if(strcmp(type, "RR") == 0){
                temparam.sched_priority = count;
            }else if(strcmp(type, "FIFO") == 0){
                temparam.sched_priority = count--;
            }else{
                temparam.sched_priority = record[k]+10;
            }
            if(sched_setparam(task[k].pid, &temparam) < 0){
                printf("set priority error\n");
            }
        }
    }
    while(wait(NULL)>0);
    //struct timeval t;
    //gettimeofday(&t, NULL);
    //printf("parent end at time = %f, cpu = %d\n", t.tv_sec+t.tv_usec*0.000001, sched_getcpu());
    exit(0);
}
