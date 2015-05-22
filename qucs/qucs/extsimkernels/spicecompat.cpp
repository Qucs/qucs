#include "spicecompat.h"

/*!
 * \brief spicecompat::check_refdes If starting letters of the component name
 *        match spice model (i.e. R1, C1, L1)returns Name. Otherwise function
 *        returns an unique Spice Refdes (Name+SpiceModel)
 * \param Name
 * \param SpiceModel
 * \return
 */
QString spicecompat::check_refdes(QString &Name,QString &SpiceModel)
{
    if (Name.startsWith(SpiceModel,Qt::CaseInsensitive)) {
        return Name;
    } else {
        return (SpiceModel + Name);
    }
}

/*!
 * \brief spicecompat::normalize_value Remove units from component values and
 *        replace Spice-incompatible factors (i.e M, Meg). Wrap value in braces
 *        if it contains variables.
 * \param[in] Value Qucs-style component value
 * \return Spice-style component value
 */
QString spicecompat::normalize_value(QString Value)
{
    QRegExp r_pattern("^[0-9]+.*Ohm$");
    QRegExp c_pattern("^[0-9]+.*F$");
    QRegExp l_pattern("^[0-9]+.*H$");
    QRegExp v_pattern("^[0-9]+.*V$");
    QRegExp hz_pattern("^[0-9]+.*Hz$");
    QRegExp s_pattern("^[0-9]+.*S$");
    QRegExp var_pattern("^[A-Za-z].*$");

    QString s = Value.remove(' ');
    if (r_pattern.exactMatch(s)) {
        s.remove("Ohm");
        s.replace("M","Meg");
    } else if (c_pattern.exactMatch(s)) {
        s.remove("F");
        s.replace("M","Meg");
    } else if (l_pattern.exactMatch(s)) {
        s.remove("H");
        s.replace("M","Meg");
    } else if (v_pattern.exactMatch(s)) {
        s.remove("V");
        s.replace("M","Meg");
    } else if (hz_pattern.exactMatch(s)) {
        s.remove("Hz");
        s.replace("M","Meg");
    } else if (s_pattern.exactMatch(s)) {
        s.remove("S");
        s.replace("M","Meg");
    } else if (var_pattern.exactMatch(s)) {
        s = "{" + s + "}";
    }


    return s.toUpper();
}

/*!
 * \brief spicecompat::convert_functions Convert Qucs mathematical function name
 *        to Spice mathematical function name.
 * \param tok[in] Qucs function name
 * \param isXyce[in] True if Xyce is used, False by default.
 * \return
 */
QString spicecompat::convert_functions(QString tok, bool isXyce)
{
    QStringList conv_list_ngspice; // Put here functions need to be converted
    conv_list_ngspice<<"q"<<"1.6021765e-19"
            <<"kB"<<"1.38065e-23"
            <<"pi"<<"3.1415926539"
            <<"step"<<"stp"
            <<"sign"<<"sgn"
            <<"log10"<<"log";
    QStringList conv_list_xyce = conv_list_ngspice;

    QStringList conv_list;
    if (isXyce) conv_list = conv_list_xyce;
    else conv_list = conv_list_ngspice;

    for(int i=0;i<conv_list.count();i+=2) {
        if (conv_list.at(i)==tok)
            return conv_list.at(i+1);
    }

    return tok;
}

/*!
 * \brief spicecompat::splitEqn Split Equation into tokens. Token delimenters are mathamatical
 *        operation signs:  = ^ + - * /  and parentheses ()
 * \param[in] eqn Equation to split
 * \param[out] tokens String list in which to put tokens.
 */
void spicecompat::splitEqn(QString &eqn, QStringList &tokens)
{
    tokens.clear();
    QString tok = "";
    for (QString::iterator it=eqn.begin();it!=eqn.end();it++) {
        QString delim = "=()*/+-^";
        if (it->isSpace()) continue;
        if (delim.contains(*it)) {
            if (!tok.isEmpty()) tokens.append(tok);
            tokens.append(*it);
            tok.clear();
            continue;
        }
        tok += *it;
    }
    if (!tok.isEmpty()) tokens.append(tok);
}


/*!
 * \brief spicecompat::containNodes Determine are there in equaton node voltages
 *        and/or current porbes.
 * \param tokens List of tokens. Should be obtained with splitEqn().
 * \param vars List of vars that are used in ngnutmeg script.
 * \return Return true if equation contain node voltages and/or current probe variables
 */
bool spicecompat::containNodes(QStringList &tokens, QStringList &vars)
{
    QRegExp var_pattern("^[\\w]+\\.([IV]t|[iv]|vn|Vb|[IV])$");
    QRegExp disto_var("^[Dd][Ii][Ss][Tt][Oo][0-9]\\.[Vv]$");
    QStringList system_vars;
    system_vars.clear();
    system_vars<<"frequency"<<"acfrequency"<<"time"<<"hbfrequncy";
    foreach (QString tok,tokens) {
        if (var_pattern.exactMatch(tok)) return true;
        if (disto_var.exactMatch(tok)) return true;
        if (system_vars.contains(tok)) return true;
        if (tok.endsWith("#branch")) return true;
        if (vars.contains(tok)) return true;
    }
    for (QStringList::iterator it=tokens.begin();it!=tokens.end();it++) {
        if ((*it).toUpper()=="V") { // voltages  in spice notation
            it++;
            if ((*it)=="(") return true;
        }
    }
    return false;
}

/*!
 * \brief spicecompat::convertNodeNames convert node names form Qucs-notation to
 *        Spice-notation (i.e. Node.Vt --> V(Node) ) and determine used simualtion.
 *        This method modifies the input list of tokens.
 * \param[in/out] tokens
 * \param[out] sim Used simulation.
 *             "ac" --- AC simulation;
 *             "disto" --- DISTO simulation;
 *             "dc" --- DC simulation;
 *             "tran" --- Transient simulation;
 *             "all" --- All simulations;
 */
void spicecompat::convertNodeNames(QStringList &tokens, QString &sim)
{
    QRegExp var_pattern("^[\\w]+\\.([IV]t|[iv]|vn|Vb|[IV])$");
    QRegExp disto_var("^[Dd][Ii][Ss][Tt][Oo][0-9]\\.[Vv]$");
    for (QStringList::iterator it=tokens.begin();it!=tokens.end();it++) {
        if ((*it).endsWith("#branch")) sim="all";
        if ((*it).toUpper()=="V") {
            it++;
            if ((*it)=="(") sim="all";
        }
        if (disto_var.exactMatch(*it)) sim = "disto";
        if (var_pattern.exactMatch(*it))  {
            if (!disto_var.exactMatch(*it)) {
                if ((it->endsWith(".v"))||(it->endsWith(".i"))) sim = "ac";
                if ((it->endsWith(".Vt"))||(it->endsWith(".It"))) sim = "tran";
                if ((it->endsWith(".V"))||(it->endsWith(".I"))) sim = "dc";
                QString suffix = it->section('.',1,1);
                int idx = it->indexOf('.');
                int cnt = it->count();
                it->chop(cnt-idx);
                if (suffix.toUpper().startsWith("I"))
                    *it = QString("V%1#branch").arg(*it);
                else *it = QString("V(%2)").arg(*it);
            }
        } else if ((*it=="frequency")||(*it=="acfrequency")) {
            sim = "ac";
        } else if (*it=="time") {
            sim = "tran";
        }
    }
}
