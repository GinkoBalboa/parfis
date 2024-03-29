#include <sstream>
#include <string>
#include "parfis.h"
#include "datastruct.h"
#include "global.h"
#include "system.h"
#include "particle.h"

template<>
void parfis::Param<std::string>::setValueVec(const std::string& valstr) 
{
    m_valueVec = Global::getVector(valstr, '[', ']');
    m_size = m_valueVec.size();
}

template<>
void parfis::Param<double>::setValueVec(const std::string& valstr) 
{
    m_valueVec.clear();
    auto valvec = Global::getVector(valstr, '[', ']');
    for (auto& val: valvec)
        m_valueVec.push_back(std::strtold(val.c_str(), nullptr));
    m_size = m_valueVec.size();
}

template<>
void parfis::Param<int>::setValueVec(const std::string& valstr) 
{
    m_valueVec.clear();
    auto valvec = Global::getVector(valstr, '[', ']');
    for (auto& val: valvec)
        m_valueVec.push_back(std::strtol(val.c_str(), nullptr, 10));
    m_size = m_valueVec.size();
}

template<class S>
void parfis::ParamBase::addChild(const std::string& name) 
{
    m_childMap[name] = std::unique_ptr<ParamBase>(new Param<S>());
    m_childMap[name]->m_name = name;
    m_childMap[name]->m_parent = this;
}

template void parfis::ParamBase::addChild<std::string>(const std::string& name);
template void parfis::ParamBase::addChild<double>(const std::string& name);
template void parfis::ParamBase::addChild<int>(const std::string& name);

void parfis::ParamBase::setValueVec(ParamBase* ppb, const std::string& valstr)
{
    if (ppb->m_type == "double")
        static_cast<Param<double>*>(ppb)->setValueVec(valstr);
    else if (ppb->m_type == "int")
        static_cast<Param<int>*>(ppb)->setValueVec(valstr);
    else if (ppb->m_type == "std::string")
        static_cast<Param<std::string>*>(ppb)->setValueVec(valstr);
}

std::string parfis::ParamBase::getValueString(bool printType)
{
    if (m_size == 0)
        return "";
    std::string str;
    if (m_type == "int") {
        if (m_size == 1)
            str = std::to_string(static_cast<Param<int>*>(this)->m_valueVec[0]);
        else {
            str += '[';
            for (auto& val : static_cast<Param<int>*>(this)->m_valueVec)
                str += std::to_string(val) + ",";
            str.back() = ']';
        }
        if (printType)
            str += " <int>";
        return str;
    }
    else if (m_type == "std::string") {
        if (m_size == 1)
            str = static_cast<Param<std::string>*>(this)->m_valueVec[0];
        else {
            str += '[';
            for (auto& val : static_cast<Param<std::string>*>(this)->m_valueVec)
                str += val + ",";
            str.back() = ']';
        }
        if (printType && (m_childMap.size() > 0))
            str += " <parfis::Param>";
        else if (printType)
            str += " <std::string>";
        return str;
    }
    else if (m_type == "double") {
        if (m_size == 1)
            str = Global::to_string(static_cast<Param<double>*>(this)->m_valueVec[0]);
        else {
            str += '[';
            for (auto& val : static_cast<Param<double>*>(this)->m_valueVec)
                str += Global::to_string(val) + ",";
            str.back() = ']';
        }
        if (printType)
            str += " <double>";
        return str;
    }
    return "";
}

/**
 * @brief Sets the first value of the Param<T>::m_valueVec
 * @tparam T type of parameter (double, float, int or string)
 * @param key name of the parameter without the first, domain, name level.
 * @param valRef reference to set value to
 * @return int zero on success
 */
template<class T>
int parfis::Domain::getParamToValue(const std::string& key, T& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    if (pp->m_childMap.count(inhvec[i])) {
        valRef = static_cast<Param<T>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
        return 0;
    }
    return 1;
}

template int parfis::Domain::getParamToValue<int>(const std::string& key, int& valRef);
template int parfis::Domain::getParamToValue<double>(const std::string& key, double& valRef);
template int parfis::Domain::getParamToValue<std::string>(
    const std::string& key, std::string& valRef);

/**
 * @brief Specialization for Vec3D<double>
 * 
 * @param key name of the parameter without the first, domain, name level.
 * @param valRef reference to set value to
 * @return int zero on success
 */
template<>
int parfis::Domain::getParamToValue(const std::string& key, Vec3D<double>& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    if (pp->m_childMap.count(inhvec[i])) {
        valRef.x = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
        valRef.y = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[1];
        valRef.z = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[2];
        return 0;
    }
    return 1;
}

