/****************************************************
 *  embed_assert.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class embed_assert       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__EMBED_ASSERT_H__)
#define __EMBED_ASSERT_H__


#ifdef __cplusplus
extern "C" {
#endif

#define EA_NULL (void)0
/**
 * ! no-return function specifier
 */
#define EA_NORETURN void
#define EA_DEFINE_THIS_FILE 


/**
 * provide dummy (empty) definitions that don't generate any code...
 */
#define EA_DEFINE_THIS_MODULE(name_) 
#define EA_ASSERT(test_) (EA_NULL)
#define EA_ASSERT_ID(id_, test_) (EA_NULL)
#define EA_ALLEGE(test_) ((void)(test_))
#define EA_ALLEGE_ID(id_, test_) ((void)(test_))
#define EA_ERROR() (EA_NULL)
#define EA_ERROR_ID(id_) (EA_NULL)
/**
 * ! Callback function invoked in case of any assertion failure.
 */
EA_NORETURN EA_onAssert(const char *module_str, const int loc_s32);
/**
 * ! Assertion for checking preconditions.
 */
#define EA_REQUIRE(test_) EA_ASSERT(test_)
/**
 * ! Assertion for checking preconditions with user-specified assertion-id.
 */
#define EA_REQUIRE_ID(id_, test_) EA_ASSERT_ID((id_), (test_))
/**
 * ! Assertion for checking postconditions.
 */
#define EA_ENSURE(test_) EA_ASSERT(test_)
/**
 * ! Assertion for checking postconditions with user-specified assertion-id.
 */
#define EA_ENSURE_ID(id_, test_) EA_ASSERT_ID((id_), (test_))
/**
 * ! Assertion for checking invariants.
 */
#define EA_INVARIANT(test_) EA_ASSERT(test_)
/**
 * ! Assertion for checking invariants with user-specified assertion-id.
 */
#define EA_INVARIANT_ID(id_, test_) EA_ASSERT_ID((id_), (test_))
/**
 * ! Static (compile-time) assertion.
 */
#define EA_ASSERT_STATIC(test_) extern int_t assert_static[(test_) ? 1 : -1]
#define EA_ASSERT_COMPILE(test_) EA_ASSERT_STATIC(test_)
/**
 * ! Helper macro to calculate static dimension of a 1-dim @p array_
 */
#define EA_DIM(array_) (sizeof(array_) / sizeof((array_)[0U]))


#ifdef __cplusplus
}
#endif


#endif /*!defined(__EMBED_ASSERT_H__)*/
