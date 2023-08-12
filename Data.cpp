#include "dependencies.h"
#include "Data.h"

bool is_debug_mode = false;

std::vector<std::unordered_map<int, std::vector<double>>> ballistic_tables_map;
std::vector<std::unordered_map<int, std::vector<double>>> distance_tables_map;

std::string project_path;

std::vector<std::string> param_entries_rus;
std::vector<std::string> param_entries_rus_short;
std::vector<std::string> param_entries_eng;
std::vector<std::string> param_entries_eng_short;

std::set<std::string> target_types_eng;
std::set<std::string> target_types_rus;

const std::vector<std::string> ballistic_table_names = {
        "ballistic-table-d30-of462-full.csv",
        "ballistic-table-d30-of462-full-mortar.csv",
        "ballistic-table-d30-of462-reduced.csv",
        "ballistic-table-d30-of462-reduced-mortar.csv",
        "ballistic-table-d30-of462-1st.csv",
        "ballistic-table-d30-of462-1st-mortar.csv",
        "ballistic-table-d30-of462-2nd.csv",
        "ballistic-table-d30-of462-2nd-mortar.csv",
        "ballistic-table-d30-of462-3rd.csv",
        "ballistic-table-d30-of462-3rd-mortar.csv",
        "ballistic-table-d30-of462-4th.csv",
        "ballistic-table-d30-of462-4th-mortar.csv"
};

const std::vector<std::string> distance_table_names = {
        "minimum-distance-d30-full.csv",
        "minimum-distance-d30-reduced.csv",
        "minimum-distance-d30-1st.csv",
        "minimum-distance-d30-2nd.csv",
        "minimum-distance-d30-3rd.csv",
        "minimum-distance-d30-4th.csv",
};

const std::vector<std::string> param_entry_names = {
        "param_entries_eng.txt",
        "param_entries_eng_short.txt",
        "param_entries_rus.txt",
        "param_entries_rus_short.txt"
};

const std::vector<std::pair<int, int>> dist_boundaries = {
        {600,  15300},
        {9700, 15200},
        {400,  12800},
        {8200, 12800},
        {200,  11500},
        {7400, 11400},
        {200,  10000},
        {6500, 10000},
        {200,  8300},
        {5400, 8200},
        {200,  6300},
        {4200, 6200}
};

int floor_k(int n, int k) {
    return (n / k) * k;
}

int ceil_k(int n, int k) {
    return ((n % k) ? (n / k + 1) * k : n);
}

Point::Point() : pt_x(0), pt_y(0), pt_h(0) {}

Point::Point(double x, double y, double h) : pt_x(x), pt_y(y), pt_h(h) {}

Point& Point::operator=(const Point& m) = default;


BatteryPoints::BatteryPoints(const std::vector<Point>& guns, double front, double depth, double x, double y, double h)
                            : bp_guns(guns), bp_front(front), bp_depth(depth), bp_x(x), bp_y(y), bp_h(h) {}

bool isValidX(double val) {
    return (val>=SK_42_X_MIN && val<=SK_42_X_MAX);
}

bool isValidY(double val) {
    return (val>=SK_42_Y_MIN && val<=SK_42_Y_MAX);
}

bool isValidH(double val) {
    return (val>=SK_42_H_MIN && val<=SK_42_H_MAX);
}

bool isValidFront(double val) {
    return (val>0 && val<FRONT_MAX);
}

bool isValidDepth(double val) {
    return (val>0 && val<DEPTH_MAX);
}

bool isReachable(Point p, double dx, double dy, double dh) {
    return (sqrt(dx * dx + dy * dy) < 15300 && sqrt(dx * dx + dy * dy) > 600) &&
    (isValidX(p.pt_x + dx) && (isValidY(p.pt_y + dy)) && (isValidH(p.pt_h + dh)));
}

Point getRandomPoint() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> x(SK_42_X_MIN, SK_42_X_MAX);
    std::uniform_real_distribution<double> y(SK_42_Y_MIN, SK_42_Y_MAX);
    std::uniform_real_distribution<double> h(SK_42_H_MIN, SK_42_H_MAX);
    Point pt = {std::floor(x(gen)), std::floor(y(gen)), std::floor(h(gen))};
    return pt;
}

