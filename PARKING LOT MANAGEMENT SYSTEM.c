#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double totalEarnings = 0.0;
int truckFare = 70;
int carFare = 50;
int bikeFare = 30;
int autoFare = 20;

#define MAX_VEHICLES 500
#define MAX_PARKING_SPACES 1000
#define MAX_LINE_LENGTH 200
#define FILENAME "parking_data.txt"
#define BACKUP_FILENAME "parking_data_backup.txt"

#define MAX_PHONE_LENGTH 10
#define MAX_NAME_LENGTH 50
#define MAX_MODEL_LENGTH 30
#define MAX_VEHICLE_TYPES 5

typedef struct {
    char customerName[50];
    char vehiclePlate[15];
    char vehicleModel[30];
    char vehicleType[20];
    char insuranceProvider[50];
    char phoneNumber[15];

    int entryYear, entryMonth, entryDay;
    int entryHour, entryMinute, entrySecond;

    int exitYear, exitMonth, exitDay;
    int exitHour, exitMinute, exitSecond;

    int spaceID;
    float cost;
    int status;
    time_t entryTime;
    time_t exitTime;
} Vehicle;

float vehicleFares[] = {0, 70, 50, 40, 30, 20};
int parkingSpaces[MAX_PARKING_SPACES] = {0};
Vehicle parkingLot[MAX_VEHICLES];
int vehicleCount = 0;
int occupiedSpaces = 0;
void displayMenu();
void addVehicle();
void viewVehicles();
void deleteVehicle();
void updateVehicle();
void saveToFile();
void loadFromFile();
void generateReport();
void searchVehicle();
void displayParkingSpaces();
void confirmExit();
void backupDatabase();
void validateInputString(char *input, int size, const char *prompt);
void validatePhoneNumber(char *phone);
void clearBuffer();
int findAvailableSpace();
int findVehicleByPlateOrPhone(const char *plate, const char *phone);
void getVehicleDetails(Vehicle *vehicle);
void displayVehicleDetails();
void filterVehiclesByType();
void updateVehicleStatus();
void displayParkingOccupancyReport();

int main() {
    loadFromFile();

    int choice;
    do {
        displayMenu();
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid number.\n");
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1: addVehicle(); break;
            case 2: viewVehicles(); break;
            case 3: deleteVehicle(); break;
            case 4: updateVehicle(); break;
            case 5: generateReport(); break;
            case 6: searchVehicle(); break;
            case 7: displayParkingSpaces(); break;
            case 8: confirmExit(); break;
            case 9: manageParkingLot(); break;
            case 10: printf("Exiting program. Goodbye!\n"); break;
            default: printf("Invalid choice. Please try again.\n"); break;
        }
    } while (choice != 11);

    return 0;
}


void displayMenu() {
    printf("\n--- SCT: PARKING LOT MANAGEMENT SYSTEM :SCT---\n");
    printf("\n--- Current Fare Structure ---\n");
    displayFareStructure();
    printf("\n");
    printf("\n");
    printf("1. Add Vehicle\n");
    printf("\n");
    printf("2. View Vehicles\n");
    printf("\n");
    printf("3. Delete Vehicle\n");
    printf("\n");
    printf("4. Update Vehicle\n");
    printf("\n");
    printf("5. Generate Report\n");
    printf("\n");
    printf("6. Search Vehicle by Phone\n");
    printf("\n");
    printf("7. Display Parking Spaces\n");
    printf("\n");
    printf("8. Confirm Exit and pay\n");
    printf("\n");
    printf("9. Managing by Admin-referal use\n");
    printf("\n");
    printf("10.Exit");
    printf("\nOccupied Spaces: %d / %d\n", occupiedSpaces, MAX_PARKING_SPACES);
    printf("\n");
    printf("\n");
}

void clearBuffer() {
    while (getchar() != '\n');
}

