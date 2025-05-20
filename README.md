# Student Expense Tracker

## Project Overview

The Student Expense Tracker is a personal finance management tool designed specifically for college students. Through a command-line interface, it provides intuitive and convenient expense recording and analysis functions. The system helps students plan and track personal finances, understand spending habits, and develop good financial awareness.

## Key Features

1. **Expense Record Management**
   - Add new expense records (amount, category, date, remarks)
   - Delete existing expense records (with 15-second confirmation countdown to prevent accidental deletion)
   - View all expense records

2. **Data Sorting and Organization**
   - Sort records by date
   - Sort records by amount (ascending)

3. **Statistical Analysis**
   - Total expense amount statistics
   - Expense proportion analysis by category
   - Identification of highest spending category
   - Identification of highest single expense
   - Average expense calculation

4. **Data Persistence**
   - Automatic data saving to file
   - Automatic loading of historical data at startup

## Expense Categories

The system supports five main expense categories:
- Learning Supplies
- Daily Necessities
- Transportation
- Food
- Other

## Technical Features

- Implemented in pure C++ with no external dependencies
- Object-oriented design with clear logical organization
- Robust user input validation
- Secure data storage mechanism
- Exception handling and fault-tolerant design

## Installation Instructions

### System Requirements
- C++11 or higher compiler
- Terminal environment supporting standard input/output

### Compilation Steps

#### Linux/macOS
```bash
g++ -std=c++11 expense_tracker.cpp -o expense_tracker
```

#### Windows (using MinGW)
```bash
g++ -std=c++11 expense_tracker.cpp -o expense_tracker.exe
```

#### Windows (using Visual Studio)
1. Create a new console application project
2. Add the source code to the project
3. Compile and run

## User Guide

### Launch the Program
```bash
./expense_tracker  # Linux/macOS
expense_tracker.exe  # Windows
```

### Main Menu Options
After starting the program, the following menu will be displayed:
```
===== Student Expense Tracker =====
1. Add Expense Record
2. Delete Expense Record
3. Display All Expense Records
4. Sort Records by Date
5. Sort Records by Amount
6. Statistics and Analysis
0. Exit Program
============================
```

### Adding an Expense Record
1. Select option `1` in the main menu
2. Enter the amount as prompted (must be non-negative)
3. Choose the expense category (1-5)
4. Enter the date (year, month, day)
5. Enter remarks (optional)

### Deleting an Expense Record
1. Select option `2` in the main menu
2. The system will display all current records
3. Confirm whether deletion is needed
4. Enter the ID of the record to delete
5. The system will display a 15-second countdown; enter `y` to confirm deletion, any other key to cancel

### Viewing Statistics
1. Select option `6` in the main menu
2. The system will display total expenses, record count, average expense, and other statistics
3. Display expense details by category
4. Show the highest spending category and highest single expense

## Data Storage

The system automatically saves data to the `student_expenses.dat` file. This file stores all expense records in plain text format, automatically loading when the program starts and saving when it exits.

## Notes

- A new data file will be automatically created when first used
- Exercise caution when deleting records, as confirmed deletions cannot be recovered
- Do not directly edit the data file to ensure data security

## Potential Extensions

- Graphical user interface
- Multi-user support
- Export of statistical reports
- Budget setting and reminders
- Spending trend chart analysis

## License Information

This software is an open-source project following the MIT license. Code contributions and improvement suggestions are welcome.

---

We hope this expense tracking system helps college students better manage personal finances and develop good financial habits.