/**
 * @brief Specialization for Vec3D<int>
 * 
 * @param key name of the parameter without the first, domain, name level.
 * @param valRef reference to set value to
 * @return int zero on success
 */
template<>
int parfis::Domain::getParamToValue(const std::string& key, Vec3D<int>& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    if (pp->m_childMap.count(inhvec[i])) {
        valRef.x = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
        valRef.y = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[1];
        valRef.z = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[2];
        return 0;
    }
    return 1;
}

/**
 * @brief Sets the values of Param<T>::m_valueVec to a vector
 * @tparam T type of parameter (double, float, int or string)
 * @param key name of the parameter without the first, domain, name level.
 * @param vecRef reference of a vector to set values to
 */
template<class T>
void parfis::Domain::getParamToVector(const std::string& key, std::vector<T>& vecRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    vecRef.clear();
    if (pp->m_childMap.count(inhvec[i]))
        vecRef = static_cast<Param<T>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec;
}

template void parfis::Domain::getParamToVector<double>(
    const std::string& key, std::vector<double>& vecRef);

template void parfis::Domain::getParamToVector<std::string>(
    const std::string& key, std::vector<std::string>& vecRef);

parfis::Param<std::string>* parfis::Domain::getParent(const std::string& cstr) {
    
    auto inheritvec = Global::getInheritanceVector(cstr);
    Param<std::string>* pp = this;
    for(size_t i=1; i<inheritvec.size() - 1; i++) {
        pp = static_cast<Param<std::string>*>(pp->m_childMap[inheritvec[i]].get());
    }
    return pp;
}

std::vector<const char*> parfis::PyVecContainer::pyStrVec;

int parfis::CfgData::setPyCfgData()
{
    pyCfgData.geometry = geometry;
    pyCfgData.timestep = timestep;
    pyCfgData.geometrySize = &geometrySize;
    pyCfgData.cellSize = &cellSize;
    pyCfgData.periodicBoundary = &periodicBoundary;
    pyCfgData.cellCount = &cellCount;
    // Reserve space in the PyVecContainer::pyStrVec for strings
    auto sizeRes = 0;
    sizeRes += specieNameVec.size();
    sizeRes += gasNameVec.size();
    sizeRes += gasCollisionNameVec.size();
    sizeRes += gasCollisionFileNameVec.size();
    PyVecContainer::pyStrVec.clear();
    PyVecContainer::pyStrVec.reserve(sizeRes);
    // Fill containter with pointer to strings
    pyCfgData.specieNameVec = specieNameVec;
    pyCfgData.gasNameVec = gasNameVec;
    pyCfgData.gasCollisionNameVec = gasCollisionNameVec;
    pyCfgData.gasCollisionFileNameVec = gasCollisionFileNameVec;
    return 0;
}

/**
 * @brief Sets the PySimData pointers to coresponding references from SimData
 * @details PySimData is used to wrap the data structure in order to be
 * usable by python through ctypes. The need for this structure is mainly
 * because in ctypes there isn't a built-in representation of the std::vector
 * structure.
 * @return int Zero on success 
 */
int parfis::SimData::setPySimData()
{
    pySimData.stateVec = stateVec;
    pySimData.cellIdVec = cellIdVec;
    pySimData.cellIdAVec = cellIdAVec;
    pySimData.cellIdBVec = cellIdBVec;
    pySimData.specieVec = specieVec;
    pySimData.cellVec = cellVec;
    pySimData.nodeFlagVec = nodeFlagVec;
    pySimData.headIdVec = headIdVec;
    pySimData.gasVec = gasVec;
    // First set the wrapper data
    pyGasCollisionVec.resize(gasCollisionVec.size());
    for (auto i = 0; i < gasCollisionVec.size(); i++) {
        pyGasCollisionVec[i] = gasCollisionVec[i];
    }
    // Get references
    pySimData.pyGasCollisionVec = pyGasCollisionVec;

    // First set the wrapper data
    pyGasCollisionProbVec.resize(gasCollisionProbVec.size());
    for (auto i = 0; i < gasCollisionProbVec.size(); i++) {
        pyGasCollisionProbVec[i] = gasCollisionProbVec[i];
    }
    // Get references
    pySimData.pyGasCollisionProbVec = pyGasCollisionProbVec;

    return 0;
}

/**
 * @brief Initializes Domain from DEFAULT_INITIALIZATION_STRING
 * @param cstr initialization string is in the format key=value<type>(range). Value 
 * can be of type array in which case is given as [value1, value2, ...]. 
 * @return Zero on success 
 */