void displayVehicleDetails(int index) {
    FILE *file = fopen("parking_data.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int currentIndex = 0;
    Vehicle vehicle;
    int vehicleFound = 0;

    // Read through the file to find the vehicle at the specified index
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Customer Name:") != NULL) {
            sscanf(line, "Customer Name: %49[^\n]", vehicle.customerName);
        } else if (strstr(line, "Vehicle Plate:") != NULL) {
            sscanf(line, "Vehicle Plate: %14[^\n]", vehicle.vehiclePlate);
        } else if (strstr(line, "Vehicle Model:") != NULL) {
            sscanf(line, "Vehicle Model: %29[^\n]", vehicle.vehicleModel);
        } else if (strstr(line, "Vehicle Type:") != NULL) {
            sscanf(line, "Vehicle Type: %19[^\n]", vehicle.vehicleType);
        } else if (strstr(line, "Insurance Provider:") != NULL) {
            sscanf(line, "Insurance Provider: %49[^\n]", vehicle.insuranceProvider);
        } else if (strstr(line, "Phone Number:") != NULL) {
            sscanf(line, "Phone Number: %14[^\n]", vehicle.phoneNumber);
        } else if (strstr(line, "Exit Time:") != NULL) {
            // Parse exit time manually
            char exitTimeStr[20];
            sscanf(line, "Exit Time: %19s", exitTimeStr);
            if (strcmp(exitTimeStr, "Not yet exited") != 0) {
                sscanf(exitTimeStr, "%d-%d-%d %d:%d:%d",
                       &vehicle.exitYear, &vehicle.exitMonth, &vehicle.exitDay,
                       &vehicle.exitHour, &vehicle.exitMinute, &vehicle.exitSecond);
            } else {
                vehicle.exitYear = vehicle.exitMonth = vehicle.exitDay =
                vehicle.exitHour = vehicle.exitMinute = vehicle.exitSecond = 0;
            }
        } else if (strstr(line, "Parking Space:") != NULL) {
            sscanf(line, "Parking Space: %d", &vehicle.spaceID);
        } else if (strstr(line, "Cost:") != NULL) {
            sscanf(line, "Cost: %f", &vehicle.cost);

            // If we have reached the target vehicle index, print the details
            if (currentIndex == index) {
                vehicleFound = 1;
                break;
            }
            currentIndex++;
        }
    }

    fclose(file);

    if (vehicleFound) {
        // Display the vehicle details
        printf("Customer Name: %s\n", vehicle.customerName);
        printf("Vehicle Plate: %s\n", vehicle.vehiclePlate);
        printf("Vehicle Model: %s\n", vehicle.vehicleModel);
        printf("Vehicle Type: %s\n", vehicle.vehicleType);
        printf("Insurance Provider: %s\n", vehicle.insuranceProvider);
        printf("Phone Number: %s\n", vehicle.phoneNumber);

        // Display exit time, formatted properly
        if (vehicle.exitYear != 0) {
            printf("Exit Time: %d-%02d-%02d %02d:%02d:%02d\n",
                   vehicle.exitYear, vehicle.exitMonth, vehicle.exitDay,
                   vehicle.exitHour, vehicle.exitMinute, vehicle.exitSecond);
        } else {
            printf("Exit Time: Not yet exited\n");
        }

        printf("Parking Space: %d\n", vehicle.spaceID + 1);  // Assuming spaceID is zero-indexed
        printf("Cost: %.2f\n", vehicle.cost);
    } else {
        printf("Vehicle not found at index %d\n", index);
    }
}




void validateInputString(char *input, int size, const char *prompt) {
    printf("%s", prompt);
    clearBuffer();
    fgets(input, size, stdin);
    input[strcspn(input, "\n")] = '\0';
    while (strlen(input) == 0) {
        printf("Input cannot be empty. %s", prompt);
        fgets(input, size, stdin);
        input[strcspn(input, "\n")] = '\0';
    }
}

void validatePhoneNumber(char *phone) {
    while (1) {
        validateInputString(phone, MAX_PHONE_LENGTH, "Enter phone number: ");
        int valid = 1;
        for (int i = 0; i < strlen(phone); i++) {
            if (phone[i] < '0' || phone[i] > '9') {
                valid = 0;
                break;
            }
        }
        if (valid) {
            break;
        } else {
            printf("Invalid phone number. Please enter only digits.\n");
            printf("\n");
        }
    }
}

int findAvailableSpace() {
    FILE *file = fopen("parking_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int spaceFound = -1;
    char line[MAX_LINE_LENGTH];
    int space = 0;

    // Check each line to count vehicles in the file
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Vehicle Plate:") != NULL) {
            space++;
        }
    }

    // If the number of vehicles is less than MAX_PARKING_SPACES, return the next available space
    if (space < MAX_PARKING_SPACES) {
        spaceFound = space;
    }

    fclose(file);
    return spaceFound;
}

