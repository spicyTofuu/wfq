#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
using namespace std;

class Packet {
public:
    int id;
    double size;
    int flowId;
    double arrivalTime;  // Arrival time
    double virFinish;  // Virtual finish time
    double realFinish;  // Real finish time
    Packet(int id, double size, int flowId, double arrivalTime) : id(id), size(size), flowId(flowId), arrivalTime(arrivalTime), virFinish(0), realFinish(0) {}
};


class Queue {
public:
    std::queue<Packet*> packets;
    double lastVirFinish;
    double lastRealFinish;

    Queue() : lastVirFinish(0), lastRealFinish(0) {}

    bool empty() const {
        return packets.empty();
    }

    Packet* head() const {
        if (empty()) return nullptr;
        return packets.front();
    }

    void enqueue(Packet* pkt) {
        packets.push(pkt);
    }

    Packet* dequeue() {
        if (empty()) return nullptr;
        Packet* pkt = packets.front();
        packets.pop();
        return pkt;
    }
};

class Scheduler {
public:
    std::vector<Queue> queues;
    double now;  // Current virtual time
    double real_now;  // Current real time
    double rate;  // Bandwidth rate
    map<int, double> flowWeights;
    map<int, vector<json>> finishTimes;

    Scheduler(int n) : queues(n), now(0), real_now(0) {}

    void receive(Packet* pkt, int queueNum) {
        updateTimeOnArrival(pkt, queueNum);
        queues[queueNum].enqueue(pkt);
        cout << "Virtual Fin Time: P" << queueNum+1 << " - " << pkt->id << " = " << pkt->virFinish << endl;
        cout << "Real Finish time: P" << queueNum+1 << " - " << pkt->id << " = " << pkt->realFinish << endl;
        // Store the finish times
        finishTimes[pkt->flowId].push_back({
            {"id", pkt->id},
            {"virtual_finish_time", pkt->virFinish},
            {"real_finish_time", pkt->realFinish}
        });
    }

    void updateTimeOnArrival(Packet* pkt, int queueNum) {

        // Calculate the virtual finish time
        double virStart = std::max(now, queues[queueNum].lastVirFinish);
        pkt->virFinish = virStart + pkt->size / (rate * flowWeights[queueNum]);

        queues[queueNum].lastVirFinish = pkt->virFinish;

        // For real finish time, we still need to consider the real transmission time
        double realStart = std::max(now, queues[queueNum].lastRealFinish);
        pkt->realFinish = realStart + pkt->size;
        real_now += pkt->size;
        // pkt->realFinish = realStart + pkt->size / getRi(queueNum) / rate;
        queues[queueNum].lastRealFinish = pkt->realFinish;
    }

    double getRi(int flow_num) {
        double weightSum = 0.0;
        for (int i = 0; i < queues.size(); ++i) {
            weightSum += flowWeights[i];
            
        }

        return weightSum > 0 ? (flowWeights[flow_num] / weightSum) * rate : 0;
    }


    Packet* send() {
        int queueNum = selectQueue();
        return queues[queueNum].dequeue();
    }

    int selectQueue() {
        int selectedQueue = -1;
        double minVirFinish = std::numeric_limits<double>::infinity();
        for (int i = 0; i < queues.size(); ++i) {
            Packet* pkt = queues[i].head();
            if (pkt && pkt->virFinish < minVirFinish) {
                minVirFinish = pkt->virFinish;
                selectedQueue = i;
            }
        }
        return selectedQueue;
    }
};

struct Event {
    double time;
    Packet* pkt;
    int flowId;

    Event(double time, Packet* pkt, int flowId) : time(time), pkt(pkt), flowId(flowId) {}
};

struct EventCompare {
    bool operator()(const Event& e1, const Event& e2) {
        if (e1.time != e2.time) {
            return e1.time > e2.time;
        }
        if (e1.flowId != e2.flowId) {
            return e1.flowId > e2.flowId;
        }

        return e1.pkt->id > e2.pkt->id;
    }
};

void readFromJson(const string& filename, priority_queue<Event, vector<Event>, EventCompare>& events, Scheduler& scheduler) {
    ifstream inFile(filename);
    json j;
    inFile >> j;

    scheduler.rate = j["bandwidth"];

    for (const auto& flow : j["flows"]) {
        int flowId = flow["id"];
        double weight = flow["weight"];
        scheduler.flowWeights[flowId] = weight;

        for (const auto& packet : flow["packets"]) {
            int packetId = packet["id"];
            double arrivalTime = packet["arrival_time"];
            double size = packet["size"];
            Packet* pkt = new Packet(packetId, size, flowId, arrivalTime);
            events.push(Event(arrivalTime, pkt, flowId));
        }
    }
    inFile.close();
}

int getNumOfFlows(const string& filename) {
    ifstream inFile(filename);
    json j;
    inFile >> j;
    int n_flows = j["flows"].size();
    inFile.close();
    return n_flows;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_filename.json>" << endl;
        return 1; // Exit with a non-zero value to indicate an error
    }

    string filename = argv[1];
    priority_queue<Event, vector<Event>, EventCompare> events;
    Scheduler scheduler(getNumOfFlows(filename));

    readFromJson(filename, events, scheduler);
    cout << "Rate: " << scheduler.rate << endl;
    
    while (!events.empty()) {
        Event e = events.top();
        events.pop();
        scheduler.receive(e.pkt, e.flowId);
    }

    // Preparing JSON output
    json outputJson;
    outputJson["flows"] = json::array();
    for (const auto& [flowId, packets] : scheduler.finishTimes) {
        json flowJson;
        flowJson["id"] = flowId;
        flowJson["weight"] = scheduler.flowWeights[flowId];
        flowJson["packets"] = packets;
        outputJson["flows"].push_back(flowJson);
    }

    // Write to file
    ofstream outFile("output.json");
    cout << "Virtual finish time and real finish time is saved in output.json file. " << endl;
    outFile << setw(4) << outputJson << endl;
    outFile.close();

    return 0;
}
