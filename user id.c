#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ITEM 100
#define VAT 0.2 // discount is 20%
#define MAX_PROMO 4
#define MAX_PHONE_LENGTH 15
#define MAX_PASS_LENGTH 15
#define MAX_CUSTOMERS 100

typedef struct item {
    int code;
    char name[100];
    float price;
    int quantity;
    char certificate[10];
} Item;

typedef struct transaction {
    int code;
    float total_price;
    int quantity_sold;
} Transaction;

typedef struct customer {
    char phone[MAX_PHONE_LENGTH];
    char password[MAX_PASS_LENGTH];
} Customer;

Customer customer[MAX_CUSTOMERS];
Item inventory[MAX_ITEM];
Transaction sales[MAX_ITEM];

int customerCount = 0;
int itemCount = 0;
int salesCount = 0;

// Function Prototypes
void addItem();
void displayInventory();
void updateItem();
void customerBilling();
void saveData();
void loadData();
void loginAdmin();
void loginUser();
int check_coupon(const char *coupon_code, const char *valid_coupons[], int num_coupons);
void createCustomerAccount();
int authenticateCustomer(const char *phone, const char *password);
void autoSave();
void displayDailySalesReport();
void resetPassword();

int main() {
    loadData();
    int choice, userType;

    printf("\t--------------DREAM SUPERSHOP--------------\t\n");
    printf("If you're the owner, enter 1. If you're a customer, enter 2: ");
    scanf("%d", &userType);

    if (userType == 1) {
        loginAdmin();
        while (1) {
            printf("1. Add item\n");
            printf("2. Customer billing\n");
            printf("3. Display inventory\n");
            printf("4. Update item\n");
            printf("5. Display daily sales report\n");
            printf("6. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1: addItem(); break;
                case 2: customerBilling(); break;
                case 3: displayInventory(); break;
                case 4: updateItem(); break;
                case 5: displayDailySalesReport(); break;
                case 6:saveData();
                    printf("Exiting system........\n");
                    exit(0);
                default:
                    printf("Enter a valid choice.\n");
            }
            printf("\n");
        }
    } else if (userType == 2) {
        while (1) {
            printf("1. Register\n");
            printf("2. Login\n");
            printf("3. Reset password\n");
            printf("4. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1: createCustomerAccount(); break;
                case 2: loginUser(); break;
                case 3: resetPassword(); break;
                case 4: printf("Exiting system........");
                        exit(0);break;
                default: printf("Invalid choice.\n");
            }
        }
    } else {
        printf("Invalid input. Exiting...\n");
    }

    return 0;
}

void addItem() {
    if (itemCount >= MAX_ITEM) {
        printf("The inventory is full.\n");
        return;
    }

    Item newItem;
    printf("Enter the product code: ");
    scanf("%d", &newItem.code);
    printf("Enter the name: ");
    scanf("%s", newItem.name);
    printf("Enter Halal or Haram: ");
    scanf("%s", newItem.certificate);
    printf("Enter the price: ");
    scanf("%f", &newItem.price);
    printf("Enter the quantity: ");
    scanf("%d", &newItem.quantity);

    inventory[itemCount++] = newItem;
    printf("Item added successfully!\n");
    autoSave();
    printf("\n");
}

void displayInventory() {
    printf("CODE\tNAME\tPRICE\tQUANTITY\tCERTIFICATE\n");
    for (int i = 0; i < itemCount; i++) {
        printf("%d\t%s\t%.2f\t%d\t%s\n", inventory[i].code, inventory[i].name, inventory[i].price, inventory[i].quantity, inventory[i].certificate);
    }
    printf("\n");
}

void updateItem() {
    int code, found = 0;
    printf("Enter the product code: ");
    scanf("%d", &code);

    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].code == code) {
            found = 1;
            printf("Enter new price: ");
            scanf("%f", &inventory[i].price);
            printf("Enter new quantity: ");
            scanf("%d", &inventory[i].quantity);
            printf("Enter new certificate (Halal/Haram): ");
            scanf("%s", inventory[i].certificate);
            printf("Item updated successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Item with code %d not found.\n", code);
    }
    autoSave();
    printf("\n");
}

void customerBilling() {
    int code, quantity, found = 0;
    float total = 0, vat;
    const char *valid_coupons[] = {"SAVE10", "DISCOUNT20"};
    int num_coupons = sizeof(valid_coupons) / sizeof(valid_coupons[0]);
    char coupon_code[20];

    displayInventory();

    while (1) {
        printf("Enter the product code (or 0 to finish): ");
        scanf("%d", &code);
        if (code == 0) break;

        for (int i = 0; i < itemCount; i++) {
            if (inventory[i].code == code) {
                found = 1;
                printf("Enter the quantity: ");
                scanf("%d", &quantity);

                if (quantity > inventory[i].quantity) {
                    printf("Insufficient stock!\n");
                } else {
                    float price = inventory[i].price * quantity;
                    total += price;
                    inventory[i].quantity -= quantity;

                    sales[salesCount++] = (Transaction){code, price, quantity};
                }
                break;
            }
        }

        if (!found) {
            printf("Invalid product code.\n");
        }
    }

    printf("Enter coupon code (or 'NONE' if you don't have one): ");
    scanf("%s", coupon_code);

    if (strcmp(coupon_code, "NONE") != 0) {
        if (check_coupon(coupon_code, valid_coupons, num_coupons)) {
            printf("Coupon code '%s' is valid. Applying discount...\n", coupon_code);
            total *= 0.9; // Apply a 10% discount
        } else {
            printf("Invalid coupon code.\n");
        }
    }

    vat = total * VAT;
    total += vat;

    printf("\tThe billing system\t\n");
    printf("Total bill (including VAT): %.2f\n", total);
    autoSave();
    printf("\n");
}

