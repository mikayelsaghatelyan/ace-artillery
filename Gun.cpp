#include "Gun.h"

#include <utility>
#include "dependencies.h"

std::unordered_map<unsigned int, std::unordered_map<unsigned int, GunTargetParameters>> gun_target_parameters;

std::unordered_map<unsigned int, Gun> gun_map;

std::unordered_map<unsigned int, Target> target_map;

std::vector<GunTargetParameters> getGunTargetParameters() {
    std::vector<GunTargetParameters> all_parameters;
    for (const auto& outerPair : gun_target_parameters) {
        for (const auto& innerPair : outerPair.second) {
            all_parameters.push_back(innerPair.second);
        }
    }
    return all_parameters;
}

Gun::Gun(const double &gun_x, const double &gun_y, const double &gun_h) {
    if (!isValidX(gun_x)) {
        throw std::runtime_error("invalid gun x");
    }
    this->gun_x = gun_x;
    if (!isValidY(gun_y)) {
        throw std::runtime_error("invalid gun y");
    }
    this->gun_y = gun_y;
    if (!isValidH(gun_h)) {
        throw std::runtime_error("invalid gun h");
    }
    this->gun_h = gun_h;
    this->gun_ID = generateUniqueID();
    this->gun_name = "gun" + std::to_string(this->gun_ID);
    this->gun_description = "none";
    this->gun_charges = {
            {lt_full,    0},
            {lt_reduced, 0},
            {lt_1st,     0},
            {lt_2nd,     0},
            {lt_3rd,     0},
            {lt_4th,     0}
    };
    this->gun_covers.clear();
}

Gun::Gun(const double &gun_x, const double &gun_y, const double &gun_h,
         const Mil &gun_dir, const Mil &gun_dir_main, const Mil &gun_dir_res, const Mil &gun_dir_night,
         const std::string &name, const std::string &description, const std::map<charge_type, unsigned int> &charges,
         const std::vector<std::tuple<Mil, Mil, int, int>> &covers) {
    if (!isValidX(gun_x)) {
        throw std::runtime_error("invalid gun x");
    }
    this->gun_x = gun_x;
    if (!isValidY(gun_y)) {
        throw std::runtime_error("invalid gun y");
    }
    this->gun_y = gun_y;
    if (!isValidH(gun_h)) {
        throw std::runtime_error("invalid gun h");
    }
    this->gun_h = gun_h;
    if (name.size() > 100) {
        throw std::runtime_error("gun name too long");
    }
    this->gun_name = name;
    if (description.size() > 1000) {
        throw std::runtime_error("gun description too long");
    }
    this->gun_charges = charges;
    this->gun_covers = covers;
    this->gun_description = description;
    this->gun_dir = gun_dir;
    this->gun_dir_main = gun_dir_main;
    this->gun_dir_res = gun_dir_res;
    this->gun_dir_night = gun_dir_night;
    this->gun_h = gun_h;
    this->gun_ID = generateUniqueID();
}

Gun::Gun(const double &gun_x, const double &gun_y, const double &gun_h,
         const Mil &gun_dir, const Mil &gun_dir_main, const Mil &gun_dir_res, const Mil &gun_dir_night) {
    if (!isValidX(gun_x)) {
        throw std::runtime_error("invalid gun x");
    }
    this->gun_x = gun_x;
    if (!isValidY(gun_y)) {
        throw std::runtime_error("invalid gun y");
    }
    this->gun_y = gun_y;
    if (!isValidH(gun_h)) {
        throw std::runtime_error("invalid gun h");
    }
    gun_charges = {
            {lt_full,    0},
            {lt_reduced, 0},
            {lt_1st,     0},
            {lt_2nd,     0},
            {lt_3rd,     0},
            {lt_4th,     0}
    };
    this->gun_covers.clear();
    this->gun_h = gun_h;
    this->gun_dir = gun_dir;
    this->gun_dir_main = gun_dir_main;
    this->gun_dir_res = gun_dir_res;
    this->gun_dir_night = gun_dir_night;
    this->gun_ID = generateUniqueID();
    this->gun_name = "gun" + std::to_string(this->gun_ID);
    this->gun_description = "none";
}

