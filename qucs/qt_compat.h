
/// make Qt work with stl
///
#ifndef QUCS_QT_COMPAT
#define QUCS_QT_COMPAT


inline std::ostream& operator<<(std::ostream& o, QString const& s)
{ //
  return o << s.toStdString();
}


#endif
