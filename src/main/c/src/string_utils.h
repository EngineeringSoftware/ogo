#pragma once

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

namespace util {
namespace STRING {

/**
 * @fn      string replaceSlashWithPeriod(string str)
 * @brief   Replaces '/' with '.'
 *
 * @author  1sand0s
 * @param   str     The string instance where '/' has to
 *                  replaced with '.'
 * @return  string  The updated string instance
 */
string replaceSlashWithPeriod(string str);

/**
 * @fn      string replacePeriodWithSlash(string str)
 * @brief   Replaces '.' with '/'
 *
 * @author  1sand0s
 * @param   str     The string instance where '.' has to
 *                  replaced with '/'
 * @return  string  The updated string instance
 */
string replacePeriodWithSlash(string str);

/**
 * @fn      string cypherLabelToClass(string cypherLabel)
 * @brief   Removes enclosing '`' from Cypher Label
 *
 * @author  1sand0s
 * @param   cypherLabel  string instance containing Cypher Label
 * @return  string       The updated string instance
 */
string cypherLabelToClass(string cypherLabel);

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
string classToCypherLabel(string klass);

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
string classToCypherLabel2(string klass);

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
bool isStringPresentInList(string &str, vector<string> &list);
} // namespace STRING
} // namespace util
