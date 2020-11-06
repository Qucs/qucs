
#ifndef QUCS_LEGACY_DAT_H
#define QUCS_LEGACY_DAT_H
// "dat" file, some legacy code rearranged.
// was just one var from dat file. (fixed)
class SimOutputDat : public SimOutputDir {
public:
	SimOutputDat(std::string const& filename, std::string const& varname);
//	int loadIndepVarData(std::string const&, char* datfilecontent, DataX*);

//	bool isEmpty() const { return !numAxes(); }
//	unsigned numAxes() const { return CPointsX.size();}
//	int countY() const { return CountY; }

public: // obsolete interface. don't use.
//	DataX const* axis(uint i) const override { if (i<axis_count) return CPointsX[i]; return NULL;}
//	double *cPointsY() const { return CPointsY; }
	SimOutputData const* refresh() override;

//	virtual const_iterator begin() const {return const_iterator(CPointsX.front()->Points, CPointsY);}
//	virtual const_iterator end() const {return const_iterator(CPointsX.back()->end(), NULL);}

private:
	void clear();

public:
//	void setLimit(const double& x){
//		if (Min>x) Min=x;
//		if (Max<x) Max=x;
//	}
private:
	unsigned axis_count;
//	std::vector<DataX*>  CPointsX;
	double *CPointsY;
//	int CountY;    // number of curves
	QString Var;
	std::string _fileName;
	QDateTime lastLoaded;  // when it was loaded into memory
	unsigned _attach_count;

//	std::map<std::string, SimOutputData*> _map; -> dir
};

#endif
