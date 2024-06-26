/* uninit.c: Implementation of uninitialized page.
 *
 * All of the pages are born as uninit page. When the first page fault occurs,
 * the handler chain calls uninit_initialize (page->operations.swap_in).
 * The uninit_initialize function transmutes the page into the specific page
 * object (anon, file, page_cache), by initializing the page object,and calls
 * initialization callback that passed from vm_alloc_page_with_initializer
 * function.
 * 모든 페이지는 uninit page로 탄생합니다. 
 * 첫 번째 페이지 장애가 발생하면 핸들러 체인은 
 * uninit_initize(페이지->operations.swap_in)를 호출합니다.
 * uninit_initialize 함수는 페이지 객체를 초기화함으로써 페이지를 
 * 특정 페이지 객체(anon, file, page_cache)로 변환하고 
 * vm_alloc_page_with_initializer 함수에서 전달된 초기화 콜백을 호출합니다.
 */

#include "vm/vm.h"
#include "vm/uninit.h"

static bool uninit_initialize(struct page *page, void *kva);
static void uninit_destroy(struct page *page);

/* DO NOT MODIFY this struct */
static const struct page_operations uninit_ops = {
    .swap_in = uninit_initialize,
    .swap_out = NULL,
    .destroy = uninit_destroy,
    .type = VM_UNINIT,
};

/* DO NOT MODIFY this function */
void uninit_new(struct page *page, void *va, vm_initializer *init,
                enum vm_type type, void *aux,
                bool (*initializer)(struct page *, enum vm_type, void *))
{
    ASSERT(page != NULL);

    *page = (struct page){
        .operations = &uninit_ops,
        .va = va,
        .frame = NULL, /* no frame for now */
        .uninit = (struct uninit_page){
            .init = init,
            .type = type,
            .aux = aux,
            .page_initializer = initializer,
        }};
}

/* Initalize the page on first fault */
static bool
uninit_initialize(struct page *page, void *kva)
{
    struct uninit_page *uninit = &page->uninit;

    /* Fetch first, page_initialize may overwrite the values */
    vm_initializer *init = uninit->init;
    void *aux = uninit->aux;

    /* TODO: You may need to fix this function. */
    return uninit->page_initializer(page, uninit->type, kva) &&
           (init ? init(page, aux) : true);
}

/* Free the resources hold by uninit_page. Although most of pages are transmuted
 * to other page objects, it is possible to have uninit pages when the process
 * exit, which are never referenced during the execution.
 * PAGE will be freed by the caller. 
 * uninit_page에서 보유하고 있는 리소스를 해방합니다. 
 * 대부분의 페이지가 다른 페이지 객체로 전송되지만 
 * 프로세스가 종료될 때 uninit 페이지를 가질 수 있으며, 이는 실행 중에 참조되지 않습니다.
 * 호출자가 PAGE를 해제합니다.
 */
static void
uninit_destroy(struct page *page)
{
    struct uninit_page *uninit UNUSED = &page->uninit;
    /* TODO: Fill this function.
     * TODO: If you don't have anything to do, just return. */
    // if (uninit->type == VM_UNINIT)
    // {
    //     free(uninit->aux);
    // }
    // free(uninit);
}