void saveData() {
    FILE *file = fopen("data.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    // Save inventory data
    fprintf(file, "Inventory:\n");
    for (int i = 0; i < itemCount; i++) {
        fprintf(file, "%d %s %.2f %d %s\n", inventory[i].code, inventory[i].name, inventory[i].price, inventory[i].quantity, inventory[i].certificate);
    }
    // Save sales data
    fprintf(file, "Sales:\n");
    for (int i = 0; i < salesCount; i++) {
        fprintf(file, "%d %.2f %d\n", sales[i].code, sales[i].total_price, sales[i].quantity_sold);
    }
    // Save customer data
    fprintf(file, "Customers:\n");
    for (int i = 0; i < customerCount; i++) {
        fprintf(file, "%s %s\n", customer[i].phone, customer[i].password);
    }
    fclose(file);
}


void loadData() {
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        printf("No saved data found.\n");
        return;
    }

    char line[256];
    itemCount = 0;
    salesCount = 0;
    customerCount = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Inventory:", 10) == 0) continue;

        if (strncmp(line, "Sales:", 6) == 0) {
            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, "Customers:", 10) == 0) break;
                if (sscanf(line, "%d %f %d", &sales[salesCount].code, &sales[salesCount].total_price, &sales[salesCount].quantity_sold) == 3) {
                    salesCount++;
                }
            }
        }

        if (strncmp(line, "Customers:", 10) == 0) {
            while (fgets(line, sizeof(line), file)) {
                if (sscanf(line, "%s %s", customer[customerCount].phone, customer[customerCount].password) == 2) {
                    customerCount++;
                }
            }
        }

        if (sscanf(line, "%d %s %f %d %s", &inventory[itemCount].code, inventory[itemCount].name, &inventory[itemCount].price, &inventory[itemCount].quantity, inventory[itemCount].certificate) == 5) {
            itemCount++;
        }
    }

    fclose(file);
    printf("Data loaded successfully!\n");
}



void loginAdmin() {
    char username[50];
    char password[50];
    int attempts = 5;

    while (attempts > 0) {
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);

        if (strcmp(username, "admin") == 0 && strcmp(password, "admin2893") == 0) {
            printf("Login successful!\n");
            return;
        } else {
            attempts--;
            printf("Invalid username or password. You have %d attempt(s) left.\n", attempts);
        }
    }

    printf("Too many failed attempts. Exiting...\n");
    exit(0);
}

int check_coupon(const char *coupon_code, const char *valid_coupons[], int num_coupons) {
    for (int i = 0; i < num_coupons; i++) {
        if (strcmp(coupon_code, valid_coupons[i]) == 0) {
            return 1; // Valid
        }
    }
    return 0; // Invalid
}

void createCustomerAccount() {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Customer limit reached.\n");
        return;
    }

    Customer newCustomer;
    printf("Enter phone number: ");
    scanf("%s", newCustomer.phone);
    printf("Create a password: ");
    scanf("%s", newCustomer.password);

    customer[customerCount++] = newCustomer;
    autoSave();
    printf("Customer account created successfully!\n");
    saveData();
}

int authenticateCustomer(const char *phone, const char *password) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customer[i].phone, phone) == 0 && strcmp(customer[i].password, password) == 0) {
            return 1; // Success
        }
    }
    return 0; // Failure
}

void loginUser(){
    char phone[MAX_PHONE_LENGTH];
    char password[MAX_PASS_LENGTH];
    printf("Enter phone number: ");
    scanf("%s", phone);
    printf("Enter password: ");
    scanf("%s", password);

    if (authenticateCustomer(phone, password)) {
        printf("Login successful!\n");
        while (1) {
            int choice;
            printf("1. Buy items\n");
            printf("2. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1: customerBilling(); break;
                case 2: saveData(); printf("Exiting system...\n"); exit(0);break;
                default: printf("Invalid choice.\n");
            }
        }
    } else {
        printf("Login failed! Invalid phone number or password.\n");
    }
}

void autoSave() {
    saveData();
}

void displayDailySalesReport() {
    printf("Daily Sales Report:\n");
    printf("CODE\tTOTAL PRICE\tQUANTITY SOLD\n");
    for (int i = 0; i < salesCount; i++) {
        printf("%d\t%.2f\t%d\n", sales[i].code, sales[i].total_price, sales[i].quantity_sold);
    }
    printf("\n");
}

void resetPassword() {
    char phone[MAX_PHONE_LENGTH];
    char oldPassword[MAX_PASS_LENGTH];
    char newPassword[MAX_PASS_LENGTH];

    printf("Enter your phone number: ");
    scanf("%s", phone);
    printf("Enter your old password: ");
    scanf("%s", oldPassword);

    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customer[i].phone, phone) == 0 && strcmp(customer[i].password, oldPassword) == 0) {
            printf("Enter your new password: ");
            scanf("%s", newPassword);

            strcpy(customer[i].password, newPassword);
            printf("Password reset successful!\n");
            autoSave();
            return;
        }
    }

    printf("Password reset failed! Invalid phone number or old password.\n");
}
