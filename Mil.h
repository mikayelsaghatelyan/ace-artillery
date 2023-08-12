#ifndef ACE_ARTILLERY1_0_MIL_H
#define ACE_ARTILLERY1_0_MIL_H

#include "dependencies.h"

// mil-radian class for working with angles (6000 Mil = 360 degrees = 2 * pi radians)
class Mil {
private:
    int first;
    int second;
public:
    friend std::ostream &operator<<(std::ostream &out, const Mil &m);
    friend std::istream &operator>>(std::istream &in, Mil &m);
    Mil();
    Mil(const Mil &m);
    explicit Mil(int whole);
    explicit Mil(double angle);
    explicit Mil(const std::string& angle);
    Mil(int first, int second);
    Mil operator+(const Mil &m) const;
    Mil operator+(const std::pair<int, int>& m) const;
    Mil operator+(int m) const;
    Mil& operator+=(const Mil& m);
    Mil& operator+=(const std::pair<int,int>& m);
    Mil& operator+=(int m);
    Mil operator-(const Mil &m) const;
    Mil operator-(const std::pair<int, int>& m) const;
    Mil operator-(int m) const;
    Mil& operator-=(const Mil& m);
    Mil& operator-=(const std::pair<int,int>& m);
    Mil& operator-=(int m);
    Mil& operator=(const Mil& m);
    Mil& operator=(int m);
    bool operator!=(const Mil& m) const;
    bool operator==(const Mil& m) const;
    bool operator>(const Mil& m) const;
    bool operator>=(const Mil& m) const;
    bool operator<(const Mil& m) const;
    bool operator<=(const Mil& m) const;
    explicit operator std::string() const;  // conversion operator for working with JSON
    void setFirst(int a);
    void setSecond(int a);
    [[nodiscard]] int getFirst() const;
    [[nodiscard]] int getSecond() const;
    [[nodiscard]] double toDegrees() const;
    [[nodiscard]] double toRadians() const;
    [[nodiscard]] int toInt() const;
};

bool isValidMilString(const std::string& s);

extern std::ostream& operator<<(std::ostream& out, const Mil& m);
extern std::istream& operator>>(std::istream& in, Mil& m);

#endif //ACE_ARTILLERY1_0_MIL_H
