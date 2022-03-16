//
//  Scheduler.cpp
//  COEN346Scheduler
//
//  Created by Rohit Vaidya on 2022-03-13.
//

#include "Scheduler.h"
#include <cmath>
#include <thread>
#include <vector>

using namespace std;

Scheduler::Scheduler() {
    q2.updateFlag();
    timeSlice = 0;
    terminated = false;
}

int Scheduler::calculateTimeSlice(Process* p) {
    if (p->getPriority() < 100)
        timeSlice = (140 - p->getPriority()) * 20;
    else if (p->getPriority() >= 100)
        timeSlice = (140 - p->getPriority()) * 5;
    return timeSlice;
}

int Scheduler::calculatePriority(Process* p) {
    if (p->getCpuIteration() % 2 == 0) {
        int bonus = floor((p->getWaitingTime() * 10) / (clk->getTime() - p->getArrivalTime()));
        int newPriority = max(100, min((p->getPriority() - bonus + 5), 139));
        cout << "TIME " << clk->getTime() << ", " << p->getProcessID() << ", priority updated to " << newPriority << endl;
        return newPriority;
    }
    else
        return p->getPriority();

}

void Scheduler::swapFlag() {
    q2.updateFlag();
    q1.updateFlag();
    q2.sort();
    q1.sort();
}

void Scheduler::addProcess(Process* p) {
    if (p->getCpuIteration() == 0) {
        cout << "TIME " << clk->getTime() << ", " << p->getProcessID() << " ARRIVED" << endl;
    }
    if (q1.getFlag() == false) {
        q1.addProcess(p);
    }
    else if (!q2.getFlag()) {
        q2.addProcess(p);
    }
}

void Scheduler::setClock(Clock* c) {
    clk = c;
}

ProcessQueue* Scheduler::getActiveQueue() {
    if (q1.getFlag())
        return &q1;
    else
        return &q2;
}

ProcessQueue* Scheduler::getExpiredQueue() {
    if (!q1.getFlag())
        return &q1;
    else
        return &q2;
}

void Scheduler::schedule() {
    ProcessQueue* active = getActiveQueue();
    ProcessQueue* expired = getExpiredQueue();
    Process* tempProcess;
    while (true) {
        if (terminated && q1.checkEmpty() && q2.checkEmpty()) {
            cout << "Scheduler done schedulling" << endl;
            clk->setStartFlag(false);
            break;
        }
        while (q1.checkEmpty() && q2.checkEmpty()) {

        }

        if (active->checkEmpty()) {
            swapFlag();
            active = getActiveQueue();
            expired = getExpiredQueue();
        }

        tempProcess = active->removeProcess();
        timeSlice = calculateTimeSlice(tempProcess);
        if (tempProcess->getCpuIteration() == 0) {
            tempProcess->setState("STARTED");
            thread th(&Process::execute, tempProcess);
            cout << "TIME " << clk->getTime() << ", " << tempProcess->getProcessID() << " STARTED, GRANTED " << timeSlice << endl;
            threadVector.push_back(move(th));
        }
        else {
            tempProcess->setState("RESUMED");
            cout << "TIME " << clk->getTime() << ", " << tempProcess->getProcessID() << " RESUMED, GRANTED " << timeSlice << endl;
        }
        sleepScheduler();
        tempProcess->setState("PAUSED");
        tempProcess->setPriority(calculatePriority(tempProcess));
        if (tempProcess->getState() != "TERMINATED") {
            cout << "TIME " << clk->getTime() << ", " << tempProcess->getProcessID() << " PAUSED" << endl;
            addProcess(tempProcess);
        }

    }
}

void Scheduler::sleepScheduler() {
    int startClk = clk->getTime();
    while (timeSlice + startClk != clk->getTime()) {

    }
}

void Scheduler::setTerminated(bool flag) {
    terminated = flag;
}

bool Scheduler::getTerminated() {
    return terminated;
}