Gun::Gun(const double &gun_x, const double &gun_y, const double &gun_h,
         const std::string &name, const std::string &description) {
    if (!isValidX(gun_x)) {
        throw std::runtime_error("invalid gun x");
    }
    this->gun_x = gun_x;
    if (!isValidY(gun_y)) {
        throw std::runtime_error("invalid gun y");
    }
    this->gun_y = gun_y;
    if (!isValidH(gun_h)) {
        throw std::runtime_error("invalid gun h");
    }
    this->gun_h = gun_h;
    if (name.size() > 100) {
        throw std::runtime_error("gun name too long");
    }
    this->gun_name = name;
    if (description.size() > 1000) {
        throw std::runtime_error("gun description too long");
    }
    gun_charges = {
            {lt_full,    0},
            {lt_reduced, 0},
            {lt_1st,     0},
            {lt_2nd,     0},
            {lt_3rd,     0},
            {lt_4th,     0}
    };
    this->gun_covers.clear();
    this->gun_description = description;
    this->gun_dir = 0;
    this->gun_dir_main = 0;
    this->gun_dir_res = 0;
    this->gun_dir_night = 0;
    std::cout << "WARNING! The gun is not oriented";
    this->gun_ID = generateUniqueID();
    this->gun_name = "gun" + std::to_string(this->gun_ID);
    this->gun_description = "none";
}

Gun::Gun() {
    this->gun_x = (SK_42_X_MIN+SK_42_X_MAX)/2;
    this->gun_y = (SK_42_Y_MIN+SK_42_Y_MAX)/2;
    this->gun_h = (SK_42_H_MIN+SK_42_H_MAX)/2;
    this->gun_ID = -1;
    this->gun_dir = 0;
    this->gun_dir_main = 0;
    this->gun_dir_res = 0;
    this->gun_dir_night = 0;
    this->gun_name = "no_name";
    this->gun_description = "no_description";
    gun_charges = {
            {lt_full,    0},
            {lt_reduced, 0},
            {lt_1st,     0},
            {lt_2nd,     0},
            {lt_3rd,     0},
            {lt_4th,     0}
    };
    this->gun_covers.clear();
}

void Gun::addTarget(Target &tgt) {
    charge_type charge = determineChargeType(
            static_cast<int>(calcDistance(tgt.getTargetX(), tgt.getTargetY(), this->getGunX(), this->getGunY())),
            calcAbsAngleMil(tgt.getTargetX(), tgt.getTargetY(), this->getGunX(), this->getGunY()),
            this->getCovers(), this->getCharges());
    this->addTarget(tgt, charge);
}

void Gun::addTarget(Target &tgt, charge_type charge) {
    std::shared_ptr<Target> tgt_ptr(new Target(tgt));
    Gun &gun_ptr = *this;
    GunTargetParameters params(tgt_ptr, gun_ptr, charge);
    if (gun_target_parameters[this->getGunID()].empty()) {
        std::unordered_map<unsigned int, GunTargetParameters> param_map;
        param_map.insert(std::make_pair(tgt.getTargetID(), params));
        gun_target_parameters[this->getGunID()] = param_map;
    } else {
        gun_target_parameters[this->getGunID()].insert(std::make_pair(tgt.getTargetID(), params));
    }
}

void Gun::removeTarget(Target &tgt) {
    if (gun_target_parameters[this->getGunID()].empty()) {
        throw std::runtime_error("can't remove: no targets are bound to the gun");
    }
    if (gun_target_parameters[this->getGunID()].find(tgt.getTargetID()) ==
        gun_target_parameters[this->getGunID()].end()) {
        throw std::runtime_error("can't remove: target not bound to a gun");
    }
    gun_target_parameters[this->getGunID()].erase(tgt.getTargetID());
}

void Gun::removeAllTargets() {
    if (gun_target_parameters[this->getGunID()].empty()) {
        throw std::runtime_error("can't remove: no targets are bound to the gun");
    }
    gun_target_parameters[this->getGunID()].clear();
}

