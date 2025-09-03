#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

namespace {
    constexpr int kRows = 8;
    constexpr int kCols = 4;
    constexpr int kSeatsPerBus = kRows * kCols; // 32
    constexpr size_t kMaxBuses = 10;

    bool readIntFromLine(const string& line, int& out) {
        try {
            size_t pos = 0;
            int v = stoi(line, &pos);
            if (pos != line.size()) return false;
            out = v;
            return true;
        } catch (...) {
            return false;
        }
    }

    void vline(char ch) {
        for (int i = 0; i < 80; ++i) cout << ch;
        cout << '\n';
    }

    string toLower(string s) {
        for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        return s;
    }
}

struct Bus {
    string busn;
    string driver;
    string arrival;
    string depart;
    string from;
    string to;
    string seat[kRows][kCols]; // "Empty" or passenger name

    Bus() {
        for (int i = 0; i < kRows; ++i)
            for (int j = 0; j < kCols; ++j)
                seat[i][j] = "Empty";
    }
};

class BusManager {
public:
    void menuLoop();

    // existing
    void install();
    void allotment();
    void show();
    void avail();

    // new features
    void cancelReservation();
    void moveReservation();
    void findPassenger();
    void showFreeSeats();
    void listByRoute();

private:
    vector<Bus> buses;

    // helpers
    int findBusIndexByNumber(const string& number) const {
        for (int i = 0; i < static_cast<int>(buses.size()); ++i) {
            if (buses[i].busn == number) return i;
        }
        return -1;
    }

    static bool seatNumberToIndex(int seatNo, int& row, int& col) {
        if (seatNo < 1 || seatNo > kSeatsPerBus) return false;
        int idx = seatNo - 1;
        row = idx / kCols;
        col = idx % kCols;
        return true;
    }

    static int indexToSeatNumber(int row, int col) {
        return row * kCols + col + 1;
    }

    static void printSeatMapAndFreeCount(const Bus& b) {
        int s = 0;
        int freeCount = 0;
        for (int i = 0; i < kRows; ++i) {
            cout << '\n';
            for (int j = 0; j < kCols; ++j) {
                ++s;
                cout << setw(4) << s << ". " << setw(12) << left << b.seat[i][j];
            }
            cout << right;
        }
        for (int i = 0; i < kRows; ++i)
            for (int j = 0; j < kCols; ++j)
                if (b.seat[i][j] == "Empty") ++freeCount;

        cout << "\n\nThere are " << freeCount
             << " seats empty in Bus No: " << b.busn << "\n";
    }

    static string promptLine(const string& prompt) {
        cout << prompt;
        string line;
        getline(cin, line);
        return line;
    }

    static int promptInt(const string& prompt, int minV, int maxV) {
        while (true) {
            string s = promptLine(prompt);
            int val;
            if (readIntFromLine(s, val) && val >= minV && val <= maxV) return val;
            cout << "Please enter a number between " << minV << " and " << maxV << ".\n";
        }
    }

    int requireBusIndexByNumber() const {
        string number = promptLine("Bus no: ");
        int idx = findBusIndexByNumber(number);
        if (idx < 0) cout << "No bus found with number: " << number << "\n";
        return idx;
    }
};

void BusManager::install() {
    if (buses.size() >= kMaxBuses) {
        cout << "Cannot install more than " << kMaxBuses << " buses.\n";
        return;
    }

    Bus b;
    b.busn    = promptLine("Enter bus no: ");
    if (b.busn.empty()) { cout << "Bus no cannot be empty.\n"; return; }
    if (findBusIndexByNumber(b.busn) >= 0) { cout << "Bus no already exists.\n"; return; }

    b.driver  = promptLine("Enter Driver's name: ");
    b.arrival = promptLine("Arrival time (e.g., 10:00): ");
    b.depart  = promptLine("Departure time (e.g., 14:00): ");
    b.from    = promptLine("From: ");
    b.to      = promptLine("To: ");

    buses.push_back(move(b));
    cout << "Bus installed successfully.\n";
}

