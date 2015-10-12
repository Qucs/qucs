#ifndef __WPROBE_H__
#define __WPROBE_H__

class wprobe : public qucs::circuit
{
 public:
  CREATOR (wprobe);
  void initDC (void);
  void initAC (void);
  void initTR (void);
  void saveOperatingPoints (void);
  void calcOperatingPoints (void);
};

#endif /* __WPROBE_H__ */