void Gun::updateTargetParameters() {
    if (gun_target_parameters.find(this->getGunID()) != gun_target_parameters.end()) {
        for (auto &p: gun_target_parameters.at(this->getGunID())) {
            p.second.updateParameters();
        }
    }
}

void Gun::setCovers(const std::vector<std::tuple<Mil, Mil, int, int>> &covers) {
    this->gun_covers = covers;
}

void Gun::addCover(const Mil &direction, int cover_d, int cover_h, int cover_w) {
    if ((cover_d < 100 || cover_d > 1000) || (cover_h < 5 || cover_h > 50)) {
        throw std::runtime_error("invalid cover distance/height");
    }
    auto d = static_cast<double>(cover_d);
    auto w = static_cast<double>(cover_w);
    double side = sqrt(d * d + (w / 2) * (w / 2));
    double angle = asin(w / (2 * side));
    Mil dir_left = direction - Mil(angle);
    Mil dir_right = direction + Mil(angle);
    this->gun_covers.emplace_back(dir_left, dir_right, cover_d, cover_h);
}

void Gun::addCover(const Mil &dir_left, const Mil &dir_right, const Mil &elev, double dist) {
    auto cover_d = getCoverDistHeight(elev, dist).first;
    auto cover_h = getCoverDistHeight(elev, dist).second;
    if ((cover_d < 100 || cover_d > 1000) || (cover_h < 5 || cover_h > 50)) {
        throw std::runtime_error("invalid cover distance/height");
    }
    this->gun_covers.emplace_back(dir_left, dir_right, cover_d, cover_h);
}

void Gun::setCharges(const std::map<charge_type, unsigned int> &charges) {
    this->gun_charges = charges;
}

void Gun::addCharge(charge_type charge, int quantity) {
    if (charge % 2) {
        throw std::runtime_error("invalid charge type (mortar-fire subdivisions not allowed)");
    }
    this->gun_charges[charge] += quantity;
}

void Gun::subCharge(charge_type charge, int quantity) {
    if (charge % 2) {
        throw std::runtime_error("invalid charge type (mortar-fire charge type subdivisions not allowed)");
    }
    if (this->gun_charges[charge] < quantity) {
        throw std::runtime_error("cannot subtract quantity greater than number of remaining charges");
    }
    this->gun_charges[charge] -= quantity;
}

void Gun::setGunX(const double &val) {
    if (!isValidX(val)) {
        throw std::runtime_error("invalid gun x");
    }
    this->gun_x = val;
    this->gun_covers.clear();
    updateTargetParameters();
}

void Gun::setGunY(const double &val) {
    if (!isValidY(val)) {
        throw std::runtime_error("invalid gun y");
    }
    this->gun_y = val;
    this->gun_covers.clear();
    updateTargetParameters();

}

void Gun::setGunH(const double &val) {
    if (!isValidH(val)) {
        throw std::runtime_error("invalid gun h");
    }
    this->gun_h = val;
    this->gun_covers.clear();
    updateTargetParameters();
}

void Gun::setAbsoluteDirection(const Mil &dir) {
    this->gun_dir = dir;
    updateTargetParameters();
}

void Gun::setAbsoluteDirection(double dir) {
    Mil dir_mil(dir);
    this->gun_dir = dir_mil;
    updateTargetParameters();
}

void Gun::setDirection(const Mil &dir_main) {
    this->gun_dir_main = dir_main;
    updateTargetParameters();
}

void Gun::setDirection(const Mil &dir_main, const Mil &dir_res) {
    this->gun_dir_main = dir_main;
    this->gun_dir_res = dir_res;
    updateTargetParameters();
}

void Gun::setDirection(const Mil &dir_main, const Mil &dir_res, const Mil &dir_night) {
    this->gun_dir_main = dir_main;
    this->gun_dir_res = dir_res;
    this->gun_dir_night = dir_night;
    updateTargetParameters();
}

void Gun::setGunName(const std::string &name) {
    if (name.size() > 100) {
        throw std::runtime_error("gun name too long");
    }
    this->gun_name = name;
}

void Gun::setGunDescription(const std::string &description) {
    if (description.size() > 1000) {
        throw std::runtime_error("gun description too long");
    }
    this->gun_description = description;
}

