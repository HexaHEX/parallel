#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <time.h>

#define LEFT_END   0
#define RIGHT_END  10000000

double monteCarlo(long int count){
  int rezult = 0;
  double x,y;
  long int i = 0;

  time_t timer;
  time(&timer);

  for(; i < count; i++){
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      if (x * x + y * y <= 1)
          rezult++;
  }
  double seconds = difftime(time(NULL), timer );
//  printf("%d - %d, time = %f sec.\n", i, rezult, seconds );
  return ( (double) rezult) / count;
}

typedef struct thread_args{
  pthread_t pthread;
  double left_end;
  double right_end;
  double result;
} thread_args_str;

void* thread_calc(void* args){
  double left_end = ((thread_args_str*)args)->left_end;
  double right_end = ((thread_args_str*)args)->right_end;
//  printf("thread %lu: %f - %f\n", ((thread_args_str*)args)->pthread, left_end, right_end );
  ((thread_args_str*)args)->result = monteCarlo( right_end - left_end );
}

void* load(void* args){
  int loader = 0;
  while(1)
  loader*= 3;
}

int main(int argc, char* argv[]){

  if(argc != 2){
     printf("Wrong Input!");
     return 0;
  }
  int thread_number = strtol(argv[1], NULL, 10);
  int cpu_number = get_nprocs();
  double answer = 0;

  cpu_set_t cpu_set;
  thread_args_str* thread_args_array = (thread_args_str*)calloc(thread_number, sizeof(thread_args_str));


  for(int i = thread_number; i < cpu_number; ++i){
    CPU_ZERO(&cpu_set);
    CPU_SET(i, &cpu_set);

    thread_args_array[i].pthread = pthread_self();
    pthread_setaffinity_np(thread_args_array[i].pthread, sizeof(cpu_set_t), &cpu_set);
    pthread_create(&(thread_args_array[i].pthread), NULL, load, NULL);
  }

  srand( time(NULL) );
  for(int i = 0; i < thread_number; ++i)
  {
    thread_args_array[i].left_end = LEFT_END+(RIGHT_END-LEFT_END)/thread_number*i;
    thread_args_array[i].right_end = LEFT_END+(RIGHT_END-LEFT_END)/thread_number*(i+1);
    thread_args_array[i].result = 0;
    thread_args_array[i].pthread = pthread_self();

    CPU_ZERO(&cpu_set);
    CPU_SET(i%cpu_number ,&cpu_set);

    pthread_setaffinity_np(thread_args_array[i].pthread, sizeof(cpu_set_t), &cpu_set);
    pthread_create(&(thread_args_array[i].pthread), NULL, thread_calc, (void*)&thread_args_array[i]);
  }
  for (int i = 0; i < thread_number; ++i)
    pthread_join(thread_args_array[i].pthread, NULL);

  for (int i = 0; i < thread_number; ++i){
    answer += thread_args_array[i].result;
//    printf("%d - %f\n", i, thread_args_array[i].result );
  }
    printf("%f\n", 4 * answer / thread_number);
}
