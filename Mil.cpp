#include "Mil.h"
// USSR/Russian/Armenian Mil-radian angle measurement unit used in artillery, anti-air defence etc.
// 6000 Mils = 360 Degrees

bool isValidMilString(const std::string& s) {
    return !(s.size() != 5 || s[2] != '-' ||
        stoi(s.substr(0, 2)) < 0 || stoi(s.substr(0, 2)) > 59 ||
        stoi(s.substr(3, 2)) < 0 || stoi(s.substr(3, 2)) > 99);
}

Mil::Mil() {
    this->first = 0;
    this->second = 0;
}

Mil::Mil(const Mil &m) {
    this->first = m.first;
    this->second = m.second;
}

Mil::Mil(double angle) {
    int mil = (int) (angle * 3000 / constants::pi) % 6000;
    if (mil < 0) mil += 6000;
    first = mil / 100;
    second = mil % 100;
}

Mil::Mil(int first, int second) {
    this->first = first;
    this->second = second;
    this->first += this->second / 100;
    this->second = this->second % 100;
    this->first = this->first % 60;
}

Mil::Mil(int whole) {
    this->first = whole / 100;
    this->second = whole % 100;
}

Mil::Mil(const std::string& angle) {
    if(!isValidMilString(angle)) {
        throw std::runtime_error("invalid mil string");
    }
    this->first = stoi(angle.substr(0,2));
    this->second = stoi(angle.substr(3,2));
}

Mil Mil::operator+(const Mil &m) const {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 += m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil Mil::operator+(const std::pair<int, int> &m) const {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 += m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil Mil::operator+(int m) const {
    int m1 = this->first * 100 + this->second;
    m1 += m;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil &Mil::operator+=(const Mil &m) {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 += m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil &Mil::operator+=(const std::pair<int, int> &m) {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 += m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil &Mil::operator+=(int m) {
    int m1 = this->first * 100 + this->second;
    m1 += m;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil Mil::operator-(const Mil &m) const {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 -= m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil Mil::operator-(const std::pair<int, int> &m) const {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 -= m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil Mil::operator-(int m) const {
    int m1 = this->first * 100 + this->second;
    m1 -= m;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    return {m1 / 100, m1 % 100};
}

Mil &Mil::operator-=(const Mil &m) {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 -= m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil &Mil::operator-=(const std::pair<int, int> &m) {
    int m1 = this->first * 100 + this->second;
    int m2 = m.first * 100 + m.second;
    m1 -= m2;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil &Mil::operator-=(int m) {
    int m1 = this->first * 100 + this->second;
    m1 -= m;
    m1 = m1 % 6000;
    if (m1 < 0) m1 += 6000;
    this->first = m1 / 100;
    this->second = m1 % 100;
    return *this;
}

Mil &Mil::operator=(const Mil &m) {
    if (this != &m) {
        this->first = m.first;
        this->second = m.second;
    }
    return *this;
}

Mil &Mil::operator=(int m) {
    if (this->first * 100 + this->second != m) {
        this->first = m / 100;
        this->second = m % 100;
    }
    return *this;
}

bool Mil::operator!=(const Mil &m) const {
    return this->first != m.first || this->second != m.second;
}

bool Mil::operator==(const Mil &m) const {
    return this->first == m.first && this->second == m.second;
}

bool Mil::operator>(const Mil &m) const {
    return this->first * 100 + this->second > m.first * 100 + m.second;
}

bool Mil::operator>=(const Mil &m) const {
    return this->first * 100 + this->second >= m.first * 100 + m.second;
}

bool Mil::operator<(const Mil &m) const {
    return this->first * 100 + this->second < m.first * 100 + m.second;
}

bool Mil::operator<=(const Mil &m) const {
    return this->first * 100 + this->second <= m.first * 100 + m.second;
}

Mil::operator std::string() const {
    std::string s;
    if (first < 10) s += '0';
    s += std::to_string(first);
    s += '-';
    if (second < 10) s += '0';
    s += std::to_string(second);
    return s;
}

void Mil::setFirst(int a) {
    if (a < 0 || a > 60) {
        throw std::runtime_error("invalid mil first");
    }
    if (a == 60) {
        a = 0;
    }
    this->first = a;
}

void Mil::setSecond(int a) {
    if (a < 0 || a > 99) {
        throw std::runtime_error("invalid mil second");
    }
    this->second = a;
}

int Mil::getFirst() const {
    return this->first;
}

int Mil::getSecond() const {
    return this->second;
}

int Mil::toInt() const {
    return this->first * 100 + this->second;
}

double Mil::toDegrees() const {
    return static_cast<double>(this->first * 100 + this->second) * 360. / 6000.;
}

double Mil::toRadians() const {
    return static_cast<double>(this->first * 100 + this->second) * 2 * constants::pi / 6000.;
}

// output operator overloading for Mil (mil-radian) class
std::ostream &operator<<(std::ostream &out, const Mil &m) {
    out << std::setw(2) << std::setfill('0') << m.first << "-";
    out << std::setw(2) << std::setfill('0') << m.second;
    return out;
}

// output operator overloading for Mil (mil-radian) class
std::istream &operator>>(std::istream &in, Mil &m) {
    int a, b;
    char dash;
    in >> a >> dash >> b;
    if ((dash != '-' && dash != '_') || (a < 0 || a > 59) || (b < 0 || b > 99)) {
        throw std::runtime_error("invalid mil input");
    }
    m.first = a;
    m.second = b;
    return in;
}

Mil getRandomAngle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> alpha(0, 5999);
    int m = alpha(gen);
    return {m / 100, m % 100};
}

Mil getRandomRefAngle(const Mil &m) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> alpha(-750, 750);
    return m + 3000 + alpha(gen);
}