Mil Gun::getDirectionAbs() const {
    return this->gun_dir;
}

Mil Gun::getDirectionMain() const {
    return this->gun_dir_main;
}

Mil Gun::getDirectionRes() const {
    return this->gun_dir_res;
}

Mil Gun::getDirectionNight() const {
    return this->gun_dir_night;
}

double Gun::getGunX() const {
    return this->gun_x;
}

double Gun::getGunY() const {
    return this->gun_y;
}

double Gun::getGunH() const {
    return this->gun_h;
}

unsigned int Gun::getGunID() const {
    return this->gun_ID;
}

std::string Gun::getGunName() const {
    return this->gun_name;
}

std::string Gun::getGunDescription() const {
    return this->gun_description;
}

unsigned int Gun::getTargetNumber() const {
    if (gun_target_parameters.find(this->getGunID()) != gun_target_parameters.end()) {
        return gun_target_parameters[this->getGunID()].size();
    } else return 0;
}

std::map<charge_type, unsigned int> Gun::getCharges() const {
    return this->gun_charges;
}

std::vector<std::tuple<Mil, Mil, int, int>> Gun::getCovers() const {
    return this->gun_covers;
}

std::unordered_map<unsigned int, GunTargetParameters> Gun::getTargets() const {
    if (gun_target_parameters.find(this->getGunID()) != gun_target_parameters.end()) {
        return gun_target_parameters[this->getGunID()];
    } else {
        std::cout << "GUN " << std::setw(8) << std::setfill('0') << this->getGunID() << " HAS NO TARGETS";
    }
}

void Gun::printGunInfo() {
    std::cout << "Gun ID: " << std::setw(8) << std::setfill('0') << this->gun_ID << "\n";
    std::cout << "Gun name: " << this->gun_name << "\n";
    std::cout << "x: " << std::fixed << std::setprecision(0) << this->gun_x << "\n";
    std::cout << "y: " << std::fixed << std::setprecision(0) << this->gun_y << "\n";
    std::cout << "h: " << std::fixed << std::setprecision(0) << this->gun_h << "\n";
    std::cout << "Direction w.r.t. South:              " << this->gun_dir << "\n";
    std::cout << "Direction w.r.t. Main    Ref. Point: " << this->gun_dir_main << "\n";
    std::cout << "Direction w.r.t. Reserve Ref. Point: " << this->gun_dir_res << "\n";
    std::cout << "Direction w.r.t. Night   Ref. Point: " << this->gun_dir_night << "\n";
    std::cout << "Number of targets: " << this->getTargetNumber() << "\n";
}

void Gun::printTargetParameters(bool adv_mode) {
    if (gun_target_parameters.find(this->getGunID()) != gun_target_parameters.end()) {
        std::cout << "PRINTING TARGETS FOR " << this->gun_name << ":\n\n";
        for (auto &tp: gun_target_parameters[this->getGunID()]) {
            tp.second.consolePrint(adv_mode);
        }
    } else {
        std::cout << "NOTHING TO PRINT!\n";
    }
}


GunTargetParameters::GunTargetParameters(const std::shared_ptr<Target>& target, Gun &gun, charge_type charge) : tp_gun(gun) {
    this->tp_target = target;
    this->tp_gun = gun;
    this->tp_gun_id = gun.getGunID();
    this->tp_target_id = target->getTargetID();
    this->tp_charge = charge;
    this->tp_distance = calculateDistance();
    this->tp_level = calculateLevel();
    int distance = static_cast<int>(tp_distance);
    this->tp_elevation = getAimChargeType(distance, this->tp_charge);
    this->tp_azimuth_abs = calculateAngleMil();
    this->tp_azimuth_turn = calculateTurn();
    this->tp_azimuth_main = this->tp_gun.getDirectionMain() + this->tp_azimuth_turn;
    this->tp_azimuth_res = this->tp_gun.getDirectionRes() + this->tp_azimuth_turn;
    this->tp_azimuth_night = this->tp_gun.getDirectionNight() + this->tp_azimuth_turn;
    this->tp_ballistic_parameters = getParametersChargeType(distance, this->tp_charge);
}

