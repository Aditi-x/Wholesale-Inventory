#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Structure for database connection
typedef struct {
    sqlite3 *connection;
    char *db_name;
} Database;

// Initialize the database (create tables if they don't exist)
int initialize_database(Database *db, const char *db_name);

// Connect to the database
int connect_to_database(Database *db);

// Close the database connection
void close_database(Database *db);

// Products Table Operations
int add_product(Database *db, const char *name, const char *description, const char *category,
                double cost_price, double selling_price, int stock_quantity, int reorder_level);
int delete_product(Database *db, int product_id);
int update_stock_quantity(Database *db, int product_id, int new_quantity);
int get_product_by_id(Database *db, int product_id);
int list_all_products(Database *db);

// Suppliers Table Operations
int add_supplier(Database *db, const char *name, const char *contact_info, const char *address);
int get_supplier_by_id(Database *db, int supplier_id);
int list_all_suppliers(Database *db);

int list_low_stock_products(Database *db);  // Add this if it's missing

// Transactions Table Operations
int add_transaction(Database *db, int product_id, const char *transaction_type,
                    int quantity, const char *transaction_date, int customer_supplier_id);
int list_transactions(Database *db, const char *transaction_type);

// Sales Report
int generate_sales_report(Database *db, const char *start_date, const char *end_date);

#endif // DATABASE_H
