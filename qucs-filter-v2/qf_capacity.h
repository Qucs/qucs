// Structure and enum describing filter or transform capacities

enum {

  // Can we compute the filter via its order
  CAN_ORDER = 1,
  CAN_ORDER_ONLY = 2 * CAN_ORDER,

  // Order restrictions
  CAN_ALL_ORDERS = 2 * CAN_ORDER_ONLY,
  CAN_ORDER_EVEN = 2 * CAN_ALL_ORDERS,
  CAN_ORDER_ODD = 2 * CAN_ORDER_EVEN,
  CAN_SUBORDER = 2 * CAN_ORDER_ODD,

  // Compatibility with dialog fields
  CAN_STOPBAND = 2 * CAN_SUBORDER,
  CAN_RIPPLE = 2 * CAN_STOPBAND,
  CAN_ATTENUATION = 2 * CAN_RIPPLE,
  CAN_IMPEDANCE2 = 2 * CAN_ATTENUATION,
  CAN_BOTH_RPL_AND_I2 = 2 * CAN_IMPEDANCE2,
  CAN_ANGLE = 2 * CAN_BOTH_RPL_AND_I2,
  CAN_BANDPASS = 2 * CAN_ANGLE,

  // Compatibility with dialog boxes
  CAN_OPTIMIZE = 2 * CAN_BANDPASS,
  CAN_POLE = 2 * CAN_OPTIMIZE
};


