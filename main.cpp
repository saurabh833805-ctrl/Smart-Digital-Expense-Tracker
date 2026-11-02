
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cctype>

using namespace std;

const int MAX_EXPENSES = 1000;

// Structure to store expense details
struct Expense {
    int id;
    string date;
    string name;
    double amount;
    string category;
};

// Global variables
Expense expenses[MAX_EXPENSES];
int expenseCount = 0;

double monthlyBudget = 0;

string currentUser;
string expenseFile;
string budgetFile;

// Read a complete line
string getText(string message) {
    string value;
    cout << message;
    getline(cin, value);
    return value;
}

// Check whether text contains the file separator
bool validText(string value) {
    return !value.empty() &&
           value.find('|') == string::npos;
}

// Check username format
bool validUsername(string username) {
    if (username.empty())
        return false;

    for (char ch : username) {
        if (!isalnum(static_cast<unsigned char>(ch)) &&
            ch != '_' && ch != '-') {
            return false;
        }
    }

    return true;
}

// Read a positive integer
int readInt(string message) {
    int value;
    string input;

    while (true) {
        input = getText(message);
        stringstream ss(input);
        char extra;

        if (ss >> value && !(ss >> extra))
            return value;

        cout << "Please enter a valid number!\n";
    }
}

// Read a positive decimal amount
double readAmount(string message) {
    double value;
    string input;

    while (true) {
        input = getText(message);
        stringstream ss(input);
        char extra;

        if (ss >> value && !(ss >> extra) &&
            value > 0) {
            return value;
        }

        cout << "Please enter a valid positive amount!\n";
    }
}

// Register a new user
void registerUser() {
    string username, password;

    username = getText("Enter username: ");

    if (!validUsername(username)) {
        cout << "Use only letters, numbers, _ or - in username.\n";
        return;
    }

    password = getText("Enter password: ");

    if (!validText(password) || password.find(' ') != string::npos) {
        cout << "Invalid password! Do not use spaces or |.\n";
        return;
    }

    ifstream check("users.txt");

    string u, p;
    while (check >> u >> p) {
        if (u == username) {
            cout << "Username already exists!\n";
            return;
        }
    }

    check.close();

    ofstream file("users.txt", ios::app);

    if (!file) {
        cout << "Unable to open users file!\n";
        return;
    }

    file << username << " " << password << endl;
    file.close();

    cout << "Registration successful!\n";
}

// Login system
bool loginUser() {
    string username, password;

    username = getText("Enter username: ");
    password = getText("Enter password: ");

    ifstream file("users.txt");

    if (!file) {
        cout << "No registered users found!\n";
        return false;
    }

    string u, p;
    bool found = false;

    while (file >> u >> p) {
        if (u == username && p == password) {
            found = true;
            break;
        }
    }

    file.close();

    if (found) {
        currentUser = username;

        // Separate files for each user
        expenseFile = currentUser + "_expenses.txt";
        budgetFile = currentUser + "_budget.txt";

        cout << "Login successful! Welcome "
             << currentUser << "!\n";

        return true;
    }

    cout << "Invalid username or password!\n";
    return false;
}

// Load expenses from file
void loadExpenses() {
    expenseCount = 0;

    ifstream file(expenseFile);

    if (!file)
        return;

    string line;

    while (getline(file, line) &&
           expenseCount < MAX_EXPENSES) {

        stringstream ss(line);

        string idText, date, name, amountText, category;

        if (!getline(ss, idText, '|') ||
            !getline(ss, date, '|') ||
            !getline(ss, name, '|') ||
            !getline(ss, amountText, '|') ||
            !getline(ss, category)) {
            continue;
        }

        Expense e;
        stringstream idStream(idText);
        stringstream amountStream(amountText);

        if (!(idStream >> e.id) ||
            !(amountStream >> e.amount) ||
            e.id <= 0 || e.amount <= 0) {
            continue;
        }

        e.date = date;
        e.name = name;
        e.category = category;

        expenses[expenseCount] = e;
        expenseCount++;
    }

    file.close();
}

// Save expenses to file
void saveExpenses() {
    ofstream file(expenseFile);

    if (!file) {
        cout << "Unable to save expenses!\n";
        return;
    }

    for (int i = 0; i < expenseCount; i++) {
        file << expenses[i].id << "|"
             << expenses[i].date << "|"
             << expenses[i].name << "|"
             << fixed << setprecision(2)
             << expenses[i].amount << "|"
             << expenses[i].category << endl;
    }

    file.close();
}

