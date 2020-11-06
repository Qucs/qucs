
#include "sim/data.h"
#include "dat.h"
#include "qt_compat.h"
#include <QFile> // hmm
#include <QFileInfo> // hmm

void SimOutputDir::pushBack(QucsData* d)
{
	assert(d);
	assert(d->label()!="");
	_d[d->label().toStdString()] = d;
}

// a Var from a Dat file
class SimOutputDatVar : public SimOutputData{
	SimOutputDatVar(SimOutputDatVar const&) = delete;
public:
	explicit SimOutputDatVar(std::string const& fileName, std::string const& varname)
	  : _var(QString::fromStdString(varname)),
	    _fileName(fileName) {}

private:
	void clear();
	SimOutputData const* refresh() override;
	virtual const_iterator begin() const {incomplete(); return const_iterator(nullptr, nullptr);}
	virtual const_iterator end() const {incomplete(); return const_iterator(nullptr, nullptr);}

	unsigned numAxes() const { return CPointsX.size();}
	int loadIndepVarData(std::string const& Variable, char *FileString, DataX* pD);

	void setLimit(const double& x){
		if (Min>x) Min=x;
		if (Max<x) Max=x;
	}
private:
	QString _var;
	std::string _fileName;
	unsigned axis_count;
	std::vector<DataX*>  CPointsX;
	double *CPointsY;
	int CountY;    // number of curves ??
	QDateTime lastLoaded;
};

SimOutputDat::SimOutputDat(std::string const& fileName, std::string const& varname)
    : CPointsY(NULL),
      Var(QString::fromStdString(varname)),
      _fileName(fileName)
      // Min(INFINITY), Max(-INFINITY)
{ untested();
	QFile file(QString::fromStdString(fileName));
	if(!file.open(QIODevice::ReadOnly)) { untested();
		// BUG, throw?
		return;
	}else{ untested();
	}

	QString Line, tmp, Var;
	int varNumber = 0;
	// reading the file as a whole improves speed very much, also using
	// a QByteArray rather than a QString
	QByteArray FileString = file.readAll();
	file.close();

	int i=0, j=0;
	i = FileString.indexOf('<')+1;
	if(i > 0){ untested();
		do { untested();
			j = FileString.indexOf('>', i);
			for(int k=0;k<j-i;k++) Line[k]=FileString[k+i];
			Line.truncate(j-i);
			i = FileString.indexOf('<', j)+1;

			Var = Line.section(' ', 1, 1).remove('>');

			if(Var.length()==0){ untested();
			}else if(Var.at(0) == '_'){ untested();
				// continue;
			}else{ untested();
			}


			if(Line.left(3) == "dep") { untested();
				trace1("dep", Var);
				tmp = Line.section(' ', 2);
				varNumber++;
				SimOutputData* v = new SimOutputDatVar(_fileName, Var.toStdString());
				v->refresh();
				pushBack(v); // _map[Var.toStdString()] = v;
			} else if(Line.left(5) == "indep") { untested();
				tmp = Line.section(' ', 2, 2);
				//new Q3ListViewItem(ChooseVars, Var, "indep", tmp.remove('>'));
				trace1("indep", Var);
				varNumber++;

			}
		} while(i > 0);
	}else{ untested();
	}

//	refresh();
}