Point getRandomPoint(const Point& p, double radius) {
    if (radius < 0 || radius > 15300) {
        throw std::runtime_error("random point generation: invalid radius");
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dev_xy(-radius, radius);
    std::uniform_real_distribution<double> dev_h(-50, 50);
    double dx, dy;
    double dh = dev_h(gen);
    do {
        dx = dev_xy(gen);
        dy = dev_xy(gen);
    } while (!isReachable(p, dx, dy, dh));
    return {p.pt_x + dx, p.pt_y + dy, p.pt_h + dh};
}

void setDebugVariable(const bool& value) {
    is_debug_mode = value;
}

std::tuple<std::vector<Point>, double, double, double> getRandomGuns(double k1, double k2, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> x(SK_42_X_MIN, SK_42_X_MAX);
    std::uniform_real_distribution<double> y(SK_42_Y_MIN, SK_42_Y_MAX);
    std::uniform_real_distribution<double> h(SK_42_H_MIN, SK_42_H_MAX);
    std::uniform_real_distribution<double> dev_x(-15, 15);      // deviation by x
    std::uniform_real_distribution<double> dev_y(-15, 15);      // deviation by y
    std::uniform_real_distribution<double> dev_h(-5, 5);        // deviation by h
    std::uniform_real_distribution<double> distance(40, 100);   // distance between guns by x
    double main_x;
    double main_y;
    double main_h;
    std::vector<Point> guns{};
    guns.emplace_back(x(gen) + dev_x(gen),
                      y(gen) + dev_y(gen),
                      h(gen) + dev_h(gen));
    std::uniform_int_distribution<int> chance(0, 1);
    double dir_k = chance(gen) ? 1. : -1.;                 // coefficient for direction, either 1 or -1 for all points
    for (int i = 1; i < n; ++i) {
        Point prev = guns.back();
        double dist;
        // for an odd number of guns there is a 50% chance that they will be separated into groups of [n/2] and [n/2+1] (or vice versa)
        // for an even number of guns there is a 50% chance that they will be separated into groups of n/2
        if (((n % 2 && i == n / 2 + chance(gen)) || i == n / 2) && chance(gen)) {
            dist = distance(gen) * 2;
        } else {
            dist = distance(gen);
        }
        guns.emplace_back(prev.pt_x + dir_k * dist + dev_x(gen),
                          prev.pt_y + dir_k * k1 * dist + dev_y(gen),
                          prev.pt_h + dev_h(gen));
        if (i == n / 2) {
            main_x = guns.back().pt_x;
            main_y = guns.back().pt_y;
            main_h = guns.back().pt_h;
        }
    }
    return {guns, main_x, main_y, main_h};
}

std::pair<double, double> getBatteryFrontDepth(std::vector<Point>& proj_front, std::vector<Point>& proj_depth, int size) {
    Point &pf_max = proj_front[0];
    Point &pf_min = proj_front[0];
    Point &pd_max = proj_front[0];
    Point &pd_min = proj_front[0];
    for (int i = 1; i < size; ++i) {
        auto pf_g = proj_front[i];
        auto pd_g = proj_depth[i];
        if (pf_g.pt_x > pf_max.pt_x)
            pf_max = pf_g;
        if (pf_g.pt_x < pf_min.pt_x)
            pf_min = pf_g;
        if (pd_g.pt_x > pd_max.pt_x)
            pd_max = pd_g;
        if (pd_g.pt_x < pd_min.pt_x)
            pd_min = pd_g;
    }
    double front = sqrt((pf_max.pt_x - pf_min.pt_x) * (pf_max.pt_x - pf_min.pt_x) +
                        (pf_max.pt_y - pf_min.pt_y) * (pf_max.pt_y - pf_min.pt_y));
    double depth = sqrt((pd_max.pt_x - pd_min.pt_x) * (pd_max.pt_x - pd_min.pt_x) +
                        (pd_max.pt_y - pd_min.pt_y) * (pd_max.pt_y - pd_min.pt_y));
    if (is_debug_mode) printFrontDepthMaxMinDebug(pf_max, pf_min, pd_max, pd_min);
    return {front, depth};
}

void printProjectionDebug(const std::vector<Point>& proj_front, const std::vector<Point>& proj_depth) {
    std::cout << "projection on front line:\n";
    for (auto g: proj_front) {
        std::cout << std::fixed << std::setprecision(0) << g.pt_x << "," << g.pt_y << "\n";
    }
    std::cout << "\n";
    std::cout << "projection on depth line: \n";
    for (auto g: proj_depth) {
        std::cout << std::fixed << std::setprecision(0) << g.pt_x << "," << g.pt_y << "\n";
    }
    std::cout << "\n";
}

void printFrontDepthMaxMinDebug(const Point& pf_max, const Point& pf_min, const Point& pd_max, const Point& pd_min) {
    std::cout << "front max x: " << pf_max.pt_x << "\nfront min x: " << pf_min.pt_x << "\n";
    std::cout << "front max y: " << pf_max.pt_y << "\nfront min y: " << pf_min.pt_y << "\n";
    std::cout << "depth max x: " << pd_max.pt_x << "\ndepth min x: " << pd_min.pt_x << "\n";
    std::cout << "depth max y: " << pd_max.pt_y << "\ndepth min y: " << pd_min.pt_y << "\n";
    std::cout << "\n";
}

/**
 * @brief returns random battery coordinates
 * @param angle - angle of battery direction with respect to horizontal line
 * @param n - number of guns
 */
BatteryPoints getRandomBattery(const double& angle, const int& n) {
    double k2 = tan(angle);
    double k1 = -1 / tan(angle);
    auto guns_data = getRandomGuns(k1, k2, n);
    auto guns = std::get<0>(guns_data);
    auto main_x = std::get<1>(guns_data);
    auto main_y = std::get<2>(guns_data);
    auto main_h = std::get<3>(guns_data);
    size_t size = guns.size();
    double b1 = guns[0].pt_y - k1 * guns[0].pt_x;
    double b2 = guns[0].pt_y - k2 * guns[0].pt_x;
    std::vector<Point> proj_front{guns[0]};
    std::vector<Point> proj_depth{guns[0]};
    // creating an array of projections on the lines y = k1 * x + b1 and y = k2 * x + b2, where k1 = -1 / tan(angle), k2 = tan(angle)
    for (int i = 1; i < size; ++i) {
        auto g = guns[i];
        double pf_x = (g.pt_y - k2 * g.pt_x - b1) / (k1 - k2);
        double pf_y = k1 * pf_x + b1;
        proj_front.emplace_back(pf_x, pf_y, g.pt_h);
        double pd_x = (g.pt_y - k1 * g.pt_x - b2) / (k2 - k1);
        double pd_y = k2 * pd_x + b2;
        proj_depth.emplace_back(pd_x, pd_y, g.pt_h);
    }
    if (is_debug_mode) printProjectionDebug(proj_front, proj_depth);
    auto fd_pair = getBatteryFrontDepth(proj_front, proj_depth, size);
    return {guns, fd_pair.first, fd_pair.second, main_x, main_y, main_h};
}

void printBattery(const BatteryPoints& battery) {
    std::cout << "front: " << battery.bp_front << "\ndepth: " << battery.bp_depth << "\n";
    std::cout << "main gun: " << battery.bp_x << ", " << battery.bp_y << ", " << battery.bp_h << "\n";
    for(auto g : battery.bp_guns) {
        std::cout << std::fixed << std::setprecision(0) << g.pt_x << "," << g.pt_y << g.pt_h << "\n";
    }
}

std::vector<double> lineToDoubleVector(std::string line) {
    std::vector<double> vec;
    while (line.find(',')!=-1) {
        vec.push_back(std::stod(line.substr(0, line.find(','))));
        line = line.substr(line.find(',')+1);
    }
    vec.push_back(std::stod(line));
    return vec;
}

std::vector<int> lineToIntVector(std::string line) {
    std::vector<int> vec;
    while (line.find(',')!=-1) {
        vec.push_back(std::stoi(line.substr(0, line.find(','))));
        line = line.substr(line.find(',')+1);
    }
    return vec;
}

std::string chargeTypeToString(charge_type charge) {
    static std::map<charge_type, std::string> charge_types = {
            {lt_full, "full"},
            {lt_full_mortar, "full_mortar"},
            {lt_reduced, "reduced"},
            {lt_reduced_mortar, "reduced_mortar"},
            {lt_1st, "1st"},
            {lt_1st_mortar, "1st_mortar"},
            {lt_2nd, "2nd"},
            {lt_2nd_mortar, "2nd_mortar"},
            {lt_3rd, "3rd"},
            {lt_3rd_mortar,"3rd_mortar"},
            {lt_4th, "4th"},
            {lt_4th_mortar, "4th_mortar"}
    };
    return charge_types[charge];
}

charge_type stringToChargeType(const std::string& charge) {
    if(charge == "full") {
        return lt_full;
    }
    else if (charge == "full_mortar" || charge == "full-mortar" || charge == "full mortar") {
        return lt_full_mortar;
    }
    if(charge == "reduced") {
        return lt_reduced;
    }
    else if (charge == "reduced_mortar" || charge == "reduced-mortar" || charge == "reduced mortar") {
        return lt_reduced_mortar;
    }
    if(charge == "1st") {
        return lt_1st;
    }
    else if (charge == "1st_mortar" || charge == "1st-mortar" || charge == "1st mortar") {
        return lt_1st_mortar;
    }
    if(charge == "2nd") {
        return lt_2nd;
    }
    else if (charge == "2nd_mortar" || charge == "2nd-mortar" || charge == "2nd mortar") {
        return lt_2nd_mortar;
    }
    if(charge == "3rd") {
        return lt_3rd;
    }
    else if (charge == "3rd_mortar" || charge == "3rd-mortar" || charge == "3rd mortar") {
        return lt_3rd_mortar;
    }
    if(charge == "4th") {
        return lt_4th;
    }
    else if (charge == "4th_mortar" || charge == "4th-mortar" || charge == "4th mortar") {
        return lt_4th_mortar;
    }
    else {
        throw std::runtime_error("invalid charge type");
    }
}

void ballisticTablesToHash() {
    if(chdir((project_path+"/tables").c_str())!=0) {
        throw std::runtime_error("can't find tables directory");
    }
    std::ofstream file_out;
    bool first;
    for (const auto& filename: ballistic_table_names) {
        file_out.open(filename);
        file_out.clear();
        std::string line{};
        std::ifstream table(filename.substr(10));
        if (!table.is_open()) {
            throw std::runtime_error("cannot open csv file");
        }
        first = true;
        while (std::getline(table, line)) {
            if (first) {
                first = false;
                continue;
            }

            // converting data from 5th-from-right column, which is angle represented as integer, i.e. 15°20' = 1520, to a double value
            int commas = 0;
            int start_i = -1, end_i = -1;
            for (int i = line.size() - 1; i >= 0; --i) {
                if (line[i] == ',') {
                    commas++;
                    if (commas == 5) end_i = i - 1;
                    if (commas == 6) {
                        start_i = i + 1;
                        break;
                    }
                }
            }
            int angle = std::stoi(line.substr(start_i, end_i - start_i + 1));
            std::string d_angle = std::to_string(
                    static_cast<double>(angle / 100) + static_cast<double>(angle % 100) / 60);
            for (int k = 0; k < 4; ++k) d_angle.pop_back();
            line = line.substr(0, start_i) + d_angle + line.substr(end_i + 1);
            file_out << line;
        }
        first = true;
        file_out.close();
    }
    chdir(project_path.c_str());
}

void minDistTablesToHash() {
    if(chdir((project_path+"/tables").c_str())!=0) {
        throw std::runtime_error("can't find tables directory");
    }
    std::ofstream file_out;
    file_out.open("output.txt");
    file_out.clear();
    bool first;
    for (const auto& filename: distance_table_names) {
        file_out << filename << "\n";
        std::string line{};
        std::ifstream table(filename);
        if (!table.is_open()) {
            throw std::runtime_error("cannot open csv file");
        }
        first = true;
        while (std::getline(table, line)) {
            if (first) {
                first = false;
                continue;
            }
            std::string l1 = line.substr(0, line.find(','));
            std::string l2 = line.substr(line.find(',') + 1);
            l2.pop_back();
            file_out << "{" << l1 << ", {";
            file_out << l2 << "}},\n";
        }
        first = true;
    }
    file_out.close();
    chdir(project_path.c_str());
}

void hashToCSV() {
    if(chdir((project_path+"/tables").c_str())!=0) {
        throw std::runtime_error("can't find tables directory");
    }
    std::ofstream file_out;
    for (const auto& filename: ballistic_table_names) {
        file_out.open(filename.substr(0,filename.size()-4)+"(new).csv");
        file_out.clear();
        std::string line{};
        std::ifstream table(filename);
        if (!table.is_open()) {
            throw std::runtime_error("cannot open csv file");
        }
        while (std::getline(table, line)) {
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            line.erase(std::remove(line.begin(), line.end(), '}'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '{'), line.end());
            file_out << line;
        }
        file_out.close();
    }
    chdir(project_path.c_str());
}

void readParamNames() {
    if (chdir((project_path + "/entries").c_str()) != 0) {
        throw std::runtime_error("can't find entry directory");
    }
    for (int i = 0; i < param_entry_names.size(); ++i) {
        const auto& name = param_entry_names[i];
        std::string line{};
        std::ifstream read(name);
        while (std::getline(read, line)) {
            switch(i) {
                case 0: {
                    param_entries_eng.push_back(line);
                    break;
                }
                case 1: {
                    param_entries_eng_short.push_back(line);
                    break;
                }
                case 2: {
                    param_entries_rus.push_back(line);
                    break;
                }
                case 3: {
                    param_entries_rus_short.push_back(line);
                    break;
                }
                default: return;
            }
        }
        read.close();
    }
    chdir(project_path.c_str());
}

void readTargetTypes() {
    chdir((project_path+"/entries").c_str());
    std::string line{};
    std::ifstream read1("target_types_eng.txt");
    if (!read1.is_open()) {
        throw std::runtime_error("cannot open csv file");
    }
    while (std::getline(read1, line)) {
        target_types_eng.insert(line);
    }
    read1.close();
    std::ifstream read2("target_types_rus.txt");
    if (!read2.is_open()) {
        throw std::runtime_error("cannot open csv file");
    }
    while (std::getline(read2, line)) {
        target_types_eng.insert(line);
    }
    read2.close();
    chdir(project_path.c_str());
}

void readBallisticTableData() {
    if (chdir((project_path + "/tables").c_str()) != 0) {
        throw std::runtime_error("can't find table directory");
    }
    for (const auto &name: ballistic_table_names) {
        std::unordered_map<int, std::vector<double>> current_map;
        std::string line{};
        std::ifstream table(name);
        bool first = true;
        if (!table.is_open()) {
            throw std::runtime_error("cannot open csv file");
        }
        while (std::getline(table, line)) {
            if(first){
                first = false;
                continue;
            }
            int dist = std::stoi(line.substr(0,line.find(',')));
            line = line.substr(line.find(',')+1);
            auto params = lineToDoubleVector(line);
            current_map[dist] = params;
        }
        first = true;
        ballistic_tables_map.push_back(current_map);
        table.close();
    }
    chdir(project_path.c_str());
}

void readMinDistTableData() {
    if (chdir((project_path + "/tables").c_str()) != 0) {
        throw std::runtime_error("can't find table directory");
    }
    for (const auto &name: distance_table_names) {
        std::unordered_map<int, std::vector<double>> current_map;
        std::string line{};
        std::ifstream table(name);
        bool first = true;
        if (!table.is_open()) {
            throw std::runtime_error("cannot open csv file");
        }
        while (std::getline(table, line)) {
            if(first){
                first = false;
                continue;
            }
            int dist = std::stoi(line.substr(0,line.find(',')));
            line = line.substr(line.find(',')+1);
            auto params = lineToDoubleVector(line);
            current_map[dist] = params;
        }
        distance_tables_map.push_back(current_map);
        table.close();
    }
    chdir(project_path.c_str());
}

void readTableData() {
    // filling the ballistic table
    readBallisticTableData();
    // filling the minimum distance table
    readMinDistTableData();
}

std::vector<double> calculateParameters(int a, const std::vector<double>& a_val, int b, const std::vector<double>& b_val, int c) {
    std::vector<double> c_val = {};
    for (int i = 0; i < a_val.size(); ++i) {
        c_val.push_back((b_val[i] - a_val[i]) * (c - a) / (b - a) + a_val[i]);
    }
    return c_val;
}

std::vector<std::pair<charge_type, int>> getMinDistances(int cover_d, int cover_h) {
    if(distance_tables_map.empty()) {
        throw std::runtime_error("min distance data not loaded into hash map");
    }
    std::vector<std::pair<charge_type, int>> min_distances;
    for (int i = 0; i < 6; ++i) {
        if (cover_d < 100 || cover_d > 1000 || cover_h < 5 || cover_h > 50) {
            break;
        }
        auto &map = distance_tables_map[i];
        int low_d, high_d;
        int low_h, high_h;
        if (map.find(cover_d) != map.end()) {
            if (!(cover_h % 5)) {
                min_distances.emplace_back(charge_type(2 * i), map.at(cover_d)[cover_h / 5 - 1]);
            } else {
                low_h = floor_k(cover_h, 5);
                high_h = ceil_k(cover_h, 5);
                auto d1 = static_cast<double>(cover_h - low_h);
                auto d2 = static_cast<double>(high_h - low_h);
                auto d3 = (map.at(cover_d)[high_h / 5 - 1] - map.at(cover_d)[low_h / 5 - 1]);
                d3 = d3 * d1 / d2 + map.at(cover_d)[low_h / 5 - 1];
                int dist = static_cast<int>(d3);
                min_distances.emplace_back(charge_type(2 * i), dist);
            }
        } else {
            low_d = floor_k(cover_d, 100);
            high_d = ceil_k(cover_d, 100);
            std::vector<double> distances = calculateParameters(low_d, map[low_d], high_d, map[high_d], cover_d);
            if(!(cover_h%5)) {
                min_distances.emplace_back(charge_type(2*i),distances[cover_h/5-1]);
            }
            else {
                low_h = floor_k(cover_h,5);
                high_h = ceil_k(cover_h,5);
                auto d1 = static_cast<double>(cover_h - low_h);
                auto d2 = static_cast<double>(high_h - low_h);
                auto d3 = (distances[high_h / 5 - 1] - distances[low_h / 5 - 1]) * d1 / d2;
                d3 = d3 * d1 / d2 + distances[low_h / 5 - 1];
                int dist = static_cast<int>(d3);
                min_distances.emplace_back(charge_type(2 * i), dist);
            }
        }
    }
    return min_distances;
}

void printMinDistances(const std::vector<std::pair<charge_type, int>>& distances) {
    for(const auto& p : distances) {
        std::cout << std::setw(15) << chargeTypeToString(p.first) << ": " << p.second << " m.\n";
    }
}

void printMinDistances(const std::vector<std::pair<charge_type, int>>& distances, std::ofstream& out) {
    for(const auto& p : distances) {
        out << std::setw(15) << chargeTypeToString(p.first) << ": " << p.second << " m.\n";
    }
}


std::vector<std::pair<charge_type, std::vector<double>>> getParameters(int distance) {
    std::vector<std::pair<charge_type, std::vector<double>>> params;
    for (int i = 0; i < 12; ++i) {
        if (distance < dist_boundaries[i].first || distance > dist_boundaries[i].second) {
            continue;
        }
        const auto &map = ballistic_tables_map[i];
        int low, high;
        if (map.find(distance) != map.end()) {
            params.emplace_back(charge_type(i), map.at(distance));
        } else {
            if (map.find(floor_k(distance, 100)) != map.end()) {
                low = floor_k(distance, 100);
            } else {
                low = floor_k(distance, 200);
            }
            if (map.find(ceil_k(distance, 100)) != map.end()) {
                high = ceil_k(distance, 100);
            } else {
                high = ceil_k(distance, 200);
            }
            if (is_debug_mode) {
                std::cout << "low and high distances available in the current map: " << low << ", " << high << "\n";
            }
            params.emplace_back(charge_type(i), calculateParameters(low, map.at(low), high, map.at(high), distance));
        }
    }
    return params;
}

std::vector<double> getParametersChargeType(int distance, charge_type charge) {
    auto all_params = getParameters(distance);
    for (const auto &p: all_params) {
        if (p.first == charge) {
            return p.second;
        }
    }
    // must be unreachable
    return {};
}

void printParameters(const std::vector<std::pair<charge_type, std::vector<double>>>& parameters) {
    for(const auto& p : parameters) {
        std::cout << std::setw(15) << chargeTypeToString(p.first) << ": ";
        for (int i = 1; i < p.second.size(); ++i) {
            if (i==1 || i==4 || i==6 || i==7 || i==16 || i==17 || i==19 || i==21) {
                std::cout << std::setw(10) << std::fixed << std::setprecision(1) << p.second[i];
            }
            else {
                std::cout << std::setw(10) << std::fixed << std::setprecision(2) << p.second[i];
            }
        }
        std::cout << "\n";
    }
}

void printParameters(const std::vector<std::pair<charge_type, std::vector<double>>>& parameters, std::ofstream& out) {
    for (const auto& p: parameters) {
        out << std::setw(15) << chargeTypeToString(p.first) << ": ";
        for (int i = 0; i < p.second.size(); ++i) {
            if (i==1 || i==4 || i==6 || i==7 || i==16 || i==17 || i==19 || i==21) {
                out << std::setw(10) << std::fixed << std::setprecision(1) << p.second[i];
            }
            else {
                out << std::setw(10) << std::fixed << std::setprecision(0) << p.second[i];
            }
        }
        out << "\n";
    }
}

bool isSameButMortar(charge_type t1, charge_type t2) {
    return ((t1 == lt_full && t2 == lt_full_mortar) ||
            (t1 == lt_reduced && t2 == lt_reduced_mortar) ||
            (t1 == lt_1st && t2 == lt_1st_mortar) ||
            (t1 == lt_2nd && t2 == lt_2nd_mortar) ||
            (t1 == lt_3rd && t2 == lt_3rd_mortar) ||
            (t1 == lt_4th && t2 == lt_4th_mortar));
}

charge_type convertIfMortar(charge_type t) {
    if (t == lt_full || t == lt_reduced || t == lt_1st || t == lt_2nd || t == lt_3rd || t == lt_4th) return t;
    switch (t) {
        case lt_full_mortar:
            return lt_full;
        case lt_reduced_mortar:
            return lt_reduced;
        case lt_1st_mortar:
            return lt_1st;
        case lt_2nd_mortar:
            return lt_2nd;
        case lt_3rd_mortar:
            return lt_3rd;
        case lt_4th_mortar:
            return lt_4th;
        default:
            throw std::runtime_error("never should have come here");
    }
}

std::vector<std::pair<charge_type, int>> getAimFromTable(int distance) {
    auto params = getParameters(distance);
    std::vector<std::pair<charge_type, int>> aims;
    for(const auto& p : params) {
        aims.emplace_back(charge_type(p.first), p.second[1]);
    }
    return aims;
}

int getAimChargeType(int distance, charge_type charge) {
    auto aims = getAimFromTable(distance);
    for (const auto& p: aims) {
        if (charge == p.first) {
            return p.second;
        }
    }
    return -1;
}

std::pair<int, int> getCoverDistHeight(const Mil& elev, double dist) {
    double alpha = elev.toRadians();
    int cover_h = static_cast<int>(dist * sin(alpha));
    int cover_d = static_cast<int>(dist * cos(alpha));
    return {cover_d, cover_h};
}

std::vector<std::pair<charge_type, int>> getMinAim(int cover_d, int cover_h) {
    auto min_distances = getMinDistances(cover_d, cover_h);
    std::vector<std::pair<charge_type, int>> min_aims;
    for (auto p : min_distances) {
        min_aims.emplace_back(p.first, getAimChargeType(p.second, p.first));
    }
    return min_aims;
}

std::vector<std::pair<charge_type, int>> getMinAim(const Mil& elev, double dist) {
    int cover_d = getCoverDistHeight(elev,dist).first;
    int cover_h = getCoverDistHeight(elev,dist).first;
    return getMinAim(cover_d, cover_h);
}

int getMinAimChargeType(int cover_d, int cover_h, charge_type lt) {
    auto min_distance = getMinDistances(cover_d, cover_h)[lt * 2].second;
    return getAimChargeType(min_distance, lt);
}

int getMinAimChargeType(const Mil& elev, double dist, charge_type lt) {
    int cover_d = getCoverDistHeight(elev,dist).first;
    int cover_h = getCoverDistHeight(elev,dist).first;
    return getMinAimChargeType(cover_d, cover_h, lt);
}

charge_type determineChargeType(int distance, const Mil& absolute_angle, const std::vector<std::tuple<Mil,Mil,int,int>>& covers,
                            const std::map<charge_type, unsigned int>& charges) {
    std::vector<std::pair<charge_type, int>> min_aims;
    std::vector<std::pair<charge_type, int>> aims = getAimFromTable(static_cast<int>(distance));
    // if the direction of fire for the considered target is intersecting with covers or mountains, the minimal aiming angles
    // will be calculated for the corresponding cover height, distance and width
    for (auto cover: covers) {
        auto dir_left = std::get<0>(cover);
        auto dir_right = std::get<1>(cover);
        auto cover_d = std::get<2>(cover);
        auto cover_h = std::get<3>(cover);
        if ((absolute_angle > dir_left) && (absolute_angle < dir_right)) {
            min_aims = getMinAim(cover_d, cover_h);
            break;
        }
    }
    // if the vector for minimal aims was not filled because the direction of fire is not intersecting with covers or mountains,
    // the aims for each of the charge types will be determined as 0
    if (min_aims.empty()) {
        for (int i = 0; i < 11; i += 2) {
            min_aims.emplace_back(static_cast<charge_type>(i), 0);
        }
    }
    std::vector<charge_type> possible_charges;
    // if the aiming angle for a specific charge is lower than the minimal possible aiming angle for that charge,
    // it will not be included in the vector of charge types allowed for firing
    for (auto &aim: aims) {
        for (auto &min_aim: min_aims) {
            if ((min_aim.first == aim.first || isSameButMortar(min_aim.first, aim.first)) &&
                (min_aim.second > aim.second)) {
                break;
            } else {
                possible_charges.push_back(aim.first);
                break;
            }
        }
    }
    if (possible_charges.empty()) {
        throw std::runtime_error("CANNOT FIRE AT THE TARGET! CHECK MINIMAL AIM TABLES");
    }
    auto available_charges = charges;
    // traversing the charge types from lowest i.e. 4th to highest i.e. full
    // the charge is picked if it is present in the map of available charges and
    // the distance coverage of +-800 meters is provided
    for (int i = possible_charges.size() - 1; i >= 0; --i) {
        auto pl = possible_charges[i];
        if (available_charges[convertIfMortar(pl)] &&
            distance + 800 <= dist_boundaries[pl].second &&
            distance - 800 >= dist_boundaries[pl].first) {
            return pl;
        }
    }
    throw std::runtime_error("CANNOT FIRE AT THE TARGET! POSSIBLE REASONS: COVER TOO HIGH / LOADS NOT PRESENT");
}



