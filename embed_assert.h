#ifndef __EMBED_ASSERT_H__
#define __EMBED_ASSERT_H__

	#define	EA_NULL (void)0

	#ifdef EA_NASSERT

		/* provide dummy (empty) definitions that don't generate any code... */
		#define EA_DEFINE_THIS_FILE
		#define EA_DEFINE_THIS_MODULE(name_)
		#define EA_ASSERT(test_)             (EA_NULL)
		#define EA_ASSERT_ID(id_, test_)     (EA_NULL)
		#define EA_ALLEGE(test_)             ((void)(test_))
		#define EA_ALLEGE_ID(id_, test_)     ((void)(test_))
		#define EA_ERROR()                   (EA_NULL)
		#define EA_ERROR_ID(id_)             (EA_NULL)

	#else  /* EA_NASSERT not defined--assertion checking enabled */

		#ifndef EA_VERSION 
			/*! typedef for character strings. */
			typedef char char_t;
			/*! typedef for assertions-ids and line numbers in assertions. */
			typedef int int_t;
		#endif

		/*! Define the file name (with `__FILE__`) for assertions in this file. */
		#define EA_DEFINE_THIS_FILE 					static char_t const EA_this_module_[] = __FILE__;
		/*! Define the user-specified module name for assertions in this file. */
		#define EA_DEFINE_THIS_MODULE(name_) 	static char_t const EA_this_module_[] = name_;
		
		/*! General purpose assertion. */
		#define EA_ASSERT(test_) 							((test_) ? EA_NULL : EA_onAssert(&EA_this_module_[0], __LINE__));
		/*! General purpose assertion with user-specified assertion-id. */
		#define EA_ASSERT_ID(id_, test_) 			((test_) ? EA_NULL : EA_onAssert(&EA_this_module_[0], (id_)))
		
		/*! General purpose assertion that __always__ evaluates the @p test_
    * expression. */
		#define EA_ALLEGE(test_)							EA_ASSERT(test_)
		/*! General purpose assertion with user-specified assertion-id that
    * __always__ evaluates the @p test_ expression. */
		#define EA_ALLEGE_ID(id_, test_) 			EA_ASSERT_ID((id_), (test_))
		
		/*! Assertion for a wrong path through the code. */
		#define EA_ERROR() 										EA_onAssert(&EA_this_module_[0], __LINE__)
    /*! Assertion with user-specified assertion-id for a wrong path. */
		#define EA_ERROR_ID(id_) 							EA_onAssert(&EA_this_module_[0], (id_))		
		
	#endif	//EA_NASSERT
	
	#ifdef __cplusplus
    extern "C" {
	#endif
	
	#ifndef EA_NORETURN
		/*! no-return function specifier */
		#define EA_NORETURN    void
	#endif /*  NORETURN */
	
	/*! Callback function invoked in case of any assertion failure. */
	EA_NORETURN EA_onAssert(char_t const * const module, int_t const location);	
	
	#ifdef __cplusplus
			}
	#endif	

	/*! Assertion for checking preconditions. */
	#define EA_REQUIRE(test_)									EA_ASSERT(test_)
	/*! Assertion for checking preconditions with user-specified assertion-id. */
	#define EA_REQUIRE_ID(id_, test_) 				EA_ASSERT_ID((id_), (test_))
		
	/*! Assertion for checking postconditions. */
	#define EA_ENSURE(test_) 									EA_ASSERT(test_)
	/*! Assertion for checking postconditions with user-specified assertion-id. */
	#define EA_ENSURE_ID(id_, test_) 					EA_ASSERT_ID((id_), (test_))
	
	/*! Assertion for checking invariants. */		
	#define EA_INVARIANT(test_) 							EA_ASSERT(test_)
	/*! Assertion for checking invariants with user-specified assertion-id. */
	#define EA_INVARIANT_ID(id_, test_) 			EA_ASSERT_ID((id_), (test_))
		
	/*! Static (compile-time) assertion. */	
	#define EA_ASSERT_STATIC(test_)						extern int_t assert_static[(test_) ? 1 : -1]	
	#define EA_ASSERT_COMPILE(test_) 					EA_ASSERT_STATIC(test_)		

	/*! Helper macro to calculate static dimension of a 1-dim @p array_ */
	#define EA_DIM(array_) 										(sizeof(array_) / sizeof((array_)[0U]))
	
		
#endif	//__EMBED_ASSERT_H__
	