// Function to find vehicle by plate or phone from the file
int findVehicleByPlateOrPhone(const char *plate, const char *phone) {
    FILE *file = fopen("parking_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int vehicleIndex = -1;
    int index = 0;
    Vehicle tempVehicle;

    // Read through the file to find matching plate or phone number
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Customer Name:") != NULL) {
            // Read customer name
            sscanf(line, "Customer Name: %49[^\n]", tempVehicle.customerName);
        }
        else if (strstr(line, "Vehicle Plate:") != NULL) {
            // Read vehicle plate
            sscanf(line, "Vehicle Plate: %14[^\n]", tempVehicle.vehiclePlate);
        }
        else if (strstr(line, "Vehicle Model:") != NULL) {
            // Read vehicle model
            sscanf(line, "Vehicle Model: %29[^\n]", tempVehicle.vehicleModel);
        }
        else if (strstr(line, "Vehicle Type:") != NULL) {
            // Read vehicle type
            sscanf(line, "Vehicle Type: %19[^\n]", tempVehicle.vehicleType);
        }
        else if (strstr(line, "Phone Number:") != NULL) {
            // Read phone number
            sscanf(line, "Phone Number: %14[^\n]", tempVehicle.phoneNumber);

            // Check if this vehicle matches the search criteria (plate or phone)
            if ((plate && strcmp(tempVehicle.vehiclePlate, plate) == 0) ||
                (phone && strcmp(tempVehicle.phoneNumber, phone) == 0)) {
                vehicleIndex = index;
            }
            index++;
        }
    }

    fclose(file);
    return vehicleIndex;
}