int parfis::Domain::initialize(const std::string& cstr)
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    std::tuple<std::string, std::string> keyString = Global::splitKeyString(cstr);
    std::string childName = Global::childName(std::get<0>(keyValue));
    Param<std::string>* pp = getParent(std::get<0>(keyValue));
    ParamBase* cp = nullptr;
    // If it is domain don't add it as a child, because there is no parent
    if (childName == "") {
        cp = pp;
    }
    else {
        if (cstr.find("<parfis::Param>") != std::string::npos) {
            if (pp->m_childMap.find(childName) != pp->m_childMap.end())
                pp->m_childMap.erase(childName);
            pp->addChild<std::string>(childName);
        }
        else if (cstr.find("<std::string>") != std::string::npos)
            pp->addChild<std::string>(childName);
        else if (cstr.find("<double>") != std::string::npos)
            pp->addChild<double>(childName);
        else if (cstr.find("<int>") != std::string::npos)
            pp->addChild<int>(childName);
        cp = pp->m_childMap[childName].get();
    }
    ParamBase::setValueVec(cp, std::get<1>(keyValue));
    return 0;
}

/**
 * @brief Loads data from the defined file name and based on 
 * the object type.
 * 
 * @return int Zero on success
 */
int parfis::FuncTable::loadData(const std::string& fileName)
{
    std::ifstream infile(fileName);
    std::string line;
    rowCnt = 1;
    colCnt = 0;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (line[0] == '#') {
            // Find nbins
            if (line.find("bins") != std::string::npos) {
                Global::setValueVec<int>(nbins, line, '[', ']');
                int vecSize = 0;
                for (auto nbin : nbins)
                    vecSize += nbin;
                xVec.resize(vecSize);
                yVec.resize(vecSize);
            }
            // Find ranges
            if (line.find("ranges") != std::string::npos)
                Global::setValueVec<double>(ranges, line, '[', ']');
        }
        else {
            if (colCnt > xVec.size() - 1)
                return 2;
            // Else is pure data
            xVec[colCnt] = Global::getNthElement<double>(line, 0);
            yVec[colCnt] = Global::getNthElement<double>(line, 1);
            colCnt++;
        }
    }
    if (colCnt != xVec.size())
        return 3;

    return 0;
}

/**
 * @brief Calculates the collision frequency.
 * 
 * @return int Zero on success
 */
int parfis::GasCollision::calculateColFreq(const Specie & spec, const Gas& gas)
{
    double vMaxSq = spec.maxVel*spec.maxVel;
    double ivMaxSq = 1.0/vMaxSq;
    double im = 1.0/spec.mass;
    double dt = spec.dt;
    // Probability data
    freqFtab.nbins = xSecFtab.nbins;
    freqFtab.ranges.resize(freqFtab.nbins.size());
    freqFtab.rowCnt = xSecFtab.rowCnt;
    freqFtab.colCnt = xSecFtab.colCnt;
    for(auto i = 0; i < xSecFtab.ranges.size(); i++) {
        freqFtab.ranges[i] = 
            2.0*xSecFtab.ranges[i]*Const::eVJ*im*ivMaxSq;
    }
    freqFtab.xVec.resize(xSecFtab.xVec.size());
    freqFtab.yVec.resize(xSecFtab.xVec.size());
    for(auto i = 0; i < xSecFtab.xVec.size(); i++) {
        freqFtab.xVec[i] = 2.0*xSecFtab.xVec[i]*Const::eVJ*im;
        freqFtab.yVec[i] = 
            gas.molDensity*Const::Na*xSecFtab.yVec[i]*1.0e-20*sqrt(freqFtab.xVec[i])*spec.dt;
        freqFtab.xVec[i] *= ivMaxSq;
    }

    return 0;
}

/**
 * @brief Calculates collision probability from the cx
 * 
 * @return int Zero on success
 */
