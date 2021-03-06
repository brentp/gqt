#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

pri_queue priq_new(int size)
{
    if (size < 4) size = 4;

    pri_queue q = malloc(sizeof(pri_queue_t));
    q->buf = malloc(sizeof(q_elem_t) * size);
    q->alloc = size;
    q->n = 1;

    return q;
}

void priq_push(pri_queue q, int *data, priority pri)
{
    q_elem_t *b;
    int n, m;

    if (q->n >= q->alloc) {
        q->alloc *= 2;
        b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);
    } else
        b = q->buf;

    n = q->n++;
    /* append at end, then up heap */
    //while ((m = n / 2) && pri < b[m].pri) {
    while ((m = n / 2) && (pricmp(pri, b[m].pri) < 0)) {
        b[n] = b[m];
        n = m;
    }
    b[n].data = data;
    b[n].pri = pri;
}

/* remove top item. returns -1 if empty. *pri can be null. */
int * priq_pop(pri_queue q, priority *pri)
{
    int *out;
    if (q->n == 1) return NULL;

    q_elem_t *b = q->buf;

    out = b[1].data;
    if (pri) *pri = b[1].pri;

    /* pull last item to top, then down heap. */
    --q->n;

    int n = 1, m;
    while ((m = n * 2) < q->n) {
        //if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;
        if ( (m + 1 < q->n) && 
             (pricmp(b[m].pri,b[m + 1].pri) > 0))
            m++;

        //if (b[q->n].pri <= b[m].pri) break;
        if (pricmp(b[q->n].pri, b[m].pri) <= 0)
            break;
        b[n] = b[m];
        n = m;
    }

    b[n] = b[q->n];
    if (q->n < q->alloc / 2 && q->n >= 16)
        q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));

    return out;
}

/* get the top element without removing it from queue */
int* priq_top(pri_queue q, priority *pri)
{
    if (q->n == 1) return NULL;
    if (pri) *pri = q->buf[1].pri;
    return q->buf[1].data;
}

/* this is O(n log n), but probably not the best */
void priq_combine(pri_queue q, pri_queue q2)
{
    int i;
    q_elem_t *e = q2->buf + 1;

    for (i = q2->n - 1; i >= 1; i--, e++)
        priq_push(q, e->data, e->pri);
    priq_purge(q2);
}

void priq_free(pri_queue q)
{
    free(q->buf);
    free(q);
}

int pricmp(priority a, priority b)
{
    if (a.sum < b.sum)
        return -1;
    else if  (a.sum > b.sum)
        return 1;
    else if  (a.len > b.len)
        return -1;
    else if  (a.len < b.len)
        return 1;
    else
        return 0;
}
