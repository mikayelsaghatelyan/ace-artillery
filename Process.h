#ifndef ACE_ARTILLERY1_0_PROCESS_H
#define ACE_ARTILLERY1_0_PROCESS_H

#include "dependencies.h"
#include "Gun.h"
#include "Data.h"
#include "Process.h"



std::string defineProjectPath();

void setProjectPath();

void clearProjectPath();

void insertGun(const Gun& g);

void insertTarget(const Target& t);

void eraseGun(const Gun& g);

void eraseTarget(const Target& t);

void assignTargetForGun(Gun& g, Target& t);

void dismissTargetForGun(Gun& g, Target& t);

void dismissMissionForGun(Gun& g);

void dismissAllMissions();

void printGunMission(const Gun& g);

void printGuns();

void printTargets();

void saveGunData();

void saveTargetData();

void loadGunData();

void loadTargetData();

void saveTargetParameters();

void loadTargetParameters();

void saveData();

void loadData();

void clearData();

void clearGunData();

void clearTargetData();

void clearParameterData();

json gunToJSON(const Gun& gun);

json targetToJSON(const Target& target);

json targetParametersToJSON(const GunTargetParameters& params);

Gun gunFromJSON(const std::string& json_filename);

Target targetFromJSON(const std::string& json_filename);

GunTargetParameters targetParametersFromJSON(const std::string& json_filename);



#endif //ACE_ARTILLERY1_0_PROCESS_H
