#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

// initialise the varibles
int onboarding_people, elevator_maximum_capacity, extra_people;
pthread_mutex_t lock;

static struct user{
  pthread_barrier_t barrier;
  int id, current, destination_floor;
  pthread_mutex_t lock;
} users[5];

static struct elevator{
  int id, floor, current_floor, passengers, occupancy;
  struct user *u;
  pthread_mutex_t lock;
} elevators[2];

int elevator;
void init_elevator(){
   pthread_mutex_init(&elevators[elevator].lock, NULL);
   elevators[elevator].current_floor=0;
   elevators[elevator].occupancy=0;
   elevators[elevator].u=NULL;
}

void input_onboardingPeople(int elevator){
  int i;

  printf("How many people are waiting outside elevator %d\n", elevator);
  scanf("%d", &onboarding_people);

  for (int i = 0; i < onboarding_people; i++)
    {
      pthread_barrier_init(&users[i].barrier, NULL, 2);
      elevators[elevator].passengers++;
    }
}

void check_elevator(int elevator){
  if(elevators[elevator].passengers > 5 && elevators[elevator].passengers <= 10){
    extra_people = (elevators[elevator].passengers)- 5;
    elevator_maximum_capacity = onboarding_people - extra_people;
  }
  if(elevators[elevator].passengers <= 5){
    elevator_maximum_capacity = onboarding_people;
  }
  if(elevators[elevator].passengers > 10){
    printf("Both elevators have a combined capacity of 10\n");
    exit(1);
  }
}

// function to control elevator movement
void userInput(){
  pthread_mutex_lock(&lock);
  check_elevator(elevator);

  printf("users waiting outside elevator 0 => %d",elevator_maximum_capacity); //checking code.
  for(int i = 0; i < elevator_maximum_capacity; i++){
    printf( "\nWhich floor is user %d going to?\n", i+1);
    scanf("%d", &users[i].destination_floor);

    //make sure that the floor choice is not greater 8 floors
    if(users[i].destination_floor > 8 || users[i].destination_floor < 0){
      printf("We dont have that floor in this building");
      exit(0);
    }
  }
  int size = 9;
  //aranging the destination floor in ascending order
  for (int i = 0; i < elevator_maximum_capacity; ++i)
    {
      for (int j = i + 1; j < elevator_maximum_capacity; ++j)
        {
          if (users[i].destination_floor > users[j].destination_floor)
            {
              int a =  users[i].destination_floor;
              users[i].destination_floor = users[j].destination_floor;
              users[j].destination_floor = a;
            }
        }
      //removing duplicate values in the elevator
      for( int j = i + 1;  j < elevator_maximum_capacity; j++){
        if(users[i].destination_floor == users[j].destination_floor){
          for(int k = j; k < size - 1; k++){
            users[k].destination_floor = users[k + 1].destination_floor;
          }
          elevator_maximum_capacity--;
          j--;
        }
      }
    }
  pthread_mutex_unlock(&lock);
  if(extra_people > 0){
    pthread_mutex_lock(&lock);
    printf("Users waiting outside elevator 1=> %d \n", extra_people);
    for(int i = 0; i < extra_people; i++){
      printf( "\nWhich floor is user %d going to?\n", i+1);
      scanf("%d", &users[i].destination_floor);

      //make sure that the floor choice is not greater 8 floors
      if(users[i].destination_floor > 8 || users[i].destination_floor < 0){
        printf("We dont have that floor in this building");
        exit(0);
      }
    }
    int size = 9;
    //aranging the destination floor in ascending order
    for (int i = 0; i < extra_people; ++i)
      {
        for (int j = i + 1; j < extra_people; ++j)
          {
            if (users[i].destination_floor > users[j].destination_floor)
              {
                int a =  users[i].destination_floor;
                users[i].destination_floor = users[j].destination_floor;
                users[j].destination_floor = a;
              }
          }
        //removing duplicate values in the elevator
        for( int j = i + 1;  j < extra_people; j++){
          if(users[i].destination_floor == users[j].destination_floor){
            for(int k = j; k < size - 1; k++){
              users[k].destination_floor = users[k + 1].destination_floor;
            }
            extra_people--;
            j--;
          }
        }
      }
    pthread_mutex_unlock(&lock);
  }
}

// function to control user movement
void elevator_movement(int elevator){
  int count = 0;
  pthread_mutex_lock(&lock);
  printf("elevator %d is  currently on floor %d\n", elevator, elevators[elevator].current_floor);
  // Going to specific floor
  for( int i ; i < 5; i++){
    printf("Elevator going to floor %d\n", users[i].destination_floor);
    sleep(2*users[i].destination_floor);
    printf("Elevator has arrived at floor %d\n", users[i].destination_floor);
    printf("Elevator has stopped for 2 seconds for user to alight\n");
    sleep(2);
    count++;
    if(count == elevator_maximum_capacity){
      printf("All users have alighted \n");
    }
  }
  pthread_mutex_unlock(&lock);
  if(extra_people > 0){
    int count = 0;
    pthread_mutex_lock(&lock);
    printf("elevator %d is  currently on floor %d\n", elevator+1, elevators[elevator].current_floor);
    // Going to specific floor
    for( int i ; i < 5; i++){
      printf("Elevator going to floor %d\n", users[i].destination_floor);
      sleep(2*users[i].destination_floor);
      printf("Elevator has arrived at floor %d\n", users[i].destination_floor);
      printf("Elevator has stopped for 2 seconds for user to alight\n");
      sleep(2);
      count++;
      if(count == extra_people){
        printf("All users have alighted \n");
        exit(0);
      }
    }
    pthread_mutex_unlock(&lock);
  }
  else{
    exit(0);
  }
}


int main(int argc, char** argv){

  input_onboardingPeople(elevator_maximum_capacity);

  // Creating a thread for each user
  pthread_t user_t;
  for(size_t i=0;i < 1;i++)  {
    pthread_create(&user_t,NULL,userInput,(void*)i);
  }

  init_elevator();

  pthread_t elevator_t[2];
  for(size_t i=0;i<1;i++) {
    pthread_create(&elevator_t[i],NULL,elevator_movement,(void*)i);
  }

  pthread_join(user_t, NULL);
  for(int i = 0; i < 2; i++)
    {
      pthread_join(elevator_t[i], NULL);
    }
}
