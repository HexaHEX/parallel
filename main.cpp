#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <time.h>


#define sub  0.0000001
#define f(x) sin(x)
#define LEFT_B 0.000001
#define RIGHT_B 90

// 1sec - 23.9
//2 sec 13.4 +
//3 sec 9.05 +
//4 sec 6.6 + 
//5 sec 5.3 +
//6 sec 4.38 +
//7 sec  3.9 +
//8 sec
//9 sec
//12 2.19 sec +

// 1sec - 24
//2 sec  12 +
//3 sec 8.3 +
//4 sec 6 +
//5 sec 4,8+
//6 sec 4.1+
//7 sec 3.639 +-
//8 sec 3.1 +
//9 sec 2,8 (2,9 max)+
//12 
//10 2,64 _+-+
//11 2,27-2,41 2,41 +-
//12 2,1 +

double trapez_integration(double left, double right,double subInterval){

 double integration=0, stepSize, k;
 stepSize = (right - left)/subInterval;
 
  for(int i=0; i< stepSize; i++){
    k = left + i*subInterval;
    integration +=  subInterval*(f(k));
 }
  //integration = integration * stepSize/2; 
  return integration;


}
double monteCarlo(long int count,double step){
  int rezult = 0;
  double x,y;
  

  //time_t timer;
  //time(&timer);


  for(long int i = 0; i < count/step; i++){
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      if (x * x + y * y <= 1)
          rezult++;
  }
  //double seconds = difftime(time(NULL), timer );
// printf("%d - %d, computing time = %f sec.\n", i, rezult, seconds );
  //return ( (double) rezult) / count;
}


typedef struct thread_args{
  pthread_t pthread;
  double left_b;
  double right_b;
  double result;
} thread_args_str;

void* thread_calc(void* args){
  double left_b = ((thread_args_str*)args)->left_b;
  double right_b = ((thread_args_str*)args)->right_b;
  printf("thread %lu\n", ((thread_args_str*)args)->pthread);
  ((thread_args_str*)args)->result = trapez_integration( left_b, right_b,sub );
}

void* load(void* args){
  int loader = 0.000001;
  while(1)
    loader*=  90;
}

int main(int argc, char* argv[]){

  if(argc != 2){
     printf("Wrong Input!");
     return 0;
  }
	
  //double answers =// trapez_integration(LEFT_B,RIGHT_B,sub);
  //printf("%f\n",answers);
  
  double answer = 0;
  int thread_num = strtol(argv[1], NULL, 10);
  int cpu_number = get_nprocs();
  
  cpu_set_t cpu_set;
  thread_args_str* thread_args_arr = (thread_args_str*)calloc(thread_num, sizeof(thread_args_str));

  for(int i = thread_num; i < cpu_number; ++i){
    CPU_ZERO(&cpu_set);
    CPU_SET(i, &cpu_set);

    thread_args_arr[i].pthread = pthread_self();
    pthread_setaffinity_np(thread_args_arr[i].pthread, sizeof(cpu_set_t), &cpu_set);
    pthread_create(&(thread_args_arr[i].pthread), NULL, load, NULL);
  }

  //srand( time(NULL) );

  for(int i = 0; i < thread_num; ++i){
    CPU_ZERO(&cpu_set);
    CPU_SET(i%cpu_number ,&cpu_set);

    thread_args_arr[i].result = 0;
    thread_args_arr[i].left_b = LEFT_B+(RIGHT_B-LEFT_B)/thread_num*i;
    thread_args_arr[i].right_b = LEFT_B+(RIGHT_B-LEFT_B)/thread_num*(i+1);
    thread_args_arr[i].pthread = pthread_self();

    pthread_setaffinity_np(thread_args_arr[i].pthread, sizeof(cpu_set_t), &cpu_set);
    pthread_create(&(thread_args_arr[i].pthread), NULL, thread_calc, (void*)&thread_args_arr[i]);
  }
  for (int i = 0; i < thread_num; ++i)
    pthread_join(thread_args_arr[i].pthread, NULL);

  for (int i = 0; i < thread_num; ++i){
    answer += thread_args_arr[i].result;
//    printf("%d - %f\n", i, thread_args_arr[i].result );
  }
   //printf("%f\n", 4 * answer / thread_num); 
   printf("%f\n", answer );
    
}