GunTargetParameters::GunTargetParameters(const std::shared_ptr<Target>& target, Gun &gun) : tp_gun(gun) {
    this->tp_target = target;
    this->tp_gun = gun;
    this->tp_gun_id = gun.getGunID();
    this->tp_target_id = target->getTargetID();
    this->tp_distance = calculateDistance();
    this->tp_level = calculateLevel();
    this->tp_azimuth_turn = calculateTurn();
    this->tp_azimuth_main = this->tp_gun.getDirectionMain() + this->tp_azimuth_turn;
    this->tp_azimuth_res = this->tp_gun.getDirectionRes() + this->tp_azimuth_turn;
    this->tp_azimuth_night = this->tp_gun.getDirectionNight() + this->tp_azimuth_turn;
    this->tp_charge = determineChargeType(static_cast<int>(this->tp_distance), this->tp_azimuth_abs,
                                          this->tp_gun.getCovers(), this->tp_gun.getCharges());
    int distance = static_cast<int>(tp_distance);
    this->tp_elevation = getAimChargeType(distance, this->tp_charge);
    this->tp_azimuth_abs = calculateAngleMil();
    this->tp_ballistic_parameters = getParametersChargeType(distance, this->tp_charge);
}

GunTargetParameters::GunTargetParameters(Gun &gun) : tp_gun(gun) {

}

void GunTargetParameters::setChargeType(charge_type charge) {
    this->tp_charge = charge;
    updateParameters();
}

void GunTargetParameters::updateParameters() {
    this->tp_distance = calculateDistance();
    this->tp_level = calculateLevel();
    int distance = static_cast<int>(tp_distance);
    this->tp_elevation = getAimChargeType(distance, this->tp_charge);
    this->tp_azimuth_turn = calculateTurn();
    this->tp_azimuth_main = this->tp_gun.getDirectionMain() + this->tp_azimuth_turn;
    this->tp_azimuth_res = this->tp_gun.getDirectionRes() + this->tp_azimuth_turn;
    this->tp_azimuth_night = this->tp_gun.getDirectionNight() + this->tp_azimuth_turn;
    this->tp_ballistic_parameters = getParametersChargeType(distance, this->tp_charge);
}

void GunTargetParameters::consolePrint(bool adv_mode) {
    std::cout << "Target ID: " << std::setw(8) << std::setfill('0') << this->tp_target->getTargetID() << "\n";
    std::cout << "Name: " << this->tp_target->getTargetName() << "\n";
    std::cout << "Description: " << this->tp_target->getTargetDescription() << "\n";
    std::cout << "x: " << this->tp_target->getTargetX() << "\n";
    std::cout << "y: " << this->tp_target->getTargetY() << "\n";
    std::cout << "h: " << this->tp_target->getTargetH() << "\n";
    std::cout << "Distance: " << this->tp_distance << "m\n";
    std::cout << "Direction w.r.t. South               " << this->tp_azimuth_abs << "\n";
    std::cout << "Direction w.r.t. Main    Ref. Point: " << this->tp_azimuth_main << "\n";
    std::cout << "Direction w.r.t. Reserve Ref. Point: " << this->tp_azimuth_res << "\n";
    std::cout << "Direction w.r.t. Night   Ref. Point: " << this->tp_azimuth_night << "\n";
    std::cout << "Angle turn: " << ((this->tp_azimuth_turn >= 0) ? ("Right ") : ("Left "))
              << Mil(abs(this->tp_azimuth_turn)) << "\n";
    std::cout << "Aiming angle: " << this->tp_elevation << "\n";
    std::cout << "Level: " << this->tp_level << "\n";
    std::cout << "Charge type: " << chargeTypeToString(this->tp_charge) << "\n\n";
    if (adv_mode) {
        std::cout << "Advanced ballistic parameters:\n";
        printBallisticParameters(true, true);
    }
    std::cout << "\n\n\n";
}

