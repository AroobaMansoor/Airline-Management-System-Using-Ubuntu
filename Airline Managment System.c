#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SEATS 10
#define MAX_FLIGHTS 10

int priceBusiness = 45000, priceEconomy = 12000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct customer {
    char name[20];
    int passport;
    int id;
    int seatNo;
    int price;
};

struct customer list[MAX_FLIGHTS][MAX_SEATS];

int menu() {
    int c;
    printf("\n1. RESERVE A TICKET\n");
    printf("2. CANCEL RESERVATION\n");
    printf("3. VIEW ALL AVAILABLE FLIGHTS\n");
    printf("4. VIEW ALL PASSENGERS\n");
    printf("5. SEARCH\n");
    printf("6. EXIT\n");
    printf("YOUR CHOICE: ");
    scanf("%d", &c);
    return c;
}

void *viewFlights() {
    printf("\nAvailable Flights:\n");
    printf("1. Lahore\n");
    printf("2. Karachi\n");
    printf("3. Multan\n");
    printf("4. Islamabad\n");
    printf("5. Gilgit Baltistan\n");
    printf("6. Quetta\n");
    printf("7. Chaman\n");
    printf("8. Peshawar\n");
    printf("9. Faisalabad\n");
    printf("10. Rawalpindi\n");
    sleep(2);
    pthread_exit(NULL);
}

int flightSelection() {
    int c;
    pthread_t viewFlightsThread;
    pthread_create(&viewFlightsThread, NULL, viewFlights, NULL);
    pthread_join(viewFlightsThread, NULL);

    printf("YOUR CHOICE: ");
    scanf("%d", &c);
    return c;
}

int prices() {
    int choice;
    printf("WHICH CLASS DO YOU WANT:\n1. BUSINESS 45000rs\n2. ECONOMY 12000rs\nCHOICE: ");
    scanf("%d", &choice);
    if (choice == 1)
        return priceBusiness;
    else if (choice == 2)
        return priceEconomy;
    else {
        printf("INVALID INPUT\n");
        return 0;
    }
}

void reserveTicket(int flightNumber, int seat) {
    if (seat < 1 || seat > MAX_SEATS) {
        printf("Invalid seat number. Please choose between 1 and 10.\n");
        return;
    }
    if (list[flightNumber - 1][seat - 1].seatNo != 0) {
        printf("\nSeat already taken.\n");
        return;
    }
    printf("ENTER ID: ");
    scanf("%d", &list[flightNumber - 1][seat - 1].id);
    printf("ENTER PASSPORT: ");
    scanf("%d", &list[flightNumber - 1][seat - 1].passport);
    printf("ENTER NAME: ");
    scanf("%s", list[flightNumber - 1][seat - 1].name);
    list[flightNumber - 1][seat - 1].price = prices();
    list[flightNumber - 1][seat - 1].seatNo = seat;
    printf("\nTicket Reserved Successfully.\n");
}

void *reserveTicketThread() {
    pthread_mutex_lock(&mutex);
    int flightNumber = flightSelection();
    int seat;
    printf("Enter seat number (1-10): ");
    scanf("%d", &seat);
    reserveTicket(flightNumber, seat);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int search(int id) {
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            if (list[i][j].id == id) {
                printf("\nData Found:\n");
                printf("ID: %d\n", list[i][j].id);
                printf("Name: %s\n", list[i][j].name);
                printf("Passport: %d\n", list[i][j].passport);
                printf("Seat No: %d\n", list[i][j].seatNo);
                printf("Price: %d\n", list[i][j].price);
                return 1;
            }
        }
    }
    return 0;
}

void *searchThread() {
    pthread_mutex_lock(&mutex);
    int id;
    printf("Enter ID: ");
    scanf("%d", &id);
    if (!search(id)) {
        printf("ID not found.\n");
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void cancelReservation(int id) {
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            if (list[i][j].id == id) {
                list[i][j] = (struct customer){0};
                printf("Reservation successfully canceled.\n");
                return;
            }
        }
    }
    printf("ID not found.\n");
}

void *cancelReservationThread() {
    pthread_mutex_lock(&mutex);
    int id;
    printf("Enter ID: ");
    scanf("%d", &id);
    cancelReservation(id);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void displayPassengers(int flightNumber) {
    int found = 0; // To check if there are any passengers
    printf("\nPassengers for Flight %d:\n", flightNumber);
    for (int i = 0; i < MAX_SEATS; i++) {
        if (list[flightNumber - 1][i].seatNo != 0) {
            found = 1;
            printf("\nPassenger %d:\n", i + 1);
            printf("ID: %d\n", list[flightNumber - 1][i].id);
            printf("Name: %s\n", list[flightNumber - 1][i].name);
            printf("Passport: %d\n", list[flightNumber - 1][i].passport);
            printf("Seat No: %d\n", list[flightNumber - 1][i].seatNo);
            printf("Price: %d\n", list[flightNumber - 1][i].price);
        }
    }
    if (!found) {
        printf("No passengers found for this flight.\n");
    }
}

void *displayPassengersThread() {
    pthread_mutex_lock(&mutex);
    int flightNumber;
    printf("Enter the flight number to view passengers (1-10): ");
    scanf("%d", &flightNumber);
    
    if (flightNumber < 1 || flightNumber > MAX_FLIGHTS) {
        printf("Invalid flight number. Please choose between 1 and 10.\n");
    } else {
        displayPassengers(flightNumber);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


void adminPanel() {
    pthread_t threads[5];
    int choice;
    while (1) {
        choice = menu();
        switch (choice) {
            case 1:
                pthread_create(&threads[0], NULL, reserveTicketThread, NULL);
                pthread_join(threads[0], NULL);
                break;
            case 2:
                pthread_create(&threads[1], NULL, cancelReservationThread, NULL);
                pthread_join(threads[1], NULL);
                break;
            case 3:
                pthread_create(&threads[2], NULL, viewFlights, NULL);
                pthread_join(threads[2], NULL);
                break;
            case 4:
                pthread_create(&threads[3], NULL, displayPassengersThread, NULL);
                pthread_join(threads[3], NULL);
                break;
            case 5:
                pthread_create(&threads[4], NULL, searchThread, NULL);
                pthread_join(threads[4], NULL);
                break;
            case 6:
                printf("Exiting admin panel.\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

int main() {
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            list[i][j].seatNo = 0;
        }
    }

        adminPanel();
    
        printf("Admin panel finished. Exiting program.\n");
    

    return 0;
}
