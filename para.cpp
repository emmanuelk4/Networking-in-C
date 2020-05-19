#include <iostream> 
#include <pthread.h>
#include <omp.h>
#include <semaphore.h>
#include <stack>
using namespace std;
sem_t empty,full;
stack<int> stk;
void produce(int i)
{
    {
    sem_wait(&empty);
            cout<<"produce "<<i*i<<endl;
            stk.push(i*i);
    sem_post(&full);
    }
}
void consume1(int &x)
{
    sem_wait(&full);
            int data=stk.top();
            stk.pop();
            x=data;
    sem_post(&empty);
}
void consume2()
{
    sem_wait(&full);
            int data=stk.top();
            stk.pop();
            cout<<"consume2 "<<data<<endl;
    sem_post(&empty);
}
int main()
{
    sem_init(&empty,0,1);
    sem_init(&full,0,0);
    pthread_t t1,t2,t3;
    omp_set_num_threads(3);
     int TID=0;
    #pragma omp parallel private(TID)
    {
            TID=omp_get_thread_num();
            if(TID==0)
            {
            cout<<"There are "<<omp_get_num_threads()<<" threads"<<endl;
            for(int i=0;i<5;i++)
                    produce(i);
            }
            else if(TID==1)
            {
                    int x;
                    while(true)
                    {
                            consume1(x);
                            cout<<"consume1 "<<x<<endl;
                    }
            }
            else if(TID==2)
            {
                    int x;
                    while(true)
                    {
                            consume1(x);
                            cout<<"consume2 "<<x<<endl;
                    }
            }
    }
    return 0;
}
