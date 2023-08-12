#ifndef ACE_ARTILLERY1_0_DATA_H
#define ACE_ARTILLERY1_0_DATA_H

#include "dependencies.h"

extern bool is_debug_mode;
extern std::string project_path;

// charge types - full, reduced, 1st, 2nd, 3rd, 4th
enum charge_type : u_int8_t {
    lt_full,    lt_full_mortar,
    lt_reduced, lt_reduced_mortar,
    lt_1st,     lt_1st_mortar,
    lt_2nd,     lt_2nd_mortar,
    lt_3rd,     lt_3rd_mortar,
    lt_4th,     lt_4th_mortar
};

// coordinate boundaries for generating random points
const double SK_42_X_MIN = 4000000.0;
const double SK_42_X_MAX = 5000000.0;
const double SK_42_Y_MIN = 8000000.0;
const double SK_42_Y_MAX = 9000000.0;
const double SK_42_H_MIN = 1000.0;
const double SK_42_H_MAX = 4000.0;
const double FRONT_MAX = 5000.0;
const double DEPTH_MAX = 5000.0;

// target types kept as strings
extern const std::set<std::string> target_type_strings;

// short entries for ballistic parameters in russian
extern  std::vector<std::string> param_entries_rus_short;

// entries for ballistic parameters in russian
extern  std::vector<std::string> param_entries_rus;

// short entries for ballistic parameters in english
extern  std::vector<std::string> param_entries_eng_short;

// entries for ballistic parameters in english
extern  std::vector<std::string> param_entries_eng;

// names of military target types in english
extern std::set<std::string> target_types_eng;

// names of military target types in russian
extern std::set<std::string> target_types_rus;

// hash tables for keeping ballistic data
extern std::vector<std::unordered_map<int, std::vector<double>>> ballistic_tables_map;

// hash tables for keeping minimum distance data
extern std::vector<std::unordered_map<int, std::vector<double>>> distance_tables_map;

// names of csv files containing ballistic tables
extern const std::vector<std::string> ballistic_table_names;

// names of csv files containing minimum distance tables
extern const std::vector<std::string> distance_table_names;

// names of txt files containing ballistic parameter entries (short and long, in english and russian)
extern const std::vector<std::string> param_entry_names;

// minimal and maximal possible distances for each table (respective mortar-like fire tables after each type)
extern const std::vector<std::pair<int, int>> dist_boundaries;

void setDebugVariable(const bool& value);

struct Point {
    double pt_x;
    double pt_y;
    double pt_h;
    Point(double x, double y, double h);
    Point();
    Point& operator=(const Point& m);
};

struct BatteryPoints {
    std::vector<Point> bp_guns;
    double bp_front;
    double bp_depth;
    // battery coordinates - coordinates of the main gun - [n/2]-th gun
    double bp_x;
    double bp_y;
    double bp_h;
    BatteryPoints(const std::vector<Point>& guns, double front, double depth, double x, double y, double h);
};

// returns unique ID for Gun, GunTargetParameters and Target class objects
static unsigned int generateUniqueID() {
    static unsigned int counter = 0;
    return ++counter;
}

// reads target types from txt files and charges them into sets of strings
void readTargetTypes();

// reads data from txt files with short and long parameter names in english and russian and charges it into vector of strings
void readParamNames();

// reads data from csv files with ballistic tables and minimum distance tables and charges it into hash tables (unordered_map)
void readTableData();

// get random point based on SK-42 coordinates
Point getRandomPoint();

// get random point which will be inside specified radius of a given point, based on SK-42 coordinates
Point getRandomPoint(const Point& p, double radius);

Mil getRandomAngle();

Mil getRandomRefAngle(const Mil& m);

// checks whether double value is a valid SK-42 X coordinate
bool isValidX(double val);

// checks whether double value is a valid SK-42 Y coordinate
bool isValidY(double val);

// checks whether double value is a valid height
bool isValidH(double val);

// checks whether double value is a valid front
bool isValidFront(double val);

// checks whether double value is a valid depth
bool isValidDepth(double val);

// secondary function used in getRandomBattery() for calculating front and depth of the-random generated battery
std::pair<double, double> getBatteryFrontDepth(std::vector<Point>& proj_front, std::vector<Point>& proj_depth, int size);