void BusManager::allotment() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    int idx = requireBusIndexByNumber();
    if (idx < 0) return;

    int seatNo = promptInt("Seat Number (1-32): ", 1, kSeatsPerBus);

    int row = 0, col = 0;
    seatNumberToIndex(seatNo, row, col);

    Bus& b = buses[idx];
    if (b.seat[row][col] != "Empty") {
        cout << "Seat " << seatNo << " is already reserved for " << b.seat[row][col] << ".\n";
        return;
    }

    string pname = promptLine("Enter passenger's name: ");
    if (pname.empty()) { cout << "Passenger name cannot be empty.\n"; return; }

    b.seat[row][col] = pname;
    cout << "Seat " << seatNo << " reserved successfully for " << pname << ".\n";
}

void BusManager::show() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    int idx = requireBusIndexByNumber();
    if (idx < 0) return;

    const Bus& b = buses[idx];
    vline('*');
    cout << "Bus no:\t\t" << b.busn
         << "\nDriver:\t\t" << b.driver
         << "\t\tArrival time:\t" << b.arrival
         << "\tDeparture time:\t" << b.depart
         << "\nFrom:\t\t" << b.from
         << "\t\tTo:\t\t" << b.to << "\n";
    vline('*');

    printSeatMapAndFreeCount(b);

    int s = 0;
    for (int i = 0; i < kRows; ++i) {
        for (int j = 0; j < kCols; ++j) {
            ++s;
            if (b.seat[i][j] != "Empty") {
                cout << "Seat " << s << " -> " << b.seat[i][j] << '\n';
            }
        }
    }
}

void BusManager::avail() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    for (const auto& b : buses) {
        vline('*');
        cout << "Bus no:\t\t" << b.busn
             << "\nDriver:\t\t" << b.driver
             << "\t\tArrival time:\t" << b.arrival
             << "\tDeparture Time:\t" << b.depart
             << "\nFrom:\t\t" << b.from
             << "\t\tTo:\t\t" << b.to << "\n";
        vline('*');
    }
}

// ========== New Features ==========

void BusManager::cancelReservation() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    int idx = requireBusIndexByNumber();
    if (idx < 0) return;

    int seatNo = promptInt("Seat Number to cancel (1-32): ", 1, kSeatsPerBus);
    int r,c; seatNumberToIndex(seatNo, r, c);

    Bus& b = buses[idx];
    if (b.seat[r][c] == "Empty") {
        cout << "Seat " << seatNo << " is already empty.\n"; return;
    }
    cout << "Cancel reservation of " << b.seat[r][c] << " (Seat " << seatNo << ")? [y/N]: ";
    string ans; getline(cin, ans);
    if (!ans.empty() && (ans[0]=='y' || ans[0]=='Y')) {
        b.seat[r][c] = "Empty";
        cout << "Reservation cancelled.\n";
    } else {
        cout << "Cancelled by user.\n";
    }
}

void BusManager::moveReservation() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    int idx = requireBusIndexByNumber();
    if (idx < 0) return;

    int fromSeat = promptInt("Current Seat Number (1-32): ", 1, kSeatsPerBus);
    int fr, fc; seatNumberToIndex(fromSeat, fr, fc);

    Bus& b = buses[idx];
    if (b.seat[fr][fc] == "Empty") {
        cout << "Seat " << fromSeat << " is empty. Nothing to move.\n"; return;
    }

    int toSeat = promptInt("New Seat Number (1-32): ", 1, kSeatsPerBus);
    int tr, tc; seatNumberToIndex(toSeat, tr, tc);

    if (b.seat[tr][tc] != "Empty") {
        cout << "Target seat " << toSeat << " is already reserved for " << b.seat[tr][tc] << ".\n";
        return;
    }

    swap(b.seat[tr][tc], b.seat[fr][fc]);
    cout << "Moved to seat " << toSeat << " successfully.\n";
}

