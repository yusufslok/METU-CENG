#include <vector>
#include <iostream>
#include <pthread.h>
#include "WriteOutput.h"
#include "monitor.h"
#include <queue>
#include "helper.h"
using namespace std;

typedef struct NarrowBridge
{
    int id;
    int travelTime;
    int maximumWaitTime;
}NarrowBridge;

typedef struct Ferry
{
    int id;
    int travelTime;
    int maximumWaitTime;
    int capacity;
}Ferry;

typedef struct CrossRoad
{
    int id;
    int travelTime;
    int maximumWaitTime;
}CrossRoad;

typedef struct Path
{
    int id;
    int typeOfRoad;
    pair<int, int> direction;
}Path;

typedef struct Car
{
    int id;
    int travelTime;
    vector<Path> path;
    pthread_t threadId;
}Car;

vector<NarrowBridge> narrowBridges;
vector<Ferry> ferries;
vector<CrossRoad> crossRoads;

class CrossRoadMonitor: public Monitor {   // inherit from Monitor
    int roadId;
    int travelTime;
    long long int maximumTime;
    int currentDirection;
    int carPassing;
    bool timeWaitFlag;
    queue<int> waitingCars[4];
    pthread_mutex_t newMutex;
    pthread_cond_t newCond, carPassingCond, timeMax[4];
    // condition varibles
public:
    CrossRoadMonitor(int roadId, int travelTime, int maximumTime)   { 
        this->roadId = roadId;
        this->travelTime = travelTime;
        this->maximumTime = maximumTime;
        this->currentDirection = 0;
        this->carPassing = 0;
        this->timeWaitFlag = false;
        pthread_mutex_init(&newMutex, NULL);
        pthread_cond_init(&newCond, NULL);
        pthread_cond_init(&carPassingCond, NULL);
        pthread_cond_init(&timeMax[0], NULL);
        pthread_cond_init(&timeMax[1], NULL);
        pthread_cond_init(&timeMax[2], NULL);
        pthread_cond_init(&timeMax[3], NULL);
    }
    void passBridge(Car car, int direction) {
        pthread_mutex_lock(&newMutex);
        waitingCars[direction].push(car.id);
        WriteOutput(car.id, 'C', roadId, ARRIVE);
        while(true)
        {
            if(direction == currentDirection && waitingCars[direction].front() == car.id)
            {
                if(carPassing > 0)
                {
                    pthread_mutex_unlock(&newMutex);
                    sleep_milli(PASS_DELAY);
                    pthread_mutex_lock(&newMutex);
                    if(direction != currentDirection)
                    {
                        continue;
                    }
                }
                carPassing++;
                WriteOutput(car.id, 'C', roadId, START_PASSING);
                waitingCars[direction].pop();
                pthread_cond_broadcast(&newCond);
                pthread_mutex_unlock(&newMutex);
                sleep_milli(travelTime);
                pthread_mutex_lock(&newMutex);
                carPassing--;
                if(carPassing == 0)
                {
                    pthread_cond_signal(&carPassingCond);
                }
                if(waitingCars[direction].empty() && carPassing == 0)
                {
                    if(direction != currentDirection)
                    {
                        timeWaitFlag = false;
                        pthread_cond_signal(&carPassingCond);
                    }
                    else
                    {
                        if(!waitingCars[(direction+1)%4].empty())
                        {
                            currentDirection = (direction+1)%4;
                        }
                        else if(!waitingCars[(direction+2)%4].empty())
                        {
                            currentDirection = (direction+2)%4;
                        }
                        else if(!waitingCars[(direction+3)%4].empty())
                        {
                            currentDirection = (direction+3)%4;
                        }
                        pthread_cond_signal(&timeMax[(direction+1)%4]);
                        pthread_cond_signal(&timeMax[(direction+2)%4]);
                        pthread_cond_signal(&timeMax[(direction+3)%4]);
                        //pthread_cond_broadcast(&newCond);
                    }    
                }
                WriteOutput(car.id, 'C', roadId, FINISH_PASSING);
                pthread_mutex_unlock(&newMutex);
                break;
            }
            else if(direction == currentDirection && waitingCars[direction].front() != car.id)
            {
                pthread_cond_wait(&newCond, &newMutex);
            }
            else if(waitingCars[(direction+1)%4].empty() && waitingCars[(direction+2)%4].empty() && waitingCars[(direction+3)%4].empty() && carPassing == 0)
            {
                currentDirection = direction;
                pthread_cond_broadcast(&newCond); // yanlış gibi
                continue;
            }
            else if(direction != currentDirection && waitingCars[direction].front() == car.id ) //&& !timeWaitFlag
            {
                timespec ts;
                timespec current;
                clock_gettime(CLOCK_REALTIME, &current);

                long long int nanoseconds = maximumTime * 1000000LL;
                long long int seconds = nanoseconds / 1000000000LL;
                nanoseconds %= 1000000000LL;

                ts.tv_sec = current.tv_sec + seconds;
                ts.tv_nsec = current.tv_nsec + nanoseconds;

                if (ts.tv_nsec >= 1000000000) {
                    ts.tv_sec++;
                    ts.tv_nsec -= 1000000000;
                }

                int ret = pthread_cond_timedwait(&timeMax[direction], &newMutex, &ts);
                if(timeWaitFlag)
                {
                    if(currentDirection == direction)
                    {
                        if(carPassing > 0)
                        {
                            pthread_cond_wait(&carPassingCond, &newMutex);
                        }
                    }    
                    continue;
                }
                if(ret == ETIMEDOUT)
                {
                    timeWaitFlag = true;
                    if(!waitingCars[(currentDirection+1)%4].empty())
                    {
                        currentDirection = (currentDirection+1)%4;
                    }
                    else if(!waitingCars[(currentDirection+2)%4].empty())
                    {
                        currentDirection = (currentDirection+2)%4;
                    }
                    else if(!waitingCars[(currentDirection+3)%4].empty())
                    {
                        currentDirection = (currentDirection+3)%4;
                    }
                    pthread_cond_broadcast(&timeMax[(direction+1)%4]);
                    pthread_cond_broadcast(&timeMax[(direction+2)%4]);
                    pthread_cond_broadcast(&timeMax[(direction+3)%4]);
                    //pthread_cond_broadcast(&newCond);
                }
                if(currentDirection == direction)
                {
                    if(carPassing > 0)
                    {
                        pthread_cond_wait(&carPassingCond, &newMutex);
                    }
                }    
                continue;
            }
            else
            {
                pthread_cond_wait(&newCond, &newMutex);
            }
        }
    }  
};