void GunTargetParameters::formatPrintParams(const std::vector<double> &params, int i) {
    if (i == 1 || i == 4 || i == 6 || i == 7 || i == 16 || i == 17 || i == 19 || i == 21) {
        std::cout << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(1) << params[i];
    } else {
        std::cout << std::setw(5) << std::fixed << std::setprecision(2) << params[i];
    }
    std::cout << "\n";
}

/**
 * @brief returns random battery coordinates
 * @param lang - language of the entries:   0 - english,        1 - russian
 * @param mode - length of the entries:     0 - short entries,  1 - full entries
 */
void GunTargetParameters::printBallisticParameters(bool lang, bool mode) {
    auto &params = this->tp_ballistic_parameters;
    if (lang) {
        if (mode) {
            for (int i = 1; i < 22; ++i) {
                std::cout << param_entries_eng[i] << ": ";
                formatPrintParams(params, i - 1);
            }
        } else {
            for (int i = 1; i < 22; ++i) {
                std::cout << param_entries_eng_short[i] << ": ";
                formatPrintParams(params, i - 1);
            }
        }
    } else {
        if (mode) {
            for (int i = 1; i < 22; ++i) {
                std::cout << param_entries_rus[i] << ": ";
                formatPrintParams(params, i - 1);
            }
        } else {
            for (int i = 1; i < 22; ++i) {
                std::cout << param_entries_rus_short[i] << ": ";
                formatPrintParams(params, i - 1);
            }
        }
    }
}

double GunTargetParameters::calculateDistance() {
    double dX = this->tp_target->getTargetX() - this->tp_gun.getGunX();
    double dY = this->tp_target->getTargetY() - this->tp_gun.getGunY();
    if (dX * dX + dY * dY < 200) {
        throw std::runtime_error("distance too small");
    }
    return sqrt(dX * dX + dY * dY);
}

Mil GunTargetParameters::calculateLevel() {
    double dist = this->calculateDistance();
    double dH = this->tp_target->getTargetH() - this->tp_gun.getGunH();
    return Mil(atan(dH / dist)) + 3000;
}

int GunTargetParameters::calculateAngleInt() {
    double dX = this->tp_target->getTargetX() - this->tp_gun.getGunX();
    double dY = this->tp_target->getTargetY() - this->tp_gun.getGunY();
    auto target_angle = static_cast<int>(atan(dY / dX) * 3000 / constants::pi) + ((dY > 0) ? 1500 : 4500);
    return target_angle;
}

Mil GunTargetParameters::calculateAngleMil() {
    return Mil(calculateAngleInt());
}

int GunTargetParameters::calculateTurn() {
    int dir = this->tp_gun.getDirectionAbs().getFirst() * 100 + this->tp_gun.getDirectionAbs().getSecond();
    int turn = dir - calculateAngleInt();
    if (turn > 3000) turn -= 6000;
    if (turn < -3000) turn += 6000;
    return turn;
}

int GunTargetParameters::getAzimuthTurn() const {
    return this->tp_azimuth_turn;
}

Mil GunTargetParameters::getAzimuthAbs() const {
    return this->tp_azimuth_abs;
}

Mil GunTargetParameters::getAzimuthMain() const {
    return this->tp_azimuth_main;
}

Mil GunTargetParameters::getAzimuthRes() const {
    return this->tp_azimuth_res;
}

Mil GunTargetParameters::getAzimuthNight() const {
    return this->tp_azimuth_night;
}

int GunTargetParameters::getElevation() const {
    return this->tp_elevation;
}

Mil GunTargetParameters::getLevel() const {
    return this->tp_level;
}

charge_type GunTargetParameters::getCharge() const {
    return this->tp_charge;
}

double GunTargetParameters::getDistance() const {
    return this->tp_distance;
}

Gun &GunTargetParameters::getGunReference() const {
    return this->tp_gun;
}

std::shared_ptr<Target> GunTargetParameters::getTargetPointer() const {
    return this->tp_target;
}

std::vector<double> GunTargetParameters::getBallisticParameters() const {
    return this->tp_ballistic_parameters;
}

unsigned int GunTargetParameters::getGunID() const {
    return this->tp_target_id;
}

unsigned int GunTargetParameters::getTargetID() const {
    return this->tp_gun_id;
}