SimOutputData const* SimOutputDat::refresh()
{
	incomplete();
	return nullptr;
}
// --------------------------------------------------------------------------
// former "loadDatFile", former "loadVarData".
SimOutputData const* SimOutputDatVar::refresh()
{ untested();
	SimOutputDatVar* g = this;
	QFile file;
	QString Variable;
	QFileInfo Info(QString::fromStdString(_fileName));

	int pos = g->_var.indexOf(':');
	//  if(g->Var.right(3) == "].X")  // e.g. stdl[8:0].X
	//    if(pos > g->Var.indexOf('['))
	//      pos = -1;

	/* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
		to change the locale to the default. */
	setlocale (LC_NUMERIC, "C");

	if(pos <= 0) { untested();
		file.setFileName(QString::fromStdString(_fileName));
		Variable = g->_var;
	} else { untested();
		incomplete();
		// is this digital stuff?? not here.
///		file.setFileName(Info.path()+QDir::separator() + g->Var.left(pos)+".dat");
///		Variable = g->Var.mid(pos+1);
	}

	Info.setFile(file);
	if(g->lastLoaded.isValid()){
		if(g->lastLoaded > Info.lastModified()){
			return this;    // dataset unchanged -> no update neccessary
		}else{
		}
	}else{
	}

	clear(); // oops?
	if(Variable.isEmpty()){
		return NULL;
	}else{
	}

#if 0 // FIXME encapsulation. implement digital waves later.
	if(Variable.right(2) == ".X")
		if(Name.at(0) != 'T')
			return 0;  // digital variables only for tabulars and timing diagram
#endif


	if(!file.open(QIODevice::ReadOnly)){
		return NULL;
	}else{
	}

	// *****************************************************************
	// To strongly speed up the file read operation the whole file is
	// read into the memory in one piece.
	QByteArray FileContent;
	FileContent = file.readAll();
	file.close();
	char *FileString = FileContent.data();
	if(!FileString)  return 0;
	char *pPos = FileString+FileContent.size()-1;
	if(*pPos > ' ')  if(*pPos != '>')  return 0;
	*pPos = 0;


	// *****************************************************************
	// look for variable name in data file  ****************************
	bool isIndep = false;
	Variable = "dep "+Variable+" ";
	// "pFile" is used through-out the whole function and must NOT used
	// for other purposes!
	char *pFile = strstr(FileString, Variable.toLatin1());
	while(pFile) { untested();
		if(*(pFile-1) == '<') { untested();
			// is dependent variable ?
			break;
		}else if(strncmp(pFile-3, "<in", 3) == 0) {  // is independent variable ?
			isIndep = true;
			break;
		}
		pFile = strstr(pFile+4, Variable.toLatin1());
	}

	if(!pFile)  return 0;   // data not found

	QString Line, tmp;
	pFile += Variable.length();
	pPos = strchr(pFile, '>');
	if(!pPos)  return 0;   // file corrupt
	*pPos = 0;
	Line = QString(pFile);
	*pPos = '>';
	pFile = pPos+1;
	if(!isIndep) { untested();
		pos = 0;
		tmp = Line.section(' ', pos, pos);
		while(!tmp.isEmpty()) { untested();
			CPointsX.push_back(new DataX(tmp.toStdString()));  // name of independet variable
			pos++;
			tmp = Line.section(' ', pos, pos);
		}
	}

	// *****************************************************************
	// get independent variable ****************************************
	bool ok=true;
	double *p;
	int counting = 0;
	if(isIndep) {    // create independent variable by myself ?
		counting = Line.toInt(&ok);  // get number of values
		CPointsX.push_back(new DataX("number", 0, counting));
		if(!ok)  return 0;

		p = new double[counting];  // memory of new independent variable
		CountY = 1;
		CPointsX.back()->Points = p;
		for(int z=1; z<=counting; z++)  *(p++) = double(z);
		auto Axis = CPointsX.back();
		Axis->setLimit(1.);
		Axis->setLimit(double(counting));
		trace3("indep", counting, Axis->max(), Axis->min());
	}else{  // ...................................
		// get independent variables from data file
		CountY = 1;
#if 0 // FIXME: we do not have a Name.
		DataX *bLast = 0;
		if(Name == "Rect3D")  bLast = g->axis(1);  // y axis for Rect3D
#endif

#if 0 // FIXME: this is about diagram. do after load.
		double min_tmp = xAxis.min, max_tmp = xAxis.max;
#endif
		DataX const *pD;
		for(int ii= g->numAxes(); (pD = g->axis(--ii)); ) { untested();
#if 0 // FIXME: this is about diagram. do after load.
			pa = &xAxis;
			if(pD == g->axis(0)) { untested();
				xAxis.min = min_tmp;    // only count first independent variable
				xAxis.max = max_tmp;
			}
			else if(pD == bLast)  pa = &yAxis;   // y axis for Rect3D
#endif
			trace1("refresh axis", pD->Var);
			counting = loadIndepVarData(pD->Var, FileString, CPointsX[ii]);
			if(counting <= 0) { untested();
				trace0("huh, nothing there");
				return 0;
			}else{ untested();
				trace2("found sth", pD->Var, counting);
			}

			CountY *= counting;
		}
		CountY /= counting;
	}


	// *****************************************************************
	// get dependent variables *****************************************
	counting *= CountY;
	p = new double[2*counting]; // memory for dependent variables
	CPointsY = p;
#if 0 // FIXME: what does this do?!
	if(g->yAxisNo == 0)  pa = &yAxis;   // for which axis
	else  pa = &zAxis;
	(pa->numGraphs)++;    // count graphs
#endif

	char *pEnd;
	double x, y;
	pPos = pFile;

	if(Variable.right(3) != ".X ") { // not "digital"

		for(int z=counting; z>0; z--) { untested();
			pEnd = 0;
			while((*pPos) && (*pPos <= ' '))  pPos++; // find start of next number
			x = strtod(pPos, &pEnd);  // real part
			pPos = pEnd + 1;
			if(*pEnd < ' ')   // is there an imaginary part ?
				y = 0.0;
			else { untested();
				if(((*pEnd != '+') && (*pEnd != '-')) || (*pPos != 'j')) { untested();
					delete[] CPointsY; CPointsY = NULL;
					return 0;
				}
				*pPos = *pEnd;  // overwrite 'j' with sign
				pEnd = 0;
				y = strtod(pPos, &pEnd); // imaginary part
				*pPos = 'j';   // write back old character
				pPos = pEnd;
			}
			*(p++) = x;
			*(p++) = y;
#if 0 // FIXME there is no Name here.
			if(Name[0] != 'C')
#endif
			if(0) { untested();
				if(fabs(y) >= 1e-250){ untested();
				  	x = sqrt(x*x+y*y);
				}else{ untested();
				}
				if(std::isfinite(x)){ untested();
				  	setLimit(x);
				}else{ untested();
				}
			}

#if 0 // this is not location curve
			else {   // location curve needs different treatment
				if(std::isfinite(x)) { untested();
					if(x > xAxis.max) xAxis.max = x;
					if(x < xAxis.min) xAxis.min = x;
				}
				if(std::isfinite(y)) { untested();
					if(y > pa->max) pa->max = y;
					if(y < pa->min) pa->min = y;
				}
			}
#endif
		}

	} else {  // of "if not digital"

		char *pc = (char*)p;
		pEnd = pc + 2*(counting-1)*sizeof(double);
		// for digital variables (e.g. 100ZX0):
		for(int z=counting; z>0; z--) { untested();

			while((*pPos) && (*pPos <= ' '))  pPos++; // find start of next bit vector
			if(*pPos == 0) { untested();
				delete[] CPointsY; CPointsY = NULL;
				return 0;
			}

			while(*pPos > ' ') {    // copy bit vector
				*(pc++) = *(pPos++);
				if(pEnd <= pc) { untested();
					counting = pc - (char*)CPointsY;
					pc = (char*)realloc(g->CPointsY, counting+1024);
					pEnd = pc;
					CPointsY = (double*)pEnd;
					pc += counting;
					pEnd += counting+1020;
				}
			}
			*(pc++) = 0;   // terminate each vector with NULL
		}

	}  // of "if not digital"

//	qDebug() << "loaded" << Var << min() << max();
	if (numAxes()>1){ untested();
		trace3("dep", axis(1)->Var, axis(1)->min(), axis(1)->max());
	}
	if(numAxes()){ untested();
		trace3("dep", axis(0)->Var, axis(0)->min(), axis(0)->max());
	}

	lastLoaded = QDateTime::currentDateTime();
	return this;
}