class FerryMonitor: public Monitor {   // inherit from Monitor
    int roadId;
    int travelTime;
    long maximumTime;
    int capacity;
    bool busy[2];
    queue<int> waitingCars[2];
    pthread_mutex_t newMutex;
    pthread_cond_t busyCond, newCond[2], timeMax[2];
        // condition varibles
public:
    FerryMonitor(int roadId, int travelTime, int maximumTime, int capacity)   {   // pass "this" to cv constructors
        this->roadId = roadId;
        this->travelTime = travelTime;
        this->maximumTime = maximumTime;
        this->capacity = capacity;
        this->busy[0] = false;
        this->busy[1] = false;
        pthread_mutex_init(&newMutex, NULL);
        pthread_cond_init(&newCond[0], NULL);
        pthread_cond_init(&newCond[1], NULL);
        pthread_cond_init(&timeMax[0], NULL);
        pthread_cond_init(&timeMax[1], NULL);
        pthread_cond_init(&busyCond, NULL);
    }
    void passFerry(Car car, int direction) {
        pthread_mutex_lock(&newMutex);
        while(busy[direction])
        {
            pthread_cond_wait(&busyCond, &newMutex);
        }
        WriteOutput(car.id, 'F', roadId, ARRIVE);
        waitingCars[direction].push(car.id);
        while(true)
        {
            if(waitingCars[direction].size() == capacity)
            {
                WriteOutput(car.id, 'F', roadId, START_PASSING);
                waitingCars[direction].pop();
                pthread_cond_signal(&timeMax[direction]);
                busy[direction] = true;
                if(waitingCars[direction].empty())
                {
                    busy[direction] = false;
                    pthread_cond_signal(&busyCond);
                }
                pthread_mutex_unlock(&newMutex);
                sleep_milli(travelTime);
                pthread_mutex_lock(&newMutex);
                WriteOutput(car.id, 'F', roadId, FINISH_PASSING);
                pthread_mutex_unlock(&newMutex);
                break;
            }
            else if(waitingCars[direction].front() == car.id)
            {
                timespec ts;
                timespec current;
                clock_gettime(CLOCK_REALTIME, &current);

                // Convert milliseconds to seconds and nanoseconds
                long long int nanoseconds = maximumTime * 1000000LL; // Using LL for long long literal
                long long int seconds = nanoseconds / 1000000000LL;
                nanoseconds %= 1000000000LL;

                // Add to current time
                ts.tv_sec = current.tv_sec + seconds;
                ts.tv_nsec = current.tv_nsec + nanoseconds;
                // Normalize timespec
                if (ts.tv_nsec >= 1000000000) {
                    ts.tv_sec++;
                    ts.tv_nsec -= 1000000000;
                }

                int ret = pthread_cond_timedwait(&timeMax[direction], &newMutex, &ts);
                WriteOutput(car.id, 'F', roadId, START_PASSING);
                waitingCars[direction].pop();
                pthread_cond_broadcast(&newCond[direction]); // değiştirdim
                busy[direction] = true;
                if(waitingCars[direction].empty())
                {
                    busy[direction]= false;
                    pthread_cond_signal(&busyCond);
                }
                pthread_mutex_unlock(&newMutex);
                sleep_milli(travelTime);
                pthread_mutex_lock(&newMutex);
                WriteOutput(car.id, 'F', roadId, FINISH_PASSING);
                pthread_mutex_unlock(&newMutex);
                break;
            }
            else
            {
                pthread_cond_wait(&newCond[direction], &newMutex);
                WriteOutput(car.id, 'F', roadId, START_PASSING);
                waitingCars[direction].pop();
                busy[direction] = true;
                if(waitingCars[direction].empty())
                {
                    busy[direction] = false;
                    pthread_cond_signal(&busyCond);
                }
                pthread_mutex_unlock(&newMutex);
                sleep_milli(travelTime);
                pthread_mutex_lock(&newMutex);
                WriteOutput(car.id, 'F', roadId, FINISH_PASSING);
                pthread_mutex_unlock(&newMutex);
                break;
            }
        }
    }  
};

