/*
 * applications.h - the Qucs application list
 *

  { "==", TAG_BOOLEAN, evaluate::equal_d_d, 2,
    { TAG_DOUBLE, TAG_DOUBLE } },
  { "==", TAG_BOOLEAN, evaluate::equal_d_c, 2,
    { TAG_DOUBLE, TAG_COMPLEX } },
  { "==", TAG_BOOLEAN, evaluate::equal_d_v, 2,
    { TAG_DOUBLE, TAG_VECTOR } },
  { "==",  TAG_BOOLEAN, evaluate::equal_c_d, 2,
    { TAG_COMPLEX, TAG_DOUBLE  } },
  { "==",  TAG_BOOLEAN, evaluate::equal_c_c, 2,
    { TAG_COMPLEX, TAG_COMPLEX } },
  { "==",  TAG_VECTOR,  evaluate::equal_c_v, 2,
    { TAG_COMPLEX, TAG_VECTOR  } },
  { "==",  TAG_VECTOR,  evaluate::equal_v_d, 2,
    { TAG_VECTOR,  TAG_DOUBLE  } },
  { "==",  TAG_VECTOR,  evaluate::equal_v_c, 2,
    { TAG_VECTOR,  TAG_COMPLEX } },
  { "==",  TAG_VECTOR,  evaluate::equal_v_v, 2,
    { TAG_VECTOR,  TAG_VECTOR  } },

  { "!=", TAG_BOOLEAN, evaluate::notequal_d_d, 2,
    { TAG_DOUBLE, TAG_DOUBLE } },
  { "!=", TAG_BOOLEAN, evaluate::notequal_d_c, 2,
    { TAG_DOUBLE, TAG_COMPLEX } },
  { "!=", TAG_BOOLEAN, evaluate::notequal_d_v, 2,
    { TAG_DOUBLE, TAG_VECTOR } },
  { "!=",  TAG_BOOLEAN, evaluate::notequal_c_d, 2,
    { TAG_COMPLEX, TAG_DOUBLE  } },
  { "!=",  TAG_BOOLEAN, evaluate::notequal_c_c, 2,
    { TAG_COMPLEX, TAG_COMPLEX } },
  { "!=",  TAG_VECTOR,  evaluate::notequal_c_v, 2,
    { TAG_COMPLEX, TAG_VECTOR  } },
  { "!=",  TAG_VECTOR,  evaluate::notequal_v_d, 2,
    { TAG_VECTOR,  TAG_DOUBLE  } },
  { "!=",  TAG_VECTOR,  evaluate::notequal_v_c, 2,
    { TAG_VECTOR,  TAG_COMPLEX } },
  { "!=",  TAG_VECTOR,  evaluate::notequal_v_v, 2,
    { TAG_VECTOR,  TAG_VECTOR  } },


  { "!",  TAG_BOOLEAN, evaluate::not_b, 1,   { TAG_BOOLEAN } },
  { "||", TAG_BOOLEAN, evaluate::or_b_b, 2,  { TAG_BOOLEAN, TAG_BOOLEAN } },
  { "&&", TAG_BOOLEAN, evaluate::and_b_b, 2, { TAG_BOOLEAN, TAG_BOOLEAN } },
  { "==", TAG_BOOLEAN, evaluate::equal_b_b, 2,
    { TAG_BOOLEAN, TAG_BOOLEAN } },
  { "!=", TAG_BOOLEAN, evaluate::notequal_b_b, 2,
    { TAG_BOOLEAN, TAG_BOOLEAN } },

  { "random",  TAG_DOUBLE, evaluate::rand,    0, { TAG_UNKNOWN } },
  { "srandom", TAG_DOUBLE, evaluate::srand_d, 1, { TAG_DOUBLE  } },

  { "vector", TAG_VECTOR, evaluate::vector_x, -1, { TAG_UNKNOWN } },
  { "matrix", TAG_MATRIX, evaluate::matrix_x, -1, { TAG_UNKNOWN } },

  { NULL, 0, NULL, 0, { 0 } /* end of list */ }
};

// Converts a TAG_XXX value into a unique string.
const char * checker::tag2key (int tag) {
  const char * key = "";
  if (tag == TAG_RANGE)
    key = "R";
  else switch (tag & ~TAG_RANGE) {
  case TAG_UNKNOWN:
    key = "U"; break;
  case TAG_DOUBLE:
    key = "D"; break;
  case TAG_DOUBLE|TAG_COMPLEX:
    key = "D"; break;
  case TAG_BOOLEAN:
    key = "B"; break;
  case TAG_COMPLEX:
    key = "C"; break;
  case TAG_VECTOR:
    key = "V"; break;
  case TAG_MATRIX:
    key = "M"; break;
  case TAG_MATVEC:
    key = "MV"; break;
  case TAG_CHAR:
    key = "CHR"; break;
  case TAG_STRING:
    key = "STR"; break;
  }
  return key;
}

#endif /* __APPLICATIONS_H__ */
