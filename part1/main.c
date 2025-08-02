#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
  int customer_id = (int)(long) tid;

  int i;
  for(i = 0; i < ORDERS_PER_CUSTOMER; i++){
     Order* order = (Order*)malloc(sizeof(Order));
    if(!order){
      printf("Memory allocation failed");
      exit(1); 
    } 

    order->menu_item = PickRandomMenuItem(); 
    order->customer_id=customer_id;    
    order->order_number=0; 
    order->next=NULL; 

    printf("Customer #%d ordered %s\n", (customer_id+1), order->menu_item);

    AddOrder(bcb, order); 
  }
 

   
	return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
  int cook_id = (int)(long) tid;

  while(true){
    Order* order = GetOrder(bcb); 

    if(!order){
      break;
    }
       
      printf("Cook #%d is fullfilling order #%d for customer #%d: %s\n", (cook_id+1), order->order_number, order->customer_id, order->menu_item);

      free(order);
  }
  return NULL; 
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    srand(time(NULL)); 

    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS); 

    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];
    
    int i; 
    for(i=0; i < NUM_CUSTOMERS; i++){
      pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, (void*)(long)i); 
    }

    
    int j; 
    for(j=0; j < NUM_COOKS; j++){
      pthread_create(&cooks[j], NULL, BENSCHILLIBOWLCook, (void*)(long)j); 
    }

    for(i=0; i<NUM_CUSTOMERS; i++){
      pthread_join(customers[i], NULL); 
    }

    for(j=0; j<NUM_COOKS; j++){
      pthread_join(cooks[j], NULL); 
    }

    if(bcb->orders_handled == bcb->expected_num_orders){
      CloseRestaurant(bcb); 
    }

    return 0;
}