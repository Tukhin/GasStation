

#include <iostream>
#include <vector>
#include <string>
#include <chrono>  //controls time management
#include <thread>
#include <deque>
#include <mutex>
#include "assert.h"


/**
 *
 * Write a program using C++ that simulates a Gas Station.
 *
 * You have 10 cars
 * You have 2 gas pumps
 * There should be one car line waiting to use the available gas pumps
 * Each car spends 30 milliseconds at a gas pump which represents one fill up
 * Once filled up, the car then gets back in line
 * Scenario runs for 30 seconds
 * Count the number of fillups for each car
 * Count the number of fillups for each pump
 * Print the result to stdout
 * This will need to be a multi-threaded program
 * The cars should be represented as individual threads, not the pumps
 * Normally, it would make sense to make the pump as the worker, but for this assignment we're looking for the cars to be the workers.
 * The car thread should initiate each action when it's turn comes.
 * Try to use STL only, third party libraries should not be needed.
 */
using namespace std;
using namespace std::literals::chrono_literals;
std::mutex mtx;

std::mutex pump1mtx;
std::mutex pump2mtx;
std::mutex read_pump1mtx;
std::mutex read_pump2mtx;


struct Car {
    int numFillups = 0;
    int id;
};

struct GasPump {
    int numFillups = 0;
    int id;
    bool isPumping = false;
};

//void MultiThread(vector<GasPump> &pumps, Car &car) {
//    thread t1(fillUpCar(pumps, car));
//}
//
//


bool canPump(vector<GasPump> &pumps) {
    pump1mtx.lock();
    auto& pump1 = pumps.at(0);
    pump1mtx.unlock();

    pump2mtx.lock();
    auto& pump2 = pumps.at(1);
    pump2mtx.unlock();

        if (pump1.isPumping && pump2.isPumping) {
        return false;
    }

    return true;
}



void successfullyFilledUpCar(vector<GasPump> &pumps, Car &car, deque<Car>& cars) {
    cout << "Filling up car with id " << car.id << endl;
    pump1mtx.lock();
    auto& pump1 = pumps.at(0);
    pump1mtx.unlock();


    pump2mtx.lock();
    auto& pump2 = pumps.at(1);
    pump2mtx.unlock();

    cout << "Did we ever get here?";

    if (!pump1.isPumping)  {
        pump1mtx.lock();
        cout << "Pump1 is pumping" << endl;
        pump1.isPumping = true;
        pump1mtx.unlock();

                                this_thread::sleep_for(30ms);
        car.numFillups++;

        pump1mtx.lock();
        pump1.numFillups++;
        pump1.isPumping = false;
        pump1mtx.unlock();
    } else if (!pump2.isPumping) {
        pump2mtx.lock();
        pump2.isPumping = true;
        pump2mtx.unlock();

                                this_thread::sleep_for(30ms);
        car.numFillups++;

        pump2mtx.lock();
        pump2.numFillups++;
        pump2.isPumping = false;
        pump2mtx.unlock();
    }

    mtx.lock();
    cars.push_back(car);
    cout << "pump1 fillups " << pump1.numFillups << "pump2 fillups " << pump2.numFillups << endl;
    mtx.unlock();
}

void printCarStats(deque<Car>& cars) {
    cout << "==========================" << endl;
    cout << "Print car stats" << endl;
    cout << "==========================" << endl;
    int totalFillups = 0;
    for(int i = 0; i < cars.size(); i++) {
        cout << "We have car id "
             << cars[i].id
             << " with numFillups at: "
             << cars[i].numFillups << endl;
        totalFillups += cars[i].numFillups;
    }


    cout << "Total fillups: " << totalFillups << endl;
}

void printPumpStats(vector<GasPump>& pumps) {
    cout << "==========================" << endl;
    cout << "Print pump stats" << endl;
    cout << "==========================" << endl;

    int totalFillups = 0;
    for (int i = 0; i < pumps.size(); i++) {
        cout << "We have pump id "
             << pumps[i].id
             << " with numFillups at: "
             << pumps[i].numFillups << endl;
        totalFillups += pumps[i].numFillups;
    }

    cout << "Total fillups: " << totalFillups << endl;
}

int main() {
    deque<Car> cars(10);

    for(int i = 0; i < cars.size(); i++) {
        cars[i].id = i;
        cout << "We have car id " << cars[i].id << endl;
    }

    vector<GasPump> pumps(2);
    for(int i = 0; i < pumps.size(); i++) {
        pumps[i].id = i;
        cout << "We have pump id " << pumps[i].id << endl;
    }

    auto finish = chrono::system_clock::now() + 0.05min;
    std::vector<std::thread> thread_vec;
    do {
        mtx.lock();
        auto& car = cars.front();
        cars.pop_front();
        mtx.unlock();

        while(!canPump(pumps)) {

        }

       thread_vec.push_back(thread(successfullyFilledUpCar, ref(pumps), ref(car), ref(cars)));
    } while(chrono::system_clock::now() < finish);

    for(int i = 0; i < thread_vec.size(); i++) {
      thread_vec[i].join();
    }

    printCarStats(cars);
    printPumpStats(pumps);

    return 0;
}
