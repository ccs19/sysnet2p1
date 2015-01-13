/* Banking Customer program                         */
/*                                                  */
/* The majority of this code is taken from the man  */
/* pages.                                           */
/*                                                  */
/* In the UNIX lab...                               */
/* gcc -g -o customer customer.c -lrt -lpthread     */
/*                                                  */
/* In Linux...                                      */
/* gcc -g -o customer customer.c -lpthread          */
/*                                                  */
/* This is NOT complete code.  Race conditions      */
/* still exist.  It is only intended to demonstrate */
/* the use of threads and semaphores.               */



#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/types.h>
#include <unistd.h>

#define TELLERS 10
#define CUSTOMERS 100

#define FALSE 0
#define TRUE 1

sem_t  bank_line;      /* semaphore */
int balance = 1000;

void *customer(void *);
void *do_business(void *);

int main(int argc, char ** argv) {
    int customer_count = 0, deposit_withdrawal;
    pthread_t tid[CUSTOMERS];

    sem_init(&bank_line,
#ifdef linux
       FALSE,
#else
            TRUE,
#endif
            TELLERS);  /* 10 tellers available */

    while(customer_count < CUSTOMERS) {
        deposit_withdrawal = rand() % 1000 - 500;
        pthread_create(&tid[customer_count], NULL, customer, (void *)deposit_withdrawal);
        printf("created customer %d [%d]\n", customer_count, tid[customer_count]);
        customer_count++;
    }

    for(customer_count=0; customer_count < CUSTOMERS; customer_count++)
        pthread_join(tid[customer_count], NULL);

}

void *customer(void *deposit_withdrawal)
{
    int this_customer, in_a_hurry = 5, num_tellers;
    pthread_t tid = pthread_self();
    this_customer = rand() % 100;
    if (this_customer <= in_a_hurry)  {
        if (sem_trywait(&bank_line) != 0)
        if (errno == EAGAIN) {    /* no teller available */
            printf("[%d] Customer leaving (%d <= %d) - can't wait\n",
                    tid, this_customer, in_a_hurry);
            return;
        }     /*else go immediately to available teller
              & decrement bank_line*/
    }
    else {
        sem_getvalue(&bank_line,&num_tellers);
        printf("[%d] waiting for teller (available: %d)\n",
                tid, num_tellers);
        sem_wait(&bank_line); /* wait for next teller,
                             then proceed, and decrement bank_line */
        printf("[%d] Found teller\n", tid);
    }
    do_business(deposit_withdrawal);
    sem_post(&bank_line); /* increment bank_line;
                           this_customer's teller is now available */
}

void *do_business(void *deposit_withdrawal)
{
    int amount;
    sleep(1);
    amount = (int)deposit_withdrawal;
    if (-amount > balance) {
        printf("insufficient funds available ($%d) to withdraw $%d\n",
                balance, amount);
        return;
    }
    balance += amount;
    printf("transaction: $%d\tbalance: $%d\n",
            amount, balance);
}