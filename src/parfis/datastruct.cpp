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

template<>
void parfis::Param<std::string>::setRangeVec(const std::string& ranstr) 
{
    m_rangeVec = Global::getVector(ranstr, '(', ')');
}

template<>
void parfis::Param<double>::setRangeVec(const std::string& ranstr) 
{
    auto ranvec = Global::getVector(ranstr, '(', ')');
    for (auto& ran: ranvec)
        m_rangeVec.push_back(std::strtold(ran.c_str(), nullptr));
}

template<>
void parfis::Param<int>::setRangeVec(const std::string& ranstr) 
{
    auto ranvec = Global::getVector(ranstr, '(', ')');
    for (auto& ran: ranvec)
        m_rangeVec.push_back(std::strtol(ran.c_str(), nullptr, 10));
}

template<class T>
bool parfis::Param<T>::inRange(T value)
{
    return value >= m_rangeVec[0] && value <= m_rangeVec[1];
}

template<>
bool parfis::Param<std::string>::inRange(std::string valstr)
{
    for(auto& str: m_rangeVec)
        if (str == valstr)
            return true;
    return false;
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

bool parfis::ParamBase::inRange(const std::string& valstr)
{
    if (m_type == "int")
        return static_cast<Param<int>*>(this)->inRange(std::strtol(valstr.c_str(), nullptr, 10));
    else if (m_type == "double")
        return static_cast<Param<double>*>(this)->inRange(std::strtod(valstr.c_str(), nullptr));
    else if (m_type == "std::string")
        return static_cast<Param<std::string>*>(this)->inRange(valstr);
    return false;
}

void parfis::ParamBase::setValueVec(ParamBase* ppb, const std::string& valstr)
{
    if (ppb->m_type == "double")
        static_cast<Param<double>*>(ppb)->setValueVec(valstr);
    else if (ppb->m_type == "int")
        static_cast<Param<int>*>(ppb)->setValueVec(valstr);
    else if (ppb->m_type == "std::string")
        static_cast<Param<std::string>*>(ppb)->setValueVec(valstr);
}

void parfis::ParamBase::setRangeVec(ParamBase* ppb, const std::string& ranstr)
{
    if (ppb->m_type == "double")
        static_cast<Param<double>*>(ppb)->setRangeVec(ranstr);
    else if (ppb->m_type == "int")
        static_cast<Param<int>*>(ppb)->setRangeVec(ranstr);
    else if (ppb->m_type == "std::string")
        static_cast<Param<std::string>*>(ppb)->setRangeVec(ranstr);
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
 */
template<class T>
void parfis::Domain::getParamToValue(const std::string& key, T& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    valRef = static_cast<Param<T>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
}

template void parfis::Domain::getParamToValue<int>(const std::string& key, int& valRef);
template void parfis::Domain::getParamToValue<double>(const std::string& key, double& valRef);
template void parfis::Domain::getParamToValue<std::string>(
    const std::string& key, std::string& valRef);

template<>
void parfis::Domain::getParamToValue(const std::string& key, Vec3D<double>& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    valRef.x = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
    valRef.y = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[1];
    valRef.z = static_cast<Param<double>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[2];
}

template<>
void parfis::Domain::getParamToValue(const std::string& key, Vec3D<int>& valRef) 
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = this;
    size_t i;
    for(i=0; i<inhvec.size() - 1; i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    valRef.x = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[0];
    valRef.y = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[1];
    valRef.z = static_cast<Param<int>*>(pp->m_childMap[inhvec[i]].get())->m_valueVec[2];
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

int parfis::CfgData::setPyCfgData()
{
    pyCfgData.geometry = geometry.c_str();
    pyCfgData.timestep = timestep;
    pyCfgData.geometrySize = &geometrySize;
    pyCfgData.cellSize = &cellSize;
    pyCfgData.periodicBoundary = &periodicBoundary;
    pyCfgData.cellCount = &cellCount;
    pyCfgData.specieNameVec = specieNameVec;
    return 0;
}

int parfis::SimData::setPySimData()
{
    pySimData.stateVec = stateVec;
    pySimData.cellIdVec = cellIdVec;
    pySimData.specieVec = specieVec;
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
    ParamBase::setRangeVec(cp, std::get<1>(keyString));
    ParamBase::setValueVec(cp, std::get<1>(keyValue));
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