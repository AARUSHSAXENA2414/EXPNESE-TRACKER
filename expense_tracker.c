/*
 * ============================================================
 *   PERSONAL EXPENSE TRACKER - C Programming Mini Project
 * ============================================================
 *   Storage : binary file "expenses.dat"
 *   Method  : load whole file into an array at start,
 *             work on the array, write whole array back
 *             to the file after every add / update / delete.
 * ============================================================
 */

#include <stdio.h>    /* printf, scanf, FILE, fopen, fread, fwrite, fclose, perror */
#include <string.h>   /* strcpy */

#define MAX 100                    /* maximum number of expenses we can store */
#define FILENAME "expenses.dat"    /* name of the data file on disk */

/* One expense = one structure */
struct Expense {
    int   id;               /* unique expense ID, e.g. 101 */
    float amount;           /* money spent, must be > 0 */
    int   category;         /* 0=Food 1=Travel 2=Shopping 3=Education 4=Others */
    char  date[11];         /* "DD-MM-YYYY" -> 10 characters + '\0' */
    char  description[50];  /* short note, may contain spaces */
};

/* Category names. categoryNames[e.category] gives the name of a category. */
char categoryNames[5][15] = {"Food", "Travel", "Shopping", "Education", "Others"};

/* ---------------- Function prototypes ---------------- */
void displayMenu(void);
void clearInputBuffer(void);
void loadExpenses(struct Expense list[], int *count);
void saveExpenses(struct Expense list[], int count);
int  findIndexById(struct Expense list[], int count, int id);
void addExpense(struct Expense list[], int *count);
void viewExpenses(struct Expense list[], int count);
void searchExpense(struct Expense list[], int count);
void updateExpense(struct Expense list[], int count);
void deleteExpense(struct Expense list[], int *count);
void calculateTotal(struct Expense list[], int count);
void categoryAnalysis(struct Expense list[], int count);

/* ======================== main ======================== */
int main(void)
{
    struct Expense list[MAX];   /* all expenses are kept here while the program runs */
    int count = 0;              /* how many slots of list[] are actually filled */
    int choice = 0;             /* user's menu choice */

    loadExpenses(list, &count); /* bring old data from the file into the array */

    do {
        displayMenu();

        /* scanf returns 1 if it read one integer successfully */
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();                     /* throw away the wrong input */
            printf("Invalid input! Please enter a number from 1 to 8.\n");
            choice = 0;                             /* so the loop does not end */
            continue;
        }

        switch (choice) {
            case 1: addExpense(list, &count);      break;
            case 2: viewExpenses(list, count);     break;
            case 3: searchExpense(list, count);    break;
            case 4: updateExpense(list, count);    break;
            case 5: deleteExpense(list, &count);   break;
            case 6: calculateTotal(list, count);   break;
            case 7: categoryAnalysis(list, count); break;
            case 8: printf("Data saved. Exiting program. Goodbye!\n"); break;
            default: printf("Invalid choice! Please enter a number from 1 to 8.\n");
        }
    } while (choice != 8);

    return 0;   /* exit status 0 = program ended successfully */
}

/* Prints the menu */
void displayMenu(void)
{
    printf("\n========== PERSONAL EXPENSE TRACKER ==========\n");
    printf("1. Add Expense\n");
    printf("2. View All Expenses\n");
    printf("3. Search Expense\n");
    printf("4. Update Expense\n");
    printf("5. Delete Expense\n");
    printf("6. Total Expenses\n");
    printf("7. Category-wise Analysis\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
}

/* Reads and discards leftover characters up to the end of the line.
   Needed when the user types letters where a number was expected. */
void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;   /* null statement: loop body intentionally empty */
}

/* ===================== FILE MODULE ===================== */

/* Reads all records from the file into list[] and sets *count.
   count is a pointer because this function must change the value in main(). */
void loadExpenses(struct Expense list[], int *count)
{
    FILE *fp = fopen(FILENAME, "rb");   /* "rb" = read, binary */

    if (fp == NULL) {
        /* File does not exist yet (first run). Start with no records. */
        printf("No previous data found. Starting fresh.\n");
        *count = 0;
        return;
    }

    /* fread returns how many complete records it actually read */
    *count = (int) fread(list, sizeof(struct Expense), MAX, fp);
    fclose(fp);

    printf("%d expense(s) loaded from file.\n", *count);
}

/* Writes all records from list[] into the file, replacing old contents. */
void saveExpenses(struct Expense list[], int count)
{
    FILE *fp = fopen(FILENAME, "wb");   /* "wb" = write, binary (erases old data) */

    if (fp == NULL) {
        perror("Error: could not save data");   /* prints the system's reason */
        return;
    }

    fwrite(list, sizeof(struct Expense), count, fp);  /* write count records in one call */
    fclose(fp);                                       /* flushes data to disk */
}

/* ===================== HELPER ===================== */

/* Linear search. Returns the array index of the expense with this id,
   or -1 if no such expense exists. */
int findIndexById(struct Expense list[], int count, int id)
{
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].id == id)
            return i;
    }
    return -1;
}

/* ===================== RECORD MODULE ===================== */

