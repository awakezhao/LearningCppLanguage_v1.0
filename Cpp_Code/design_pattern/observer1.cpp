#include <iostream>
using namespace std;

class DisplayA {
public:
    void Show(float temperature){
        cout << temperature << endl;
    }
};

class DisplayB {
public:
    void Show(float temperature) {
        cout << temperature << endl;
    }
};

class WeatherData{
};

class DataCenter {
public:
    float CalcTemperature() {
        WeatherData *data = GetWeatherData();
        // ...
        float temper = 3.14;
        return temper;
    }
private:
    WeatherData *GetWeatherData(){ //不同方式
        return nullptr;
    }
};

int main() {
    DataCenter *center = new DataCenter;
    DisplayA *da = new DisplayA;
    DisplayB *db = new DisplayB;
    float temper = center->CalcTemperature();
    da->Show(temper);
    db->Show(temper);
    return 0;
}
