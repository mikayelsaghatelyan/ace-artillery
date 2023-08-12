#ifndef ACE_ARTILLERY1_0_GUN_H
#define ACE_ARTILLERY1_0_GUN_H

#include "dependencies.h"
#include "Data.h"

class Gun;
class Target;
class GunTargetParameters;

extern std::unordered_map<unsigned int, std::unordered_map<unsigned int, GunTargetParameters>> gun_target_parameters;

extern std::unordered_map<unsigned int, Gun> gun_map;

extern std::unordered_map<unsigned int, Target> target_map;

std::vector<GunTargetParameters> getGunTargetParameters();

class Gun {
private:
    unsigned int gun_ID;
    std::string gun_name;
    std::string gun_description;
    double gun_x;                                           // Gun Latitude (Northing) in CK-42 Coordinate System (X)
    double gun_y;                                           // Gun Longitude (Easting) in CK-42 Coordinate System (Y)
    double gun_h;                                           // Gun Altitude  (h)
    Mil gun_dir;                                            // Mission Azimuth
    Mil gun_dir_main;                                       // Mission Azimuth w.r.t. Main Reference Point    (MRP)
    Mil gun_dir_res;                                        // Mission Azimuth w.r.t. Reserve Reference Point (RRP)
    Mil gun_dir_night;                                      // Mission Azimuth w.r.t. Night Reference Point   (NRP)
    std::map<charge_type, unsigned int> gun_charges;        // List of charges with quantities for each charge type present
    std::vector<std::tuple<Mil,Mil,int,int>> gun_covers;    // List of terrain covers and obstacles with azimuth borders, distance and height
public:
    Gun(const double& gun_x, const double& gun_y, const double& gun_h,
        const Mil& gun_dir, const Mil& gun_dir_main, const Mil& gun_dir_res, const Mil& gun_dir_night,
        const std::string& name, const std::string& description, const std::map<charge_type, unsigned int>& charges,
        const std::vector<std::tuple<Mil,Mil,int,int>>& covers);
    Gun(const double& gun_x, const double& gun_y, const double& gun_h,
        const std::string& name, const std::string& description);
    Gun(const double& gun_x, const double& gun_y, const double& gun_h,
        const Mil& gun_dir, const Mil& gun_dir_main, const Mil& gun_dir_res, const Mil& gun_dir_night);
    Gun(const double& gun_x, const double& gun_y, const double& gun_h);
    Gun();

    void updateTargetParameters();
    void setCharges(const std::map<charge_type, unsigned int>& charges);
    void addCharge(charge_type lt, int quantity);
    void subCharge(charge_type lt, int quantity);
    void setCovers(const std::vector<std::tuple<Mil,Mil,int,int>>& covers);
    void addCover(const Mil& direction, int cover_d, int cover_h, int cover_w);
    void addCover(const Mil& dir_left, const Mil& dir_right, const Mil& elev, double dist);
    void setAbsoluteDirection(const Mil& dir);
    void setAbsoluteDirection(double dir);
    void setDirection(const Mil& dir_main);
    void setDirection(const Mil& dir_main, const Mil& dir_res);
    void setDirection(const Mil& dir_main, const Mil& dir_res, const Mil& dir_night);
    void addTarget(Target& tgt, charge_type charge);
    void addTarget(Target& tgt);
    void removeAllTargets();
    void removeTarget(Target& tgt);
    void setGunX(const double& val);
    void setGunY(const double& gun_Y);
    void setGunH(const double& gun_H);
    void setGunName(const std::string& name);
    void setGunDescription(const std::string& description);
    void printGunInfo();
    void printTargetParameters(bool adv_mode);
    [[nodiscard]] Mil getDirectionAbs() const;
    [[nodiscard]] Mil getDirectionMain() const;
    [[nodiscard]] Mil getDirectionRes() const;
    [[nodiscard]] Mil getDirectionNight() const;
    [[nodiscard]] double getGunX() const;
    [[nodiscard]] double getGunY() const;
    [[nodiscard]] double getGunH() const;
    [[nodiscard]] unsigned int getGunID() const;
    [[nodiscard]] unsigned int getTargetNumber() const;
    [[nodiscard]] std::string getGunName() const;
    [[nodiscard]] std::string getGunDescription() const;
    [[nodiscard]] std::map<charge_type, unsigned int> getCharges() const;
    [[nodiscard]] std::vector<std::tuple<Mil,Mil,int,int>> getCovers() const;
    [[nodiscard]] std::unordered_map<unsigned int, GunTargetParameters> getTargets() const;
};

