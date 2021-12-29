// List of test modules to register

#ifdef JERRY_ENABLE_DEBUG
#define TEST_MAP_DEBUG(XX)
#else
#define TEST_MAP_DEBUG(XX)
#endif

#define TEST_MAP(XX)                                                                                                   \
	XX(fatal)                                                                                                          \
	XX(types)                                                                                                          \
	XX(event)                                                                                                          \
	XX(parser)                                                                                                         \
	XX(context)