void GunTargetParameters::setTarget(unsigned int target_id) {
    if(target_map.find(target_id)!=target_map.end()) {
       this->tp_target = std::make_shared<Target>(target_map.at(target_id));
    }
    else {
        throw std::runtime_error("can't manually set target for parameters object: no such target");
    }
}

void GunTargetParameters::setGun(unsigned int gun_id) {
    if(gun_map.find(gun_id)!=gun_map.end()) {
        this->tp_gun = gun_map.at(gun_id);
    }
    else {
        throw std::runtime_error("can't manually set gun for parameters object: no such gun");
    }
}

void GunTargetParameters::setDistance(double distance) {
    this->tp_distance = distance;
}

void GunTargetParameters::setAzimuthAbs(const Mil& azimuth_abs) {
    this->tp_azimuth_abs = azimuth_abs;
}

void GunTargetParameters::setAzimuthMain(const Mil& azimuth_main) {
    this->tp_azimuth_main = azimuth_main;
}

void GunTargetParameters::setAzimuthRes(const Mil& azimuth_res) {
    this->tp_azimuth_res = azimuth_res;
}

void GunTargetParameters::setAzimuthNight(const Mil& azimuth_night) {
    this->tp_azimuth_night = azimuth_night;
}

void GunTargetParameters::setAzimuthTurn(int azimuth_turn) {
    this->tp_azimuth_turn = azimuth_turn;
}

void GunTargetParameters::setElevation(int elevation) {
    this->tp_elevation = elevation;
}

void GunTargetParameters::setLevel(const Mil& level) {
    this->tp_level = level;
}

void GunTargetParameters::setCharge(charge_type type) {
    this->tp_charge = type;
}

void GunTargetParameters::setBallisticParameters(const std::vector<double>& ballistic_params) {
    this->tp_ballistic_parameters = ballistic_params;
}


Target::Target(double tg_x, double tg_y, double tg_h, double tg_front, double tg_depth,
               const std::string &name, const std::string &description) {
    if (!isValidX(tg_x)) {
    }
    this->tg_x = tg_x;
    if (!isValidY(tg_y)) {
        throw std::runtime_error("invalid target y [constructor]");
    }
    this->tg_y = tg_y;
    if (!isValidH(tg_h)) {
        throw std::runtime_error("invalid target h [constructor]");
    }
    this->tg_h = tg_h;
    if (!isValidFront(tg_front)) {
        throw std::runtime_error("invalid target front [constructor]");
    }
    this->tg_front = tg_front;
    if (!isValidDepth(tg_depth)) {
        throw std::runtime_error("invalid target depth [constructor]");
    }
    this->tg_depth = tg_depth;
    if (name.size() > 100) {
        throw std::runtime_error("target name too long [constructor]");
    }
    if (description.size() > 1000) {
        throw std::runtime_error("target name too long [constructor]");
    }
    this->tg_description = description;
    this->tg_name = name;
    this->tg_ID = generateUniqueID();
}

Target::Target(double tg_x, double tg_y, double tg_h, double tg_front, double tg_depth) {
    if (!isValidX(tg_x)) {
        throw std::runtime_error("invalid target x [constructor]");
    }
    this->tg_x = tg_x;
    if (!isValidY(tg_y)) {
        throw std::runtime_error("invalid target y [constructor]");
    }
    this->tg_y = tg_y;
    if (!isValidH(tg_h)) {
        throw std::runtime_error("invalid target h [constructor]");
    }
    this->tg_h = tg_h;
    if (!isValidFront(tg_front)) {
        throw std::runtime_error("invalid target front");
    }
    this->tg_front = tg_front;
    if (!isValidDepth(tg_depth)) {
        throw std::runtime_error("invalid target depth");
    }
    this->tg_depth = tg_depth;
    this->tg_ID = generateUniqueID();
    this->tg_name = "target" + std::to_string(tg_ID);
    this->tg_description = "none";
}