void getVehicleDetails(Vehicle *vehicle) {
    // Prompt for vehicle details
    printf("Enter customer name: ");
    scanf("%49s", vehicle->customerName);
    printf("\n");

    printf("Enter vehicle plate: ");
    scanf("%14s", vehicle->vehiclePlate);
    printf("\n");

    printf("Enter vehicle model: ");
    scanf("%29s", vehicle->vehicleModel);
    printf("\n");

    printf("Enter vehicle type (Car, Bike, Truck, Electric, Auto): ");
    scanf("%19s", vehicle->vehicleType);
    printf("\n");

    printf("Enter phone number: ");
    scanf("%14s", vehicle->phoneNumber);
    printf("\n");

    // Open file in append mode
    FILE *file = fopen("parking_data.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write vehicle details to the file
    fprintf(file, "Customer Name: %s\n", vehicle->customerName);
    fprintf(file, "Vehicle Plate: %s\n", vehicle->vehiclePlate);
    fprintf(file, "Vehicle Model: %s\n", vehicle->vehicleModel);
    fprintf(file, "Vehicle Type: %s\n", vehicle->vehicleType);
    fprintf(file, "Phone Number: %s\n", vehicle->phoneNumber);
    fprintf(file, "-----------------------------\n");

    // Close the file
    fclose(file);

    printf("Vehicle details saved to parking_data.txt\n");
}






void addVehicle() {
    if (vehicleCount >= MAX_VEHICLES) {
        printf("Parking lot is full!\n");
        return;
    }

    int spaceID = findAvailableSpace();
    if (spaceID == -1) {
        printf("No available parking spaces.\n");
        return;
    }

    // Define a new vehicle
    Vehicle newVehicle;
    newVehicle.spaceID = spaceID;
    newVehicle.status = 1;  // Active status
    newVehicle.entryTime = time(NULL);  // Set entry time to current time
    newVehicle.exitTime = 0;  // Set exit time to 0 (indicating not exited yet)
    newVehicle.cost = 0.0;

    // Get vehicle details from the user
    getVehicleDetails(&newVehicle);

    // Set the cost based on vehicle type
    if (strcmp(newVehicle.vehicleType, "truck") == 0) {
        newVehicle.cost = (float)truckFare;
    } else if (strcmp(newVehicle.vehicleType, "car") == 0) {
        newVehicle.cost = (float)carFare;
    } else if (strcmp(newVehicle.vehicleType, "bike") == 0) {
        newVehicle.cost = (float)bikeFare;
    } else if (strcmp(newVehicle.vehicleType, "auto") == 0) {
        newVehicle.cost = (float)autoFare;
    } else {
        printf("Invalid vehicle type! Entry failed.\n");
        return;
    }

    // Add the new vehicle to the parking lot
    parkingLot[vehicleCount++] = newVehicle;
    parkingSpaces[spaceID] = 1;  // Mark the space as occupied
    occupiedSpaces++;

    printf("Vehicle added successfully at space %d.\n", spaceID + 1);
    printf("Cost at the time of entry: %.2f\n", newVehicle.cost);
}


void viewVehicles() {
    if (vehicleCount == 0) {
        printf("No vehicles currently parked.\n");
        return;
    }

    printf("\n--- Parked Vehicles ---\n");
    for (int i = 0; i < vehicleCount; i++) {
        printf("Space ID: %d\n", parkingLot[i].spaceID + 1);
        printf("Customer Name: %s\n", parkingLot[i].customerName);
        printf("Vehicle Plate: %s\n", parkingLot[i].vehiclePlate);
        printf("Vehicle Model: %s\n", parkingLot[i].vehicleModel);
        printf("Vehicle Type: %s\n", parkingLot[i].vehicleType);
        printf("Phone Number: %s\n", parkingLot[i].phoneNumber);
        printf("Entry Time: %s", ctime(&parkingLot[i].entryTime));
        if (parkingLot[i].status == 0) {
            printf("Exit Time: %s", ctime(&parkingLot[i].exitTime));
            printf("Cost: %.2f\n", parkingLot[i].cost);
        }
        printf("Status: %s\n", parkingLot[i].status == 1 ? "Active" : "Completed");
        printf("----------------------------------\n");
    }
}

void deleteVehicle() {
    char plate[15];
    validateInputString(plate, 15, "Enter the vehicle plate to delete: ");

    int index = findVehicleByPlateOrPhone(plate, NULL);
    if (index == -1) {
        printf("Vehicle with plate '%s' not found.\n", plate);
        return;
    }

    int spaceID = parkingLot[index].spaceID;
    parkingSpaces[spaceID] = 0;
    occupiedSpaces--;

    for (int i = index; i < vehicleCount - 1; i++) {
        parkingLot[i] = parkingLot[i + 1];
    }
    vehicleCount--;

    printf("Vehicle with plate '%s' has been deleted.\n", plate);
}

void updateVehicle() {
    char plate[15];
    validateInputString(plate, 15, "Enter the vehicle plate to update: ");

    int index = findVehicleByPlateOrPhone(plate, NULL);
    if (index == -1) {
        printf("Vehicle with plate '%s' not found.\n", plate);
        return;
    }

    printf("\nUpdating vehicle details for plate '%s'.\n", plate);
    getVehicleDetails(&parkingLot[index]);

    printf("Vehicle details updated successfully.\n");
}

void saveToFile() {
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        perror("Error opening file for saving.");
        return;
    }

    fwrite(&vehicleCount, sizeof(int), 1, file);
    fwrite(parkingLot, sizeof(Vehicle), vehicleCount, file);
    fwrite(parkingSpaces, sizeof(int), MAX_PARKING_SPACES, file);
    fclose(file);

    printf("Data saved successfully to '%s'.\n", FILENAME);
}

void loadFromFile() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No saved data found. Starting with a fresh database.\n");
        return;
    }

    fread(&vehicleCount, sizeof(int), 1, file);
    fread(parkingLot, sizeof(Vehicle), vehicleCount, file);
    fread(parkingSpaces, sizeof(int), MAX_PARKING_SPACES, file);
    fclose(file);

    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parkingSpaces[i] == 1) {
            occupiedSpaces++;
        }
    }

    printf("Data loaded successfully from '%s'.\n", FILENAME);
}

void generateReport() {
    if (vehicleCount == 0) {
        printf("No vehicles to generate a report.\n");
        return;
    }

    printf("\n--- Parking Lot Report ---\n");
    printf("Total Vehicles Parked: %d\n", vehicleCount);
    printf("Total Occupied Spaces: %d / %d\n", occupiedSpaces, MAX_PARKING_SPACES);
    int vehicleTypes[MAX_VEHICLE_TYPES] = {0};

    for (int i = 0; i < vehicleCount; i++) {
        if (strcmp(parkingLot[i].vehicleType, "car") == 0) {
            vehicleTypes[1]++;
        } else if (strcmp(parkingLot[i].vehicleType, "bike") == 0) {
            vehicleTypes[2]++;
        } else if (strcmp(parkingLot[i].vehicleType, "truck") == 0) {
            vehicleTypes[3]++;
        } else if (strcmp(parkingLot[i].vehicleType, "auto") == 0) {
            vehicleTypes[4]++;
        }
    }

    printf("Vehicle Types:\n");
    printf("  Cars: %d\n", vehicleTypes[1]);
    printf("  Bikes: %d\n", vehicleTypes[2]);
    printf("  Trucks: %d\n", vehicleTypes[3]);
    printf("  Auto: %d\n", vehicleTypes[4]);
}