int parfis::SimData::calculateColProb(const CfgData * pCfgData) 
{
    // Calculate total collison data
    for (size_t i = 0; i < pCfgData->specieNameVec.size(); i++) {
        if (specieVec[i].gasCollisionVecId.size() == 0) continue;
        gasCollisionProbVec.push_back({});
        FuncTable * pft = &gasCollisionProbVec.back();
        specieVec[i].gasCollisionProbId = gasCollisionProbVec.size() - 1;
        pft->type = 1; // nonlinear tabulation
        // Construct matrix dimensions here
        pft->colCnt = specieVec[i].gasCollisionVecId.size();
        for (size_t j = 0; j < pft->colCnt; j++) {
            GasCollision * pGasCol = &gasCollisionVec[specieVec[i].gasCollisionVecId[j]];
            // Initialize
            if (j == 0) {
                pft->nbins = pGasCol->freqFtab.nbins;
                pft->ranges = pGasCol->freqFtab.ranges;
                pft->rowCnt = pGasCol->freqFtab.xVec.size();
                pft->xVec = pGasCol->freqFtab.xVec;
                pft->yVec.resize(pft->rowCnt*pft->colCnt, 0);
            }
            for (auto k = 0; k < pft->rowCnt; k++) {
                pft->yVec[k*pft->colCnt + j] = pGasCol->freqFtab.yVec[k];
                if (j > 0)
                    pft->yVec[k*pft->colCnt + j] += pft->yVec[k*pft->colCnt + j - 1];
            }
        }

        auto jt = pft->colCnt - 1;
        double prob;
        for (size_t j = 0; j < pft->colCnt - 1; j++) {
            for (auto k = 0; k < pft->rowCnt; k++) {
                // Relative collision probabilities
                prob = pft->yVec[k*pft->colCnt + j]/pft->yVec[k*pft->colCnt + jt];
                if (prob != prob)
                    prob = 0;
                pft->yVec[k*pft->colCnt + j] = prob;
            }
        }
        // Last column is a total collision probability
        for (auto k = 0; k < pft->rowCnt; k++) {
            pft->yVec[k*pft->colCnt + jt] = 1.0 - exp(-pft->yVec[k*pft->colCnt + jt]);
        }
        for (size_t j = 0; j < pft->colCnt - 1; j++) {
            for (auto k = 0; k < pft->rowCnt; k++) {
                // Scale the relative collision prob. to the total col. prob.
                pft->yVec[k*pft->colCnt + j] *= pft->yVec[k*pft->colCnt + jt];
            }
        }
    }
    return 0;
}

/**
 * @brief Configures initialized Domain
 * @param cstr configuration string is in the format key=value 
 * @return Zero on success 
 */
int parfis::Domain::configure(const std::string& cstr) 
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    std::string childName = Global::childName(std::get<0>(keyValue));
    Param<std::string>* pp = getParent(std::get<0>(keyValue));
    // If reconfiguration is ongoing then childMap must be reassembled
    if (pp->m_childMap.find(childName) == pp->m_childMap.end())
        initialize(cstr);
    ParamBase::setValueVec(pp->m_childMap[childName].get(), std::get<1>(keyValue));
    return 0;
}

/**
 * @brief Constructs file name for the log files
 * @param id of the Parfis object
 * @param cnt number of log file for the same id
 * @return Log file name
 */
std::string parfis::Logger::getLogFileName(uint32_t id, uint32_t cnt) {
    return "./parfisLog_id" + std::to_string(id) + "_cnt" + std::to_string(cnt) + ".log";
}

/**
 * @brief Initializes log string and file name 
 * @param fname name of the file to write log to
 */
void parfis::Logger::initialize(const std::string& fname) {
    m_fname = fname;
    m_str = "Parfis log file\n";
    m_str += "Created on: " + Global::currentDateTime() + "\n";
    m_str += "api::info():\n";
    m_str += "--------------\n";
    m_str += std::string(api::info()) + "\n";
    m_str += "--------------\n";
}

/**
 * @brief Logs strings into Logger::m_str
 * @param mask logging mask defined by parfis::LogMask
 * @param msg string that is copied to log (that is Logger::m_str memeber)
 */
void parfis::Logger::logToStr(LogMask mask, const std::string& msg)
{
    if (mask == LogMask::Error)
        m_str += "[error] ";
    else if (mask == LogMask::Info)
        m_str += "[info] ";
    else if (mask == LogMask::Memory)
        m_str += "[memory] ";
    else if (mask == LogMask::Warning)
        m_str += "[warning] ";    
    m_str += msg;
}


/**
 * @brief Prints the log string to the defined file
 */
void parfis::Logger::printLogFile()
{
    if (m_fname != "") {
        std::ofstream logFile(m_fname, std::ofstream::app);
        logFile << m_str;
        logFile.close();
        m_str.clear();
    }
}

/**
 * @brief Generates new domain based on dname
 * @param dname domain name one of predefined ("system", "particle")
 * @param logger reference to the logger of the Parfis object
 * @param cfgData reference to the CfgData object from the Parfis object
 * @param simData reference to the SimData object from the Parfis object
 * @param cmdChainMap reference to the command map from the Parfis object
 * @return Domain created with new
 */
std::unique_ptr<parfis::Domain> parfis::Domain::generateDomain(const std::string& dname, 
    Logger& logger, CfgData& cfgData, SimData& simData, 
    std::map<std::string, std::unique_ptr<CommandChain>>& cmdChainMap) 
{
    if (dname == "system")
        return std::unique_ptr<Domain>(new System(dname, logger, cfgData, simData, cmdChainMap));
    if (dname == "particle")
        return std::unique_ptr<Domain>(new Particle(dname, logger, cfgData, simData, cmdChainMap));
    else 
        return std::unique_ptr<Domain>(nullptr);
}