void BusManager::findPassenger() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    string key = promptLine("Passenger name (partial match ok): ");
    if (key.empty()) { cout << "Name cannot be empty.\n"; return; }
    string keyL = toLower(key);

    bool found = false;
    for (const auto& b : buses) {
        for (int i = 0; i < kRows; ++i) {
            for (int j = 0; j < kCols; ++j) {
                if (b.seat[i][j] != "Empty") {
                    string nameL = toLower(b.seat[i][j]);
                    if (nameL.find(keyL) != string::npos) {
                        cout << "Found: " << b.seat[i][j]
                             << " on Bus " << b.busn
                             << " seat " << indexToSeatNumber(i,j)
                             << " (" << b.from << " -> " << b.to << ")\n";
                        found = true;
                    }
                }
            }
        }
    }
    if (!found) cout << "No passenger matched \"" << key << "\".\n";
}

void BusManager::showFreeSeats() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    int idx = requireBusIndexByNumber();
    if (idx < 0) return;

    const Bus& b = buses[idx];
    vector<int> freeSeats;
    for (int i = 0; i < kRows; ++i)
        for (int j = 0; j < kCols; ++j)
            if (b.seat[i][j] == "Empty")
                freeSeats.push_back(indexToSeatNumber(i,j));

    if (freeSeats.empty()) {
        cout << "No free seats on Bus " << b.busn << ".\n";
        return;
    }

    cout << "Free seats on Bus " << b.busn << ": ";
    for (size_t i = 0; i < freeSeats.size(); ++i) {
        if (i) cout << ", ";
        cout << freeSeats[i];
    }
    cout << "\n";
}

void BusManager::listByRoute() {
    if (buses.empty()) { cout << "No buses installed yet.\n"; return; }

    string fromQ = promptLine("Filter From (leave empty for any): ");
    string toQ   = promptLine("Filter To   (leave empty for any): ");

    string fromL = toLower(fromQ);
    string toL   = toLower(toQ);

    bool any = false;
    for (const auto& b : buses) {
        bool fromOk = fromQ.empty() || toLower(b.from).find(fromL) != string::npos;
        bool toOk   = toQ.empty()   || toLower(b.to).find(toL)     != string::npos;
        if (fromOk && toOk) {
            any = true;
            vline('*');
            cout << "Bus no:\t\t" << b.busn
                 << "\nDriver:\t\t" << b.driver
                 << "\t\tArrival time:\t" << b.arrival
                 << "\tDeparture Time:\t" << b.depart
                 << "\nFrom:\t\t" << b.from
                 << "\t\tTo:\t\t" << b.to << "\n";
            vline('*');
        }
    }
    if (!any) cout << "No buses matched your route filter.\n";
}

// ========== Menu Loop ==========
void BusManager::menuLoop() {
    while (true) {
        cout << "\n";
        vline('=');
        cout << "BUS RESERVATION SYSTEM\n";
        vline('=');
        cout << "1. Install Bus\n"
             << "2. Reserve Seat\n"
             << "3. Show Bus Details\n"
             << "4. Buses Available (All)\n"
             << "5. Cancel Reservation\n"
             << "6. Move Reservation\n"
             << "7. Find Passenger\n"
             << "8. Show Free Seats (Bus)\n"
             << "9. List Buses by Route\n"
             << "0. Exit\n";

        int choice = promptInt("Enter your choice: ", 0, 9);
        switch (choice) {
            case 1: install(); break;
            case 2: allotment(); break;
            case 3: show(); break;
            case 4: avail(); break;
            case 5: cancelReservation(); break;
            case 6: moveReservation(); break;
            case 7: findPassenger(); break;
            case 8: showFreeSeats(); break;
            case 9: listByRoute(); break;
            case 0: cout << "Goodbye!\n"; return;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BusManager app;
    app.menuLoop();
    return 0;
}
