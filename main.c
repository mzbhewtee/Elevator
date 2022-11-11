#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

// initialise the variables
int elevator_passengers, maximum_capacity, 
additional_passengers;
pthread_mutex_t lock; // lock is a mutex lock that is used to ensure that only one thread is accessing the elevator at a time.

static struct user{
  pthread_barrier_t barrier; // create a barrier to synchronize the threads.
  int id, current, destination_floor;
  pthread_mutex_t lock; //  lock the user struct
} users[10]; // create an array of 10 users

static struct elevator{
  int id, floor, current_floor, passengers, occupancy;
  struct user *u; // pointer to the user struct
  pthread_mutex_t lock;
} elevators[2]; // create an array of 2 elevators

int elevator;
void init_elevator(){
   pthread_mutex_init(&elevators[elevator].lock, NULL); // initialise the elevator lock
   elevators[elevator].current_floor=0; // set the current floor of the elevator to 0
   elevators[elevator].occupancy=0; // set the occupancy of the elevator to 0
   elevators[elevator].u=NULL; // set the user pointer to NULL
}

// function to get the number of people waiting outside the elevator
void input_Elevatorpassengers(int elevator){
  int i;

  printf("Number of people in the lobby area waiting:: %d\n", elevator);
  scanf("%d", &elevator_passengers);

  for (int i = 0; i < elevator_passengers; i++) // loop through the number of passengers
    {
      pthread_barrier_init(&users[i].barrier, NULL, 2); // initialise the barrier
      elevators[elevator].passengers++; // increment the number of passengers
    }
}

// function to check the number of passengers in the elevator
void check_elevator(int elevator){
  if(elevators[elevator].passengers > 5 && elevators[elevator].passengers <= 10){ // if the number of passengers is greater than 5 and less than or equal to 10
    additional_passengers= (elevators[elevator].passengers)- 5; // calculate the additional passengers
    maximum_capacity = elevator_passengers-additional_passengers; // calculate the maximum capacity of the elevator
  }
  if(elevators[elevator].passengers <= 5){ // if the number of passengers is less than or equal to 5
    maximum_capacity = elevator_passengers; // set the maximum capacity to the number of passengers
  }
  if(elevators[elevator].passengers > 10){ // if the number of passengers is greater than 10
    printf("Maximum capacity has been reached\n"); // print the message
    exit(1); // exit the program
  }
}

// function to control elevator movement
void userInput(){
  pthread_mutex_lock(&lock); // lock the mutex
  check_elevator(elevator); // call the check_elevator function

  printf("Number of people in the lobby area waiting: 0 => %d",maximum_capacity); //checking code.
  for(int i = 0; i < maximum_capacity; i++){ // loop through the maximum capacity
    printf( "\nWhich floor is passenger%d going?\n", i+1);
    scanf("%d", &users[i].destination_floor);

    // check if the destination floor is valid
    if(users[i].destination_floor > 8 || users[i].destination_floor < 0){ // if the destination floor is greater than 8 or less than 0
      printf("ERROR. Floor not found!"); // print the error message
      exit(1); // exit the program
    }
  }

  // initialise the size / stories of the building
  int size = 9;
  
  for (int i = 0; i < maximum_capacity; ++i) // loop through the maximum capacity
    {
      for (int j = i + 1; j < maximum_capacity; ++j) // loop through the maximum capacity
        {
          if (users[i].destination_floor > users[j].destination_floor) // if the destination floor of the first user is greater than the destination floor of the second user
            {
              int a =  users[i].destination_floor; // set a to the destination floor of the first user
              users[i].destination_floor = users[j].destination_floor; // set the destination floor of the first user to the destination floor of the second user
              users[j].destination_floor = a; // set the destination floor of the second user to a
            }
        }
        {
          
        }

      //removing duplicate values in the elevator
      for( int j = i + 1;  j < maximum_capacity; j++){ // loop through the maximum capacity
        if(users[i].destination_floor == users[j].destination_floor){ // if the destination floor of the first user is equal to the destination floor of the second user
          for(int k = j; k < size - 1; k++){ // loop through the size of the building
            users[k].destination_floor = users[k + 1].destination_floor; // set the destination floor of the first user to the destination floor of the second user
          }
          maximum_capacity--; // decrement the maximum capacity
          j--; // decrement j
        }
      }
    }

  pthread_mutex_unlock(&lock); // unlock the mutex
  if(additional_passengers> 0){ // if the additional passengers is greater than 0
    pthread_mutex_lock(&lock); // lock the mutex
    printf("Number of people in the lobby area waiting:1 => %d \n", additional_passengers); // print the number of additional passengers
    for(int i = 0; i < additional_passengers; i++){ // loop through the additional passengers
      printf( "\n Which floor is user %d going to?\n", i+1); // print the message
      scanf("%d", &users[i].destination_floor); // scan the destination floor

      //make sure that the floor choice is not greater 8 floors
      if(users[i].destination_floor > 8 || users[i].destination_floor < 0){ // if the destination floor is greater than 8 or less than 0
        printf("ERROR. Floor not found!"); // print the error message
        exit(1); // exit the program
      }
    }
    int size = 9;
    //aranging the destination floor in ascending order
    for (int i = 0; i < additional_passengers; ++i)
      {
        for (int j = i + 1; j < additional_passengers; ++j)
          {
            if (users[i].destination_floor > users[j].destination_floor)
              {
                int a =  users[i].destination_floor;
                users[i].destination_floor = users[j].destination_floor;
                users[j].destination_floor = a;
              }
          }
        //removing duplicate values in the elevator
        for( int j = i + 1;  j < additional_passengers; j++){
          if(users[i].destination_floor == users[j].destination_floor){
            for(int k = j; k < size - 1; k++){
              users[k].destination_floor = users[k + 1].destination_floor;
            }
            additional_passengers--;
            j--;
          }
        }
      }
    pthread_mutex_unlock(&lock);
  }
}

