// List of test modules to register

// Assertions prevent testing of fatal error paths
#ifdef JERRY_ENABLE_DEBUG
#define TEST_MAP_DEBUG(XX)
#else
#define TEST_MAP_DEBUG(XX) XX(fatal)
#endif

#define TEST_MAP(XX)                                                                                                   \
	TEST_MAP_DEBUG(XX)                                                                                                 \
	XX(types)                                                                                                          \
	XX(event)                                                                                                          \
	XX(parser)