class GunTargetParameters {
private:
    std::shared_ptr<Target> tp_target;              // Shared pointer to the target
    Gun& tp_gun;                                    // Reference to the gun to which the target is bound
    unsigned int tp_gun_id;                         // Gun ID
    unsigned int tp_target_id;                      // Target ID
    double tp_distance;                             // Target Distance
    Mil tp_azimuth_abs;                             // Target Azimuth w.r.t. True South (00-00 Mil) Counterclockwise
    Mil tp_azimuth_main;                            // Mission Azimuth w.r.t. Main Reference Point    (MRP)
    Mil tp_azimuth_res;                             // Mission Azimuth w.r.t. Reserve Reference Point (RRP)
    Mil tp_azimuth_night;                           // Mission Azimuth w.r.t. Night Reference Point   (NRP)
    int tp_azimuth_turn;                            // Azimuth Correction w.r.t. Mission Azimuth (Right for Positive, Left for Negative)
    int tp_elevation;                               // Elevation
    Mil tp_level;                                   // Level
    charge_type tp_charge;                          // Charge Type
    std::vector<double> tp_ballistic_parameters;    // Extended Ballistic Parameters for the target
    static void formatPrintParams(const std::vector<double>& params, int i);
public:
    GunTargetParameters(const std::shared_ptr<Target>& target, Gun& gun, charge_type charge);
    GunTargetParameters(const std::shared_ptr<Target>& target, Gun& gun);
    GunTargetParameters(Gun &gun);
    void updateParameters();
    void setChargeType(charge_type charge);
    void consolePrint(bool adv_mode);
    void printBallisticParameters(bool lang, bool mode);
    double calculateDistance();
    Mil calculateLevel();
    int calculateAngleInt();
    Mil calculateAngleMil();
    int calculateTurn();

    // manual setters for loading data
    void setTarget(unsigned int target_id);
    void setGun(unsigned int gun_id);
    void setDistance(double distance);
    void setAzimuthAbs(const Mil& azimuth_abs);
    void setAzimuthMain(const Mil& azimuth_main);
    void setAzimuthRes(const Mil& azimuth_res);
    void setAzimuthNight(const Mil& azimuth_night);
    void setAzimuthTurn(int azimuth_turn);
    void setElevation(int elevation);
    void setLevel(const Mil& level);
    void setCharge(charge_type type);
    void setBallisticParameters(const std::vector<double>& ballistic_params);

    [[nodiscard]] std::shared_ptr<Target> getTargetPointer() const;
    [[nodiscard]] Gun& getGunReference() const;
    [[nodiscard]] unsigned int getGunID() const;
    [[nodiscard]] unsigned int getTargetID() const;
    [[nodiscard]] int getAzimuthTurn() const;
    [[nodiscard]] Mil getAzimuthAbs() const;
    [[nodiscard]] Mil getAzimuthMain() const;
    [[nodiscard]] Mil getAzimuthRes() const;
    [[nodiscard]] Mil getAzimuthNight() const;
    [[nodiscard]] int getElevation() const;
    [[nodiscard]] Mil getLevel() const;
    [[nodiscard]] double getDistance() const;
    [[nodiscard]] charge_type getCharge() const;
    [[nodiscard]] std::vector<double> getBallisticParameters() const;
};

class Target {
private:
    unsigned int tg_ID;
    std::string tg_name;
    std::string tg_description;
    double tg_x;                        // Target Latitude (Northing) in CK-42 Coordinate System (X)
    double tg_y;                        // Target Longitude (Easting) in CK-42 Coordinate System (Y)
    double tg_h;                        // Target Altitude (h)
    double tg_front;                    // Target Front
    double tg_depth;                    // Target Depth
public:
    Target(double tg_x, double tg_y, double tg_h, double tg_front, double tg_depth, const std::string& name, const std::string& description);
    Target(double tg_x, double tg_y, double tg_h, double tg_front, double tg_depth);
    Target(double tg_x, double tg_y, double tg_h);
    void setTargetX(const int& val);
    void setTargetY(const int& val);
    void setTargetH(const int& val);
    void setTargetName(const std::string& name);
    void setTargetDescription(const std::string& description);
    void setTargetFront(double front);
    void setTargetDepth(double depth);
    void updateBoundGunParameters();
    [[nodiscard]] double getTargetX() const;
    [[nodiscard]] double getTargetY() const;
    [[nodiscard]] double getTargetH() const;
    [[nodiscard]] std::string getTargetName() const;
    [[nodiscard]] std::string getTargetDescription() const;
    [[nodiscard]] unsigned int getTargetID() const;
    [[nodiscard]] double getTargetFront() const;
    [[nodiscard]] double getTargetDepth() const;
};

int calcAbsAngle(double tg_x, double tg_y, double gun_x, double gun_y);

Mil calcAbsAngleMil(double tg_x, double tg_y, double gun_x, double gun_y);

double calcDistance(double tg_x, double tg_y, double gun_x, double gun_y);


std::unordered_map<unsigned int, GunTargetParameters> getAllGunParamsForTarget(Target& t);

#endif //ACE_ARTILLERY1_0_GUN_H