// Load saved budget
void loadBudget() {
    monthlyBudget = 0;

    ifstream file(budgetFile);

    if (file) {
        double value;

        if (file >> value && value > 0)
            monthlyBudget = value;
    }

    file.close();
}

// Save budget
void saveBudget() {
    ofstream file(budgetFile);

    if (file) {
        file << fixed << setprecision(2)
             << monthlyBudget;
        file.close();
    } else {
        cout << "Unable to save budget!\n";
    }
}

// Find next expense ID
int getNextId() {
    int nextId = 1;

    for (int i = 0; i < expenseCount; i++) {
        nextId = max(nextId, expenses[i].id + 1);
    }

    return nextId;
}

// Add daily expense
void addExpense() {
    if (expenseCount >= MAX_EXPENSES) {
        cout << "Expense storage is full!\n";
        return;
    }

    Expense e;

    e.id = getNextId();

    e.date = getText("Enter date (YYYY-MM-DD): ");

    if (e.date.length() != 10 ||
        e.date[4] != '-' || e.date[7] != '-') {
        cout << "Invalid date format!\n";
        return;
    }

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 &&
            !isdigit(static_cast<unsigned char>(e.date[i]))) {
            cout << "Invalid date!\n";
            return;
        }
    }

    e.name = getText("Enter expense name: ");

    if (!validText(e.name)) {
        cout << "Invalid expense name!\n";
        return;
    }

    e.category = getText(
        "Enter category (Food/Travel/Shopping): ");

    if (!validText(e.category)) {
        cout << "Invalid category!\n";
        return;
    }

    e.amount = readAmount("Enter amount: Rs. ");

    expenses[expenseCount] = e;
    expenseCount++;

    saveExpenses();

    cout << "Expense added successfully!\n";
    cout << "Expense ID: " << e.id << endl;
}

// View expense history
void viewHistory() {
    if (expenseCount == 0) {
        cout << "No expenses found.\n";
        return;
    }

    // Sorting expenses by date
    sort(expenses, expenses + expenseCount,
         [](const Expense &a, const Expense &b) {
             return a.date < b.date;
         });

    cout << "\n========== EXPENSE HISTORY ==========\n";

    cout << left
         << setw(6) << "ID"
         << setw(14) << "Date"
         << setw(20) << "Name"
         << setw(12) << "Amount"
         << "Category\n";

    cout << "----------------------------------------------------------\n";

    for (int i = 0; i < expenseCount; i++) {
        cout << left
             << setw(6) << expenses[i].id
             << setw(14) << expenses[i].date
             << setw(20) << expenses[i].name
             << setw(12) << fixed << setprecision(2)
             << expenses[i].amount
             << expenses[i].category << endl;
    }
}

// Category-wise expense
void categoryExpense() {
    string category = getText("Enter category: ");

    double total = 0;
    bool found = false;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].category == category) {
            total += expenses[i].amount;
            found = true;
        }
    }

    if (found) {
        cout << "Total spending in " << category
             << " = Rs. " << total << endl;
    } else {
        cout << "No expenses found in this category.\n";
    }
}

// Monthly expense report
void monthlyReport() {
    string month = getText("Enter month (YYYY-MM): ");

    if (month.length() != 7 ||
        month[4] != '-') {
        cout << "Invalid month format!\n";
        return;
    }

    double total = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].date.substr(0, 7) == month) {
            total += expenses[i].amount;
        }
    }

    cout << "\n========== MONTHLY REPORT ==========\n";

    cout << "Month: " << month << endl;
    cout << "Total expense: Rs. " << total << endl;

    if (monthlyBudget > 0) {
        cout << "Monthly budget: Rs. "
             << monthlyBudget << endl;

        cout << "Remaining budget: Rs. "
             << monthlyBudget - total << endl;
    }
}

// Set monthly budget
void setBudget() {
    double newBudget =
        readAmount("Enter monthly budget: Rs. ");

    monthlyBudget = newBudget;

    saveBudget();

    cout << "Monthly budget saved successfully!\n";
}

// Budget warning
void budgetWarning() {
    if (monthlyBudget <= 0) {
        cout << "Please set your monthly budget first.\n";
        return;
    }

    string month = getText("Enter month (YYYY-MM): ");

    double total = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].date.substr(0, 7) == month) {
            total += expenses[i].amount;
        }
    }

    cout << "\nTotal spending: Rs. "
         << total << endl;

    cout << "Monthly budget: Rs. "
         << monthlyBudget << endl;

    if (total >= monthlyBudget) {
        cout << "WARNING: Monthly budget exceeded!\n";
    }
    else if (total >= monthlyBudget * 0.8) {
        cout << "WARNING: You are close to your budget!\n";
    }
    else {
        cout << "Your spending is within budget.\n";
    }
}

