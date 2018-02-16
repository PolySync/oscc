#ifndef VECTOR_HEADER
#define VECTOR_HEADER

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

typedef void (*GenericDestructor)(void *);
typedef struct CgreenVector_ CgreenVector;

CgreenVector *create_cgreen_vector(GenericDestructor destructor);
void destroy_cgreen_vector(CgreenVector *vector);
void cgreen_vector_add(CgreenVector *vector, void *item);
void *cgreen_vector_remove(CgreenVector *vector, int position);
void *cgreen_vector_get(const CgreenVector *vector, int position);
int cgreen_vector_size(const CgreenVector *vector);

#ifdef __cplusplus
    }
}
#endif

#endif
