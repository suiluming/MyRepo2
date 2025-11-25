#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>

using namespace std;

// 传感器类
class Sensor {
public:
    virtual bool detect() = 0;
};

// 温度传感器
class TemperatureSensor : public Sensor {
private:
    int temperature;  // 当前温度
public:
    TemperatureSensor() : temperature(20) {}
    
    bool detect() override {
        // 模拟温度检测
        return temperature;
    }
    
    void setTemperature(int temp) {
        temperature = temp;
    }
    
    int getTemperature() {
        return temperature;
    }
};

// 水量传感器
class WaterLevelSensor : public Sensor {
private:
    bool hasWater;  // 是否有水
public:
    WaterLevelSensor() : hasWater(true) {}
    
    bool detect() override {
        // 模拟水量检测
        return hasWater;
    }
    
    void setWaterLevel(bool hasWater) {
        this->hasWater = hasWater;
    }
};

// 继电器
class Relay {
private:
    bool isOn;  // 是否开启
public:
    Relay() : isOn(false) {}
    
    void turnOn() {
        isOn = true;
        cout << "继电器已开启，开始烧水" << endl;
    }
    
    void turnOff() {
        isOn = false;
        cout << "继电器已关闭，停止烧水" << endl;
    }
    
    bool getState() {
        return isOn;
    }
};

// 恒温水箱状态类
class WaterTankState {
public:
    virtual void handleState() = 0;
    virtual string getStateName() = 0;
};

// 正常工作状态
class NormalState : public WaterTankState {
private:
    TemperatureSensor* tempSensor;
    WaterLevelSensor* waterSensor;
    Relay* relay;
    
public:
    NormalState(TemperatureSensor* tempSensor, WaterLevelSensor* waterSensor, Relay* relay) 
        : tempSensor(tempSensor), waterSensor(waterSensor), relay(relay) {}
    
    void handleState() override {
        int temp = tempSensor->getTemperature();
        bool hasWater = waterSensor->detect();
        
        cout << "当前状态：正常工作，温度：" << temp << "°C，水量：" << (hasWater ? "正常" : "缺水") << endl;
        
        // 温度低于20°C且有水，开启加热
        if (temp < 20 && hasWater) {
            if (!relay->getState()) {
                relay->turnOn();
            }
        }
        // 温度达到100°C，关闭加热
        else if (temp >= 100) {
            if (relay->getState()) {
                relay->turnOff();
            }
        }
        // 缺水状态，关闭加热
        else if (!hasWater) {
            if (relay->getState()) {
                relay->turnOff();
                cout << "警告：水箱缺水，已停止加热" << endl;
            }
        }
    }
    
    string getStateName() override {
        return "正常工作状态";
    }
};

// 休眠状态
class SleepState : public WaterTankState {
private:
    Relay* relay;
    
public:
    SleepState(Relay* relay) : relay(relay) {}
    
    void handleState() override {
        // 确保继电器关闭
        if (relay->getState()) {
            relay->turnOff();
        }
        cout << "当前状态：休眠中" << endl;
    }
    
    string getStateName() override {
        return "休眠状态";
    }
};

// 故障状态
class FaultState : public WaterTankState {
public:
    void handleState() override {
        cout << "当前状态：故障，需要维修" << endl;
    }
    
    string getStateName() override {
        return "故障状态";
    }
};

// 恒温水箱
class WaterTank {
private:
    WaterTankState* currentState;
    TemperatureSensor* tempSensor;
    WaterLevelSensor* waterSensor;
    Relay* relay;
    bool isFault;
    
public:
    WaterTank() {
        tempSensor = new TemperatureSensor();
        waterSensor = new WaterLevelSensor();
        relay = new Relay();
        currentState = new NormalState(tempSensor, waterSensor, relay);
        isFault = false;
    }
    
    ~WaterTank() {
        delete tempSensor;
        delete waterSensor;
        delete relay;
        delete currentState;
    }
    
    void setState(WaterTankState* state) {
        delete currentState;
        currentState = state;
    }
    
    void update() {
        if (isFault) {
            setState(new FaultState());
            isFault = false;  // 维修后恢复正常
        }
        
        currentState->handleState();
    }
    
    void setTemperature(int temp) {
        tempSensor->setTemperature(temp);
    }
    
    void setWaterLevel(bool hasWater) {
        waterSensor->setWaterLevel(hasWater);
    }
    
    void setFault() {
        isFault = true;
    }
    
    // 检查时间，切换休眠/工作状态
    void checkTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        
        int hour = ltm->tm_hour;
        
        // 晚上11点到早上7点，进入休眠状态
        if (hour >= 23 || hour < 7) {
            if (dynamic_cast<SleepState*>(currentState) == nullptr) {
                setState(new SleepState(relay));
            }
        }
        // 其他时间，进入正常工作状态
        else {
            if (dynamic_cast<NormalState*>(currentState) == nullptr && 
                dynamic_cast<FaultState*>(currentState) == nullptr) {
                setState(new NormalState(tempSensor, waterSensor, relay));
            }
        }
    }
};

int main() {
    WaterTank tank;
    
    // 模拟24小时运行
    for (int hour = 0; hour < 24; hour++) {
        cout << "\n=== " << hour << ":00 ===" << endl;
        
        // 设置当前时间
        time_t now = time(0);
        tm* ltm = localtime(&now);
        ltm->tm_hour = hour;
        now = mktime(ltm);
        
        // 检查时间，切换状态
        tank.checkTime();
        
        // 模拟不同情况
        if (hour >= 8 && hour <= 10) {
            // 早上，温度较低
            tank.setTemperature(18);
            tank.setWaterLevel(true);
        } 
        else if (hour >= 11 && hour <= 14) {
            // 中午，温度升高
            tank.setTemperature(80);
            tank.setWaterLevel(true);
        }
        else if (hour >= 15 && hour <= 17) {
            // 下午，温度达到100°C
            tank.setTemperature(100);
            tank.setWaterLevel(true);
        }
        else if (hour == 18) {
            // 模拟缺水
            tank.setTemperature(50);
            tank.setWaterLevel(false);
        }
        else if (hour == 19) {
            // 模拟故障
            tank.setFault();
        }
        else if (hour >= 20 && hour <= 22) {
            // 晚上，恢复正常
            tank.setTemperature(25);
            tank.setWaterLevel(true);
        }
        else {
            // 休眠时间
            tank.setTemperature(20);
            tank.setWaterLevel(true);
        }
        
        // 更新水箱状态
        tank.update();
        
        // 等待1秒
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    return 0;
}