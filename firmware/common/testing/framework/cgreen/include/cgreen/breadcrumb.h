#ifndef BREADCRUMB_HEADER
#define BREADCRUMB_HEADER

struct CgreenBreadcrumb_ {
    const char **trail;
    int depth;
    int space;
};

typedef struct CgreenBreadcrumb_ CgreenBreadcrumb;

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

CgreenBreadcrumb *create_breadcrumb(void);
void destroy_breadcrumb(CgreenBreadcrumb *breadcrumb);
void push_breadcrumb(CgreenBreadcrumb *breadcrumb, const char *name);
void pop_breadcrumb(CgreenBreadcrumb *breadcrumb);
const char *get_current_from_breadcrumb(CgreenBreadcrumb *breadcrumb);
int get_breadcrumb_depth(CgreenBreadcrumb *breadcrumb);
void walk_breadcrumb(CgreenBreadcrumb *breadcrumb, void (*walker)(const char *, void *), void *memo);

#ifdef __cplusplus
    }
}
#endif

#endif
