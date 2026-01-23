#include "string_utils.h"

using namespace util;

/**
 * @fn      string replaceSlashWithPeriod(string str)
 * @brief   Replaces '/' with '.'
 *
 * @author  1sand0s
 * @param   str     The string instance where '/' has to
 *                  replaced with '.'
 * @return  string  The updated string instance
 */
string STRING::replaceSlashWithPeriod(string str) {
  string replaced_str = str;
  replace(replaced_str.begin(), replaced_str.end(), '/', '.');
  return replaced_str;
}

/**
 * @fn      string replaceSlashWithPeriod(string str)
 * @brief   Replaces '.' with '/'
 *
 * @author  1sand0s
 * @param   str     The string instance where '.' has to
 *                  replaced with '/'
 * @return  string  The updated string instance
 */
string STRING::replacePeriodWithSlash(string str) {
  string replaced_str = str;
  replace(replaced_str.begin(), replaced_str.end(), '.', '/');
  return replaced_str;
}

/**
 * @fn      string cypherLabelToClass(string cypherLabel)
 * @brief   Removes enclosing '`' from Cypher Label
 *
 * @author  1sand0s
 * @param   cypherLabel  string instance containing Cypher Label
 * @return  string       The updated string instance
 */
string STRING::cypherLabelToClass(string cypherLabel) {
  return cypherLabel.substr(1, cypherLabel.size() - 2);
}

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
string STRING::classToCypherLabel(string klass) {
  string cypherLabel = "";
  int j = 0;

  if (klass.size() == 0) {
    return cypherLabel;
  }

  if (klass[j] == '[') {
    j++;
  }
  if (klass[j] == 'L') {
    j++;
  }
  if (klass[klass.size() - 1] == ';') {
    cypherLabel = klass.substr(j, klass.size() - 1 - j);
  } else {
    cypherLabel = klass.substr(j, klass.size() - j);
  }

  cypherLabel = replaceSlashWithPeriod(cypherLabel);
  return cypherLabel;
}

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
string STRING::classToCypherLabel2(string klass) {
  string cypherLabel = "";
  int j = 0;

  if (klass.size() == 0) {
    return cypherLabel;
  }

  if (klass[j] == 'L') {
    j++;
  }
  if (klass[klass.size() - 1] == ';') {
    cypherLabel = klass.substr(j, klass.size() - 1 - j);
  } else {
    cypherLabel = klass.substr(j, klass.size() - j);
  }

  cypherLabel = replaceSlashWithPeriod(cypherLabel);
  return cypherLabel;
}

/**
 * @fn      string classToCypherLabel(string klass)
 * @brief   Removes 'L','[' and ';' from java class signatures
 *
 * @author  1sand0s
 * @param   klass    string instance containing java class signature
 * @return  string   The updated string instance
 */
bool STRING::isStringPresentInList(string &str, vector<string> &list) {
  for (string listElement : list) {
    if (str.find(listElement) != string::npos) {
      return true;
    }
  }
  return false;
}
