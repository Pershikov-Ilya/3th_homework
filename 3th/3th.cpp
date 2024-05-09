#include <iostream>
#include <fstream>
#include <thread>
#include <windows.h>
#include <chrono>

using namespace std;

bool just_read = false;
HANDLE semaphore;

void clearFile() {
    std::ofstream file("text.txt", std::ofstream::out | std::ofstream::trunc);
    file.close();
}

void writeThreadF() {
    string message;
    ofstream file;
    while (true) {
        file.open("text.txt", ios::app);
        if (file.is_open()) {
            getline(cin, message);
            file << message << '\n';
            file.close();
            this_thread::sleep_for(chrono::milliseconds(100));
            just_read = true;
            ReleaseSemaphore(semaphore, 2, NULL);
        }
    }
}

void readThreadF() {
    string text;
    ifstream file;
    file.open("text.txt");
    while (true) {
        WaitForSingleObject(semaphore, INFINITE);
        if (file.is_open()) {
            getline(file, text);
            if (!just_read) {
                cout << "user: " + text << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
            just_read = false;
        }
    }
}

int main() {
    clearFile();
    const char* semaphore_name = "my_named_semaphore";
    semaphore = CreateSemaphoreA(NULL, 0, 2, semaphore_name);

    thread write(writeThreadF);
    thread read(readThreadF);

    write.join();
    read.join();

    CloseHandle(semaphore);
    return 0;
}