class NarrowBridgeMonitor: public Monitor {   // inherit from Monitor
    int roadId;
    int travelTime;
    long long int maximumWaitTime;
    int currentDirection;
    int carPassing;
    queue<int> waitingCars[2]; // waiting cars in each direction  
    pthread_mutex_t newMutex, temp;
    pthread_cond_t newCond, timeMax[2], carPassingCond;
   // condition varibles
public:
    NarrowBridgeMonitor(int roadId, int travelTime, int maximumTravelTime) {  // pass "this" to cv constructors
        this->roadId = roadId;
        this->travelTime = travelTime;
        this->maximumWaitTime = maximumTravelTime;
        this->carPassing = 0;
        this->currentDirection = 0;
        pthread_mutex_init(&newMutex, NULL);
        pthread_mutex_init(&temp, NULL);
        pthread_cond_init(&newCond, NULL);
        pthread_cond_init(&timeMax[0], NULL);
        pthread_cond_init(&timeMax[1], NULL);
        pthread_cond_init(&carPassingCond, NULL);
    }
        
    void passBridge(Car car, int direction)
    {
        pthread_mutex_lock(&newMutex);
        waitingCars[direction].push(car.id);
        WriteOutput(car.id, 'N', roadId, ARRIVE);
        while(true)
        {
            if(direction == currentDirection && waitingCars[direction].front() == car.id)
            {
                if(carPassing > 0)
                {
                    pthread_mutex_unlock(&newMutex);
                    sleep_milli(PASS_DELAY);
                    pthread_mutex_lock(&newMutex);
                    if(direction != currentDirection)
                    {
                        continue;
                    }
                }
                carPassing++;
                WriteOutput(car.id, 'N', roadId, START_PASSING);
                waitingCars[direction].pop();
                pthread_cond_broadcast(&newCond);
                pthread_mutex_unlock(&newMutex);
                sleep_milli(travelTime);
                pthread_mutex_lock(&newMutex);
                carPassing--;
                if(carPassing == 0)
                {
                    pthread_cond_signal(&carPassingCond);
                }
                if(waitingCars[direction].empty() && carPassing == 0)
                {
                    pthread_cond_signal(&timeMax[1-direction]);
                }
                WriteOutput(car.id, 'N', roadId, FINISH_PASSING);
                pthread_mutex_unlock(&newMutex);
                break;
            }
            else if(direction == currentDirection && waitingCars[direction].front() != car.id)
            {
                pthread_cond_wait(&newCond, &newMutex);
            }
            else if(waitingCars[currentDirection].empty() && carPassing == 0)
            {
                currentDirection = direction;
                pthread_cond_broadcast(&newCond);
                continue;
            }
            else if(direction != currentDirection && waitingCars[direction].front() == car.id)
            {
                timespec ts;
                timespec current;
                clock_gettime(CLOCK_REALTIME, &current);

                // Convert milliseconds to seconds and nanoseconds
                long long int nanoseconds = maximumWaitTime * 1000000LL; // Using LL for long long literal
                long long int seconds = nanoseconds / 1000000000LL;
                nanoseconds %= 1000000000LL;

                // Add to current time
                ts.tv_sec = current.tv_sec + seconds;
                ts.tv_nsec = current.tv_nsec + nanoseconds;
                // Normalize timespec
                if (ts.tv_nsec >= 1000000000) {
                    ts.tv_sec++;
                    ts.tv_nsec -= 1000000000;
                }
                int ret = pthread_cond_timedwait(&timeMax[direction], &newMutex, &ts);
                currentDirection = direction;
                if(carPassing > 0)
                {
                    pthread_cond_wait(&carPassingCond, &newMutex);
                }
                continue;
            }
            else if(direction != currentDirection && waitingCars[direction].front() != car.id)
            {
                pthread_cond_wait(&newCond, &newMutex);
            }
            
        }
    }

};

