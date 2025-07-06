# Library Management System (LMS) - README

## Overview

This project is a **Library Management System (LMS)** implemented in C++ using **Object-Oriented Programming (OOP)** concepts. The system allows different types of users (Students, Faculty, and Librarians) to interact with the library, manage books, and track borrowing activities. The system demonstrates key OOP principles such as **abstraction**, **inheritance**, **polymorphism**, and **encapsulation**.

## Features

### User Roles and Permissions

1. **Students**:
   - Can borrow up to **3 books** at a time.
   - Maximum borrowing period: **15 days**.
   - Fines: **10 rupees per day** for overdue books.
   - Cannot manage books or other users.

2. **Faculty**:
   - Can borrow up to **5 books** at a time.
   - Maximum borrowing period: **30 days**.
   - **No fines** for overdue books.
   - Cannot manage books or other users.

3. **Librarian**:
   - Can manage the library, including:
     - Adding, removing, or updating books.
     - Adding or removing users.
   - **Cannot borrow books**.

### Book Management

- **Book Class**:
  - Attributes: `title`, `author`, `publisher`, `year`, `ISBN`, and `status` (Available, Borrowed, or Reserved).
  - Books can only be borrowed if their status is **"Available"**.

### Account Management

- **Account Class**:
  - Tracks user activity, including currently borrowed books and overdue books.
  - Calculates fines for overdue books.
  - Maintains a record of borrowing history.

### System Constraints and Rules

1. **Borrowing Rules**:
   - Students and faculty can borrow books based on their role constraints.
   - Users cannot borrow more than their allowed number of books.
   - Users with unpaid fines cannot borrow new books until fines are cleared.
   - Users can view and pay fines through a dedicated menu option.

2. **Returning and Updating Rules**:
   - Upon returning a book, its status is updated to **"Available"**.
   - Fines are calculated based on overdue days:
     - **Students**: `Fine = Days Overdue × 10 rupees/day`.
     - **Faculty**: No fines for overdue books.
   - Faculty members cannot borrow additional books if:
     - They have already reached the limit of 5 borrowed books.
     - They have an overdue book for more than 60 days.

3. **Overdue Check**:
   - If a book is returned after the borrowing period, the system:
     - Calculates the overdue period.
     - Updates the user's account by removing the book from the current borrow list and adding it to the borrowing history.
     - Prevents further borrowing if fines exist.

## Implementation Details

### Classes

1. **User** (Base Class):
   - Derived classes: `Student`, `Faculty`, and `Librarian`.
   - Encapsulates user details and provides methods for borrowing and returning books.

2. **Book**:
   - Represents books in the library.
   - Tracks book status (Available, Borrowed, or Reserved).

3. **Account**:
   - Tracks user activity, including currently borrowed books and borrowing history.
   - Calculates fines for overdue books.

4. **Library**:
   - Manages the collection of books and users.
   - Provides methods for adding, removing, and updating books and users.

### Encapsulation

- Sensitive information like user credentials and account details are stored as **private attributes**.
- Public methods are provided for controlled access to private attributes.

### Polymorphism

- Common methods like `borrowBook()` and `returnBook()` are implemented differently for `Student` and `Faculty` classes.

### File System

- The system persists data using files to ensure that the library's state (e.g., user records, borrowed books, and fines) is retained between program sessions.
- Data is loaded from files when the program starts and saved back to files when the program shuts down.

## How to Run the Program

1. **Compile the Code**:
   - Use a C++ compiler (e.g., `g++`) to compile the code:
     ```bash
     g++ -o lms main.cpp
     ```

2. **Run the Program**:
   - Execute the compiled program:
     ```bash
     ./lms
     ```

3. **Interact with the System**:
   - Follow the on-screen prompts to log in as a Student, Faculty, or Librarian.
   - Perform operations such as borrowing, returning, and viewing books based on your role.

## Example Usage

### Student Login

1. **Borrow a Book**:
   - Log in as a student.
   - Select the option to borrow a book.
   - Enter the book ID to borrow the book.

2. **Return a Book**:
   - Log in as a student.
   - Select the option to return a book.
   - Enter the book ID to return the book.

3. **View Borrowing History**:
   - Log in as a student.
   - Select the option to view borrowing history.

### Faculty Login

1. **Borrow a Book**:
   - Log in as a faculty member.
   - Select the option to borrow a book.
   - Enter the book ID to borrow the book.

2. **Return a Book**:
   - Log in as a faculty member.
   - Select the option to return a book.
   - Enter the book ID to return the book.

3. **View Borrowing History**:
   - Log in as a faculty member.
   - Select the option to view borrowing history.

### Librarian Login

1. **Add a New Book**:
   - Log in as a librarian.
   - Select the option to add a new book.
   - Enter the book details (title, author, publisher, ISBN, year).

2. **Remove a Book**:
   - Log in as a librarian.
   - Select the option to remove a book.
   - Enter the book ID to remove the book.

3. **Add a New User**:
   - Log in as a librarian.
   - Select the option to add a new user.
   - Enter the user details (name, ID, password, role).

4. **Remove a User**:
   - Log in as a librarian.
   - Select the option to remove a user.
   - Enter the user ID to remove the user.

## Data Persistence

- The system saves data to `library_data.csv` when the program shuts down.
- Data is loaded from `library_data.csv` when the program starts.

---

This README provides a comprehensive guide to the Library Management System, including its features, implementation details, and instructions for running the program.
