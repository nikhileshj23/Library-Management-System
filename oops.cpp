#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <unordered_set>

using namespace std;

// Global variables
unordered_set<string> existingIds;
class Book;
class User;
class Student;
class Faculty;
class Librarian;
class Library;

// Function prototypes
string generateUniqueId();
tm parseDate(const string &date);
int daysBetweenDates(const string &date1, const string &date2);
int calculateFine(const string &borrowDate, const string &returnDate);

// Enum for booking type
enum class BookingType
{
    RESERVED,
    DIRECT_BORROW
};

// Enum for book status
enum class BookStatus
{
    AVAILABLE,
    BORROWED
};

// Class declarations
class Book
{
public:
    string bookId;
    string title;
    string author;
    string publisher;
    string ISBN;
    int year;
    BookStatus status;
    vector<string> reservationQueue;

    Book() {}
    Book(string bookId, string title, string author, string publisher, string ISBN, int year);
};

class Booking : public Book
{
public:
    string bookingId;
    string bookingDate;
    string borrowDate;
    string returnDate;
    int fine;
    BookingType type;
    string bookId;

    Booking() {}
    Booking(string bookingId, string bookingDate, string borrowDate, string returnDate, int fine, BookingType type,
            string bookId, string title, string author, string publisher, string ISBN, int year);
};

class Account
{
public:
    map<string, Booking *> current;
    map<string, Booking *> history;
};

class User
{
protected:
    string password; // Password is protected
public:
    string UniqueId;
    string name;
    Account account;

    User() {}
    User(string name, string ID, string password);

    // Public method to get the password
    string getPassword() const
    {
        return password;
    }

    // Public method to set the password
    void setPassword(const string &newPassword)
    {
        password = newPassword;
    }

    virtual void borrowBook(string date) = 0;
    virtual void returnBook(string date) = 0;
    virtual void current_booking(string date) = 0;
    virtual void login() = 0;

    void cancelReservation(const string &bookId);
    void showHistory();
    bool authenticate(string pass);

    void list_books();
};

class Student : public User
{
public:
    Student(string name, string ID, string password);

    void borrowBook(string date) override;
    void returnBook(string date) override;
    void current_booking(string date) override;
    void login() override;

    int tell_fine(string returnDate);
    bool isEligibleToBorrow(string date);
};

class Faculty : public User
{
public:
    Faculty(string name, string ID, string password);

    void borrowBook(string date) override;
    void returnBook(string date) override;
    void current_booking(string date) override;
    bool isEligibleToBorrow(string date);
    void login() override;
};

class Librarian : public User
{
public:
    Librarian(string name, string ID, string password);

    void borrowBook(string date) override {}
    void returnBook(string date) override {}
    void current_booking(string date) override {}
    void addNewUser();
    void addNewBook();
    void listUsers();
    void deleteUser();
    void deleteBook();
    void login() override;
};

class Library
{
public:
    map<string, Book *> books;
    map<string, Student *> students;
    map<string, Faculty *> faculties;
    map<string, Librarian *> librarians;
    map<string, string> userTypes; // Map to store user types (student, faculty, librarian)

    Library()
    {
        this->books = map<string, Book *>();
        this->students = map<string, Student *>();
        this->faculties = map<string, Faculty *>();
        this->librarians = map<string, Librarian *>();
        this->userTypes = map<string, string>();
    }
};

// Global library instance
Library library;

// Function definitions
string generateUniqueId()
{
    const string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int idLength = 10;
    string id;
    srand(time(0));

    do
    {
        id.clear();
        for (int i = 0; i < idLength; ++i)
        {
            id += charset[rand() % charset.length()];
        }
    } while (existingIds.count(id));

    existingIds.insert(id);
    return id;
}

tm parseDate(const string &date)
{
    tm t = {};

    if (date.length() != 8)
    {
        cerr << "Invalid date format. Use ddmmyyyy." << endl;
        exit(1);
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(2, 2));
    int year = stoi(date.substr(4, 4));

    t.tm_mday = day;
    t.tm_mon = month - 1;
    t.tm_year = year - 1900;

    return t;
}

int daysBetweenDates(const string &date1, const string &date2)
{
    tm t1 = parseDate(date1);
    tm t2 = parseDate(date2);

    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);

    if (time1 == -1 || time2 == -1)
    {
        cerr << "Error parsing dates." << endl;
        return -1;
    }

    double difference = difftime(time2, time1) / (60 * 60 * 24);
    return abs(static_cast<int>(difference));
}

int calculateFine(const string &borrowDate, const string &returnDate)
{
    int days = daysBetweenDates(borrowDate, returnDate);
    int fine = 0;

    if (days > 15)
    {
        fine = (days - 15) * 10; // Fine of 10 rupees per day for each day beyond 15
    }

    return fine;
}

// Book class functions
Book::Book(string bookId, string title, string author, string publisher, string ISBN, int year)
{
    this->bookId = bookId;
    this->title = title;
    this->author = author;
    this->publisher = publisher;
    this->ISBN = ISBN;
    this->year = year;
    this->status = BookStatus::AVAILABLE;
    this->reservationQueue = vector<string>();
}