// function to control elevator movement
void elevator_movement(int elevator){ 
  int count = 0; // set count to 0
  pthread_mutex_lock(&lock); // lock the mutex
  printf("Elevator %d is on floor %d\n", elevator, elevators[elevator].current_floor); // print the message
  // Going to specific floor
  for( int i ; i < 5; i++){
    printf("Elevator is moving to floor %d\n", users[i].destination_floor);
    sleep(2*users[i].destination_floor);
    printf("Elevator at floor %d\n", users[i].destination_floor);
    printf("DELAY. Passenger%d leaves \n ", users[i+1]);
    sleep(2);
    count++;
    if(count == maximum_capacity){
      printf("All passengers have gone out. Elevator is now empty \n");
    }
  }

  // Going back to the lobby
  pthread_mutex_unlock(&lock);
  if(additional_passengers> 0){ 
    int count = 0;
    pthread_mutex_lock(&lock);
    printf("Elevator %d is on floor %d\n", elevator+1,elevators[elevator].current_floor);
    // Going to specific floor
    for( int i ; i < 5; i++){
      printf("Elevator is moving to floor %d\n", users[i].destination_floor);
      sleep(2*users[i].destination_floor);
      printf("Elevator at floor %d\n", users[i].destination_floor);
      printf("DELAY. Passenger%d leaves \n ", users[i+1]);
      sleep(2);
      count++;
      if(count == additional_passengers){
        printf("All passengers have gone out. Elevator is now empty\n");
        exit(0);
      }
    }
    pthread_mutex_unlock(&lock); // unlock the mutex
  }
  else{
    exit(1);
  }
}


int main(int argc, char** argv){ // main function

  input_Elevatorpassengers(maximum_capacity); // call the input_Elevatorpassengers function

  // Creating a thread for each user
  pthread_t user_t;
  for(size_t i=0;i < 1;i++)  { // loop through the maximum capacity
    pthread_create(&user_t,NULL,userInput,(void*)i); // create the thread
  }

  init_elevator(); // call the init_elevator function

  pthread_t elevator_t[2]; // create a thread for the elevator
  for(size_t i=0;i<1;i++) { // loop through the elevator
    pthread_create(&elevator_t[i],NULL,elevator_movement,(void*)i); // create a thread for the elevator
  }

  pthread_join(user_t, NULL); // join the user thread
  for(int i = 0; i < 2; i++) // loop through the elevator
    {
      pthread_join(elevator_t[i], NULL); // join the elevator thread
    }
}


