#define MSMPI_NO_SAL
#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int pid, nprocs;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    
    //создаем общую группу
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    
    int n = 2;
    const int rank[2]  = {0,2};
    
    //создаем группу для четных номеров процесса
    MPI_Group even_group;
    MPI_Group_incl(world_group,n,rank,&even_group);
    
    //создаем комуникатор для четных номеров процесса
    MPI_Comm even_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, even_group,0,&even_comm);
    
    //создаем группу для нечетных номеров процесса логической операцией
    MPI_Group uneven_group;
    MPI_Group_difference(world_group,even_group,&uneven_group);
    
    //создаем уомуникатор для нечетных номеров процесса логической операцией
    MPI_Comm uneven_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, uneven_group,1,&uneven_comm);
    
    //создаем группу для нечетных номеров процесса исключением
    MPI_Group odd_group;
    MPI_Group_excl(world_group,n,rank,&odd_group);
    
    //создаем комуникатор для четных номеров процесса исключением
    MPI_Comm odd_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, odd_group,2,&odd_comm);
    
    //проверяем равенство групп
    int uneven_rank = -1;
    MPI_Group_rank(uneven_group, &uneven_rank);
    printf("pid = %d  rank = %d\n",pid,uneven_rank);
   
    int odd_rank = -1;
    MPI_Group_rank(odd_group, &odd_rank);
    printf("pid = %d  rank = %d\n",pid,odd_rank);
    
   ///////////////////////////ломаем//////////////////////////////////////////////////
    int tmp = -1 ;
    if(pid == 0){
    	tmp = 0;
    	MPI_Bcast(&tmp,1,MPI_INT,0,even_comm);
    }
    
   // MPI_Barrier(MPI_COMM_WORLD);
    printf("tmp = %d\n",tmp);
    
    int er = -1;
    if(pid == 0){
    	er = 1;
    	MPI_Bcast(&tmp,1,MPI_INT,0,uneven_comm);
    }
    printf("error = %d\n",er);
    
    ////////////////////////////////////////////////////////////////////////////////
    MPI_Group_free( &world_group);
    MPI_Group_free( &even_group );
    MPI_Group_free( &uneven_group );
    MPI_Group_free( &odd_group );
    //MPI_Comm_free( &even_comm );
    //MPI_Comm_free( &uneven_comm);
    //MPI_Comm_free( &odd_comm );
    
    
    MPI_Finalize();
    return 0;
}
