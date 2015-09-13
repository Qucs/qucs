#ifndef __CIRCLINE_H__
#define __CIRCLINE_H__

  static const double pe11 = 1.841;// First root of J'_{1}(x) (Solution for the TE11 mode)
  static const double pm01 = 2.405;// First root of J'_{1}(x) (Solution for the TM01 mode)

class circline : public qucs::circuit
{
 public:
  CREATOR (circline);
  void initSP (void);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void saveCharacteristics (nr_complex_t);

 private:
  void calcPropagation (nr_double_t);
  void initCheck (void);
  void calcResistivity (const char * const, nr_double_t);
  /*! attenuation constant */
  nr_double_t alpha;
  /*! propagation constant */
  nr_double_t beta;
  /*! wave impedance */
  nr_complex_t zl;
  /*! cut off frequency lower bound */
  nr_double_t fc_low;
  /*! cut off frequency higher mode */
  nr_double_t fc_high;
  /*! resistivity */
  nr_double_t rho;
};

#endif /* __CIRCLINE_H__ */
