#include <fstream>
#include "particle.h"
#include "global.h"

/**
 * @brief Loads data into CfgData object 
 * @details We access the CfgData object through a pointer, and the data object itself is a 
 * member of the Parfis class.
 */
int parfis::Particle::loadCfgData()
{
    std::string str;
    std::vector<std::string> strVec;
    getParamToVector("specie", strVec);

    m_pCfgData->specieVec.resize(strVec.size());
    for (size_t i = 0; i < strVec.size(); i++) {
        m_pCfgData->specieVec[i].name = strVec[i];
        getParamToValue("specie." + strVec[i] + ".statesPerCell", 
            m_pCfgData->specieVec[i].statesPerCell);
    }

    return 0;
}
