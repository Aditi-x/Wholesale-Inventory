#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"

// Initialize the database (create tables if they don't exist)
int initialize_database(Database *db, const char *db_name) {
    db->db_name = strdup(db_name);
    if (connect_to_database(db) != SQLITE_OK) {
        fprintf(stderr, "Failed to connect to database: %s\n", db_name);
        return -1;
    }

    const char *create_tables_query =
        "CREATE TABLE IF NOT EXISTS Products ("
        "product_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "product_name TEXT NOT NULL, "
        "description TEXT, "
        "category TEXT, "
        "cost_price REAL NOT NULL, "
        "selling_price REAL NOT NULL, "
        "stock_quantity INTEGER NOT NULL, "
        "reorder_level INTEGER NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS Suppliers ("
        "supplier_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "supplier_name TEXT NOT NULL, "
        "contact_info TEXT, "
        "address TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS Transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "product_id INTEGER NOT NULL, "
        "transaction_type TEXT NOT NULL, "
        "quantity INTEGER NOT NULL, "
        "transaction_date TEXT NOT NULL, "
        "customer_supplier_id INTEGER, "
        "FOREIGN KEY (product_id) REFERENCES Products(product_id)"
        ");"
        "CREATE TABLE IF NOT EXISTS Customers ("
        "customer_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "customer_name TEXT NOT NULL, "
        "contact_info TEXT"
        ");";

    char *err_msg = NULL;
    if (sqlite3_exec(db->connection, create_tables_query, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create tables: %s\n", err_msg);
        sqlite3_free(err_msg);
        close_database(db);
        return -1;
    }

    return 0;
}

// Connect to the database
int connect_to_database(Database *db) {
    if (sqlite3_open(db->db_name, &db->connection) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }
    return SQLITE_OK;
}

// Close the database connection
void close_database(Database *db) {
    if (db->connection) {
        sqlite3_close(db->connection);
        db->connection = NULL;
    }
    if (db->db_name) {
        free(db->db_name);
    }
}

// Products Table Operations

// Add a new product
int add_product(Database *db, const char *name, const char *description, const char *category,
                double cost_price, double selling_price, int stock_quantity, int reorder_level) {
    const char *query =
        "INSERT INTO Products (product_name, description, category, cost_price, selling_price, stock_quantity, reorder_level) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, category, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, cost_price);
    sqlite3_bind_double(stmt, 5, selling_price);
    sqlite3_bind_int(stmt, 6, stock_quantity);
    sqlite3_bind_int(stmt, 7, reorder_level);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db->connection));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Delete a product
int delete_product(Database *db, int product_id) {
    const char *query = "DELETE FROM Products WHERE product_id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, product_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db->connection));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// List all products(       CHANGE 1        )
int list_all_products(Database *db) {
    const char *query = "SELECT * FROM Products;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    printf("%-4s %-20s %-30s %-15s %-10s %-12s %-8s %-15s\n",
       "ID", "Name", "Description", "Category", "Cost", "Selling", "Stock", "Reorder Level");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    const char *description = (const char *)sqlite3_column_text(stmt, 2);
    const char *category = (const char *)sqlite3_column_text(stmt, 3);
    double cost_price = sqlite3_column_double(stmt, 4);
    double selling_price = sqlite3_column_double(stmt, 5);
    int stock_quantity = sqlite3_column_int(stmt, 6);
    int reorder_level = sqlite3_column_int(stmt, 7);

    printf("%-4d %-20s %-30s %-15s %-10.2f %-12.2f %-8d %-15d\n",
           id, name, description, category, cost_price, selling_price, stock_quantity, reorder_level);
    }


// Sales Report
int generate_sales_report(Database *db, const char *start_date, const char *end_date) {
    const char *query =
        "SELECT p.product_name, SUM(t.quantity) AS total_sold, SUM(t.quantity * p.selling_price) AS total_sales "
        "FROM Transactions t "
        "JOIN Products p ON t.product_id = p.product_id "
        "WHERE t.transaction_type = 'OUT' "
        "AND t.transaction_date BETWEEN ? AND ? "
        "GROUP BY p.product_name;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);

    printf("Sales Report (%s to %s)\n", start_date, end_date);
    printf("Product\t\tTotal Sold\tTotal Sales\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *product_name = (const char *)sqlite3_column_text(stmt, 0);
        int total_sold = sqlite3_column_int(stmt, 1);
        double total_sales = sqlite3_column_double(stmt, 2);

        printf("%s\t\t%d\t\t%.2f\n", product_name, total_sold, total_sales);
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Suppliers Table Operations

// Add a new supplier
int add_supplier(Database *db, const char *name, const char *contact_info, const char *address) {
    const char *query =
        "INSERT INTO Suppliers (supplier_name, contact_info, address) "
        "VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contact_info, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, address, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db->connection));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// List all suppliers
int list_all_suppliers(Database *db) {
    const char *query = "SELECT * FROM Suppliers;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    printf("ID\tName\tContact Info\tAddress\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *contact_info = (const char *)sqlite3_column_text(stmt, 2);
        const char *address = (const char *)sqlite3_column_text(stmt, 3);

        printf("%d\t%s\t%s\t%s\n", id, name, contact_info, address);
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Transactions Table Operations

// Add a new transaction
int add_transaction(Database *db, int product_id, const char *transaction_type,
                    int quantity, const char *transaction_date, int customer_supplier_id) {
    const char *query =
        "INSERT INTO Transactions (product_id, transaction_type, quantity, transaction_date, customer_supplier_id) "
        "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, product_id);
    sqlite3_bind_text(stmt, 2, transaction_type, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_text(stmt, 4, transaction_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, customer_supplier_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db->connection));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// List transactions by type (IN or OUT)
int list_transactions(Database *db, const char *transaction_type) {
    const char *query =
        "SELECT t.transaction_id, p.product_name, t.quantity, t.transaction_date, t.customer_supplier_id "
        "FROM Transactions t "
        "JOIN Products p ON t.product_id = p.product_id "
        "WHERE t.transaction_type = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, transaction_type, -1, SQLITE_STATIC);

    printf("Transaction ID\tProduct\tQuantity\tDate\tCustomer/Supplier ID\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int transaction_id = sqlite3_column_int(stmt, 0);
        const char *product_name = (const char *)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        const char *transaction_date = (const char *)sqlite3_column_text(stmt, 3);
        int customer_supplier_id = sqlite3_column_int(stmt, 4);

        printf("%d\t%s\t%d\t%s\t%d\n", transaction_id, product_name, quantity, transaction_date, customer_supplier_id);
    }

    sqlite3_finalize(stmt);
    return 0;
}
int list_low_stock_products(Database *db) {
    const char *query = "SELECT product_id, name, stock_quantity, reorder_level FROM Products WHERE stock_quantity < reorder_level;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db->connection, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare low stock query: %s\n", sqlite3_errmsg(db->connection));
        return -1;
    }

    printf("ID\tName\tStock Quantity\tReorder Level\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int product_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int stock_quantity = sqlite3_column_int(stmt, 2);
        int reorder_level = sqlite3_column_int(stmt, 3);

        printf("%d\t%s\t%d\t%d\n", product_id, name, stock_quantity, reorder_level);
    }

    sqlite3_finalize(stmt);
    return 0;
}
