#include "Process.h"
#include "dependencies.h"




std::string defineProjectPath() {
    fs::path executablePath = fs::current_path();
    fs::path projectPath = executablePath.parent_path();
    return projectPath.string();
}

void setProjectPath() {
    project_path = defineProjectPath();
}

void clearProjectPath() {
    project_path = "";
}

void saveData() {
    saveGunData();
    saveTargetData();
    saveTargetParameters();
}

void loadData() {
    loadGunData();
    loadTargetData();
    loadTargetParameters();
}

void saveGunData() {
    for(const auto& item : gun_map) {
        gunToJSON(item.second);
    }
}

void saveTargetData() {
    for(const auto& item : target_map) {
        targetToJSON(item.second);
    }
}

void saveTargetParameters() {
    for(const auto& outer_item : gun_target_parameters) {
        for(const auto& inner_item : outer_item.second) {
            targetParametersToJSON(inner_item.second);
        }
    }
}

void loadGunData() {

}

void loadTargetData() {

}

void loadTargetParameters() {

}

void insertGun() {

}

void insertTarget() {

}

void eraseGun(){

}

void eraseTarget(){

}

void assignTargetForGun(){

}

void dismissTargetForGun(){

}

void printGunMission(){

}

void printGuns(){

}

void printTargets(){

}

void clearData() {
    clearGunData();
    clearTargetData();
    clearParameterData();
}

void clearGunData() {
    std::cout << chdir((project_path+"/object_data/guns").c_str());
    for(const auto& entry: fs::directory_iterator(fs::current_path())) {
        fs::remove(entry.path());
    }
    chdir(project_path.c_str());
}

void clearTargetData() {
    chdir((project_path+"/object_data/targets").c_str());
    for(const auto& entry: fs::directory_iterator(fs::current_path())) {
        std::cout << "removing " << entry.path() << "\n";
        fs::remove(entry.path());
    }
    chdir(project_path.c_str());
}

void clearParameterData() {
    chdir((project_path+"/object_data/parameters").c_str());
    for(const auto& entry: fs::directory_iterator(fs::current_path())) {
        fs::remove(entry.path());
    }
    chdir(project_path.c_str());
}

json gunToJSON(const Gun& gun) {
    json gun_json;
    gun_json["id"] = gun.getGunID();
    gun_json["name"] = gun.getGunName();
    gun_json["description"] = gun.getGunDescription();
    gun_json["x"] = gun.getGunX();
    gun_json["y"] = gun.getGunY();
    gun_json["h"] = gun.getGunH();
    gun_json["direction absolute"] = static_cast<std::string>(gun.getDirectionAbs());
    gun_json["direction main"] = static_cast<std::string>(gun.getDirectionMain());
    gun_json["direction reserve"] = static_cast<std::string>(gun.getDirectionRes());
    gun_json["direction night"] = static_cast<std::string>(gun.getDirectionNight());
    gun_json["number of targets"] = gun.getTargetNumber();
    gun_json["gun charges"]["full"] = gun.getCharges()[lt_full];
    gun_json["gun charges"]["reduced"] = gun.getCharges()[lt_reduced];
    gun_json["gun charges"]["1st"] = gun.getCharges()[lt_1st];
    gun_json["gun charges"]["2nd"] = gun.getCharges()[lt_2nd];
    gun_json["gun charges"]["3rd"] = gun.getCharges()[lt_3rd];
    gun_json["gun charges"]["4th"] = gun.getCharges()[lt_4th];
    auto covers = gun.getCovers();
    for (int i = 0; i < covers.size(); ++i) {
        gun_json["covers"][i]["left"] = std::get<0>(covers[i]);
        gun_json["covers"][i]["right"] = std::get<1>(covers[i]);
        gun_json["covers"][i]["distance"] = std::get<2>(covers[i]);
        gun_json["covers"][i]["height"] = std::get<3>(covers[i]);
    }

    // saving JSON file to objects directory
    chdir((project_path+"/object_data/guns").c_str());
    std::string name = gun.getGunName();
    std::replace(name.begin(), name.end(), ' ', '_');
    std::ofstream out(name+".txt");
    out << gun_json.dump(4);
    chdir(project_path.c_str());
    return gun_json;
}

json targetToJSON(const Target& target) {
    json target_json;
    target_json["id"] = target.getTargetID();
    target_json["name"] = target.getTargetName();
    target_json["description"] = target.getTargetDescription();
    target_json["x"] = target.getTargetX();
    target_json["y"] = target.getTargetY();
    target_json["h"] = target.getTargetH();
    target_json["front"] = target.getTargetFront();
    target_json["depth"] = target.getTargetDepth();

    // saving JSON file to objects directory
    chdir((project_path+"/object_data/targets").c_str());
    std::string name = target.getTargetName();
    std::replace(name.begin(), name.end(), ' ', '_');
    std::ofstream out(name+".txt");
    out << target_json.dump(4);
    chdir(project_path.c_str());
    return target_json;
}

