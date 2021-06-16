
#include <iostream>
#include <string>
#include <cctype>
#include <ctime>
#include <iomanip>
#include<cstdlib>

#define perc_of_prof 20.0
#define profit 0.0
using namespace std;

class DayOfYear {
private:
    unsigned int dayOfYear, year;
    static unsigned const int days[13]; //to be called outside the class
public:
    explicit DayOfYear(unsigned int day = 1, unsigned int year = 365) : year(year) {
        if (day > 365) { dayOfYear = day - ((day / 365) * 365); }
        else dayOfYear = day;
    }

    DayOfYear(unsigned int day, unsigned int month, unsigned int year) :
            dayOfYear(day + days[month - 1]), year(year) {}

    unsigned int get_dayOfYear() const { return dayOfYear; }//there will be no change so i used constant
    DayOfYear &operator++() {
        DayOfYear static temp;
        temp.year = year;
        temp.dayOfYear = dayOfYear;
        if (dayOfYear == 365) {
            ++temp.year;
            temp.dayOfYear = 1;
        } else ++temp.dayOfYear;
        return temp;
    }

    friend istream &operator>>(istream &is, DayOfYear &doy) {
        unsigned int month, day;
        is >> doy.year;
        is.ignore();
        is >> month;
        is.ignore();
        is >> day;
        doy.dayOfYear = day + days[month - 1];
        return is;
    }