// Booking class functions
Booking::Booking(string bookingId, string bookingDate, string borrowDate, string returnDate, int fine, BookingType type,
                 string bookId, string title, string author, string publisher, string ISBN, int year)
{
    this->bookId = bookId;
    this->title = title;
    this->author = author;
    this->publisher = publisher;
    this->ISBN = ISBN;
    this->year = year;
    this->bookingId = bookingId;
    this->bookingDate = bookingDate;
    this->borrowDate = borrowDate;
    this->returnDate = returnDate;
    this->fine = fine;
    this->type = type;
}

// User class functions
User::User(string name, string ID, string password)
{
    this->name = name;
    this->UniqueId = ID;
    this->password = password;
    this->account = Account();
}

void User::cancelReservation(const string &bookId)
{
    if (library.books.count(bookId))
    {
        Book *book = library.books[bookId];
        auto it = find(book->reservationQueue.begin(), book->reservationQueue.end(), this->UniqueId);
        if (it != book->reservationQueue.end())
        {
            book->reservationQueue.erase(it);
            cout << "Reservation cancelled successfully for book ID: " << bookId << endl;

            for (auto &bookingPair : account.current)
            {
                if (bookingPair.second->ISBN == bookId && bookingPair.second->type == BookingType::RESERVED)
                {
                    account.current.erase(bookingPair.first);
                    break;
                }
            }
        }
        else
        {
            cout << "No reservation found for book ID: " << bookId << endl;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }
}

void User::showHistory()
{
    if (account.history.empty())
    {
        cout << "No history to show" << endl;
    }
    else
    {
        for (auto &i : account.history)
        {
            Booking *booking = i.second;
            cout << "Booking ID: " << booking->bookingId << endl;
            cout << "Booking Date: " << booking->bookingDate << endl;
            cout << "Borrow Date: " << booking->borrowDate << endl;
            cout << "Return Date: " << booking->returnDate << endl;
            cout << "Fine: " << booking->fine << endl;
            cout << "Booking Type: " << (booking->type == BookingType::RESERVED ? "Reserved" : "Direct Borrow") << endl;
            cout << "Book Title: " << booking->title << endl;
            cout << "Book Author: " << booking->author << endl;
            cout << "Book Publisher: " << booking->publisher << endl;
            cout << "Book ISBN: " << booking->ISBN << endl;
            cout << "Book Year: " << booking->year << endl;
            cout << "----------------------------------------" << endl;
        }
    }
}

bool User::authenticate(string pass)
{
    return pass == (this->password);
}

void Student::current_booking(string date)
{
    if (account.current.empty())
    {
        cout << "Nothing to show" << endl;
    }
    else
    {
        for (auto &i : account.current)
        {
            Booking *booking = i.second;
            if (booking->type != BookingType::RESERVED)
                booking->fine = calculateFine(booking->borrowDate, date);
            else
                booking->fine = 0;
            cout << "Booking ID: " << booking->bookingId << endl;
            cout << "Booking Date: " << booking->bookingDate << endl;
            cout << "Borrow Date: " << booking->borrowDate << endl;
            cout << "Return Date: " << booking->returnDate << endl;
            cout << "Fine: " << booking->fine << endl;
            cout << "Booking Type: " << (booking->type == BookingType::RESERVED ? "Reserved" : "Direct Borrow") << endl;
            cout << "Book Title: " << booking->title << endl;
            cout << "Book Author: " << booking->author << endl;
            cout << "Book Publisher: " << booking->publisher << endl;
            cout << "Book ISBN: " << booking->ISBN << endl;
            cout << "Book Year: " << booking->year << endl;
            cout << "----------------------------------------" << endl;
        }
    }
}
void Faculty::current_booking(string date)
{
    if (account.current.empty())
    {
        cout << "Nothing to show" << endl;
    }
    else
    {
        for (auto &i : account.current)
        {
            Booking *booking = i.second;
            booking->fine = 0;
            cout << "Booking ID: " << booking->bookingId << endl;
            cout << "Booking Date: " << booking->bookingDate << endl;
            cout << "Borrow Date: " << booking->borrowDate << endl;
            cout << "Return Date: " << booking->returnDate << endl;
            cout << "Fine: " << booking->fine << endl;
            cout << "Booking Type: " << (booking->type == BookingType::RESERVED ? "Reserved" : "Direct Borrow") << endl;
            cout << "Book Title: " << booking->title << endl;
            cout << "Book Author: " << booking->author << endl;
            cout << "Book Publisher: " << booking->publisher << endl;
            cout << "Book ISBN: " << booking->ISBN << endl;
            cout << "Book Year: " << booking->year << endl;
            cout << "----------------------------------------" << endl;
        }
    }
}

void User::list_books()
{
    if (library.books.empty())
    {
        cout << "No books available in the library." << endl;
    }
    else
    {
        cout << "List of all books in the library:" << endl;
        cout << "----------------------------------------" << endl;
        for (const auto &bookPair : library.books)
        {
            const string &bookId = bookPair.first;
            Book *book = bookPair.second;
            cout << "Book ID: " << bookId << endl;
            cout << "Title: " << book->title << endl;
            cout << "Author: " << book->author << endl;
            cout << "Publisher: " << book->publisher << endl;
            cout << "ISBN: " << book->ISBN << endl;
            cout << "Year: " << book->year << endl;
            cout << "Status: " << (book->status == BookStatus::AVAILABLE ? "Available" : "Borrowed") << endl;
            cout << "Reservations in queue: " << book->reservationQueue.size() << endl;
            cout << "----------------------------------------" << endl;
        }
    }
}

// Student class functions
Student::Student(string name, string ID, string password) : User(name, ID, password) {}

int Student::tell_fine(string returnDate)
{
    int total = 0;
    for (auto i : this->account.current)
    {
        Booking *booking = i.second;
        if (booking->type != BookingType::RESERVED)
            total += calculateFine(booking->bookingDate, returnDate);
    }
    return total;
}

bool Student::isEligibleToBorrow(string date)
{
    // Check if the student has a fine greater than 0
    int totalFine = tell_fine(date); // Replace with actual date logic
    if (totalFine > 0)
    {
        cout << "You have a total fine of " << totalFine << " rupees. Please pay the fine to borrow a book.\n";
        return false;
    }

    // Check if the student has 3 or more books in their current map
    if (account.current.size() >= 3)
    {
        cout << "You have already borrowed or reserved " << account.current.size() << " books. Maximum of 3 books are allowed.\n";
        return false;
    }

    return true;
}

void Student::borrowBook(string date)
{
    int fine = tell_fine(date);
    if (isEligibleToBorrow(date) == false)
        return;
    cout << "Enter the book ID that you want to borrow: ";
    string bookId;
    cin >> bookId;

    if (library.books.count(bookId))
    {
        Book *book = library.books[bookId];

        // Cast BookStatus to int for the switch statement
        switch (static_cast<int>(book->status))
        {
        case static_cast<int>(BookStatus::AVAILABLE):
        {
            cout << "The book is available. Do you want to borrow it? (yes/no): ";
            string choice;
            cin >> choice;

            if (choice == "yes")
            {
                string bookingId = generateUniqueId();
                Booking *newBooking = new Booking(
                    bookingId, date, date, "N/A", 0, BookingType::DIRECT_BORROW, book->bookId,
                    book->title, book->author, book->publisher, book->ISBN, book->year);

                account.current[bookingId] = newBooking;
                book->status = BookStatus::BORROWED;

                cout << "Book borrowed successfully! Booking ID: " << bookingId << endl;
            }
            else
            {
                cout << "Borrowing cancelled." << endl;
            }
            break;
        }

        case static_cast<int>(BookStatus::BORROWED):
        {
            cout << "The book is currently borrowed. Do you want to reserve it? (yes/no): ";
            string choice;
            cin >> choice;

            if (choice == "yes")
            {
                book->reservationQueue.push_back(this->UniqueId);
                string bookingId = generateUniqueId();
                Booking *newBooking = new Booking(
                    bookingId, date, "N/A", "N/A", 0, BookingType::RESERVED, book->bookId,
                    book->title, book->author, book->publisher, book->ISBN, book->year);

                account.current[bookingId] = newBooking;
                cout << "Book reserved successfully! Booking ID: " << bookingId << endl;
            }
            else
            {
                cout << "Reservation cancelled." << endl;
            }
            break;
        }

        default:
            cout << "Invalid book status." << endl;
            break;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }
}

void Student::returnBook(string date)
{
    if (account.current.empty())
    {
        cout << "There is nothing to return.\n";
        return;
    }

    int num = 0;
    cout << "Books available for return:\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
    cout << "No. | Booking ID | Book Title          | Fine | Type\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
    for (auto &i : account.current)
    {
        Booking *booking = i.second;
        if (booking->type != BookingType::RESERVED)
            booking->fine = calculateFine(booking->borrowDate, date);
        else
            booking->fine = 0;
        cout << num << " | "
             << booking->bookingId << " | "
             << booking->title << " | "
             << booking->fine << " | "
             << (booking->type == BookingType::RESERVED ? "Reserved" : "Direct Borrow") << endl;
        num++;
    }
    cout << "--------------------------------------------------------------------------------------------------------\n";

    cout << "Enter the number of the book you want to return: ";
    int choice;
    cin >> choice;

    if (choice < 0 || choice >= account.current.size())
    {
        cout << "Invalid choice. Return failed.\n";
        return;
    }

    auto it = account.current.begin();
    advance(it, choice);
    Booking *booking = it->second;

    if (booking->type == BookingType::RESERVED)
    {
        Book *book = library.books[booking->bookId];
        auto reservationIt = find(book->reservationQueue.begin(), book->reservationQueue.end(), this->UniqueId);
        if (reservationIt != book->reservationQueue.end())
        {
            book->reservationQueue.erase(reservationIt);
            cout << "Reservation cancelled successfully.\n";
        }
        else
        {
            cout << "Reservation not found. Return failed.\n";
        }
    }
    else if (booking->type == BookingType::DIRECT_BORROW)
    {
        booking->returnDate = date;
        int fine = calculateFine(booking->borrowDate, booking->returnDate);
        if (fine > 0)
        {
            cout << "You have a fine of " << fine << " rupees. Do you want to pay it? (yes/no): ";
            string payChoice;
            cin >> payChoice;

            if (payChoice != "yes")
            {
                cout << "Return failed. Please pay the fine to return the book.\n";
                return;
            }

            booking->fine = fine;
            cout << "Fine paid successfully.\n";
        }

        cout << "Book returned successfully.\n";

        Book *book = library.books[booking->bookId];
        while (!book->reservationQueue.empty())
        {
            string nextUserId = book->reservationQueue.front();
            User *nextUser = nullptr;

            if (library.students.count(nextUserId))
            {
                nextUser = library.students[nextUserId];
            }
            else if (library.faculties.count(nextUserId))
            {
                nextUser = library.faculties[nextUserId];
            }

            if (nextUser)
            {
                // Check if the next user is eligible to borrow the book
                bool isEligible = false;
                if (library.userTypes[nextUserId] == "student")
                {
                    // cout<<"i am here\n";
                    isEligible = static_cast<Student *>(nextUser)->isEligibleToBorrow(date);
                    cout << isEligible << endl;
                }
                else if (library.userTypes[nextUserId] == "faculty")
                {
                    isEligible = static_cast<Faculty *>(nextUser)->isEligibleToBorrow(date);
                }

                if (isEligible)
                {
                    // Find the reservation booking in the next user's current map
                    for (auto &bookingPair : nextUser->account.current)
                    {
                        Booking *reservationBooking = bookingPair.second;
                        if (reservationBooking->type == BookingType::RESERVED && reservationBooking->bookId == book->bookId)
                        {
                            // Update the reservation booking to DIRECT_BORROW
                            reservationBooking->type = BookingType::DIRECT_BORROW;
                            reservationBooking->borrowDate = date;
                            reservationBooking->returnDate = "N/A";
                            reservationBooking->fine = 0;

                            book->status = BookStatus::BORROWED;
                            cout << "Reservation converted to borrow for user: " << nextUserId << endl;
                            book->reservationQueue.erase(book->reservationQueue.begin());
                            account.history[booking->bookingId] = booking;
                            account.current.erase(it);
                            return;
                        }
                    }
                }
                else
                {
                    // Remove ineligible user from the reservation queue and their current map
                    book->reservationQueue.erase(book->reservationQueue.begin());
                    for (auto &bookingPair : nextUser->account.current)
                    {
                        Booking *reservationBooking = bookingPair.second;
                        if (reservationBooking->type == BookingType::RESERVED && reservationBooking->bookId == book->bookId)
                        {
                            nextUser->account.current.erase(bookingPair.first);
                            break;
                        }
                    }
                    cout << "User " << nextUserId << " is ineligible. Removed from the reservation queue.\n";
                }
            }
            else
            {
                // Remove invalid user from the reservation queue
                book->reservationQueue.erase(book->reservationQueue.begin());
                cout << "User " << nextUserId << " not found. Removed from the reservation queue.\n";
            }
        }

        // If no eligible user is found, mark the book as available
        book->status = BookStatus::AVAILABLE;
        cout << "No eligible reservations left. Book is now available.\n";
    }

    account.history[booking->bookingId] = booking;
    account.current.erase(it);
}

void Student::login()
{
    cout << "Student logged in successfully. Welcome, " << name << "!\n";
    string date;
    cout << "Enter today's date (DDMMYYYY): ";
    cin >> date;
    while (true)
    {
        cout << "\nWhat would you like to do?\n";
        cout << "1. Borrow Book\n";
        cout << "2. Return Book\n";
        cout << "3. View Booking History\n";
        cout << "4. View Current Booking Status\n";
        cout << "5. List Books in Library\n";
        cout << "6. Log Out\n";
        cout << "Enter your choice: \n";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            borrowBook(date);
            break;
        }
        case 2:
        {
            returnBook(date);
            break;
        }
        case 3:
            showHistory();
            break;
        case 4:
            current_booking(date);
            break;
        case 5:
            list_books();
            break;
        case 6:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Faculty class functions
Faculty::Faculty(string name, string ID, string password) : User(name, ID, password) {}

bool Faculty::isEligibleToBorrow(string date)
{
    // Check if the faculty has 5 or more books in their current map
    if (account.current.size() >= 5)
    {
        cout << "You have already borrowed or reserved " << account.current.size() << " books. Maximum of 5 books are allowed.\n";
        return false;
    }

    // Check if the faculty has any book borrowed more than 90 days ago
    string currentDate = date; // Replace with actual date logic
    for (auto &bookingPair : account.current)
    {
        Booking *booking = bookingPair.second;
        int daysSinceBorrowed = 0;
        if (booking->type != BookingType::RESERVED)
            daysSinceBorrowed = daysBetweenDates(booking->borrowDate, currentDate);
        if (daysSinceBorrowed > 90)
        {
            cout << "You have a book borrowed more than 90 days ago. Please return it to borrow a new book.\n";
            return false;
        }
    }

    return true;
}

void Faculty::borrowBook(string date)
{
    if (isEligibleToBorrow(date) == false)
        return;
    cout << "Enter the book ID that you want to borrow: ";
    string bookId;
    cin >> bookId;

    if (library.books.count(bookId))
    {
        Book *book = library.books[bookId];

        switch (static_cast<int>(book->status))
        {
        case static_cast<int>(BookStatus::AVAILABLE):
        {
            cout << "The book is available. Do you want to borrow it? (yes/no): ";
            string choice;
            cin >> choice;

            if (choice == "yes")
            {
                string bookingId = generateUniqueId();
                Booking *newBooking = new Booking(
                    bookingId, date, date, "N/A", 0, BookingType::DIRECT_BORROW, book->bookId,
                    book->title, book->author, book->publisher, book->ISBN, book->year);

                account.current[bookingId] = newBooking;
                book->status = BookStatus::BORROWED;

                cout << "Book borrowed successfully! Booking ID: " << bookingId << endl;
            }
            else
            {
                cout << "Borrowing cancelled." << endl;
            }
            break;
        }

        case static_cast<int>(BookStatus::BORROWED):
        {
            cout << "The book is currently borrowed. Do you want to reserve it? (yes/no): ";
            string choice;
            cin >> choice;

            if (choice == "yes")
            {
                book->reservationQueue.push_back(this->UniqueId);
                string bookingId = generateUniqueId();
                Booking *newBooking = new Booking(
                    bookingId, date, "N/A", "N/A", 0, BookingType::RESERVED, book->bookId,
                    book->title, book->author, book->publisher, book->ISBN, book->year);

                account.current[bookingId] = newBooking;
                cout << "Book reserved successfully! Booking ID: " << bookingId << endl;
            }
            else
            {
                cout << "Reservation cancelled." << endl;
            }
            break;
        }

        default:
            cout << "Invalid book status." << endl;
            break;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }
}

void Faculty::returnBook(string date)
{
    if (account.current.empty())
    {
        cout << "There is nothing to return.\n";
        return;
    }

    int num = 0;
    cout << "Books available for return:\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
    cout << "No. | Booking ID | Book Title          | Fine | Type\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
    for (auto &i : account.current)
    {
        Booking *booking = i.second;
        booking->fine = calculateFine(booking->borrowDate, date);
        cout << num << " | "
             << booking->bookingId << " | "
             << booking->title << " | "
             << booking->fine << " | "
             << (booking->type == BookingType::RESERVED ? "Reserved" : "Direct Borrow") << endl;
        num++;
    }
    cout << "--------------------------------------------------------------------------------------------------------\n";

    cout << "Enter the number of the book you want to return: ";
    int choice;
    cin >> choice;

    if (choice < 0 || choice >= account.current.size())
    {
        cout << "Invalid choice. Return failed.\n";
        return;
    }

    auto it = account.current.begin();
    advance(it, choice);
    Booking *booking = it->second;

    if (booking->type == BookingType::RESERVED)
    {
        Book *book = library.books[booking->bookId];
        auto reservationIt = find(book->reservationQueue.begin(), book->reservationQueue.end(), this->UniqueId);
        if (reservationIt != book->reservationQueue.end())
        {
            book->reservationQueue.erase(reservationIt);
            cout << "Reservation cancelled successfully.\n";
        }
        else
        {
            cout << "Reservation not found. Return failed.\n";
        }
    }
    else if (booking->type == BookingType::DIRECT_BORROW)
    {
        booking->returnDate = date;
        cout << "Book returned successfully.\n";

        Book *book = library.books[booking->bookId];
        while (!book->reservationQueue.empty())
        {
            string nextUserId = book->reservationQueue.front();
            User *nextUser = nullptr;

            if (library.students.count(nextUserId))
            {
                nextUser = library.students[nextUserId];
            }
            else if (library.faculties.count(nextUserId))
            {
                nextUser = library.faculties[nextUserId];
            }

            if (nextUser)
            {
                // Check if the next user is eligible to borrow the book
                bool isEligible = false;
                if (library.userTypes[nextUserId] == "student")
                {
                    // cout<<"i am here\n";
                    isEligible = static_cast<Student *>(nextUser)->isEligibleToBorrow(date);
                    cout << isEligible << endl;
                }
                else if (library.userTypes[nextUserId] == "faculty")
                {
                    isEligible = static_cast<Faculty *>(nextUser)->isEligibleToBorrow(date);
                }

                if (isEligible)
                {
                    // Find the reservation booking in the next user's current map
                    for (auto &bookingPair : nextUser->account.current)
                    {
                        Booking *reservationBooking = bookingPair.second;
                        if (reservationBooking->type == BookingType::RESERVED && reservationBooking->bookId == book->bookId)
                        {
                            // Update the reservation booking to DIRECT_BORROW
                            reservationBooking->type = BookingType::DIRECT_BORROW;
                            reservationBooking->borrowDate = date;
                            reservationBooking->returnDate = "N/A";
                            reservationBooking->fine = 0;

                            book->status = BookStatus::BORROWED;
                            cout << "Reservation converted to borrow for user: " << nextUserId << endl;
                            book->reservationQueue.erase(book->reservationQueue.begin());
                            account.history[booking->bookingId] = booking;
                            account.current.erase(it);
                            return;
                        }
                    }
                }
                else
                {
                    // Remove ineligible user from the reservation queue and their current map
                    book->reservationQueue.erase(book->reservationQueue.begin());
                    for (auto &bookingPair : nextUser->account.current)
                    {
                        Booking *reservationBooking = bookingPair.second;
                        if (reservationBooking->type == BookingType::RESERVED && reservationBooking->bookId == book->bookId)
                        {
                            nextUser->account.current.erase(bookingPair.first);
                            break;
                        }
                    }
                    cout << "User " << nextUserId << " is ineligible. Removed from the reservation queue.\n";
                }
            }
            else
            {
                // Remove invalid user from the reservation queue
                book->reservationQueue.erase(book->reservationQueue.begin());
                cout << "User " << nextUserId << " not found. Removed from the reservation queue.\n";
            }
        }

        // If no eligible user is found, mark the book as available
        book->status = BookStatus::AVAILABLE;
        cout << "No eligible reservations left. Book is now available.\n";
    }

    account.history[booking->bookingId] = booking;
    account.current.erase(it);
}

void Faculty::login()
{
    cout << "Faculty logged in successfully. Welcome, " << name << "!\n";
    string date;
    cout << "Enter today's date (DDMMYYYY): ";
    cin >> date;
    while (true)
    {
        cout << "\nWhat would you like to do?\n";
        cout << "1. Borrow Book\n";
        cout << "2. Return Book\n";
        cout << "3. View Booking History\n";
        cout << "4. View Current Booking Status\n";
        cout << "5. List Books in Library\n";
        cout << "6. Log Out\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {

            borrowBook(date);
            break;
        }
        case 2:
        {
            returnBook(date);
            break;
        }
        case 3:
            showHistory();
            break;
        case 4:
            current_booking(date);
            break;
        case 5:
            list_books();
            break;
        case 6:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Librarian class functions
Librarian::Librarian(string name, string ID, string password) : User(name, ID, password) {}

void Librarian::addNewUser()
{
    cout << "Select the user type\n1. Student\n2. Faculty\n";
    int type;
    cin >> type;
    cout << "Enter the name\n";
    string name;
    cin >> name;
    string uniqueId = generateUniqueId();
    cout << "Enter password\n";
    string pass;
    cin >> pass;
    switch (type)
    {
    case 1:
    {
        Student *student = new Student(name, uniqueId, pass);
        library.students[uniqueId] = student;
        library.userTypes[uniqueId] = "student";
        cout << "New student added. His Unique Id is " << uniqueId << "\n";
        break;
    }
    case 2:
    {
        Faculty *faculty = new Faculty(name, uniqueId, pass);
        library.faculties[uniqueId] = faculty;
        library.userTypes[uniqueId] = "faculty";
        cout << "New faculty added. His Unique Id is " << uniqueId << "\n";
        break;
    }
    }
}

void Librarian::addNewBook()
{
    string bookId = generateUniqueId();
    string title, author, publisher, ISBN;
    int year;

    cout << "Enter book title: ";
    cin.ignore();
    getline(cin, title);

    cout << "Enter book author: ";
    getline(cin, author);

    cout << "Enter book publisher: ";
    getline(cin, publisher);

    cout << "Enter book ISBN: ";
    cin >> ISBN;

    cout << "Enter book year: ";
    cin >> year;

    library.books[bookId] = new Book(bookId, title, author, publisher, ISBN, year);
    cout << "Book added successfully. ID: " << bookId << endl;
}

void Librarian::listUsers()
{
    cout << "List of all users:\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
    cout << "Type      | ID    | Name\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";

    for (auto &studentPair : library.students)
    {
        Student *student = studentPair.second;
        cout << "Student   | " << student->UniqueId << " | " << student->name << endl;
    }

    for (auto &facultyPair : library.faculties)
    {
        Faculty *faculty = facultyPair.second;
        cout << "Faculty   | " << faculty->UniqueId << " | " << faculty->name << endl;
    }

    cout << "--------------------------------------------------------------------------------------------------------\n";
}

void Librarian::login()
{
    cout << "Librarian logged in successfully. Welcome, " << name << "!\n";

    while (true)
    {
        cout << "\nWhat would you like to do?\n";
        cout << "1. Add New User\n";
        cout << "2. Add New Book\n";
        cout << "3. List Users\n";
        cout << "4. List Books in Library\n";
        cout << "5. Delete User\n";
        cout << "6. Delete Book\n";
        cout << "7. Log Out\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            addNewUser();
            break;
        case 2:
            addNewBook();
            break;
        case 3:
            listUsers();
            break;
        case 4:
            list_books();
            break;
        case 5:
            deleteUser();
            break;
        case 6:
            deleteBook();
            break;
        case 7:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

void Librarian::deleteUser()
{
    cout << "Enter the ID of the user to delete: ";
    string userId;
    cin >> userId;

    // Check if the user is a student
    if (library.students.count(userId))
    {
        Student *student = library.students[userId];
        if (student->account.current.empty())
        {
            library.students.erase(userId);
            library.userTypes.erase(userId);
            cout << "Student with ID " << userId << " deleted successfully.\n";
        }
        else
        {
            cout << "Cannot delete student. They have active bookings.\n";
        }
    }
    // Check if the user is a faculty
    else if (library.faculties.count(userId))
    {
        Faculty *faculty = library.faculties[userId];
        if (faculty->account.current.empty())
        {
            library.faculties.erase(userId);
            library.userTypes.erase(userId);
            cout << "Faculty with ID " << userId << " deleted successfully.\n";
        }
        else
        {
            cout << "Cannot delete faculty. They have active bookings.\n";
        }
    }
    // Check if the user is a librarian
    else if (library.librarians.count(userId))
    {
        cout << "Librarians cannot be deleted.\n";
    }
    // User not found
    else
    {
        cout << "User with ID " << userId << " not found.\n";
    }
}

// Function to delete a book
void Librarian::deleteBook()
{
    cout << "Enter the ID of the book to delete: ";
    string bookId;
    cin >> bookId;

    if (library.books.count(bookId))
    {
        Book *book = library.books[bookId];
        if (book->status == BookStatus::AVAILABLE && book->reservationQueue.empty())
        {
            library.books.erase(bookId);
            cout << "Book with ID " << bookId << " deleted successfully.\n";
        }
        else
        {
            cout << "Cannot delete book. It is either borrowed or has reservations.\n";
        }
    }
    else
    {
        cout << "Book with ID " << bookId << " not found.\n";
    }
}

// Login function
void login()
{
    string userType, ID, password;

    cout << "Enter user type (student/faculty/librarian): ";
    cin >> userType;

    cout << "Enter your ID: ";
    cin >> ID;
    cout << "Enter your password: ";
    cin >> password;

    if (userType == "student")
    {
        if (library.students.count(ID) && library.students[ID]->authenticate(password))
        {
            library.students[ID]->login();
        }
        else
        {
            cout << "Invalid ID or password for student.\n";
        }
    }
    else if (userType == "faculty")
    {
        if (library.faculties.count(ID) && library.faculties[ID]->authenticate(password))
        {
            library.faculties[ID]->login();
        }
        else
        {
            cout << "Invalid ID or password for faculty.\n";
        }
    }
    else if (userType == "librarian")
    {
        if (library.librarians.count(ID) && library.librarians[ID]->authenticate(password))
        {
            library.librarians[ID]->login();
        }
        else
        {
            cout << "Invalid ID or password for librarian.\n";
        }
    }
    else
    {
        cout << "Invalid user type. Use 'student', 'faculty', or 'librarian'.\n";
    }
}

void saveToCSV()
{
    ofstream file("library_data.csv");

    // Save books
    file << "Books\n";
    file << "BookID,Title,Author,Publisher,ISBN,Year,Status,ReservationQueue\n";
    for (const auto &bookPair : library.books)
    {
        Book *book = bookPair.second;
        file << book->bookId << ","
             << book->title << ","
             << book->author << ","
             << book->publisher << ","
             << book->ISBN << ","
             << book->year << ","
             << (book->status == BookStatus::AVAILABLE ? "Available" : "Borrowed") << ",";
        for (const string &userId : book->reservationQueue)
        {
            file << userId << ";";
        }
        file << "\n";
    }

    // Save users (using getPassword() to access protected password)
    file << "\nUsers\n";
    file << "UserID,Name,Password,UserType\n";
    for (const auto &studentPair : library.students)
    {
        Student *student = studentPair.second;
        file << student->UniqueId << ","
             << student->name << ","
             << student->getPassword() << "," // Use getPassword()
             << "student\n";
    }
    for (const auto &facultyPair : library.faculties)
    {
        Faculty *faculty = facultyPair.second;
        file << faculty->UniqueId << ","
             << faculty->name << ","
             << faculty->getPassword() << "," // Use getPassword()
             << "faculty\n";
    }
    for (const auto &librarianPair : library.librarians)
    {
        Librarian *librarian = librarianPair.second;
        file << librarian->UniqueId << ","
             << librarian->name << ","
             << librarian->getPassword() << "," // Use getPassword()
             << "librarian\n";
    }

    // Save current bookings
    file << "\nCurrentBookings\n";
    file << "BookingID,UserID,BookID,BookingDate,BorrowDate,ReturnDate,Fine,Type\n";
    for (const auto &studentPair : library.students)
    {
        Student *student = studentPair.second;
        for (const auto &bookingPair : student->account.current)
        {
            Booking *booking = bookingPair.second;
            file << booking->bookingId << ","
                 << student->UniqueId << ","
                 << booking->bookId << ","
                 << booking->bookingDate << ","
                 << booking->borrowDate << ","
                 << booking->returnDate << ","
                 << booking->fine << ","
                 << (booking->type == BookingType::RESERVED ? "Reserved" : "DirectBorrow") << "\n";
        }
    }
    for (const auto &facultyPair : library.faculties)
    {
        Faculty *faculty = facultyPair.second;
        for (const auto &bookingPair : faculty->account.current)
        {
            Booking *booking = bookingPair.second;
            file << booking->bookingId << ","
                 << faculty->UniqueId << ","
                 << booking->bookId << ","
                 << booking->bookingDate << ","
                 << booking->borrowDate << ","
                 << booking->returnDate << ","
                 << booking->fine << ","
                 << (booking->type == BookingType::RESERVED ? "Reserved" : "DirectBorrow") << "\n";
        }
    }

    // Save history bookings
    file << "\nHistoryBookings\n";
    file << "BookingID,UserID,BookID,BookingDate,BorrowDate,ReturnDate,Fine,Type\n";
    for (const auto &studentPair : library.students)
    {
        Student *student = studentPair.second;
        for (const auto &bookingPair : student->account.history)
        {
            Booking *booking = bookingPair.second;
            file << booking->bookingId << ","
                 << student->UniqueId << ","
                 << booking->bookId << ","
                 << booking->bookingDate << ","
                 << booking->borrowDate << ","
                 << booking->returnDate << ","
                 << booking->fine << ","
                 << (booking->type == BookingType::RESERVED ? "Reserved" : "DirectBorrow") << "\n";
        }
    }
    for (const auto &facultyPair : library.faculties)
    {
        Faculty *faculty = facultyPair.second;
        for (const auto &bookingPair : faculty->account.history)
        {
            Booking *booking = bookingPair.second;
            file << booking->bookingId << ","
                 << faculty->UniqueId << ","
                 << booking->bookId << ","
                 << booking->bookingDate << ","
                 << booking->borrowDate << ","
                 << booking->returnDate << ","
                 << booking->fine << ","
                 << (booking->type == BookingType::RESERVED ? "Reserved" : "DirectBorrow") << "\n";
        }
    }

    file.close();
    cout << "Library data saved to library_data.csv\n";
}

void loadFromCSV()
{
    ifstream file("library_data.csv");
    if (!file.is_open())
    {
        cerr << "Error: Could not open library_data.csv\n";
        return;
    }

    string line;
    string section = "";

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // Check for section headers
        if (line == "Books")
        {
            section = "Books";
            getline(file, line); // Skip header
            continue;
        }
        else if (line == "Users")
        {
            section = "Users";
            getline(file, line); // Skip header
            continue;
        }
        else if (line == "CurrentBookings")
        {
            section = "CurrentBookings";
            getline(file, line); // Skip header
            continue;
        }
        else if (line == "HistoryBookings")
        {
            section = "HistoryBookings";
            getline(file, line); // Skip header
            continue;
        }

        // Process data based on the current section
        if (section == "Books")
        {
            stringstream ss(line);
            string bookId, title, author, publisher, ISBN, yearStr, status, reservationQueue;
            getline(ss, bookId, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, publisher, ',');
            getline(ss, ISBN, ',');
            getline(ss, yearStr, ',');
            getline(ss, status, ',');
            getline(ss, reservationQueue); // Read the rest of the line as reservationQueue

            // Validate year (ensure it's a valid integer)
            int year = 0;
            try
            {
                year = stoi(yearStr);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Invalid year for book " << bookId << ": " << yearStr << "\n";
                continue; // Skip this book
            }
            catch (const out_of_range &e)
            {
                cerr << "Year out of range for book " << bookId << ": " << yearStr << "\n";
                continue; // Skip this book
            }

            // Create the book
            Book *book = new Book(bookId, title, author, publisher, ISBN, year);
            book->status = (status == "Available") ? BookStatus::AVAILABLE : BookStatus::BORROWED;

            // Parse reservation queue (handle empty or invalid queues)
            if (!reservationQueue.empty())
            {
                stringstream queueStream(reservationQueue);
                string userId;
                while (getline(queueStream, userId, ';'))
                {
                    if (!userId.empty())
                    {
                        book->reservationQueue.push_back(userId);
                    }
                }
            }

            library.books[bookId] = book;
        }
        else if (section == "Users")
        {
            stringstream ss(line);
            string userId, name, password, userType;
            getline(ss, userId, ',');
            getline(ss, name, ',');
            getline(ss, password, ',');
            getline(ss, userType, ',');

            if (userType == "student")
            {
                Student *student = new Student(name, userId, "");
                student->setPassword(password); // Use setPassword()
                library.students[userId] = student;
                library.userTypes[userId] = "student";
            }
            else if (userType == "faculty")
            {
                Faculty *faculty = new Faculty(name, userId, "");
                faculty->setPassword(password); // Use setPassword()
                library.faculties[userId] = faculty;
                library.userTypes[userId] = "faculty";
            }
            else if (userType == "librarian")
            {
                Librarian *librarian = new Librarian(name, userId, "");
                librarian->setPassword(password); // Use setPassword()
                library.librarians[userId] = librarian;
                library.userTypes[userId] = "librarian";
            }
        }
        else if (section == "CurrentBookings" || section == "HistoryBookings")
        {
            stringstream ss(line);
            string bookingId, userId, bookId, bookingDate, borrowDate, returnDate, fineStr, typeStr;
            getline(ss, bookingId, ',');
            getline(ss, userId, ',');
            getline(ss, bookId, ',');
            getline(ss, bookingDate, ',');
            getline(ss, borrowDate, ',');
            getline(ss, returnDate, ',');
            getline(ss, fineStr, ',');
            getline(ss, typeStr, ',');

            // Validate fine (ensure it's a valid integer)
            int fine = 0;
            try
            {
                fine = stoi(fineStr);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Invalid fine for booking " << bookingId << ": " << fineStr << "\n";
                continue; // Skip this booking
            }
            catch (const out_of_range &e)
            {
                cerr << "Fine out of range for booking " << bookingId << ": " << fineStr << "\n";
                continue; // Skip this booking
            }

            // Determine booking type
            BookingType type = (typeStr == "Reserved") ? BookingType::RESERVED : BookingType::DIRECT_BORROW;

            // Create the booking
            Booking *booking = new Booking(
                bookingId, bookingDate, borrowDate, returnDate, fine, type, bookId,
                library.books[bookId]->title, library.books[bookId]->author,
                library.books[bookId]->publisher, library.books[bookId]->ISBN, library.books[bookId]->year);

            // Add booking to the appropriate user's account
            if (section == "CurrentBookings")
            {
                if (library.userTypes[userId] == "student")
                {
                    library.students[userId]->account.current[bookingId] = booking;
                }
                else if (library.userTypes[userId] == "faculty")
                {
                    library.faculties[userId]->account.current[bookingId] = booking;
                }
            }
            else if (section == "HistoryBookings")
            {
                if (library.userTypes[userId] == "student")
                {
                    library.students[userId]->account.history[bookingId] = booking;
                }
                else if (library.userTypes[userId] == "faculty")
                {
                    library.faculties[userId]->account.history[bookingId] = booking;
                }
            }
        }
    }

    file.close();
    cout << "Library data loaded from library_data.csv\n";
}
// Main function
int main()
{
    // Load data from CSV
    loadFromCSV();

    // Main program logic
    bool f = true;
    while (f)
    {
        cout << "WELCOME TO THE LIBRARY MANAGEMENT SYSTEM" << endl;
        cout << "----------------------------------------" << endl;
        login();
        cout << "Do you want to re-login (Y/N)" << endl;
        char choice;
        cin >> choice;
        if (choice != 'Y' && choice != 'y')
            f = false;
    }

    // Save data to CSV before exiting
    saveToCSV();
    cout << "System Closed" << endl;

    return 0;
}