void SimOutputDat::clear()
{ untested();
	// memory leak
//	SimOutputDir::clear();
}

void SimOutputDatVar::clear()
{ untested();
  for(auto i : CPointsX){ untested();
	  delete[] i;
  }
  delete[] CPointsY;
}

/*!
   Reads the data of an independent variable. Returns the number of points.
*/
int SimOutputDatVar::loadIndepVarData(std::string const& Variable,
			      char *FileString, DataX* pD)
{ untested();
  bool isIndep = false;
  QString Line, tmp;

  /* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
     to change the locale to the default. */
  setlocale (LC_NUMERIC, "C");

  Line = QString::fromStdString("dep "+Variable+std::string(" "));
  // "pFile" is used through-out the whole function and must NOT used
  // for other purposes!
  char *pFile = strstr(FileString, Line.toLatin1());
  while(pFile) { untested();
    if(*(pFile-1) == '<') {     // is dependent variable ?
      trace0("huh? dependent variable in loadIndep");
      break;
    }else if(strncmp(pFile-3, "<in", 3) == 0) {  // is independent variable ?
      trace0("okay, really independent.");
      isIndep = true;
      break;
    }
    pFile = strstr(pFile+4, Line.toLatin1());
  }

  if(!pFile)  return -1;   // data not found

  pFile += Line.length();
  char *pPos = strchr(pFile, '>');
  if(!pPos)  return -1;   // file corrupt
  *pPos = 0;
  Line = QString(pFile);
  *pPos = '>';
  pFile = pPos+1;
  char *pEnd;
  if(!isIndep) {           // dependent variable can also be used...
    if(Line.indexOf(' ') >= 0)  return -1; // ...if only one dependency
    Line = "<indep "+Line+" ";
    pPos = strstr(FileString, Line.toLatin1());
    if(!pPos)  return -1;
    pPos += Line.length();
    pEnd = strchr(pPos, '>');
    if(!pEnd)  return -1;   // file corrupt
    *pEnd = 0;
    Line = QString(pPos);
    *pEnd = '>';
  }


  bool ok;
  int n = Line.toInt(&ok);  // number of values
  if(!ok)  return -1;

  double *p = new double[n];     // memory for new independent variable
  assert(!pD->Points);
  pD->Points = p;
  pD->count  = n;


  double x;
  pPos = pFile;
  // find first position containing no whitespace
  while((*pPos) && (*pPos <= ' '))  pPos++;

  for(int z=0; z<n; z++) { untested();
    pEnd = 0;
    x = strtod(pPos, &pEnd);  // real part
    if(pPos == pEnd) { untested();
      delete[] pD->Points;  pD->Points = 0;
      return -1;
    }
    
    *(p++) = x;
#if 0 // this is not location curve code
    if(Name[0] != 'C')   // not for location curves
#endif
      if(std::isfinite(x)) { untested();
        pD->setLimit(x);
        pD->setLimit(x);
      }
    
    pPos = pEnd;
    while((*pPos) && (*pPos <= ' '))  pPos++;  // find start of next number
  }

  return n; // number of independent data points
}
