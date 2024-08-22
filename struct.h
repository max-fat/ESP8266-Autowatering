
class Time {
private:
    int totalMinutes;
 
    void Normalize() {  // смотрите, как упростилась эта функция!
        const int MinutesInMonth = 31 * 24 * 60 ;
 
        totalMinutes %= MinutesInMonth;
        if (totalMinutes < 0) {
            totalMinutes += MinutesInMonth;
        }
    }
 
public:
    Time(int d, int h, int m) {
        totalMinutes = d * 24 * 60 + h * 60 + m;
        Normalize();
    }
 
    int GetDays() const {
        return totalMinutes / (24 * 60);
    }
 
    int GetHours() const {
        return (totalMinutes / 60) % 24;
    }
 
    int GetMinutes() const {
        return totalMinutes % 60;
    }
    
    int TotalMinutes() const {
        return totalMinutes;
    }
    
    Time& operator = (int m) {
        totalMinutes = m;
        Normalize();
        return *this;
    }
    
    Time& operator += (int m) {
        totalMinutes += m;
        Normalize();
        return *this;
    }
};