void addExpense(struct Expense list[], int *count)
{
    struct Expense e;   /* temporary structure to fill before storing */
    int cat;

    if (*count >= MAX) {
        printf("Storage full! Cannot add more than %d expenses.\n", MAX);
        return;
    }

    printf("Enter Expense ID: ");
    if (scanf("%d", &e.id) != 1 || e.id <= 0) {
        clearInputBuffer();
        printf("Invalid ID! ID must be a positive number.\n");
        return;
    }
    if (findIndexById(list, *count, e.id) != -1) {
        printf("Expense ID %d already exists! Use a different ID.\n", e.id);
        return;
    }

    printf("Enter Amount: ");
    if (scanf("%f", &e.amount) != 1 || e.amount <= 0) {
        clearInputBuffer();
        printf("Invalid amount! Amount must be greater than 0.\n");
        return;
    }

    printf("Select Category (1-Food 2-Travel 3-Shopping 4-Education 5-Others): ");
    if (scanf("%d", &cat) != 1 || cat < 1 || cat > 5) {
        clearInputBuffer();
        printf("Invalid category! Choose 1 to 5.\n");
        return;
    }
    e.category = cat - 1;   /* user types 1-5, we store 0-4 (array index) */

    printf("Enter Date (DD-MM-YYYY): ");
    scanf("%10s", e.date);  /* no & : array name is already an address */
    clearInputBuffer();

    printf("Enter Description: ");
    scanf(" %49[^\n]", e.description);  /* reads a whole line including spaces */
    clearInputBuffer();

    list[*count] = e;       /* copy the whole structure into the next free slot */
    (*count)++;             /* one more record now */

    saveExpenses(list, *count);
    printf("Expense added successfully!\n");
}

void viewExpenses(struct Expense list[], int count)
{
    int i;

    if (count == 0) {
        printf("No expenses recorded yet.\n");
        return;
    }

    printf("\n%-6s %-10s %-11s %-12s %s\n", "ID", "Amount", "Category", "Date", "Description");
    printf("----------------------------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("%-6d %-10.2f %-11s %-12s %s\n",
               list[i].id, list[i].amount, categoryNames[list[i].category],
               list[i].date, list[i].description);
    }
    printf("----------------------------------------------------------------\n");
    printf("Total records: %d\n", count);
}

void searchExpense(struct Expense list[], int count)
{
    int id, index;

    printf("Enter Expense ID to search: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID!\n");
        return;
    }

    index = findIndexById(list, count, id);
    if (index == -1) {
        printf("Expense with ID %d not found.\n", id);
        return;
    }

    printf("\nExpense found:\n");
    printf("ID          : %d\n", list[index].id);
    printf("Amount      : %.2f\n", list[index].amount);
    printf("Category    : %s\n", categoryNames[list[index].category]);
    printf("Date        : %s\n", list[index].date);
    printf("Description : %s\n", list[index].description);
}

/* count is NOT a pointer here: update does not change how many records exist.
   list[] changes are still visible in main() because arrays are passed by address. */
void updateExpense(struct Expense list[], int count)
{
    int id, index, cat;
    float amount;

    printf("Enter Expense ID to update: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID!\n");
        return;
    }

    index = findIndexById(list, count, id);
    if (index == -1) {
        printf("Expense with ID %d not found.\n", id);
        return;
    }

    printf("Current: %.2f | %s | %s | %s\n", list[index].amount,
           categoryNames[list[index].category], list[index].date, list[index].description);

    printf("Enter new Amount: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        clearInputBuffer();
        printf("Invalid amount! Update cancelled.\n");
        return;
    }

    printf("Select new Category (1-Food 2-Travel 3-Shopping 4-Education 5-Others): ");
    if (scanf("%d", &cat) != 1 || cat < 1 || cat > 5) {
        clearInputBuffer();
        printf("Invalid category! Update cancelled.\n");
        return;
    }

    /* Only change the record after amount and category are both valid */
    list[index].amount = amount;
    list[index].category = cat - 1;

    printf("Enter new Date (DD-MM-YYYY): ");
    scanf("%10s", list[index].date);
    clearInputBuffer();

    printf("Enter new Description: ");
    scanf(" %49[^\n]", list[index].description);
    clearInputBuffer();

    saveExpenses(list, count);
    printf("Expense updated successfully!\n");
}

/* count IS a pointer here: deleting reduces the number of records in main(). */
void deleteExpense(struct Expense list[], int *count)
{
    int id, index, i;

    printf("Enter Expense ID to delete: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID!\n");
        return;
    }

    index = findIndexById(list, *count, id);
    if (index == -1) {
        printf("Expense with ID %d not found.\n", id);
        return;
    }

    /* Shift every record after 'index' one place to the left,
       overwriting the record being deleted. */
    for (i = index; i < *count - 1; i++) {
        list[i] = list[i + 1];
    }
    (*count)--;   /* one record fewer */

    saveExpenses(list, *count);
    printf("Expense with ID %d deleted successfully!\n", id);
}

/* ===================== ANALYSIS MODULE ===================== */

void calculateTotal(struct Expense list[], int count)
{
    int i;
    float total = 0;

    if (count == 0) {
        printf("No expenses recorded yet.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += list[i].amount;
    }
    printf("Total expenses (%d records): %.2f\n", count, total);
}

void categoryAnalysis(struct Expense list[], int count)
{
    float totals[5] = {0};   /* one running total per category, all start at 0 */
    float grandTotal = 0;
    int i;

    if (count == 0) {
        printf("No expenses recorded yet.\n");
        return;
    }

    /* category is 0-4, so it can be used directly as the index of totals[] */
    for (i = 0; i < count; i++) {
        totals[list[i].category] += list[i].amount;
        grandTotal += list[i].amount;
    }

    printf("\n%-12s %-12s %s\n", "Category", "Amount", "Share");
    printf("-----------------------------------\n");
    for (i = 0; i < 5; i++) {
        printf("%-12s %-12.2f %.1f%%\n", categoryNames[i], totals[i],
               totals[i] * 100 / grandTotal);
    }
    printf("-----------------------------------\n");
    printf("%-12s %.2f\n", "TOTAL", grandTotal);
}
