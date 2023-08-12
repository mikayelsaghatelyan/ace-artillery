#include "dependencies.h"
#include "Process.h"
#include "Gun.h"
#include "Data.h"


void init() {
    setProjectPath();
    readTableData();
    readParamNames();
    readTargetTypes();
    BatteryPoints bpt = getRandomBattery(getRandomAngle().toRadians(), 6);
    std::vector<Gun> gun_vec;
    for(auto g : bpt.bp_guns) {
        auto dir = getRandomAngle();
        auto dir_m = getRandomRefAngle(dir);
        auto dir_r = getRandomRefAngle(dir);
        auto dir_n = getRandomRefAngle(dir);
        gun_vec.emplace_back(g.pt_x,g.pt_y,g.pt_h, dir, dir_m,dir_r,dir_n);
        gun_vec.back().setCharges({{lt_full,    100},
                                 {lt_reduced, 100},
                                 {lt_1st,     100},
                                 {lt_2nd,     100},
                                 {lt_3rd,     100},
                                 {lt_4th,     100}});
    }
    Point main_gun_pt = bpt.bp_guns[2];
    std::vector<Target> targets;
    Point t;
    for (int i = 0; i < 10; ++i) {
        t = getRandomPoint(main_gun_pt, 10000);
        if(is_debug_mode) std::cout << t.pt_x << ", " << t.pt_y << ", " << t.pt_h << "\n";
        targets.emplace_back(t.pt_x, t.pt_y, t.pt_h);
        if(is_debug_mode) std::cout << "address of target vector back: " << &targets.back() << "\n";
        for(auto g : gun_vec) {
            g.addTarget(targets.back());
        }
        if(is_debug_mode) std::cout << "successful\n";
    }

    std::cout << "PRINTING ALL GUN INFOS AND ALL TARGET PARAMS\n\n";
    for(auto g : gun_vec) {
        g.printGunInfo();
        g.printTargetParameters(true);
    }
    std::cout << "PRINTING ALL GUN PARAMS FOR A SPECIFIC TARGET:\n\n";
    auto tgt_map = getAllGunParamsForTarget(targets[0]);
    for(auto& p : tgt_map) {
        std::cout << "gun ID: " << p.first << "\n\n";
        p.second.consolePrint(true);
    }
    for(const auto& g : gun_vec) {
        std::cout << gunToJSON(g).dump(4) << "\n\n\n\n\n";
    }
    for(const auto& t : targets) {
        std::cout << targetToJSON(t).dump(4) << "\n\n\n\n\n";
    }
    for(const auto& param : getGunTargetParameters()) {
        std::cout << targetParametersToJSON(param).dump(4) << "\n\n\n\n\n";
    }
}

int main(int argc, char* argv[]) {
  init();
//    std::cout << project_path << "\n";
//    MyObject myObj;
//    myObj.id = 1;
//    myObj.name = "Example Object";
//    myObj.data = {
//            {"key1", 10},
//            {"key2", 20}
//    };
//
//    json jsonOutput = toJson(myObj);
//
//    // Print the JSON
//    std::cout << jsonOutput.dump(4) << std::endl;
    return 0;
}