json targetParametersToJSON(const GunTargetParameters& params) {
    json params_json;
    params_json["distance"] = params.getDistance();
    params_json["azimuth absolute"] = params.getAzimuthAbs();
    params_json["azimuth main"] = params.getAzimuthAbs();
    params_json["azimuth reserve"] = params.getAzimuthAbs();
    params_json["azimuth turn"] = params.getAzimuthAbs();
    params_json["target id"] = params.getTargetID();
    params_json["gun id"] = params.getGunID();
    params_json["elevation"] = params.getElevation();
    params_json["level"] = params.getLevel();
    params_json["charge"] = params.getCharge();
    auto ballistic_parameters = params.getBallisticParameters();
    for (int i = 0; i < ballistic_parameters.size(); ++i) {
        params_json["ballistic parameters"][i] = ballistic_parameters[i];
    }

    // saving JSON file to objects directory
    chdir((project_path + "/object_data/parameters").c_str());
    std::string name = "params_tgt_" + std::to_string(params.getTargetID());
    name += "_for_gun_" + std::to_string(params.getGunID());
    std::replace(name.begin(), name.end(), ' ', '_');
    std::ofstream out(name + ".txt");
    out << params_json.dump(4);
    chdir(project_path.c_str());
    return params_json;
}

Gun gunFromJSON(const std::string& json_filename) {
    json gun_json = json_filename;
    double gun_x = gun_json["x"];
    double gun_y = gun_json["y"];
    double gun_h = gun_json["h"];
    Mil gun_dir((std::string) gun_json["direction absolute"]);
    Mil gun_dir_main((std::string) gun_json["direction main"]);
    Mil gun_dir_res((std::string) gun_json["direction reserve"]);
    Mil gun_dir_night((std::string) gun_json["direction night"]);
    std::string name = gun_json["name"];
    std::string description = gun_json["description"];

    std::map<charge_type, unsigned int> charges;
    charges[lt_full] = gun_json["gun charges"]["full"];
    charges[lt_reduced] = gun_json["gun charges"]["reduced"];
    charges[lt_1st] = gun_json["gun charges"]["1st"];
    charges[lt_2nd] = gun_json["gun charges"]["2nd"];
    charges[lt_3rd] = gun_json["gun charges"]["3rd"];
    charges[lt_4th] = gun_json["gun charges"]["4th"];

    std::vector<std::tuple<Mil, Mil, int, int>> covers;
    for (const auto &cover: gun_json["covers"]) {
        Mil left((std::string) cover["left"]);
        Mil right((std::string) cover["right"]);
        int distance = cover["distance"];
        int height = cover["height"];
        covers.emplace_back(left, right, distance, height);
    }
    Gun g(gun_x, gun_y, gun_h, gun_dir, gun_dir_main, gun_dir_res, gun_dir_night,
          name, description, charges, covers);
    return g;
}


Target targetFromJSON (const std::string& json_filename) {
    json target_json = json_filename;
    double tg_x = target_json["x"];
    double tg_y = target_json["y"];
    double tg_h = target_json["h"];
    double tg_front = target_json["front"];
    double tg_depth = target_json["depth"];
    std::string name = target_json["name"];
    std::string description = target_json["description"];
    Target t(tg_x, tg_y, tg_h, tg_front, tg_depth, name, description);
    return t;
}

GunTargetParameters targetParametersFromJSON(const std::string& json_filename) {
    json params_json = json_filename;
    unsigned int target_id = params_json["target id"];
    unsigned int gun_id = params_json["gun id"];
    double distance = params_json["distance"];
    Mil azimuth_abs((std::string) params_json["azimuth absolute"]);
    Mil azimuth_main((std::string) params_json["azimuth main"]);
    Mil azimuth_res((std::string) params_json["azimuth reserve"]);
    int azimuth_turn = params_json["azimuth turn"];
    int elevation = params_json["elevation"];
    Mil level((std::string) params_json["level"]);
    charge_type charge = params_json["charge"];
    std::vector<double> ballistic_parameters;
    for (const auto &param: params_json["ballistic parameters"]) {
        ballistic_parameters.push_back(param);
    }
    Gun g;
    GunTargetParameters params(g);
    params.setTarget(target_id);
    params.setGun(gun_id);
    params.setDistance(distance);
    params.setAzimuthAbs(azimuth_abs);
    params.setAzimuthMain(azimuth_main);
    params.setAzimuthRes(azimuth_res);
    params.setAzimuthTurn(azimuth_turn);
    params.setElevation(elevation);
    params.setLevel(level);
    params.setCharge(charge);
    params.setBallisticParameters(ballistic_parameters);
    return params;
}