vector<class NarrowBridgeMonitor> narrowBridgeMonitors;
vector<class FerryMonitor> ferriesMonitors;
vector<class CrossRoadMonitor> crossRoadMonitors;


void* carFunction(void* arg)
{
    Car* car = (Car*)arg;
    for(int i=0; i < car->path.size(); i++)
    {
        if(car->path[i].typeOfRoad == 0) // NarrowBridge
        {
            WriteOutput(car->id, 'N', car->path[i].id, TRAVEL);
            sleep_milli(car->travelTime);
            narrowBridgeMonitors[car->path[i].id].passBridge(*car, car->path[i].direction.first);
        }
        else if(car->path[i].typeOfRoad == 1) // Ferry
        {
            WriteOutput(car->id, 'F', car->path[i].id, TRAVEL);
            sleep_milli(car->travelTime);
            ferriesMonitors[car->path[i].id].passFerry(*car, car->path[i].direction.first);
        }
        else // CrossRoad
        {
            WriteOutput(car->id, 'C', car->path[i].id, TRAVEL);
            sleep_milli(car->travelTime);
            crossRoadMonitors[car->path[i].id].passBridge(*car, car->path[i].direction.first);
        }
    }
    return NULL;
}



int main()
{
    int n, numberOfCars;
    cin >> n;
    for(int i=0; i < n; i++)
    {
        int travelTime, maximumWaitTime;
        cin >> travelTime >> maximumWaitTime;
        NarrowBridge temp;
        temp.id = i;
        temp.travelTime = travelTime;
        temp.maximumWaitTime = maximumWaitTime;
        narrowBridges.push_back(temp);
    }
    cin >> n;
    for(int i=0; i < n; i++)
    {
        int travelTime, maximumWaitTime, capacity;
        cin >> travelTime >> maximumWaitTime >> capacity;
        Ferry temp;
        temp.id = i;
        temp.travelTime = travelTime;
        temp.maximumWaitTime = maximumWaitTime;
        temp.capacity = capacity;
        ferries.push_back(temp);
    }
    cin >> n;
    for(int i=0; i < n; i++)
    {
        int travelTime, maximumWaitTime;
        cin >> travelTime >> maximumWaitTime;
        CrossRoad temp;
        temp.id = i;
        temp.travelTime = travelTime;
        temp.maximumWaitTime = maximumWaitTime;
        crossRoads.push_back(temp);
    }
    cin >> numberOfCars;
    vector<Car> cars;
    for(int i=0; i < numberOfCars; i++)
    {
        int travelTime, numberOfRoads;
        cin >> travelTime;
        Car temp;
        temp.id = i;
        temp.travelTime = travelTime;
        cin >> numberOfRoads;
        for(int j=0; j < numberOfRoads; j++)
        {
            string s;
            int typeOfRoad, roadId, directionFrom, directionTo;
            cin >> s;
            if(s[0] == 'N')
            {
                typeOfRoad = 0;
            }
            else if(s[0] == 'F')
            {
                typeOfRoad = 1;
            }
            else
            {
                typeOfRoad = 2;
            }
            roadId = stoi(s.substr(1)); 
            cin >> directionFrom >> directionTo;

            Path tempPath;
            tempPath.id = roadId;
            tempPath.typeOfRoad = typeOfRoad;
            tempPath.direction = make_pair(directionFrom, directionTo);

            temp.path.push_back(tempPath);
        }
        cars.push_back(temp);
    }

    for(int i=0; i < narrowBridges.size(); i++)
    {
        class NarrowBridgeMonitor temp(narrowBridges[i].id, narrowBridges[i].travelTime, narrowBridges[i].maximumWaitTime);
        narrowBridgeMonitors.push_back(temp);
    }

    for(int i=0; i < ferries.size(); i++)
    {
        class FerryMonitor temp(ferries[i].id, ferries[i].travelTime, ferries[i].maximumWaitTime, ferries[i].capacity);
        ferriesMonitors.push_back(temp);
    }

    for(int i=0; i < crossRoads.size(); i++)
    {
        class CrossRoadMonitor temp(crossRoads[i].id, crossRoads[i].travelTime, crossRoads[i].maximumWaitTime);
        crossRoadMonitors.push_back(temp);
    }

    InitWriteOutput();
    for(int i=0; i < numberOfCars; i++)
    {
        pthread_t threadId;
        Car* car = &cars[i];
        pthread_create(&threadId, NULL, carFunction, (void *) car);
        cars[i].threadId = threadId;
    }

    for(int i=0; i < numberOfCars; i++)
    {
        pthread_join(cars[i].threadId, NULL);
    }
    return 0;
}