// secondary function used in getRandomBattery() for debug-printing projection points
void printProjectionDebug(const std::vector<Point>& proj_front, const std::vector<Point>& proj_depth);

// secondary function used in getRandomBattery() for printing maximal and minimal points in front and depth projections
void printFrontDepthMaxMinDebug(const Point& pf_max, const Point& pf_min, const Point& pd_max, const Point& pd_min);

// generates several random points that can be used as gun battery for testing
BatteryPoints getRandomBattery(const double& angle, const int& n);

// prints front, depth, main gun coordinates, as well as coordinates of each gun
void printBattery(const BatteryPoints& battery);

// converts line from ballistic table csv files to a vector of parameters kept as double values
std::vector<double> lineToDoubleVector(std::string line);

// converts line from ballistic table csv files to a vector of parameters kept as integers
std::vector<int> lineToIntVector(std::string line);

// converts csv files with ballistic data to hash tables (unordered_map)
void ballisticTablesToHash();

// outputs contents of hash tables minimum distance data to csv files
void minDistTablesToHash();

// outputs contents of hath tables with ballistic data to csv files
void hashToCSV();

// calculates parameters for distances not present in the table based on a linear
std::vector<double> calculateParameters(int a, const std::vector<double>& a_val,
                                        int b, const std::vector<double>& b_val,
                                        int c);

// parameters for each charge type based on ballistic table data
std::vector<std::pair<charge_type, std::vector<double>>> getParameters(int distance);

// parameters for a specific charge type based on ballistic table data
std::vector<double> getParametersChargeType(int distance, charge_type charge);

// console output for ballistic parameters for each charge type
void printParameters(const std::vector<std::pair<charge_type, std::vector<double>>>& parameters);

// file output for ballistic parameters for each charge type
void printParameters(const std::vector<std::pair<charge_type, std::vector<double>>>& parameters, std::ofstream& out);

// closest number to n which is divisible by k and smaller than n (returns n if k|n)
int floor_k(int n, int k);
// closest number to n which is divisible by k and greater than n (returns n if k|n)
int ceil_k(int n, int k);

// calculates aim for given distance for each charge type
std::vector<std::pair<charge_type, int>> getAimFromTable(int distance);

// calculates aim for given distance for a certain charge type
int getAimChargeType(int distance, charge_type charge);

// minimum distances for each charge type based on distance to the cover and cover height
std::vector<std::pair<charge_type, int>> getMinDistances(int cover_d, int cover_h);

// console output for minimum distances for each charge type
void printMinDistances(const std::vector<std::pair<charge_type, int>>& parameters);

// file output for minimum distances for each charge type
void printMinDistances(const std::vector<std::pair<charge_type, int>>& parameters, std::ofstream& out);

// converts enumerated charge types to string
std::string chargeTypeToString(charge_type charge);

// converts string to enumerated charge type
charge_type stringToChargeType(const std::string& charge);

// calculates distance to cover base and cover height based on cover elevation angle and distance to cover peak
std::pair<int, int> getCoverDistHeight(const Mil& elev, double dist);

// calculates minimum aiming angle based on distance to cover and cover height for all charge types
std::vector<std::pair<charge_type, int>> getMinAim(int cover_d, int cover_h);

// calculates minimum aiming angle based on cover elevation angle and distance to cover peak for all charge types
std::vector<std::pair<charge_type, int>> getMinAim(const Mil& elev, double dist);

// calculates minimum aiming angle based on distance to cover and cover height for certain charge type
int getMinAimChargeType(int cover_d, int cover_h, charge_type lt);

// calculates minimum aiming angle based on cover elevation angle and distance to cover peak for certain charge type
int getMinAimChargeType(const Mil& elev, double dist, charge_type lt);

// returns if t1 is the same enumerated charge type as t2 but for mortar-like fire
bool isSameButMortar(charge_type t1, charge_type t2);

// converts mortar-like fire charge types to corresponding normal ones
charge_type convertIfMortar(charge_type t);

// determines the charge type for the target based on the distance, direction, presence of covers and mountains and presence of charges
charge_type determineChargeType(int distance, const Mil& absolute_angle, const std::vector<std::tuple<Mil,Mil,int,int>>& covers,
                            const std::map<charge_type, unsigned int>& charges);

#endif
