# include <stdio.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>
# include <sys/time.h>
#include <stdlib.h>

#include "mpi.h"
#include <string.h>

#define N 10

extern double mysecond();

int cmp(const void *x, const void *y)
{
  double xx = *(double*)x, yy = *(double*)y;
  if (xx < yy) return -1;
  if (xx > yy) return  1;
  return 0;
}

int main(int argc, char *argv[])
{
	int message_size = 1000;
	if(argc>1) {  // erster Parameter ist message_size
		message_size = (int) atoi(argv[1]);
	}
	
	int myrank, i, j, world_size, processor_name_len, tag1=99, tag2=123;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	
	char *message;
	message = malloc(message_size*sizeof(char));

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);   // X-1, X von piX 
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);  // N
	MPI_Get_processor_name(processor_name, &processor_name_len);

	
	if (myrank == 0) {
		// aus stream benchmark //
		/*int quantum, checktick();
		if( (quantum = checktick()) >= 1) 
			printf("Your clock granularity/precision appears to be "
			"%d microseconds.\n", quantum);
		else {
			printf("Your clock granularity appears to be "
				"less than one microsecond.\n");
			quantum = 1;
		}*/
		//////////////////////////
		
		double t, dt, t_sum=0;
		//double times[N];
		
		char *message_recv;
		message_recv = malloc(message_size*sizeof(char));
		//char message_recv[message_size];
				
		printf("%d;", message_size);
		
		//printf("    send: %d Byte, strlen=%d\n", sizeof(message), strlen(message));
		for(i=0; i<N; i++) {  // N mal für besseren Wert
			for(j=0; j<message_size-1; j++) {
				message[j] = (char) ( (rand() % 200)  +50 );  // zufällige Nachricht erzeugen
			}
			t = mysecond();
			MPI_Send(message     , message_size, MPI_CHAR, 1, tag1, MPI_COMM_WORLD);		  // ping!
			MPI_Recv(message_recv, message_size, MPI_CHAR, 1, tag1, MPI_COMM_WORLD, &status); // pong?
			
			//times[i] = mysecond()-t;
			t_sum += mysecond()-t;
			printf("%f;", (mysecond()-t));
			//printf("received %d in %fs\n", i, (mysecond()-t));
		}
		//qsort(times, sizeof(times)/sizeof(times[0]), sizeof(times[0]), cmp);
		dt = t_sum/N; // Durchschnitt aus den 10 Durchläufen
		printf("%f\n", dt);
		/*for(i=0; i<N; i++) {
			printf("%f;", times[i]);
		}
		printf("mean: %f\n", times[((int) trunc(N/2))] );
		*/
	}
	else {
		//char message_recv[message_size];
		for(i=0; i<N; i++) {
			MPI_Recv(message, message_size, MPI_CHAR, 0, tag1, MPI_COMM_WORLD, &status); // ping?
			// // message[999] = (char) "x"; // klappt nicht...
			//strcpy(message_recv,message);
			//strcat(message_recv,"x");
			// // printf("=> %d: %d %d -- %d %d \n",i,sizeof(message),strlen(message),sizeof(message_recv),strlen(message_recv));
			MPI_Send(message, message_size, MPI_CHAR, 0, tag1, MPI_COMM_WORLD);          // pong!
		}
	}

	MPI_Finalize();
	return 0;
}





// aus stream benchmark

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

double mysecond()
{
	struct timeval tp;
	struct timezone tzp;
	int i;
	i = gettimeofday(&tp,&tzp);
	return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

# define	M	20
int checktick()
{
	int		i, minDelta, Delta;
	double	t1, t2, timesfound[M];
/*  Collect a sequence of M unique time values from the system. */
	for (i = 0; i < M; i++) {
		t1 = mysecond();
		while( ((t2=mysecond()) - t1) < 1.0E-6 )
		;
		timesfound[i] = t1 = t2;
	}
/*
 * Determine the minimum difference between these M values.
 * This result will be our estimate (in microseconds) for the
 * clock granularity.
 */
	minDelta = 1000000;
	for (i = 1; i < M; i++) {
	Delta = (int)( 1.0E6 * (timesfound[i]-timesfound[i-1]));
	minDelta = MIN(minDelta, MAX(Delta,0));
	}
	return(minDelta);
}