void searchVehicle() {
    char phone[MAX_PHONE_LENGTH];
    validatePhoneNumber(phone);

    int index = findVehicleByPlateOrPhone(NULL, phone);
    if (index == -1) {
        printf("No vehicle found with phone number '%s'.\n", phone);
        return;
    }

    Vehicle *vehicle = &parkingLot[index];
    printf("\n--- Vehicle Details ---\n");
    printf("Customer Name: %s\n", vehicle->customerName);
    printf("\n");
    printf("Vehicle Plate: %s\n", vehicle->vehiclePlate);
    printf("\n");
    printf("Vehicle Model: %s\n", vehicle->vehicleModel);
    printf("\n");
    printf("Vehicle Type: %s\n", vehicle->vehicleType);
    printf("\n");
    printf("Phone Number: %s\n", vehicle->phoneNumber);
    printf("\n");
    printf("Entry Time: %s", ctime(&vehicle->entryTime));
    printf("\n");
    printf("Status: %s\n", vehicle->status == 1 ? "Active" : "Completed");
    printf("\n");
    if (vehicle->status == 0) {
        printf("Exit Time: %s", ctime(&vehicle->exitTime));
        printf("\n");
        printf("Cost: %.2f\n", vehicle->cost);
        printf("\n");
    }
}

void displayParkingSpaces() {
    printf("\n--- Parking Space Status ---\n");
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        printf("Space %d: %s\n", i + 1, parkingSpaces[i] ? "Occupied" : "Available");
    }
}

void confirmExit() {
    char plate[15];
    validateInputString(plate, 15, "Enter the vehicle plate for exit: ");

    FILE *file = fopen("parking_data.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    Vehicle vehicle;
    int vehicleFound = 0;
    long vehiclePos = -1;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Vehicle Plate:") != NULL) {
            sscanf(line, "Vehicle Plate: %14s", vehicle.vehiclePlate);
            if (strcmp(vehicle.vehiclePlate, plate) == 0) {
                vehicleFound = 1;
                vehiclePos = ftell(file) - strlen(line);
                break;
            }
        }
    }

    if (!vehicleFound) {
        printf("Vehicle with plate '%s' not found in the parking lot.\n", plate);
        fclose(file);
        return;
    }

    vehicle.exitTime = time(NULL);
    double hoursParked = difftime(vehicle.exitTime, vehicle.entryTime) / 3600.0;
    hoursParked = round(hoursParked * 100) / 100.0;
    double hourlyRate = 0.0;

    if (strcmp(vehicle.vehicleType, "truck") == 0) {
        hourlyRate = truckFare;
    } else if (strcmp(vehicle.vehicleType, "car") == 0) {
        hourlyRate = carFare;
    } else if (strcmp(vehicle.vehicleType, "bike") == 0) {
        hourlyRate = bikeFare;
    } else if (strcmp(vehicle.vehicleType, "auto") == 0) {
        hourlyRate = autoFare;
    } else {
        printf("Vehicle type '%s' not recognized in the fare structure.\n", vehicle.vehicleType);
        printf("Vehicle exit failed. Please update the fare structure or contact admin.\n");
        fclose(file);
        return;
    }

    double totalCost = hoursParked * hourlyRate;
    totalEarnings += totalCost;

    vehicle.status = 0;
    parkingSpaces[vehicle.spaceID] = 0;
    occupiedSpaces--;

    fseek(file, vehiclePos, SEEK_SET);

    fprintf(file, "Customer Name: %s\n", vehicle.customerName);
    fprintf(file, "Vehicle Plate: %s\n", vehicle.vehiclePlate);
    fprintf(file, "Vehicle Model: %s\n", vehicle.vehicleModel);
    fprintf(file, "Vehicle Type: %s\n", vehicle.vehicleType);
    fprintf(file, "Insurance Provider: %s\n", vehicle.insuranceProvider);
    fprintf(file, "Phone Number: %s\n", vehicle.phoneNumber);
    fprintf(file, "Entry Time: %s", ctime(&vehicle.entryTime));
    fprintf(file, "Exit Time: %s", ctime(&vehicle.exitTime)); // Update exit time
    fprintf(file, "Parking Space: %d\n", vehicle.spaceID + 1);
    fprintf(file, "Cost: %.2f\n", totalCost);

    fclose(file);

    printf("\n--- VEHICLE EXIT SUMMARY ---\n");
    printf("Vehicle Plate: %s\n", plate);
    printf("Vehicle Type: %s\n", vehicle.vehicleType);
    printf("Total Hours Parked: %.2f hours\n", hoursParked);
    printf("Hourly Rate: Rs %.2f\n", hourlyRate);
    printf("Total Parking Cost: Rs %.2f\n", totalCost);
    printf("Total Earnings So Far: Rs %.2f\n", totalEarnings);
    printf("--------------------------------\n");
}