Target::Target(double tg_x, double tg_y, double tg_h) {
    if (!isValidX(tg_x)) {
        throw std::runtime_error("invalid target x [constructor]");
    }
    this->tg_x = tg_x;
    if (!isValidY(tg_y)) {
        throw std::runtime_error("invalid target y [constructor]");
    }
    this->tg_y = tg_y;
    if (!isValidH(tg_h)) {
        throw std::runtime_error("invalid target h [constructor]");
    }
    this->tg_h = tg_h;
    this->tg_front = 100.;
    this->tg_depth = 100.;
    this->tg_ID = generateUniqueID();
    this->tg_name = "target" + std::to_string(tg_ID);
    this->tg_description = "none";
}


void Target::setTargetX(const int &val) {
    if (!isValidX(val)) {
        throw std::runtime_error("invalid target x [setter]");
    }
    this->tg_x = val;
    updateBoundGunParameters();
}

void Target::setTargetY(const int &val) {
    if (!isValidY(val)) {
        throw std::runtime_error("invalid target y [setter]");
    }
    this->tg_y = val;
    updateBoundGunParameters();
}

void Target::setTargetH(const int &val) {
    if (!isValidH(val)) {
        throw std::runtime_error("invalid target h [setter]");
    }
    this->tg_h = val;
    updateBoundGunParameters();
}

void Target::setTargetFront(double front) {
    if (!isValidFront(front)) {
        throw std::runtime_error("invalid target front [setter]");
    }
    this->tg_front = front;
}

void Target::setTargetDepth(double depth) {
    if (!isValidDepth(depth)) {
        throw std::runtime_error("invalid target depth [setter]");
    }
    this->tg_depth = depth;
}

double Target::getTargetX() const {
    return this->tg_x;
}

double Target::getTargetY() const {
    return this->tg_y;
}

double Target::getTargetH() const {
    return tg_h;
}

double Target::getTargetFront() const {
    return tg_front;
}

double Target::getTargetDepth() const {
    return tg_depth;
}

std::string Target::getTargetName() const {
    return this->tg_name;
}

std::string Target::getTargetDescription() const {
    return this->tg_description;
}

unsigned int Target::getTargetID() const {
    return this->tg_ID;
}

void Target::setTargetName(const std::string &name) {
    if (name.size() > 100) {
        throw std::runtime_error("target name too long");
    }
    this->tg_name = name;
}

void Target::setTargetDescription(const std::string &description) {
    if (description.size() > 1000) {
        throw std::runtime_error("target name too long");
    }
    this->tg_name = description;
}

void Target::updateBoundGunParameters() {
    for (auto &gun_params_pair: gun_target_parameters) {
        for (auto &tgt_param_pair: gun_params_pair.second) {
            if (tgt_param_pair.first == this->getTargetID()) {
                tgt_param_pair.second.updateParameters();
            }
        }
    }
}

std::unordered_map<unsigned int, GunTargetParameters> getAllGunParamsForTarget(Target &t) {
    std::unordered_map<unsigned int, GunTargetParameters> all_params_for_target;
    for (const auto &gun_params_pair: gun_target_parameters) {
        for (const auto &tgt_param_pair: gun_params_pair.second) {
            if (tgt_param_pair.first == t.getTargetID()) {
                all_params_for_target.insert(std::make_pair(gun_params_pair.first, tgt_param_pair.second));
            }
        }
    }
    return all_params_for_target;
}

int calcAbsAngle(double tg_x, double tg_y, double gun_x, double gun_y) {
    double dX = tg_x - gun_x;
    double dY = tg_y - gun_y;
    auto target_angle = static_cast<int>(atan(dY / dX) * 3000 / constants::pi) + ((dY > 0) ? 1500 : 4500);
    return target_angle;
}

Mil calcAbsAngleMil(double tg_x, double tg_y, double gun_x, double gun_y) {
    return Mil(calcAbsAngle(tg_x, tg_y, gun_x, gun_y));
}

double calcDistance(double tg_x, double tg_y, double gun_x, double gun_y) {
    double dX = tg_x - gun_x;
    double dY = tg_y - gun_y;
    if (dX * dX + dY * dY < 200) {
        throw std::runtime_error("distance too small");
    }
    return sqrt(dX * dX + dY * dY);
}