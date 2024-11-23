#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

// Function prototypes for menu operations
void display_menu();
void handle_add_product(Database *db);
void handle_list_products(Database *db);
void handle_delete_product(Database *db);
void handle_generate_sales_report(Database *db);
void handle_add_supplier(Database *db);
void handle_list_transactions(Database *db);
void handle_list_suppliers(Database *db);
void handle_add_transaction(Database *db);
void handle_exit(Database *db);
void handle_exit_to_main_menu();
void handle_low_stock_products();

int main() {
    Database db;
    const char *db_name = "inventory.db";

    // Initialize the database
    if (initialize_database(&db, db_name) != 0) {
        return -1;
    }

    int choice;
    while (1) {
        // Display the menu
        display_menu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input\n");
            continue;
        }

        // Handle user choice
        switch (choice) {
            case 1:
                handle_add_product(&db);
                break;
            case 2:
                handle_list_products(&db);
                break;
            case 3:
                handle_generate_sales_report(&db);
                break;
            case 4:
                handle_add_supplier(&db);
                break;
            case 5:
                handle_add_transaction(&db);
                break;
            case 6:
                handle_delete_product(&db);
                break;
            case 7:
                handle_list_transactions(&db);
                break;
            case 8:
                handle_list_suppliers(&db);
                break;
            case 9:
                handle_exit(&db);
                return 0;
            case 10: // New case
                handle_low_stock_products(&db);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

void display_menu() {
    printf("\n------ Inventory Management System ------\n");
    printf("1. Add Product\n");
    printf("2. List All Products\n");
    printf("3. Generate Sales Report\n");
    printf("4. Add Supplier\n");
    printf("5. Add Transaction\n");
    printf("6. Delete Product\n");
    printf("7. List Transactions\n");
    printf("8. List Suppliers\n");
    printf("9. Exit\n");
    printf("10.List Low Stock Products\n");
}

void handle_add_product(Database *db) {
    char name[100], description[255], category[50];
    double cost_price, selling_price;
    int stock_quantity, reorder_level;

    // Get product details
    printf("Enter product name: ");
    scanf(" %[^\n]", name);
    printf("Enter description: ");
    scanf(" %[^\n]", description);
    printf("Enter category: ");
    scanf(" %[^\n]", category);
    printf("Enter cost price: ");
    scanf("%lf", &cost_price);
    printf("Enter selling price: ");
    scanf("%lf", &selling_price);
    printf("Enter stock quantity: ");
    scanf("%d", &stock_quantity);
    printf("Enter reorder level: ");
    scanf("%d", &reorder_level);

    // Add product to the database
    if (add_product(db, name, description, category, cost_price, selling_price, stock_quantity, reorder_level) == 0) {
        printf("Product added successfully!\n");
    } else {
        printf("Failed to add product.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_list_products(Database *db) {
    // List all products from the database
    if (list_all_products(db) != 0) {
        printf("Failed to retrieve products.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_delete_product(Database *db) {
    int product_id;
    printf("Enter product ID to delete: ");
    scanf("%d", &product_id);

    // Delete the product from the database
    if (delete_product(db, product_id) == 0) {
        printf("Product deleted successfully.\n");
    } else {
        printf("Failed to delete product.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_generate_sales_report(Database *db) {
    char start_date[20], end_date[20];

    // Get the date range for the sales report
    printf("Enter start date (YYYY-MM-DD): ");
    scanf("%s", start_date);
    printf("Enter end date (YYYY-MM-DD): ");
    scanf("%s", end_date);

    // Generate the sales report
    if (generate_sales_report(db, start_date, end_date) == 0) {
        printf("Sales report generated successfully!\n");
    } else {
        printf("Failed to generate sales report.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_add_supplier(Database *db) {
    char name[100], contact_info[255], address[255];

    // Get supplier details
    printf("Enter supplier name: ");
    scanf(" %[^\n]", name);
    printf("Enter contact info: ");
    scanf(" %[^\n]", contact_info);
    printf("Enter address: ");
    scanf(" %[^\n]", address);

    // Add supplier to the database
    if (add_supplier(db, name, contact_info, address) == 0) {
        printf("Supplier added successfully!\n");
    } else {
        printf("Failed to add supplier.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_list_transactions(Database *db) {
    char transaction_type[10];

    // Get transaction type (IN or OUT)
    printf("Enter transaction type (IN/OUT): ");
    scanf("%s", transaction_type);

    // List transactions based on type
    if (list_transactions(db, transaction_type) == 0) {
        printf("Transactions listed successfully!\n");
    } else {
        printf("Failed to list transactions.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_list_suppliers(Database *db) {
    // List all suppliers from the database
    if (list_all_suppliers(db) != 0) {
        printf("Failed to retrieve suppliers.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_add_transaction(Database *db) {
    int product_id, quantity, customer_supplier_id;
    char transaction_type[10], transaction_date[20];

    // Get transaction details
    printf("Enter product ID: ");
    scanf("%d", &product_id);
    printf("Enter transaction type (IN/OUT): ");
    scanf("%s", transaction_type);
    printf("Enter quantity: ");
    scanf("%d", &quantity);
    printf("Enter transaction date (YYYY-MM-DD): ");
    scanf("%s", transaction_date);
    printf("Enter customer/supplier ID: ");
    scanf("%d", &customer_supplier_id);

    // Add transaction to the database
    if (add_transaction(db, product_id, transaction_type, quantity, transaction_date, customer_supplier_id) == 0) {
        printf("Transaction added successfully!\n");
    } else {
        printf("Failed to add transaction.\n");
    }

    handle_exit_to_main_menu();  // Return to main menu after the operation
}

void handle_low_stock_products(Database *db) {
    printf("=== Low Stock Products ===\n");
    if (list_low_stock_products(db) == 0) {
        printf("Low stock products listed successfully.\n");
    } else {
        printf("Failed to retrieve low stock products.\n");
    }

    handle_exit_to_main_menu();
}

void handle_exit(Database *db) {
    // Close the database and exit the program
    close_database(db);
    printf("Exiting program.\n");
}

void handle_exit_to_main_menu() {
    // Just print a message to let the user know they will return to the main menu
    printf("Returning to main menu...\n");
}