void backupDatabase() {
    FILE *backupFile = fopen(BACKUP_FILENAME, "w");
    if (!backupFile) {
        perror("Error creating backup file.");
        printf("\n");
        return;
    }

    fwrite(&vehicleCount, sizeof(int), 1, backupFile);
    fwrite(parkingLot, sizeof(Vehicle), vehicleCount, backupFile);
    fwrite(parkingSpaces, sizeof(int), MAX_PARKING_SPACES, backupFile);
    fclose(backupFile);

    printf("Database backed up successfully to '%s'.\n", BACKUP_FILENAME);
    printf("\n");
}


void filterVehiclesByType() {
    char vehicleType[20];
    validateInputString(vehicleType, sizeof(vehicleType), "Enter vehicle type to filter (Truck, Car, Electric, Bike, Auto): ");

    // Open the parking data file for reading
    FILE *file = fopen("parking_data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int found = 0;

    // Read through the file to filter vehicles by type
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Vehicle Type:") != NULL) {
            char type[20];
            sscanf(line, "Vehicle Type: %19s", type); // Extract the vehicle type

            if (strcmp(type, vehicleType) == 0) {
                // If the vehicle type matches, print the details
                Vehicle vehicle;

                // Read and print the details of the matching vehicle
                fseek(file, -strlen(line), SEEK_CUR); // Move the file pointer back to the start of the current vehicle's data block
                while (fgets(line, sizeof(line), file)) {
                    if (strstr(line, "Customer Name:") != NULL) {
                        sscanf(line, "Customer Name: %49[^\n]", vehicle.customerName);
                    } else if (strstr(line, "Vehicle Plate:") != NULL) {
                        sscanf(line, "Vehicle Plate: %14s", vehicle.vehiclePlate);
                    } else if (strstr(line, "Vehicle Model:") != NULL) {
                        sscanf(line, "Vehicle Model: %29[^\n]", vehicle.vehicleModel);
                    } else if (strstr(line, "Insurance Provider:") != NULL) {
                        sscanf(line, "Insurance Provider: %49[^\n]", vehicle.insuranceProvider);
                    } else if (strstr(line, "Phone Number:") != NULL) {
                        sscanf(line, "Phone Number: %14s", vehicle.phoneNumber);
                    } else if (strstr(line, "Entry Time:") != NULL) {
                        char entryTimeStr[30];
                        sscanf(line, "Entry Time: %29[^\n]", entryTimeStr);

                        // Manually parse entry time string (format: "YYYY-MM-DD HH:MM:SS")
                        struct tm tm = {0};
                        sscanf(entryTimeStr, "%d-%d-%d %d:%d:%d",
                               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                               &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

                        tm.tm_year -= 1900;  // Year since 1900
                        tm.tm_mon -= 1;      // Months since January (0-11)
                        vehicle.entryTime = mktime(&tm);  // Convert to time_t
                    } else if (strstr(line, "Exit Time:") != NULL) {
                        char exitTimeStr[30];
                        sscanf(line, "Exit Time: %29[^\n]", exitTimeStr);
                        if (strcmp(exitTimeStr, "Not yet exited") != 0) {
                            // Manually parse exit time string (format: "YYYY-MM-DD HH:MM:SS")
                            struct tm tm = {0};
                            sscanf(exitTimeStr, "%d-%d-%d %d:%d:%d",
                                   &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                                   &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

                            tm.tm_year -= 1900;  // Year since 1900
                            tm.tm_mon -= 1;      // Months since January (0-11)
                            vehicle.exitTime = mktime(&tm);  // Convert to time_t
                        } else {
                            vehicle.exitTime = 0;
                        }
                    } else if (strstr(line, "Parking Space:") != NULL) {
                        sscanf(line, "Parking Space: %d", &vehicle.spaceID);
                    } else if (strstr(line, "Cost:") != NULL) {
                        sscanf(line, "Cost: %lf", &vehicle.cost);
                        break; // After reading all details of a vehicle, break out of the loop
                    }
                }

                // Display the found vehicle's details
                printf("\nVehicle found:\n");
                printf("Customer Name: %s\n", vehicle.customerName);
                printf("Vehicle Plate: %s\n", vehicle.vehiclePlate);
                printf("Vehicle Model: %s\n", vehicle.vehicleModel);
                printf("Insurance Provider: %s\n", vehicle.insuranceProvider);
                printf("Phone Number: %s\n", vehicle.phoneNumber);
                printf("Entry Time: %s", ctime(&vehicle.entryTime));
                printf("Exit Time: %s", vehicle.exitTime == 0 ? "Not yet exited\n" : ctime(&vehicle.exitTime));
                printf("Parking Space: %d\n", vehicle.spaceID + 1);
                printf("Cost: %.2f\n", vehicle.cost);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No vehicles found of type '%s'.\n", vehicleType);
    }

    fclose(file);
}



void displayFareStructure() {
    char inputPassword[50];
    int choice;

    // Open the file to read the current fare structure (or create if it doesn't exist)
    FILE *file = fopen("parking_data.txt", "r+");

    // If file doesn't exist, create it with default values
    if (file == NULL) {
        printf("File not found. Creating the file with default fare values...\n");

        // Open the file for writing (create the file if it doesn't exist)
        file = fopen("parking_data.txt", "w+");
        if (file == NULL) {
            printf("Error creating file.\n");
            return;
        }

        // Write the default fare values into the file
        fprintf(file, "Truck Fare: %d\n", truckFare);
        fprintf(file, "Car Fare: %d\n", carFare);
        fprintf(file, "Bike Fare: %d\n", bikeFare);
        fprintf(file, "Auto Fare: %d\n", autoFare);

        // Close the file after writing
        fclose(file);

        // Now, reopen the file to read the values
        file = fopen("parking_data.txt", "r+");
    }

    // Read current fares from the file
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Truck Fare:") != NULL) {
            sscanf(line, "Truck Fare: %d", &truckFare);
        } else if (strstr(line, "Car Fare:") != NULL) {
            sscanf(line, "Car Fare: %d", &carFare);
        } else if (strstr(line, "Bike Fare:") != NULL) {
            sscanf(line, "Bike Fare: %d", &bikeFare);
        } else if (strstr(line, "Auto Fare:") != NULL) {
            sscanf(line, "Auto Fare: %d", &autoFare);
        }
    }

    // Display current fare structure
    printf("\n--- FARE STRUCTURE ---\n");
    printf("\nVehicle Type       Current Fare\n");
    printf("-------------------------------\n");
    printf("1. Truck           %d\n", truckFare);
    printf("2. Car             %d\n", carFare);
    printf("3. Bike            %d\n", bikeFare);
    printf("4. Auto            %d\n", autoFare);
    printf("-------------------------------\n");

    // Ask if the user wants to update the fares
    printf("\nWould you like to update the fares? (1 for Yes, 0 for No): ");
    if (scanf("%d", &choice) != 1 || (choice != 0 && choice != 1)) {
        printf("Invalid input! Returning to the main menu.\n");
        clearBuffer();
        fclose(file);
        return;
    }
    clearBuffer();

    if (choice == 0) {
        printf("Returning to the main menu.\n");
        fclose(file);
        return;
    }

    // Admin password prompt to update fares
    printf("Enter admin password to update fares: ");
    scanf("%s", inputPassword);

    if (strcmp(inputPassword, "gilli_gilli_sim_sim") != 0) {
        printf("Incorrect password! You cannot update the fare structure.\n");
        fclose(file);
        return;
    }

    printf("Access granted. You may now update the fares.\n");
    int newFare;

    // Ask for new fares for each vehicle type
    printf("Enter new fare for Truck (current: %d): ", truckFare);
    if (scanf("%d", &newFare) == 1 && newFare > 0) {
        truckFare = newFare;
    } else {
        printf("Invalid input! Keeping the current fare.\n");
        clearBuffer();
    }

    printf("Enter new fare for Car (current: %d): ", carFare);
    if (scanf("%d", &newFare) == 1 && newFare > 0) {
        carFare = newFare;
    } else {
        printf("Invalid input! Keeping the current fare.\n");
        clearBuffer();
    }

    printf("Enter new fare for Bike (current: %d): ", bikeFare);
    if (scanf("%d", &newFare) == 1 && newFare > 0) {
        bikeFare = newFare;
    } else {
        printf("Invalid input! Keeping the current fare.\n");
        clearBuffer();
    }

    printf("Enter new fare for Auto (current: %d): ", autoFare);
    if (scanf("%d", &newFare) == 1 && newFare > 0) {
        autoFare = newFare;
    } else {
        printf("Invalid input! Keeping the current fare.\n");
        clearBuffer();
    }

    // Now, write the updated fare structure back to the file
    rewind(file);  // Go back to the beginning of the file for updating
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Truck Fare:") != NULL) {
            sprintf(line, "Truck Fare: %d\n", truckFare);
        } else if (strstr(line, "Car Fare:") != NULL) {
            sprintf(line, "Car Fare: %d\n", carFare);
        } else if (strstr(line, "Bike Fare:") != NULL) {
            sprintf(line, "Bike Fare: %d\n", bikeFare);
        } else if (strstr(line, "Auto Fare:") != NULL) {
            sprintf(line, "Auto Fare: %d\n", autoFare);
        }
        fputs(line, file);
    }

    fflush(file);
    fclose(file);

    printf("\nFare structure updated successfully!\n");

    // Display the updated fare structure
    printf("\n--- UPDATED FARE STRUCTURE ---\n");
    printf("\nVehicle Type       Current Fare\n");
    printf("-------------------------------\n");
    printf("1. Truck           %d\n", truckFare);
    printf("2. Car             %d\n", carFare);
    printf("3. Bike            %d\n", bikeFare);
    printf("4. Auto            %d\n", autoFare);
    printf("-------------------------------\n");
}