    friend ostream &operator<<(ostream &os, const DayOfYear &doy) {
        int start = 0;
        int answer = -1; //any value for initialisation
        int end = 12;
        unsigned int _dayOfYear = doy.dayOfYear;
        unsigned int day, month = 0;
        //there are three cases : first day in year or the last day or in between
        if (_dayOfYear == 1) {
            os << doy.year << "-" << "01" << "-" << "01";
        } else if (_dayOfYear == 365) {
            os << doy.year << "-" << "12" << "-" << "31";
        }
            //Binary search
        else {
            while (start <= end) {
                int middle = (start + end) / 2;
                if (days[middle] < _dayOfYear) { start = middle + 1; }
                else {
                    answer = middle;
                    end = middle - 1;
                }
            }
            month = answer;
            day = _dayOfYear - days[month - 1];
            os << doy.year << "-" << (month > 9 ? to_string(month) : to_string(0) + to_string(month)) << "-"
               << (day > 9 ? to_string(day) : to_string(0) + to_string(day));
        }
        return os;

    }

};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class Vehicle {
private:
    const int no;
    string model;
    float price24h;
public:
    Vehicle(int x, string y, float z) : no(x), model(y), price24h(z) {}

    virtual ~Vehicle() = default; //destructor
    int get_no() const {
        return no;
    }

    const string &get_model() const {
        return model;
    }

    float get_price(const int rentaldays) const {
        return float(rentaldays) * price24h;
    }

    virtual void print() = 0;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class Bike : public Vehicle {
public:
    Bike(int A, string B, float c = 9.99) : Vehicle(A, B, c) {}

    virtual void print() override { cout << get_no() << ": " << get_model() << " (Bike)" << endl; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------

class EmotorVehicle : public Vehicle {
public:
    EmotorVehicle(const int no, string model, float price24h) : Vehicle(no, model, price24h) {}

    virtual bool is_streetlegal() = 0;

    void print() {
        string message;
        message += to_string(get_no()) + ": " + get_model();
        if (!is_streetlegal()) {
            message += " (not street legal)";
        }
        cout << message;
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class Ebike : public EmotorVehicle {
public:
    Ebike(const int no, string model, float price24h = 29.99) : EmotorVehicle(no, model, price24h) {}

    bool is_streetlegal() override {
        return true;
    }

    void print() override {
        EmotorVehicle::print();
        cout << " (EBike)\n";
    }

};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class EScooter : public EmotorVehicle {
private:
    bool streetlegal;
public:
    EScooter(const int no, string model, float price24h = 19.99, bool streetlegal = false) : EmotorVehicle(no, model,
                                                                                                           price24h),
                                                                                             streetlegal(streetlegal) {}

    bool is_streetlegal() override {
        return streetlegal;
    }

    void print() override {
        EmotorVehicle::print();
        cout << " (Escooter)\n";
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class Rental {
private:
    const int no;
    DayOfYear from;
    int days;
    string customer;
    static int last_no;
public:
    Rental(const string &person, const DayOfYear SDR, int days = 1) : customer(person), from(SDR), days(days),
                                                                      no(last_no) { ++last_no; }

    int get_days() {
        return days;
    }

    DayOfYear get_from() {
        return from;
    }

    DayOfYear get_until() {
        DayOfYear until = from;
        for (int i = 0; i <= days; i++) { until = ++until; }
        return until;
    }

    void print() {
        cout << from << " to " << get_until() << ", rental no. " << no << " for " << customer;
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class schedule {
private:
    Vehicle *vehicle;
    Rental *year[365];
    schedule *Next;
public:
    explicit schedule(Vehicle *_vehicle) : vehicle(_vehicle), year{nullptr}, Next(nullptr) {}

    Vehicle *get_vehicle() {
        return vehicle;
    }

    schedule *get_next() const {
        return Next;
    }

    void set_next(schedule *_next) {
        Next = _next;
    }

    bool isfree(DayOfYear x, int y) {
        unsigned int st_check_date = x.get_dayOfYear();
        unsigned int en_check_date = st_check_date + y;
        unsigned int freedays = 0;
        unsigned int reservation_period = y + 1;
        for (unsigned int Begin = st_check_date - 1; Begin < en_check_date; ++Begin) {
            if (year[Begin] == nullptr)
                ++freedays;
        }
        if (freedays == reservation_period) { return true; }
        else return false;
    }

    float book(string &person, DayOfYear &x, int y) {
        unsigned int begin = x.get_dayOfYear();
        unsigned int fin = begin + y;
        auto rental = new Rental(person, x, y);
        for (unsigned int i = begin - 1; i < fin; ++i) { year[i] = rental; }
        return vehicle->get_price(y);
    }

    void print(DayOfYear &_doy) {
        unsigned int index = _doy.get_dayOfYear() - 1;
        get_vehicle()->print();
        if (year[index] != nullptr) {
            year[index]->print();
        }
    }

    void print() {
        cout << "SCHEDULE FOR ";
        vehicle->print();
        for (unsigned int i = 1; i < 365; ++i) {
            if (year[i] != nullptr && year[i] != year[i+1]) {
                year[i]->print();
                cout << " ,total: " << get_vehicle()->get_price(year[i]->get_days()) << " EUR \n";
            }
        }
    }

};

//------------------------------------------------------------------------------------------------------------------------------------------------------
class Agency {
private:
    float profitPercent;
    float _profit;
    schedule *head, *last;
public:
    Agency() : profitPercent(perc_of_prof), _profit(profit), head(nullptr), last(nullptr) {}

    float get_profit() const { return _profit; }

    void set_profitPercent(float prof) { profitPercent = prof; }

    void add(Vehicle *A) {
        auto sch = new schedule(A);
        if (head == nullptr) { head = sch; }
        else if (last == nullptr) {
            head->set_next(sch);
            last = sch;
        } else {
            last->set_next(sch);
            last = sch;
        }
    }

    //vn : vehicle number
    bool isFree(int vn, DayOfYear d, int Nodays) {
        bool flag = false;
        auto schedule = searchfor(vn);
        if (schedule != nullptr) { flag = schedule->isfree(d, Nodays); }
        return flag;
    }

    float book(int vn, string &customer, DayOfYear &d, int days) {
        auto schedule = searchfor(vn);
        float price = 0;
        if (isFree(vn, d, days)) { price = schedule->book(customer, d, days); }
        _profit += (price * (profitPercent / 100));
        return price;

    }

    int choosevehicle() {
        string name;
        int vehicle_no, howlong;
        DayOfYear reservation_date;
        for (auto p = head; p != nullptr; p = p->get_next()) { p->get_vehicle()->print(); }
        cout << "choose vehicle no: ";
        cin >> vehicle_no;
        cout << "day of retnal: ";
        cin >> reservation_date;
        cout << "number of days: ";
        cin >> howlong;
        if (isFree(vehicle_no, reservation_date, howlong)) {
            cout << " input data vehicle renter name: \n";
            cin >> name;
            float price = book(vehicle_no, name, reservation_date, howlong);
            cout << "booked, price for this rental: " << setiosflags(ios::fixed) << setprecision(2) << price
                 << "EUR \n";
        } else { cout << "sorry vehicle already booked \n"; }
        return 0;
    }

    void print(DayOfYear &dayofyear) {
        for (auto p = head; p != nullptr; p = p->get_next()) {
            p->print(dayofyear);
            cout << endl;
        }
    }

    void print() {
        for (auto p = head; p != nullptr; p = p->get_next()) { p->print(); }
        cout << " profit: " << setiosflags(ios::fixed) << setprecision(2) << get_profit() << " EUR \n";
    }

protected:
    schedule *searchfor(int z) {
        for (auto p = head; p != nullptr; p = p->get_next()) {
            if (z == p->get_vehicle()->get_no()) { return p; }
        }
        return nullptr;
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
const unsigned int DayOfYear::days[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

int Rental::last_no = 1; //scoope
//------------------------------------------------------------------------------------------------------------------------------------------------------
int gen_random(int min, int max) {
    return min + (rand() % (max - min + 1));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
int main() {
    char choice;
    Agency agency;
    float profit_percent;
    unsigned int day, month, year;
    int n_bike, n_ebike, n_escooter, howmany, counter;
    counter = 1;
    n_bike = n_ebike = n_escooter = howmany = 0;
    time_t currentime = time(nullptr);
    tm *_1tm = localtime(&currentime);
    day = _1tm->tm_mday;
    month = 1 + _1tm->tm_mon;
    year = 1900 + _1tm->tm_year;
    DayOfYear today(day, month, year);
    cout << "SIMPLIFIED SIMULATION OF A RENTAL AGENCY\n";
    cout << "=================================================================\n";
    do {
        cout << "MENU\n";
        cout << "A end of simulation\n";
        cout << "B set new simulation date\n";
        cout << "C new rental manually\n";
        cout << "D print rental of today " << today << "\n";
        cout << "E print rental of tomorrow " << ++today << "\n";
        cout << "F print all rentals\n";
        cout << "G print agency profit\n";
        cout << "H set agency profit percent\n";
        cout << "I add bikes\n";
        cout << "J add E-bikes\n";
        cout << "K add E-scooters\n";
        cout << "L new rental simulation\n";
        cout << "choice: ";
        cin >> choice;
        switch (toupper(choice)) {
            case 'A':
                exit(0);
            case 'B':
                cout << "input date of today: ";
                cin >> today;
                break;
            case 'C':
                agency.choosevehicle();
                break;
            case 'D':
                cout << "SCHEDULE FOR " << today << "\n";
                agency.print(today);
                break;
            case 'E':
                cout << "SCHEDULE FOR " << ++today << "\n";
                agency.print(today);
                break;
            case 'F':
                agency.print();
                break;
            case 'G':
                cout << "agency profit: " << setiosflags(ios::fixed) << setprecision(2) << agency.get_profit()
                     << " EUR\n";
                break;
            case 'H':
                cout << "agency profit percent: ";
                cin >> profit_percent;
                agency.set_profitPercent(profit_percent);
                break;
            case 'I':
                cout << "how many bikes in this simulation? ";
                cin >> howmany;
                int i;
                for (i = n_bike; i < howmany + n_bike; ++i) {
                    auto bike = new Bike(counter, "city" + to_string(i + 1));
                    agency.add(bike);
                    ++counter;
                }
                n_bike = i;
                break;
            case 'J':
                cout << "how many EBikes in this simulation? ";
                cin >> howmany;
                int j;
                for (j = n_ebike; j < howmany + n_ebike; ++j) {
                    auto e_bike = new Ebike(counter, "Trekky" + to_string(j + 1));
                    agency.add(e_bike);
                    ++counter;
                }
                n_ebike = j;
                break;
            case 'K':
                cout << "how many EScooters in this simulation? ";
                cin >> howmany;
                int k;
                for (k = n_escooter; k < howmany + n_escooter; ++k) {
                    auto e_scooter = new EScooter(counter, "Scooty" + to_string(k + 1));
                    agency.add(e_scooter);
                    ++counter;
                }
                n_escooter = k;
                break;
            case 'L':
                cout << "how many reservation to simulate? ";
                cin >> howmany;
                while (howmany--) {
                    int d;                                              // random day of year
                    int numOfDays;                                      // number of days to reserve
                    int vehicleId;                                      // vehicle Id
                    const string renter = "customer";
                    int customerNo = 1;

                    // vehicle Id should be between 1 & number of agency's vehicles
                    vehicleId = gen_random(1, counter - 1);

                    // day of year value should be between 1 & 365
                    d = gen_random(1, 365);

                    DayOfYear reservation_start_date(d, 2021);

                    // reservation period should be between 1 & 7
                    numOfDays = gen_random(1, 7);

                    // customer number
                    customerNo = gen_random(1, 365);

                    string name = renter + to_string(customerNo);

                    cout << "rental vehicle " << vehicleId << " from ";
                    cout << reservation_start_date << " for " << numOfDays;
                    cout << (numOfDays > 1 ? " days" : " day");

                    if (agency.book(vehicleId, name, reservation_start_date, numOfDays) == 0.0) {
                        // no reservation
                        cout << " sorry vehicle already booked";
                    }

                    cout << endl;
                }
                break;
            default:
                exit(0);
        }
    } while (choice);
    return 0;
}