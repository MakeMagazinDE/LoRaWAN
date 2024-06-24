#define APP_LOG(TS,VL,...) \
  do { \
    { UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__); } \
  } while(0);
