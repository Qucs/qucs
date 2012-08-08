/*
 * evaluate.h - definitions for Qucs equation evaluations
 *

  static constant * equal_d_c (constant *);
  static constant * equal_d_v (constant *);
  static constant * equal_c_d (constant *);
  static constant * equal_c_c (constant *);
  static constant * equal_c_v (constant *);
  static constant * equal_v_d (constant *);
  static constant * equal_v_c (constant *);
  static constant * equal_v_v (constant *);
  static constant * equal_b_b (constant *);
  static constant * notequal_d_d (constant *);
  static constant * notequal_d_c (constant *);
  static constant * notequal_d_v (constant *);
  static constant * notequal_c_d (constant *);
  static constant * notequal_c_c (constant *);
  static constant * notequal_c_v (constant *);
  static constant * notequal_v_d (constant *);
  static constant * notequal_v_c (constant *);
  static constant * notequal_v_v (constant *);

  static constant * notequal_b_b (constant *);
  static constant * not_b (constant *);
  static constant * or_b_b (constant *);
  static constant * and_b_b (constant *);

  static constant * rand (constant *);
  static constant * srand_d (constant *);

  static constant * vector_x (constant *);
  static constant * matrix_x (constant *);
};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

// Structure defining an application.
struct application_t
{
  const char * application; /* the name of the application     */
  int retval;               /* its return type                 */
  evaluator_t eval;         /* the actual application function */
  int nargs;                /* number of arguments             */
  int args[16];             /* the appropriate argument types  */
};

extern struct application_t applications[];

} /* namespace */

#endif /* __EVALUATE_H__ */