// Predict future expenses
void expensePrediction() {
    string month = getText("Enter month (YYYY-MM): ");

    int daysElapsed = readInt(
        "Enter days elapsed in this month: ");

    int totalDays = readInt(
        "Enter total days in this month: ");

    if (month.length() != 7 ||
        month[4] != '-' ||
        daysElapsed <= 0 ||
        totalDays < daysElapsed ||
        totalDays > 31) {

        cout << "Invalid input!\n";
        return;
    }

    double total = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].date.substr(0, 7) == month) {
            total += expenses[i].amount;
        }
    }

    double predicted =
        (total / daysElapsed) * totalDays;

    cout << "\n========== EXPENSE PREDICTION ==========\n";

    cout << "Current expense: Rs. "
         << total << endl;

    cout << "Predicted monthly expense: Rs. "
         << predicted << endl;

    if (monthlyBudget > 0 &&
        predicted > monthlyBudget) {

        cout << "Prediction: You may exceed your budget!\n";
    }
    else {
        cout << "Prediction: Spending is within budget.\n";
    }
}

// Search expense
void searchExpense() {
    string keyword =
        getText("Enter expense name to search: ");

    bool found = false;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].name.find(keyword) != string::npos) {

            cout << "\nExpense found!\n";

            cout << "ID: " << expenses[i].id << endl;
            cout << "Name: " << expenses[i].name << endl;
            cout << "Date: " << expenses[i].date << endl;
            cout << "Amount: Rs. "
                 << expenses[i].amount << endl;
            cout << "Category: "
                 << expenses[i].category << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "Expense not found.\n";
    }
}

// Delete expense
void deleteExpense() {
    int id = readInt("Enter expense ID to delete: ");

    bool found = false;

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].id == id) {

            // Shift remaining elements left
            for (int j = i; j < expenseCount - 1; j++) {
                expenses[j] = expenses[j + 1];
            }

            expenseCount--;
            found = true;

            saveExpenses();

            cout << "Expense deleted successfully!\n";
            break;
        }
    }

    if (!found) {
        cout << "Expense ID not found.\n";
    }
}

// Display main menu
void displayMenu() {
    cout << "\n========================================\n";
    cout << "       SMART DIGITAL EXPENSE TRACKER\n";
    cout << "========================================\n";

    cout << "1. Add Daily Expense\n";
    cout << "2. View Expense History\n";
    cout << "3. Category-wise Expense\n";
    cout << "4. Monthly Expense Report\n";
    cout << "5. Set Monthly Budget\n";
    cout << "6. Budget Warning\n";
    cout << "7. Expense Prediction\n";
    cout << "8. Search Expense\n";
    cout << "9. Delete Expense\n";
    cout << "10. Exit\n";

    cout << "========================================\n";
}

// Main program
int main() {
    int choice;

    cout << "========================================\n";
    cout << "     SMART DIGITAL EXPENSE TRACKER\n";
    cout << "========================================\n";

    cout << "1. Register\n";
    cout << "2. Login\n";

    choice = readInt("Enter choice: ");

    if (choice == 1) {
        registerUser();

        cout << "\nPlease login to continue.\n";
    }
    else if (choice != 2) {
        cout << "Invalid choice!\n";
        return 0;
    }

    if (!loginUser()) {
        return 0;
    }

    // Load user's saved information
    loadExpenses();
    loadBudget();

    cout << "\nWelcome to your Expense Tracker!\n";

    do {
        displayMenu();

        choice = readInt("Enter your choice: ");

        switch (choice) {

            case 1:
                addExpense();
                break;

            case 2:
                viewHistory();
                break;

            case 3:
                categoryExpense();
                break;

            case 4:
                monthlyReport();
                break;

            case 5:
                setBudget();
                break;

            case 6:
                budgetWarning();
                break;

            case 7:
                expensePrediction();
                break;

            case 8:
                searchExpense();
                break;

            case 9:
                deleteExpense();
                break;

            case 10:
                saveExpenses();
                cout << "Thank you for using Expense Tracker!\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
        }

    } while (choice != 10);

    return 0;
}
