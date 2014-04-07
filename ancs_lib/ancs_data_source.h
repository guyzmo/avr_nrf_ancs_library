/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */

#ifndef _DATA_SOURCE_H_
#define _DATA_SOURCE_H_

#include "ancs_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ancs_parsing_env_t {
    uint8_t* buffer;
    uint8_t  index;
    uint8_t  ahead;
    uint32_t nid = 0;
    uint8_t  aid = 0;
} ancs_parsing_env_t;

void ancs_data_source_parser(const uint8_t* buffer);
void ancs_cache_attribute(uint32_t nid, uint8_t aid, const char* buffer);

#ifdef __cplusplus
}
#endif

#endif