void displayAvailableSpaces() {
    int availableSpaces = 0;
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parkingSpaces[i] == 0) {
            availableSpaces++;
        }
    }
    printf("Total available parking spaces: %d\n", availableSpaces);
}

void manageParkingLot() {
    char role[20];
    char enteredPassword[50];
    const char adminPassword[] = "gilli_gilli_sim_sim";

    // Role-checking prompt
    printf("\n=== Manage Parking Lot ===\n");
    printf("Are you an 'employee' or 'admin'? ");
    scanf("%s", role);

    // Check role
    if (strcmp(role, "employee") == 0) {
        printf("\nYou can't access this feature. Returning to the main menu...\n");
        return;
    }

    if (strcmp(role, "admin") == 0) {
        printf("Enter the admin password: ");
        scanf("%s", enteredPassword);

        if (strcmp(enteredPassword, adminPassword) != 0) {
            printf("\nIncorrect password. Access denied.\n");
            return;
        }

        printf("\nAccess granted. Loading parking lot menu...\n");
    } else {
        printf("\nInvalid role entered. Returning to the main menu...\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n--- PARKING LOT MENU ---\n");
        printf("1. Add Vehicle\n");
        printf("2. Vehicle Exit\n");
        printf("3. View Report\n");
        printf("4. Display Available Parking Spaces\n");
        printf("5. View Fare Structure\n");
        printf("6. Load Data\n");
        printf("7. Backup Data\n");
        printf("8. Exit Program\n");
        printf("Enter your choice (1-9): ");

        if (scanf("%d", &choice) != 1) { // Validate input
            printf("Invalid input! Please enter a number between 1 and 9.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1:
                addVehicle();
                break;

            case 2:
                deleteVehicle();
                break;

            case 3:
                generateReport();
                break;

            case 4:
                displayAvailableSpaces();
                break;

            case 5:
                displayFareStructure();
                break;

            case 6:
                loadFromFile();
                break;

            case 7:
                backupDatabase();
                break;

            case 8:
                printf("Exiting program... Goodbye!\n");
                return;

            default:
                printf("Invalid choice! Please select a number between 1 and 9.\n");
        }

        printf("\nNo. of Occupied Spaces: %d/%d\n", occupiedSpaces, MAX_PARKING_SPACES);
    }
}
